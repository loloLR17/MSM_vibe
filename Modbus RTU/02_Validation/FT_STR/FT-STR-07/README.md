# FT-STR-07 — Stabilité d’image et cohérence temporelle

## Objet

FT-STR-07 valide la stabilité des données réellement statiques et la cohérence temporelle des réponses Modbus multi-registres.

## Référentiel

Ordre de vérité :
1. spécification Modbus RTU V1 (`bloc0.md` à `bloc7.md`, `charte_typage.md`) ;
2. GEL-MAP-V1, mapping dérivé ;
3. `source/FT-STR-07.md` ;
4. `detaille/TT-STR-07-GEN-XXX.md` ;
5. `instancie/`.

## Doctrine

FT-STR-07 distingue strictement :
- la **stabilité entre requêtes**, exigible uniquement pour une donnée normativement ou contextuellement stable ;
- la **cohérence interne d’une réponse multi-registres**, exigée même si les données évoluent entre deux requêtes.

Une donnée dynamique (`current_time`, `uptime_s`, compteurs, états, mesures, etc.) n’est pas non conforme parce qu’elle varie normalement entre deux lectures.

Une réponse multi-registres doit en revanche représenter un **même instant logique**, conformément à `charte_typage.md`.

## Validation active

- `GEN-001` : répétabilité des données réellement statiques ;
- `GEN-002` : atomicité des `uint32` en évolution contrôlée ;
- `GEN-003` : cohérence intra-réponse des zones dynamiques ;
- `GEN-004` : absence d’effet de bord lié au mode de lecture.

Les anciens tests sont archivés et ne participent plus à la validation active.

Aucun résultat terrain n’est déduit du mapping seul.