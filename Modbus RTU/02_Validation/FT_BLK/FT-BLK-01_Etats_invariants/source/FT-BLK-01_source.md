# FT-BLK-01 — Source normalisée

## 1. Référentiel normatif

Références principales :
- `01_Specification_source/bloc1.md` — §6.1 et §7 ;
- `01_Specification_source/bloc2.md` — §6.1 à §6.4 et §7 ;
- `01_Specification_source/bloc3.md` — §8.1 à §8.7 et §9 ;
- `01_Specification_source/bloc7.md` — §5, §6, §8 et §9 ;
- `01_Specification_source/charte_typage.md` pour les conventions générales applicables.

Les sections « Compléments métier » sont informatives et exclues des oracles de test.

## 2. Exigences normalisées

### BLK01-B1-001 — Persistance d’un défaut actif
Le Bloc 1 impose que les défauts restent persistants tant que la condition associée est présente.

Classification : `CONDITIONAL`.
Motif : nécessite de pouvoir provoquer et maintenir une condition de défaut déterministe.

### BLK01-B1-002 — Persistance d’un avertissement actif
Le Bloc 1 impose que les avertissements restent persistants tant que leur condition est présente.

Classification : `CONDITIONAL`.
Motif : nécessite de pouvoir provoquer et maintenir une condition d’avertissement déterministe.

### BLK01-B1-003 — Cohérence exacte `system_status` / flags
La V1 indique que les flags doivent être cohérents avec les états globaux, mais le domaine détaillé de `system_status` reste explicitement À ARBITRER et les bitfields B1 ne sont pas normativement détaillés.

Classification : `NOT_DEFINED`.

### BLK01-B2-001 — Temps invalide / bit de validité
Lorsque l’état temporel expose explicitement un temps invalide, le flag `Temps valide` ne peut simultanément qualifier le temps de valide.

Classification : `COVERED`.

### BLK01-B2-002 — Temps synchronisé / indication de synchronisation
Lorsque `time_status = 3` (`Temps synchronisé`), l’état des indicateurs de synchronisation doit rester compatible avec cette qualification ; une contradiction explicite est non conforme.

Classification : `COVERED`.
Remarque : la V1 ne définit pas une table de vérité exhaustive de tous les bits annexes.

### BLK01-B2-003 — Temps préparé disponible
Lorsque `prepared_time_status = 1` (`Temps préparé disponible`), le bit 3 `Temps préparé disponible` de `time_flags` doit être cohérent avec cet état.

Classification : `COVERED`.

### BLK01-B2-004 — État temporel dégradé
La V1 définit `time_status = 4` (`Temps dégradé`) et plusieurs flags de qualification, mais ne formalise pas la combinaison obligatoire correspondant à cet état.

Classification : `CONDITIONAL`.

### BLK01-B3-001 — Dépassement global redondant
`B3_EXCEED_GLOBAL` et le bit `EXCEED_GLOBAL_ACTIVE` de `B3_ALARM_FLAGS` décrivent le même état courant de dépassement global.

Classification : `COVERED`.

### BLK01-B3-002 — Dépassement X redondant
`B3_EXCEED_X` et `EXCEED_X_ACTIVE` décrivent le même état courant.

Classification : `COVERED`.

### BLK01-B3-003 — Dépassement Y redondant
`B3_EXCEED_Y` et `EXCEED_Y_ACTIVE` décrivent le même état courant.

Classification : `COVERED`.

### BLK01-B3-004 — Dépassement Z redondant
`B3_EXCEED_Z` et `EXCEED_Z_ACTIVE` décrivent le même état courant.

Classification : `COVERED`.

### BLK01-B3-005 — Alarme mémorisée redondante
`B3_ALARM_LATCHED` et `ALARM_LATCHED_PRESENT` décrivent la présence d’une alarme mémorisée.

Classification : `COVERED`.

### BLK01-B3-006 — Statut global / flags de validité
La V1 définit les états de `B3_STATUS_GLOBAL` et les bits de `B3_VALIDITY_FLAGS`, mais ne donne pas de table de vérité exhaustive entre eux.

Classification : `CONDITIONAL`.

### BLK01-B3-007 — Sévérité globale / flags d’alarme
La V1 ne définit pas la fonction complète de dérivation de `B3_SEVERITY_GLOBAL` depuis les flags d’alarme ou de dépassement.

Classification : `NOT_DEFINED`.

### BLK01-B7-001 — Sentinelle aucun défaut connu
La V1 définit explicitement `last_fault_code = 0` comme « aucun défaut connu ».

Classification : `COVERED`.

### BLK01-B7-002 — Flags défaut / santé globale
La V1 ne définit pas la fonction de dérivation `system_fault_flags → system_health_status`.

Classification : `NOT_DEFINED`.

### BLK01-B7-003 — Flags défaut / dernier code défaut
La V1 ne définit pas la fonction de dérivation `system_fault_flags → last_fault_code`.

Classification : `NOT_DEFINED`.

### BLK01-B7-004 — Priorité entre plusieurs défauts
La priorité du « défaut principal » n’est évoquée que dans les compléments métier informatifs.

Classification : `NOT_DEFINED`.

### BLK01-B7-005 — Cohérence autotest
Les états de `selftest_status` sont normativement définis, mais les domaines et relations complètes avec `selftest_result_code` et `selftest_detail` ne le sont pas.

Classification : `CONDITIONAL`.

## 3. Règle d’acceptation

Aucun cas `NOT_DEFINED` ne peut être marqué PASS ou FAIL sur la base d’un oracle inventé. Il constitue une dette normative traçable.
