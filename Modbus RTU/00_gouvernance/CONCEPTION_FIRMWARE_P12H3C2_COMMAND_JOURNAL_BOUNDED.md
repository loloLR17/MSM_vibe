# P12-H3c2 — Conception du CommandJournalStore borné à 256 transactions

## 1. Objet

Définir le remplacement du store dense indexé par `transaction_id` par un journal persistant borné conforme à la politique H3c1.

Cette tranche est une conception pré-implémentation. Elle ne modifie pas encore le code.

## 2. Contraintes héritées

Le nouveau store doit préserver :
- `transaction_id` valide sur 1..65535 ;
- fenêtre d'idempotence de 256 transactions distinctes ;
- même ID + même identité = retry sans réexécution ;
- même ID + identité différente = collision ;
- réutilisation possible uniquement après éviction ;
- une transaction non terminale jamais évincée ;
- une seule transaction active/incomplète à la fois ;
- recovery après power-loss ;
- résultat de la dernière commande terminée ;
- séquence persistante `RESERVED -> recovery context éventuel -> STARTED -> COMPLETED`.

Le record métier courant de 66 octets, son CRC et sa version restent réutilisables.

## 3. Séparation clé : transaction_id != emplacement

Le nouveau store ne calcule plus l'adresse persistante à partir du `transaction_id`.

Chaque transaction occupe un **slot logique** parmi 256 :

```text
slot 0
slot 1
...
slot 255
```

Le `transaction_id` est une donnée du record. `find(id)` recherche l'entrée dont le record courant porte cet ID.

Cette séparation supprime le couplage 65 535 IDs -> 65 535 emplacements.

## 4. Layout proposé

### 4.1 Slots transactionnels

Chaque slot logique conserve deux copies alternées du `CommandJournalRecord` actuel :

```text
256 slots * 2 copies * 66 octets = 33 792 octets
```

Pour une mutation d'une transaction déjà présente :
- lire/sélectionner la copie valide de génération la plus élevée ;
- écrire la génération suivante dans l'autre copie ;
- commit ;
- l'ancienne copie reste le fallback power-loss.

### 4.2 Métadonnées globales

Le store ajoute deux copies redondantes d'un petit enregistrement de métadonnées avec CRC/version/génération contenant au minimum :
- `next_completion_order` ;
- compteur monotone d'admission `next_admission_order` ou équivalent ;
- version du format.

La politique d'éviction ne doit pas dépendre de la position physique seule.

### 4.3 Ordre d'admission

Chaque entrée doit disposer d'un ordre persistant permettant d'identifier sans ambiguïté la transaction terminale la plus ancienne.

Décision : ajouter un `admission_order` persistant au format du journal borné.

Le format de stockage H3c2 est donc une nouvelle version persistante. Il n'est pas nécessaire de modifier `CommandJournalEntry` public si l'ordre d'admission reste une métadonnée interne du store.

## 5. Lookup

`find(transaction_id)` :
1. scanner au maximum 256 slots ;
2. sélectionner pour chaque slot sa copie courante valide ;
3. comparer le `transaction_id` ;
4. retourner NOT_FOUND si absent.

Complexité maximale bornée : O(256), indépendante du domaine uint16.

Aucun index persistant séparé n'est requis en V1 : cela évite une seconde structure à rendre atomique.

## 6. Admission d'une nouvelle transaction

`reserve(request)` suit l'ordre strict :

1. rechercher le `transaction_id`;
2. s'il existe, ne pas créer de nouvelle entrée ; la logique CommandEngine décide retry/collision à partir de l'identité existante ;
3. sinon chercher un slot vide ;
4. si aucun slot vide, sélectionner la transaction COMPLETED ayant le plus ancien `admission_order`;
5. ne jamais sélectionner RESERVED ou STARTED ;
6. si aucun slot évictable n'existe, retourner une erreur de capacité/état sans effet métier ;
7. écrire atomiquement la nouvelle entrée RESERVED avec un nouvel `admission_order`.

L'éviction et l'admission doivent être réalisées de façon à ce qu'un power-loss ne puisse pas rendre simultanément valide l'ancienne identité et une nouvelle identité incohérente dans le même slot.

## 7. Mutations d'une entrée

Les opérations existantes restent :
- `set_recovery_context`;
- `mark_started`;
- `complete`.

Elles ciblent d'abord le slot par lookup ID, puis alternent entre ses deux copies avec `generation + 1`.

Une mutation ne change pas `admission_order`.

`complete` attribue et persiste le `completion_order` existant.

## 8. Recovery au boot

Le recovery ne scanne plus 65 535 IDs. Il scanne les 256 slots physiques.

Pour chaque slot :
1. lire les deux copies ;
2. ignorer les copies vides ;
3. valider magic/version/taille/CRC/cohérence ;
4. sélectionner la génération valide la plus récente ;
5. détecter tout doublon de `transaction_id` entre slots comme corruption ;
6. compter les transactions connues ;
7. retrouver le plus grand `completion_order`;
8. retrouver le plus grand `admission_order`;
9. détecter les entrées non terminales.

Plus d'une transaction non terminale est une corruption, conformément à l'invariant une seule commande active.

Les compteurs suivants sont reconstruits depuis les records lorsque possible :
- `next_completion_order = max(completion_order) + 1`;
- `next_admission_order = max(admission_order) + 1`.

Les métadonnées globales servent de preuve/redondance et ne doivent pas être l'unique source nécessaire au recovery.

## 9. Wrap des compteurs internes

Les compteurs d'ordre internes sont `uint32`.

Le wrap `UINT32_MAX` n'est pas traité par comparaison naïve. Avant épuisement, le store doit refuser une nouvelle admission nécessitant un nouvel ordre et signaler un état nécessitant maintenance/migration.

Aucune remise à zéro silencieuse n'est autorisée.

## 10. Power-loss

Les propriétés minimales sont :
- une écriture candidate ne détruit jamais la dernière copie valide avant commit ;
- après reboot, soit l'ancienne génération, soit la nouvelle génération complète est sélectionnable ;
- une copie partielle/CRC invalide est ignorée si l'autre copie est valide ;
- deux copies non vides mais toutes deux invalides pour un slot utilisé entraînent un statut de corruption ;
- l'éviction d'une entrée terminale n'autorise la nouvelle identité qu'après persistance valide de son record candidat.

La traduction de `commit()` vers les garanties physiques du média STM32 sera définie séparément.

## 11. Impact sur CommandBootRecovery

L'API actuelle de `command_boot_recovery_scan()` parcourt `1..max_transaction_id` et appelle `find()`.

Cette dépendance à l'ancien layout dense doit disparaître.

Décision : étendre l'abstraction `CommandJournal` avec une primitive d'énumération bornée, ou fournir au recovery une primitive équivalente indépendante du domaine des IDs.

Le recovery applicatif ne doit connaître ni 256 slots physiques ni leur layout.

La conception détaillée de cette primitive est incluse dans la tranche d'implémentation H3c3.

## 12. Taille cible

Avec le record courant :
- records : 33 792 octets ;
- métadonnées et éventuelle extension interne `admission_order` : quelques kilo-octets au maximum selon le format final.

Objectif de conception : **< 40 KiB** pour le journal B5 complet.

Ce chiffre est un objectif de layout, pas encore une qualification de compatibilité avec la FLASH interne STM32.

## 13. Tests obligatoires avant gel

H3c3 devra couvrir au minimum :
- IDs 1 et 65535 ;
- 256 admissions distinctes ;
- 257e admission avec éviction du terminal le plus ancien ;
- absence d'éviction d'une entrée RESERVED/STARTED ;
- retry même ID/même identité avant éviction ;
- collision même ID/identité différente avant éviction ;
- réutilisation du même ID après éviction ;
- wrap de la séquence centrale 65535 -> 1 lorsque l'ancien 1 est hors fenêtre ;
- recovery avec 256 slots ;
- recovery d'une transaction RESERVED ;
- recovery d'une transaction STARTED ;
- double ID détecté comme corruption ;
- copie A valide / B corrompue et inversement ;
- power-loss simulé entre write et commit ;
- conservation de `latest_completed`;
- absence de réexécution B5 après reboot.

## 14. Décision

La conception retenue pour la suite est un **journal borné de 256 slots logiques, deux copies persistantes par slot, lookup borné par scan, ordre d'admission persistant et éviction du terminal le plus ancien**.

Aucun composant mémoire physique n'est choisi dans cette tranche.

Prochaine tranche : **P12-H3c3 — évolution des contrats et implémentation Host du journal borné**, en petites sous-tranches avec tests avant remplacement du store dense dans `SystemRuntime`.
