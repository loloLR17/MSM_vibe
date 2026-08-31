# FT-RBT-04 — Exigences source normalisées

## 1. Références normatives

- `01_Specification_source/charte_typage.md` V1 ;
- spécifications V1 des blocs concernés ;
- `02_Validation/FT_STR/FT-STR-07/` ;
- `02_Validation/FT_SEQ/` ;
- plan maître de validation Modbus TR2.

## 2. Exigences retenues

### RBT04-R01 — Cohérence interne pendant évolution
- Classification : `CONDITIONAL` dans FT-RBT-04.
- Oracle propriétaire : FT-STR-07.
- Exigence composée : lorsqu'une réponse multi-registres est obtenue pendant une évolution contrôlée des données, cette réponse doit rester cohérente avec un même instant logique selon l'oracle FT-STR.
- Test : `TT-RBT-GEN-020`.
- Condition : disposer d'une transition contrôlable ou identifiable et d'observables permettant d'évaluer la cohérence sans exiger un état intermédiaire particulier.

### RBT04-R02 — Cohérence multi-registres nominale
- Classification : `DELEGATED`.
- Propriétaire : FT-STR-07.
- Justification : la règle existe indépendamment de toute perturbation FT-RBT.

### RBT04-R03 — Valeurs différentes entre lectures dynamiques
- Classification : `TRACE_ONLY`.
- Règle : une donnée dynamique peut évoluer entre deux requêtes ; cette variation n'est pas une anomalie en soi.
- Justification : protège contre un faux oracle de stabilité inter-requêtes.

### RBT04-R04 — Observation obligatoire d'un état intermédiaire
- Classification : `NOT_DEFINED`.
- Justification : la V1 ne garantit pas qu'un polling donné observera chaque état transitoire.

### RBT04-R05 — Durée de transition
- Classification : `NOT_DEFINED`.
- Justification : aucune durée maximale/minimale générique de transition n'est définie.

### RBT04-R06 — Cadence de polling / sollicitations rapprochées
- Classification : `NOT_DEFINED`.
- Justification : aucun intervalle minimal, maximal ou recommandé n'est normatif en V1.

### RBT04-R07 — Nombre minimal de lectures ou transitions à capturer
- Classification : `NOT_DEFINED`.
- Justification : aucun seuil quantitatif n'est spécifié.

## 3. Doctrine de verdict

Le verdict FT-RBT-04 porte sur chaque réponse obtenue pendant la fenêtre dynamique, pas sur la chronologie exacte des valeurs entre réponses.

Une réponse incohérente en elle-même constitue un échec selon l'oracle FT-STR. Le fait de ne pas avoir capturé un état intermédiaire ne constitue pas un échec V1.

## 4. Anti-fabrication

Ne pas :
- définir artificiellement une fréquence de polling ;
- exiger une transition observable dans un délai arbitraire ;
- exiger la capture de tous les états ;
- comparer deux lectures dynamiques comme si elles devaient être identiques ;
- créer un nouvel oracle métier sur la seule base de la proximité temporelle des requêtes.
