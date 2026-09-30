# P12-H3h-D4-B — Validation codec et recovery des métadonnées bulk

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
870de32f70f2294bf42cdc2632cebac30474bf40
Firmware: fix H3h-D4B CampaignId include
```

## Objet

Implémenter et tester l'autorité transactionnelle des métadonnées bulk définie
par H3h-D4-A, indépendamment du raccordement au checkpoint D3.

## Implémentation validée

H3h-D4-B introduit :

- un descriptor persistant V1 de 512 octets ;
- encodage explicite des champs persistants ;
- CRC32 du descriptor ;
- deux copies A/B ;
- génération 64 bits ;
- états OPEN et FINISHED ;
- publication par écriture de la copie inactive, sync média, readback et
  validation avant changement de l'autorité RAM ;
- recovery indépendant des deux copies avec sélection de la génération valide
  la plus récente.

## Cas validés

Les tests couvrent notamment :

- alternance A/B et génération croissante ;
- sélection de la génération valide la plus récente après reboot ;
- échec de sync : l'autorité précédente reste récupérable ;
- écriture candidate déchirée : la copie précédente valide reste autorité ;
- CRC corrompu rejeté ;
- version inconnue classée UNSUPPORTED ;
- conflit de deux copies incompatibles à génération identique classé
  CORRUPTED ;
- état FINISHED conservé au recovery.

La validation complète `tr2_validate.sh` a été exécutée localement et déclarée
entièrement verte.

## Correction intégrée avant validation

La première version du header D4-B référençait un chemin de header inexistant.
La baseline validée utilise le header réel déjà employé par
`campaign_data_store.h` :

```c
#include "tr2/domain/campaign/campaign.h"
```

Cette correction ne modifie pas le format ni la sémantique D4-B.

## Limites volontaires

D4-B ne raccorde pas encore cette autorité persistante à la barrière D3.

D4-B ne ferme pas non plus le GAP d'intégrité globale du payload durable
identifié en D4-A : le CRC du descriptor protège la métadonnée, pas l'ensemble
des octets bulk qu'elle désigne.

## Suite

La tranche suivante doit examiner puis implémenter le raccordement exact entre :

- la barrière de données H3h-D3 ;
- la publication de métadonnée H3h-D4-B ;
- les contrats `CampaignDataStore` existants.

Toute dépendance ou API utilisée dans cette intégration doit être vérifiée dans
l'état courant réel de `main` avant implémentation.
