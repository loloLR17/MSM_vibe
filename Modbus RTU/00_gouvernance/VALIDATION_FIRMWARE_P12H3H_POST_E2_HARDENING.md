# P12-H3h — Validation durcissement D4-B / D5-C post-E2

## Statut

VALIDÉ ET GELÉ.

Baseline validée :

```text
29ee931789b15a880b39bf2bdc9dfc468066fa31
Firmware: clarify D5C zero-prefix overlap test
```

Validation locale rapportée :

```text
101/101 tests verts
cross-build STM32 : OK
```

## Corrections validées

- comparaison des `CampaignBulkDescriptor` champ par champ au lieu d'un
  `memcmp` dépendant du padding C ;
- géométrie D4-B : copies A/B alignées sur 512 octets, non chevauchantes et
  bornées par le média ;
- scan D5-C : détection globale de `campaign_id` dupliqués ;
- scan D5-C : détection des réservations/extents physiques incompatibles ;
- un descriptor OPEN à préfixe logique nul réserve néanmoins son
  `data_base` ;
- une réservation exactement à la fin d'un extent précédent reste valide.

Le contrôle des bornes physiques d'un préfixe non nul reste assuré par le
reader D5-B appelé par `validate_prefix()`.

## Limites

Ce gel ne remplace pas la qualification physique de la composition complète
D5-C -> E1 sur microSD et ne qualifie pas encore une coupure réelle pendant
append/checkpoint.
