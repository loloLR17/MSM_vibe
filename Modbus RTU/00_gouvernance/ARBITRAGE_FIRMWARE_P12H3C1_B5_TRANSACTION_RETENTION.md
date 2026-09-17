# P12-H3c1 — Politique V1 de rétention et de réutilisation des transaction_id B5

## 1. Objet

Lever définitivement l'ambiguïté V1 concernant la durée de vie de l'idempotence B5 et supprimer le couplage artificiel entre le domaine uint16 des `transaction_id` et une table persistante dense de 65 535 entrées.

Cette décision complète la section 7.2 du Bloc 5, qui indiquait jusqu'ici que la politique de réutilisation après disparition de l'historique d'idempotence n'était pas définie.

## 2. Principe retenu

Un `transaction_id` identifie une transaction dans une **fenêtre persistante d'idempotence**, et non pendant toute la durée de vie du capteur.

La centrale reste responsable de choisir un nouvel identifiant pour chaque nouvelle commande.

Le capteur conserve un historique persistant borné des transactions récentes suffisant pour :
- reconnaître une répétition après perte de réponse ;
- reconnaître une répétition après redémarrage/power-loss ;
- restituer le résultat d'une transaction récente déjà terminée ;
- récupérer sans réexécution une transaction récente incomplète.

## 3. Profondeur normative V1

La fenêtre d'idempotence V1 est fixée à :

```text
256 transaction_id distincts récents
```

Cette profondeur est exprimée en nombre de transactions distinctes admises, pas en durée.

Aucune expiration fondée sur le temps civil ou monotone n'est utilisée.

## 4. Règles d'admission et de réutilisation

Pour une requête reçue avec un `transaction_id` :

### 4.1 ID présent dans la fenêtre

- même identité de requête : il s'agit d'un retry ;
- la commande ne doit pas être exécutée une seconde fois ;
- si elle est terminée, son résultat précédent est réutilisé ;
- si elle est incomplète, la logique de recovery/idempotence existante s'applique ;
- identité de requête différente : collision, refus explicite, aucune exécution.

### 4.2 ID absent de la fenêtre

L'identifiant est admissible comme nouvelle transaction, sous réserve des autres règles B5.

Cela inclut un identifiant qui avait été utilisé anciennement mais dont l'entrée a été évincée de la fenêtre persistante.

## 5. Politique d'éviction

Lorsque la 257e transaction distincte doit être admise :

- la transaction **terminée** la plus ancienne de la fenêtre est évincée ;
- une transaction non terminale ne doit jamais être évincée ;
- l'éviction doit être atomique vis-à-vis de l'admission de la nouvelle transaction ;
- après reboot/power-loss, le firmware doit retrouver une fenêtre cohérente ou se placer en état de recovery sûr.

La V1 ne comporte qu'une commande active à la fois ; une entrée active/incomplète reste donc protégée contre l'éviction.

## 6. Conséquence pour la centrale

La centrale ne doit pas réutiliser volontairement un `transaction_id` tant qu'il peut encore appartenir aux 256 transactions distinctes les plus récentes du capteur.

Pour une séquence uint16 cyclique 1..65535, le wrap naturel fournit une distance très supérieure à la fenêtre de 256 transactions.

Une centrale qui redémarre et perd son compteur doit resynchroniser sa stratégie d'identifiants avant d'émettre de nouvelles commandes ; elle ne doit pas supposer qu'un petit ID est libre simplement parce qu'elle a redémarré.

## 7. Conséquence stockage

La V1 n'exige plus une table dense de 65 535 IDs.

Le `CommandJournalStore` pourra être remplacé par un journal persistant borné à 256 transactions, avec redondance/atomicité appropriée.

Ordre de grandeur brut avec le record actuel de 66 octets et deux copies :

```text
256 * 2 * 66 = 33 792 octets
```

Ce chiffre n'inclut pas les métadonnées nécessaires au ring/index, à la génération, au recovery et à l'atomicité.

La sélection du média STM32 doit être faite après conception de ce nouveau store borné.

## 8. Compatibilité protocole

Cette décision ne modifie pas :
- le mapping B5 ;
- le type uint16 de `transaction_id` ;
- le domaine valide 1..65535 ;
- la corrélation requête/réponse ;
- l'interdiction de réexécuter un retry encore présent dans la fenêtre ;
- le `CommandRequestMailbox` ;
- la règle une seule commande active ;
- l'absence de retry automatique firmware.

Elle précise la réserve normative existante sur la disparition de l'historique d'idempotence.

## 9. Migration firmware requise

Le store dense courant devient une implémentation transitoire à remplacer avant composition STM32 de production.

La prochaine tranche est **P12-H3c2 — conception du CommandJournalStore borné 256**, avant implémentation.

Elle devra définir :
- layout persistant ;
- index/ordre des entrées ;
- double copie ou autre mécanisme atomique ;
- séquence reserve -> recovery context -> started -> complete ;
- éviction du plus ancien terminal ;
- recovery power-loss ;
- comportement capacity/full ;
- tests de wrap et de réutilisation après éviction.

## 10. Validation

Décision documentaire. Aucun changement logiciel dans cette tranche.

Les validations Host/cross-build antérieures restent applicables au code existant. Le nouveau store ne sera considéré validé qu'après implémentation et tests dédiés.
