# P12-H3h-D5-A — Validation codec de bloc bulk contrôlé

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
e2768b9d3e12bc59e3cbf11b7b715b5eea4672d9
Firmware: wire H3h-D5A bulk block tests
```

## Objet

Introduire le format physique élémentaire permettant de contrôler l'intégrité
du payload bulk sans modifier le record logique P8-B de 16 octets.

## Implémentation validée

D5-A introduit :

- magic et version explicites ;
- header de taille explicite ;
- `campaign_id` ;
- index de bloc 64 bits ;
- taille utile du payload ;
- payload constitué de records P8-B complets ;
- CRC32 couvrant header et payload ;
- encodage/décodage explicites, sans persistance d'une structure C brute.

La taille du payload reste variable et n'est pas une constante de production.

## Invariants validés

- payload non vide ;
- taille du payload multiple de 16 octets ;
- `campaign_id` valide ;
- index de bloc contrôlé ;
- capacité du buffer de sortie contrôlée ;
- CRC faux détecté ;
- corruption du header détectée ;
- version inconnue remontée comme UNSUPPORTED ;
- champs réservés non nuls rejetés ;
- bloc tronqué ou étendu rejeté.

## Validation

La validation complète `tr2_validate.sh` a été exécutée localement sur la
baseline ci-dessus et déclarée entièrement verte.

## Limites

D5-A ne définit pas :

- la taille de bloc de production ;
- la taille des buffers RAM ;
- l'allocation des campagnes sur le média ;
- le writer/reader séquentiel ;
- le checkpoint ;
- le backend SDMMC ;
- la composition dans SystemRuntime.

## Suite

H3h-D5-B doit fournir le writer/reader séquentiel au-dessus de
`CampaignBulkMedia` et du codec D5-A, avec contrôle des offsets, de la capacité
média et de la continuité des blocs, sans figer une taille de production.
