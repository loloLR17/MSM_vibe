# P12-H3h-D4-A — Conception de l'autorité transactionnelle des métadonnées bulk

## Statut

**CONCEPTION gelée avant implémentation.**

Baseline d'entrée :

```text
0e4fe6562f21335038a3ee55de269284b8fcca99
Firmware: validate H3h-D3 durable checkpoint barrier
```

## 1. Objet

Définir la publication persistante et récupérable du préfixe durable d'une
campagne bulk microSD sans recopier le payload de campagne à chaque checkpoint.

Cette tranche ne définit pas encore la stratégie d'intégrité de l'ensemble du
payload durable. Elle définit l'autorité transactionnelle des métadonnées.

## 2. Contrats conservés

Le contrat P8-B reste inchangé :

- records logiques fixes de 16 octets ;
- checkpoint à la fin de chaque AcquisitionWindow ;
- seule l'autorité du dernier checkpoint ou finish durable est récupérable ;
- une queue non checkpointée peut être perdue ;
- une campagne OPEN n'est jamais reprise automatiquement.

H3h-D3 reste la barrière de données :

```text
append
→ flush queue
→ sync données
→ préfixe candidat de checkpoint
```

D4-A ajoute ensuite la publication persistante de ce candidat.

## 3. Principe retenu

Le bulk n'utilise pas une image complète A/B.

Le payload est append-only dans une zone data dédiée. Les métadonnées seules
sont publiées par deux copies alternées A/B.

Chaque copie de métadonnée occupe **un secteur logique SD de 512 octets**.
Cette granularité est cohérente avec le média ciblé et évite qu'une publication
de métadonnée partage son secteur avec le payload.

Le choix de 512 octets est un choix de layout V1 pour la métadonnée ; il ne
constitue pas une hypothèse d'atomicité d'écriture de la carte. Une écriture
déchirée reste possible et doit être détectée par validation du record.

## 4. Descriptor V1

Chaque secteur descriptor contient explicitement au minimum :

| Champ | Rôle |
|---|---|
| magic | identifier le format bulk TR2 |
| physical_format_version | rejeter proprement un format inconnu |
| descriptor_size | valider la structure persistée |
| generation | ordonner les publications valides |
| campaign_id | lier l'autorité à une campagne |
| state | OPEN ou FINISHED |
| data_base | début physique de la zone payload de cette campagne |
| durable_prefix_bytes | nombre d'octets de payload autorisés |
| reserved | évolution future, valeur imposée |
| descriptor_crc32 | détecter une métadonnée déchirée/corrompue |

Les entiers sont encodés explicitement ; aucune structure C n'est persistée par
copie brute.

Le CRC couvre tous les octets significatifs du descriptor avant son champ CRC,
réserves incluses avec leur valeur normative.

## 5. Générations et copies

- génération initiale valide : 1 ;
- 0 et UINT64_MAX sont invalides/réservées ;
- chaque publication réussie incrémente la génération ;
- la cible est la copie opposée à la dernière copie publiée ;
- l'ancienne copie valide n'est jamais invalidée avant que la nouvelle soit
  complètement écrite, synchronisée, relue et validée ;
- l'épuisement de la génération est une condition UNSUPPORTED/erreur sûre, pas
  un wrap silencieux.

Au recovery, la copie valide de génération la plus élevée est l'autorité.

Une copie invalide/déchirée n'annule pas une autre copie valide.

Deux copies valides incompatibles pour une même génération constituent une
corruption et ne sont pas arbitrées silencieusement.

## 6. Ordre transactionnel d'un checkpoint

L'ordre obligatoire est :

```text
1. append / agrégation
2. flush du payload
3. sync du média pour le payload
4. construire descriptor candidat génération N+1
5. écrire le secteur descriptor inactif
6. sync du média pour le descriptor
7. relire le descriptor candidat
8. valider magic/version/champs/CRC/génération
9. seulement alors publier N+1 comme autorité courante en RAM
```

Une panne aux étapes 1 à 8 ne doit jamais rendre N+1 autoritaire en RAM.

Après reboot, le recovery ne dépend que des descriptors effectivement lisibles
et valides sur le média.

## 7. Begin et finish

`begin_campaign()` publie une première autorité OPEN avec
`durable_prefix_bytes = 0` avant que la campagne soit considérée commencée.

`finish_campaign()` utilise la même séquence que checkpoint, mais publie
l'état FINISHED après la barrière finale du payload.

Une campagne OPEN récupérée après reboot reste historique ; elle n'est pas
reprise automatiquement, conformément à P8-B.

## 8. Recovery des métadonnées

Chaque copie est classée séparément :

- EMPTY : secteur uniformément 0x00 ou 0xFF ;
- VALID : format supporté, champs cohérents et CRC correct ;
- UNSUPPORTED : format reconnaissable mais version non supportée ;
- BAD : record présent mais invalide/corrompu ;
- IO : lecture impossible.

Règles :

- au moins une copie VALID : choisir la génération valide la plus récente ;
- peer BAD/torn + une VALID : conserver la VALID ;
- aucune VALID + au moins une IO : recovery UNAVAILABLE ;
- aucune VALID + format réellement unsupported : recovery UNSUPPORTED ;
- copies vierges : recovery EMPTY ;
- records présents mais aucune autorité valide : recovery CORRUPTED.

## 9. Géométrie

D4-A ne gèle pas encore une taille maximale de campagne ni un nombre de slots.

Elle impose seulement qu'une géométrie fournie au backend définisse des régions
non chevauchantes et bornées dans la capacité réelle du média :

- descriptor A : 512 octets ;
- descriptor B : 512 octets ;
- zone data : base + capacité utilisable.

La capacité publiée ultérieurement dans
`ConfigurationValidationEnvironment` devra être dérivée de cette géométrie
réelle et non de la capacité nominale de la carte.

## 10. Limite d'intégrité explicitement ouverte

Le CRC32 du descriptor prouve l'intégrité de la **métadonnée**. Il ne prouve pas
à lui seul l'intégrité de tous les octets du payload désigné par
`durable_prefix_bytes`.

L'ancien `CampaignDataStorePersistent` pouvait valider chaque petit chunk avec
son CRC. Cette stratégie n'est pas transposée mécaniquement au nouveau bulk :
elle modifierait le layout et/ou le coût d'écriture/recovery.

Avant de déclarer le nouveau `CampaignDataStore` physiquement conforme au
recovery P8-B, une tranche dédiée doit décider et qualifier la preuve
d'intégrité du payload durable (par exemple journal/chunks de contrôle ou autre
mécanisme explicitement spécifié).

D4-A ne prétend donc pas fermer cette question.

## 11. Tests exigés pour l'implémentation D4-B

Au minimum :

- encode/decode déterministe du descriptor ;
- CRC altéré rejeté ;
- version inconnue classée UNSUPPORTED ;
- réserves/champs incohérents rejetés ;
- alternance A/B et génération croissante ;
- panne écriture descriptor : ancienne autorité conservée au reboot ;
- panne sync descriptor : ancienne autorité conservée au reboot ;
- descriptor candidat déchiré : ancienne copie valide sélectionnée ;
- readback invalide après publication : erreur et ancienne autorité RAM
  conservée ;
- recovery choisit la génération valide la plus récente ;
- même génération avec contenus incompatibles : CORRUPTED ;
- OPEN et FINISHED récupérés sans auto-reprise.

## 12. Invariants

- aucun filesystem imposé ;
- aucune hypothèse d'atomicité secteur SD ;
- aucune copie complète du payload à chaque checkpoint ;
- aucun préfixe publié avant sync des données ;
- aucun nouveau descriptor autoritaire avant écriture + sync + readback valide ;
- aucune modification du record P8-B de 16 octets ;
- aucune taille de buffer RAM de production gelée ;
- aucune affirmation prématurée d'intégrité globale du payload.
