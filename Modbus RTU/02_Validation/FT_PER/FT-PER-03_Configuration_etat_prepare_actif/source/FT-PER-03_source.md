# FT-PER-03 — Source normative consolidée

## PER03-R01 — Persistance de la configuration active

**Classification : `NOT_DEFINED`.**

La V1 définit l'application d'une configuration préparée et l'existence d'une image active, mais ne dit pas que cette configuration active est conservée après reboot, ni qu'elle est effacée.

## PER03-R02 — Persistance de `active_config_id`

**Classification : `NOT_DEFINED`.**

Le champ identifie la configuration active dans l'état courant. Aucun texte V1 ne lui attribue une persistance post-reboot.

## PER03-R03 — Persistance de `active_config_crc`

**Classification : `NOT_DEFINED`.**

Le CRC décrit l'image active, mais aucune conservation à travers reset n'est imposée.

## PER03-R04 — Persistance de l'image active 4E

**Classification : `NOT_DEFINED`.**

Le caractère RO et la cohérence de l'image active ne signifient pas qu'elle est stockée de manière non volatile.

## PER03-R05 — Persistance de la configuration préparée

**Classification : `NOT_DEFINED`.**

La V1 ne précise ni conservation ni effacement de la zone préparée après reboot.

## PER03-R06 — Persistance de `prepared_config_id`

**Classification : `NOT_DEFINED`.**

Aucune règle post-reboot explicite.

## PER03-R07 — Persistance de `prepared_config_crc`

**Classification : `NOT_DEFINED`.**

Aucune règle post-reboot explicite.

## PER03-R08 — `config_state` après reboot

**Classification : `NOT_DEFINED`.**

La V1 définit les états `VIDE`, `BROUILLON`, `VALIDE`, `ACTIF`, erreurs, et leurs transitions en fonctionnement. Elle ne définit pas l'état initial ou restauré après reboot.

Aucun oracle V1 ne permet donc d'imposer après reboot :

- `VIDE` ;
- `ACTIF` ;
- `VALIDE` ;
- `BROUILLON` ;
- ou tout autre état autorisé.

## PER03-R09 — `config_error_code` après reboot

**Classification : `NOT_DEFINED`.**

Aucune politique de conservation ou remise à zéro post-reboot n'est définie.

## PER03-R10 — `config_revision_counter` après reboot

**Classification : `NOT_DEFINED`.**

Le champ est un compteur de révision, mais la V1 ne dit ni qu'il est monotone à travers reboot ni qu'il repart d'une valeur particulière.

## PER03-R11 — Effets normaux d'APPLY CONFIG avant reboot

**Classification : `DELEGATED`.**

- préconditions et résultats transactionnels : FT-CMD-05 ;
- identité active, état `ACTIF` et image 4E après application réussie : FT-INT-02.

FT-PER-03 ne revalide pas ces relations en l'absence de reboot.

## PER03-R12 — Observation avant/après reboot

**Classification : `TRACE_ONLY`.**

Les champs peuvent être relevés avant et après RESET SOFTWARE pour caractérisation et préparation d'une future V1.1, mais leur conservation ou modification ne constitue pas un verdict V1.

## Règles anti-fabrication

- `RO` ne signifie pas `persistent` ;
- `ACTIF` ne signifie pas automatiquement « restauré au boot » ;
- `config_revision_counter` ne reçoit aucune monotonie inter-reboot inventée ;
- une zone préparée n'est pas présumée volatile ;
- aucune valeur par défaut post-reboot n'est créée ;
- la réussite d'APPLY CONFIG avant reboot ne crée pas une garantie de conservation après reboot.
