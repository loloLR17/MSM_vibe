# P12-H3h-D5-B — Validation writer/reader séquentiel de blocs bulk

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
7909471b17cb0c2652c81a2b50dcd88ed1b10323
Firmware: wire H3h-D5B block stream tests
```

## Objet

Fournir le writer/reader séquentiel au-dessus de CampaignBulkMedia et du codec
de blocs contrôlés D5-A, sans imposer de taille de bloc ou de buffer de
production.

## Invariants validés

- capacité réelle du média contrôlée ;
- offsets et index de blocs 64 bits ;
- aucune progression du writer avant écriture réussie ;
- lecture du header avant détermination de la taille complète du bloc ;
- scratch buffer fourni par l'appelant ;
- aucune allocation dynamique cachée ;
- campaign_id et block_index vérifiés à la lecture ;
- aucune progression du reader sur bloc invalide ;
- erreur physique de lecture/écriture rend le stream faulted ;
- aucun sync implicite.

## Validation

La validation complète tr2_validate.sh a été exécutée localement sur la
baseline ci-dessus et déclarée entièrement verte.

## Limites

D5-B n'implémente pas CampaignDataStore, le checkpoint, l'allocation
multi-campagnes ni le backend SDMMC.

## Suite

H3h-D5-C : composition des metadata D4-B et des blocs D5-A/D5-B dans un
CampaignDataStore bulk multi-campagnes, avant toute substitution dans
SystemRuntime.
