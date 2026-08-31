# TT-STR-07-GEN-003 — Cohérence intra-réponse des zones dynamiques

## Objectif

Démontrer que tous les registres d’une même réponse Modbus multi-registres représentent un même instant logique, même lorsque les données évoluent entre deux requêtes.

## Procédure

1. Sélectionner une plage multi-registres contenant des données susceptibles d’évoluer.
2. Identifier un oracle de cohérence : compteur de séquence, timestamp associé, relation normative, instrumentation firmware ou mécanisme équivalent.
3. Effectuer des lectures répétées de la plage en respectant FT-STR-06.
4. Vérifier, réponse par réponse, que les champs appartiennent au même instant logique.
5. Rechercher tout mélange ancien/nouveau à l’intérieur d’une réponse.

## Cibles prioritaires

Conformément à `charte_typage.md` : Blocs 1, 2, 3, 6 et 7.

La règle normative reste applicable à toute réponse multi-registres, y compris les autres blocs.

## Critères

- aucune réponse temporellement déchirée ;
- aucune incohérence inter-champs démontrable ;
- aucune conclusion fondée uniquement sur la plausibilité.

## Absence d’oracle

Si la cohérence ne peut pas être prouvée depuis l’interface exposée, classer : **NON DÉMONTRABLE PAR L’INTERFACE SEULE / INSTRUMENTATION REQUISE**.