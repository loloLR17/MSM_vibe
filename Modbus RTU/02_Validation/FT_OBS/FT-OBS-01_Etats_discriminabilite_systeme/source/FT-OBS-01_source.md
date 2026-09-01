# FT-OBS-01 — Source normative consolidée

## 1. Références normatives

Sources principales :
- `Modbus RTU/01_Specification_source/bloc1.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc4.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc6.md` V1 ;
- familles gelées FT-STR, FT-LIM, FT-BLK, FT-INT, FT-CMD, FT-SEQ et FT-PER lorsque la propriété possède déjà un owner ;
- plan maître de validation Modbus TR2.

Les compléments métier informatifs ne sont jamais utilisés comme oracle.

## 2. Doctrine de propriété FT-OBS-01

Une propriété est propriétaire FT-OBS-01 uniquement si elle répond à la question :

> Une centrale peut-elle associer de manière déterministe une valeur exposée à un état V1 défini, sans heuristique ni connaissance privée du firmware ?

La conformité du domaine numérique appartient à FT-LIM. La production correcte d'un état ou d'une transition appartient aux familles fonctionnelles correspondantes. FT-OBS-01 vérifie l'**interprétabilité** de l'information exposée.

## 3. Exigences retenues

### OBS01-R01 — B1 `acquisition_state` est discriminant

**Classification : `COVERED`.**

La V1 définit :
- `0` = Arrêtée ;
- `1` = En cours ;
- `2` = Pause ;
- `3` = Erreur ;
- `4..65535` = réservés.

Une centrale V1 peut donc distinguer les quatre états définis sans heuristique.

Test : `TT-OBS-B01-001`.

### OBS01-R02 — B1 `storage_status` est discriminant

**Classification : `COVERED`.**

La V1 définit :
- `0` = Non disponible ;
- `1` = Disponible ;
- `2` = Plein ;
- `3` = Erreur ;
- `4..65535` = réservés.

Une centrale V1 peut distinguer disponibilité, saturation et erreur du stockage au niveau synthétique exposé par B1.

Test : `TT-OBS-B01-001`.

### OBS01-R03 — B1 `system_status` est interprétable globalement

**Classification : `NOT_DEFINED`.**

La V1 indique explicitement que le domaine détaillé de `system_status` n'est pas défini et reste à arbitrer. Le champ existe, mais aucune table normative ne permet à une centrale V1 d'en interpréter les valeurs de manière portable.

Aucun code issu des compléments métier ne peut être utilisé comme oracle.

### OBS01-R04 — B1 `system_flags` possède une sémantique détaillée exploitable

**Classification : `NOT_DEFINED`.**

B1 expose `system_flags` comme bitfield d'état système, mais la source normative V1 ne fournit pas de table détaillée des bits.

Une centrale ne peut donc pas attribuer une signification normative à chacun de ces bits sans convention externe.

### OBS01-R05 — B4 `config_state` est discriminant

**Classification : `COVERED`.**

La V1 définit explicitement :
- `0` = VIDE ;
- `1` = BROUILLON ;
- `2` = VALIDE ;
- `3` = RÉSERVÉ ;
- `4` = ACTIF ;
- `5` = ERREUR_VALIDATION ;
- `6` = ERREUR_APPLICATION ;
- `7..65535` = RÉSERVÉ.

Une centrale peut distinguer les états de préparation, validation, activation et erreurs sans inférer leur sens depuis les autres champs.

Test : `TT-OBS-B04-001`.

### OBS01-R06 — Transitions de `config_state`

**Classification : `DELEGATED`.**

La V1 définit également les transitions autorisées de `config_state`. Leur conformité fonctionnelle reste propriétaire FT-BLK / FT-CMD / FT-INT selon le stimulus. FT-OBS-01 ne réexécute pas ces transitions pour créer un second owner.

### OBS01-R07 — Valeurs réservées des enums d'état

**Classification : `DELEGATED`.**

La validation des valeurs définies/réservées et des domaines reste à FT-LIM. FT-OBS-01 utilise seulement les codes définis comme vocabulaire d'interprétation.

### OBS01-R08 — B6 `selected_campaign_valid` est un discriminant explicite

**Classification : `COVERED`.**

La V1 définit :
- `0` = index invalide ;
- `1` = campagne valide.

Elle précise en outre que lorsque `selected_campaign_valid = 0`, les métadonnées de l'entrée sélectionnée ne doivent pas être interprétées comme valides.

Une centrale dispose donc d'un marqueur normatif explicite et n'a pas à déduire la validité à partir d'un ID, d'un label ou d'un timestamp.

Test : `TT-OBS-B06-001`.

### OBS01-R09 — B6 `campaign_state` est discriminant

**Classification : `COVERED`.**

La V1 définit :
- `0` = vide ;
- `1` = en préparation ;
- `2` = en cours ;
- `3` = terminée ;
- `4` = erreur ;
- `5` = partiellement corrompue.

Une centrale peut distinguer directement ces états pour l'entrée sélectionnée lorsque celle-ci est valide.

Test : `TT-OBS-B06-001`.

### OBS01-R10 — B6 `storage_health_status` est discriminant

**Classification : `COVERED`.**

La V1 définit :
- `0` = OK ;
- `1` = warning ;
- `2` = dégradé ;
- `3` = critique.

Une centrale peut qualifier le niveau de santé du stockage sans seuil privé.

Test : `TT-OBS-B06-001`.

### OBS01-R11 — État global synthétique unique du capteur

**Classification : `NOT_DEFINED`.**

La présence de plusieurs états locaux dans B1, B4, B6 et B7 ne définit pas une règle normative permettant de calculer un « état global » par priorité, maximum de sévérité ou combinaison de bits.

En particulier, l'absence de domaine normatif de `B1.system_status` interdit de fabriquer un tel oracle en V1.

### OBS01-R12 — Relations entre états de blocs différents

**Classification : `DELEGATED`.**

Toute relation normative entre acquisition, configuration, campagne ou diagnostic appartient à FT-INT lorsqu'elle est explicitement définie. FT-OBS-01 n'invente ni égalité ni priorité implicite entre états simultanés.

## 4. Règles anti-fabrication

FT-OBS-01 interdit explicitement :
- d'utiliser les valeurs recommandées des compléments métier pour `system_status` ;
- de définir des bits de `system_flags` absents de la section normative ;
- de considérer `0` comme « inconnu » ou « absent » hors convention locale explicite ;
- de déduire qu'une campagne est valide à partir de ses métadonnées lorsque `selected_campaign_valid = 0` ;
- de transformer un état RO en preuve de stabilité ou de persistance ;
- de créer une règle de priorité entre plusieurs états de blocs différents ;
- de refaire les tests de transition ou de domaine déjà propriétaires des familles gelées.

## 5. Dette V1.1 candidate

À documenter comme évolution informative, sans modifier V1 :
- définir une table normative de `B1.system_status` si un état global synthétique est réellement souhaité ;
- définir une table normative de `B1.system_flags` si ces bits doivent être exploitables par une centrale générique ;
- définir, uniquement si nécessaire, une règle explicite de synthèse/priorité entre états locaux. Aucune telle règle n'est présumée en V1.