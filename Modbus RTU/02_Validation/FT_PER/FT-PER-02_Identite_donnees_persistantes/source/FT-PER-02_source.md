# FT-PER-02 — Source normative consolidée

## PER02-R01 — Persistance du device_id

**Classification : `COVERED`.**

Le Bloc 0 V1 impose explicitement que le `device_id` soit unique et persistent.

Pour la propriété de persistance, FT-PER-02 compare le uint32 complet avant et après une frontière de reboot contrôlée.

Test : `TT-PER-B00-001`.

## PER02-R02 — Unicité du device_id

**Classification : `DELEGATED`.**

L'unicité est normative mais ne constitue pas une propriété post-reboot à elle seule. FT-PER-02 ne tente pas de démontrer l'unicité globale d'un parc à partir d'un seul équipement.

## PER02-R03 — Stabilité des autres champs B0 pendant fonctionnement normal

**Classification : `DELEGATED` vers FT-STR-07.**

La V1 impose que les informations du Bloc 0 soient statiques pendant le fonctionnement normal. Cette stabilité sans reboot est déjà possédée par FT-STR-07.

## PER02-R04 — Persistance post-reboot de hardware_version

**Classification : `NOT_DEFINED`.**

Le champ est une information statique B0 mais la V1 ne le qualifie pas explicitement de persistent à travers reboot.

## PER02-R05 — Persistance post-reboot des versions firmware

**Classification : `NOT_DEFINED`.**

Aucune règle normative de persistance post-reboot n'est ajoutée au-delà de la stabilité normale. Une mise à jour firmware constitue en outre une cause de reset distincte dans la V1.

## PER02-R06 — Persistance post-reboot de protocol_version

**Classification : `NOT_DEFINED`.**

Aucune qualification explicite de persistance post-reboot.

## PER02-R07 — Persistance post-reboot de device_capabilities

**Classification : `NOT_DEFINED`.**

Aucune qualification explicite de persistance post-reboot.

## PER02-R08 — Persistance post-reboot de serial_number

**Classification : `NOT_DEFINED`.**

Le mapping normatif décrit `serial_number` comme information d'identification statique. La phrase « le numéro de série ne doit jamais être modifié après fabrication » apparaît dans les compléments métier, explicitement informatifs et non normatifs. Elle ne peut donc pas être utilisée comme oracle V1.

## PER02-R09 — Persistance post-reboot de manufacturer

**Classification : `NOT_DEFINED`.**

Aucune qualification explicite de persistance post-reboot.

## PER02-R10 — Persistance du device_id selon toutes les causes de reset

**Classification : `NOT_DEFINED`.**

La V1 affirme la persistance du `device_id` mais ne définit pas une matrice cause de reset → garantie de conservation. Le test V1 actif utilise RESET SOFTWARE comme frontière contrôlée. L'extension à power cycle, watchdog, brown-out, reset externe ou mise à jour firmware n'est pas imposée sans arbitrage supplémentaire.

## Règles anti-fabrication

- ne pas transformer « statique pendant le fonctionnement normal » en « non volatil » ;
- ne pas utiliser les compléments métier comme oracle ;
- ne pas imposer à tous les champs B0 la propriété explicitement écrite seulement pour `device_id` ;
- ne pas déduire une technologie de stockage Flash/EEPROM ;
- ne pas généraliser le test RESET SOFTWARE à toutes les causes de reset sans texte normatif.
