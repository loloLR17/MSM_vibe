# TT-STR-07-GEN-004 — Absence d’effet de bord du mode de lecture

## Objectif

Vérifier qu’un changement de découpage ou d’ordre de lecture ne modifie pas l’état exposé et n’introduit pas d’incohérence structurelle.

## Procédure

1. Choisir une cible accessible selon FT-STR-06.
2. Lire selon plusieurs schémas valides : A/A, A/B/A, lecture unitaire, sous-plage, plage multi-champs.
3. Pour les données statiques, comparer bit à bit les valeurs obtenues.
4. Pour les données dynamiques, comparer la cohérence et la sémantique attendue sans exiger l’identité temporelle entre requêtes.
5. Vérifier qu’aucune lecture n’entraîne de modification d’état non prévue.

## Critères

- aucune mutation induite par une lecture ;
- aucune dépendance implicite à un ordre de lecture ;
- aucune dépendance à un découpage particulier ;
- aucune fausse exigence d’identité bit à bit sur des données naturellement dynamiques.

## Limites

Le test ne démontre pas la performance du bus et ne transforme pas plusieurs requêtes successives en un snapshot atomique unique.