# FT-INT-03 — Configuration et supervision B4↔B3

## 1. Objet

FT-INT-03 valide les relations normatives inter-blocs entre la configuration de supervision du Bloc 4 et les résultats de supervision du Bloc 3.

## 2. Principe

Les décisions de supervision du Bloc 3 doivent être fondées sur la configuration B4 active au moment du calcul. Une modification de la zone préparée n'a pas d'effet direct sur les résultats B3. Un changement de configuration n'est pas rétroactif et prend effet à partir de la prochaine fenêtre de calcul validée.

## 3. Périmètre actif

- utilisation de la configuration B4 active par B3 ;
- absence d'effet d'une configuration seulement préparée ;
- absence de recalcul rétroactif des résultats déjà validés ;
- prise d'effet à la prochaine fenêtre de calcul validée ;
- scénario discriminant avec vibration contrôlée située entre deux seuils.

## 4. Statuts et limites

Les oracles existent en V1, mais les tests numériques nécessitant une vibration connue et reproductible sont `CONDITIONAL` tant qu'aucun banc d'injection, simulateur ou rejeu déterministe n'est disponible.

Restent explicitement hors oracle FT-INT-03 :
- équivalence exhaustive `B3_VALIDITY_FLAGS.CONFIG_VALID` ↔ état B4 : `NOT_DEFINED` ;
- table exhaustive seuils B4 ↔ `B3_SEVERITY_GLOBAL` / tous bits d'alarme : `NOT_DEFINED` au-delà des relations explicitement normées ;
- chronologie exacte issue de `threshold_hysteresis_mg` et `alarm_hold_time_ms` lorsqu'elle n'est pas entièrement définie par la V1 ;
- cohérences internes B3 : FT-BLK ;
- domaines et unités : FT-LIM ;
- application de configuration via B5 : FT-INT-02 / FT-CMD.

## 5. Tests

- `TT-INT-B03B04-001` — Seuil actif versus seuil préparé ;
- `TT-INT-B03B04-002` — Absence de rétroactivité ;
- `TT-INT-B03B04-003` — Prise d'effet à la prochaine fenêtre validée ;
- `TT-INT-B03B04-004` — Bascule discriminante de seuil.

## 6. Artefacts

- `source/FT-INT-03_source.md` ;
- `detaille/FT-INT-03_detaille.md` ;
- `detaille/FT-INT-03_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite sur le cadrage validé. Gel interdit avant audit croisé et validation explicite.
