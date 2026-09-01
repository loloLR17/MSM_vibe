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

**Classification : `COVERED`.**

La V1 définit explicitement :
- `0` = UNKNOWN ;
- `1` = NOMINAL ;
- `2` = DEGRADED ;
- `3` = FAULT ;
- `4..65535` = réservés.

Une centrale V1 peut donc interpréter la qualification globale exposée sans convention privée. La règle exhaustive de dérivation de `system_status` à partir des autres états et flags n'est en revanche pas définie en V1.

Test : `TT-OBS-B01-001`.

### OBS01-R04 — B1 `system_flags` possède une sémantique détaillée exploitable

**Classification : `COVERED`.**

B1 définit explicitement les bits `READY`, `ACQUISITION_ACTIVE`, `CONFIG_VALID`, `TIME_VALID` et `STORAGE_AVAILABLE`; les bits 5..15 sont réservés à 0.

Une centrale peut donc interpréter ces cinq indicateurs sans convention externe. Aucune machine d'état ni règle exhaustive de synthèse avec `system_status` n'est déduite de cette table.

Test : `TT-OBS-B01-001`.

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

`B1.system_status` fournit désormais une qualification globale directement interprétable, mais la V1 ne définit pas de règle exhaustive permettant à la centrale de la recalculer à partir des états locaux B1, B4, B6, B7 ou des bitfields.

La centrale doit donc consommer `system_status` selon sa table normative sans fabriquer une priorité ou une formule de synthèse absente de V1.

### OBS01-R12 — Relations entre états de blocs différents

**Classification : `DELEGATED`.**

Toute relation normative entre acquisition, configuration, campagne ou diagnostic appartient à FT-INT lorsqu'elle est explicitement définie. FT-OBS-01 n'invente ni égalité ni priorité implicite entre états simultanés.

## 4. Règles anti-fabrication

FT-OBS-01 interdit explicitement :
- de déduire une règle exhaustive de calcul de `system_status` à partir des autres états ou flags ;
- de donner une signification aux bits réservés de `system_flags` ;
- de considérer `0` comme « inconnu » ou « absent » hors convention locale explicite ;
- de déduire qu'une campagne est valide à partir de ses métadonnées lorsque `selected_campaign_valid = 0` ;
- de transformer un état RO en preuve de stabilité ou de persistance ;
- de créer une règle de priorité entre plusieurs états de blocs différents ;
- de refaire les tests de transition ou de domaine déjà propriétaires des familles gelées.

## 5. Dette V1.1 candidate

À documenter comme évolution informative, sans modifier V1 :
- définir, si nécessaire, une règle exhaustive de dérivation de `B1.system_status` depuis les états/flags sous-jacents ;
- définir, uniquement si nécessaire, une règle explicite de synthèse/priorité entre états locaux. Aucune telle règle n'est présumée en V1.