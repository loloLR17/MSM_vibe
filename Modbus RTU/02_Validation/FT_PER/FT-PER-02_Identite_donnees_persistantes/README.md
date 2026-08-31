# FT-PER-02 — Identité et données explicitement persistantes

## 1. Objet

FT-PER-02 valide les propriétés de persistance à travers un redémarrage uniquement lorsque la spécification V1 les qualifie explicitement comme persistantes.

## 2. Oracle V1 retenu

Le Bloc 0 impose explicitement :

- les informations d'identification sont statiques pendant le fonctionnement normal ;
- `device_id` doit être **unique et persistent**.

La propriété propriétaire de FT-PER-02 est donc :

`device_id avant reboot == device_id après reboot`.

Le reboot utilisé peut être un RESET SOFTWARE maîtrisé selon FT-PER-01. La mécanique de reset n'est pas retestée ici.

## 3. Frontière stricte avec la stabilité FT-STR

La stabilité pendant le fonctionnement normal n'est pas équivalente à une persistance à travers reboot.

FT-STR-07 possède la stabilité inter-requêtes des données explicitement statiques dans un contexte inchangé. FT-PER-02 possède uniquement le franchissement d'une frontière réelle de redémarrage.

## 4. Champs non promus à la persistance

La V1 ne qualifie pas explicitement comme persistants, dans sa partie normative :

- `hardware_version` ;
- `firmware_version_major/minor/patch` ;
- `protocol_version` ;
- `device_capabilities` ;
- `serial_number` ;
- `manufacturer`.

Ils sont statiques pendant le fonctionnement normal, mais FT-PER-02 n'invente pas pour eux une garantie de persistance post-reboot.

Le complément métier indiquant que le numéro de série ne doit jamais être modifié après fabrication est informatif et ne peut pas devenir un oracle V1.

## 5. Test actif

- `TT-PER-B00-001` — conservation bit à bit du `device_id` à travers un RESET SOFTWARE (`CONDITIONAL`, reboot contrôlé requis).

## 6. Délégations

- structure et encodage uint32 MSW/LSW : FT-STR ;
- caractère RO du Bloc 0 : FT-ACC ;
- unicité du `device_id` entre plusieurs équipements : famille structurelle/fonctionnelle appropriée, hors propriété de reboot ;
- exécution et cause du RESET SOFTWARE : FT-PER-01 / FT-CMD-07.

## 7. Dettes V1

La V1 ne précise pas explicitement si la persistance du `device_id` doit résister de manière identique à toutes les causes de reset (software reset, power-on, watchdog, brown-out, reset externe, firmware update). FT-PER-02 utilise le RESET SOFTWARE comme frontière contrôlable minimale sans généraliser à toutes les causes.

La politique cause-par-cause reste candidate V1.1.

## 8. Artefacts

- `source/FT-PER-02_source.md` ;
- `detaille/FT-PER-02_detaille.md` ;
- `detaille/FT-PER-02_matrice_couverture.csv`.

## 9. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
