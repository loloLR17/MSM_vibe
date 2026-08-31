# TT-STR-07-GLOBAL-001 — Cohérence globale sous alternance de lectures

## Objectif
Valider l’absence d’effet de bord structurel lors d’alternances de lectures entre champs et blocs différents en état stable.

## Préconditions
- capteur en état stable
- aucune acquisition active
- accès Modbus opérationnel
- FT-STR-03 validée
- FT-STR-05 validée

## Étapes
1. Sélectionner au moins un champ stable dans plusieurs blocs.
2. Exécuter une séquence alternée de type A / B / A / bloc / A / B / bloc.
3. Répéter la séquence 10 fois.
4. Comparer toutes les lectures de chaque cible entre elles.
5. Rechercher toute variation locale ou incohérence `uint32`.

## Résultat attendu
- aucune variation induite par l’ordre de lecture ;
- aucune incohérence inter-blocs ;
- aucune instabilité observable.

## Critères d’acceptation
- identité de toutes les lectures d’une même cible ;
- 0 effet de bord ;
- 0 mismatch multi-registres.
