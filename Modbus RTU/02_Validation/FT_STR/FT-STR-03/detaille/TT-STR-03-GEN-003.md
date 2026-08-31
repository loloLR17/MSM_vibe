# TT-STR-03-GEN-003 — Absence d'heuristique d'endianness

## Objectif

Vérifier que l'ordre MSW/LSW est imposé par la spécification et n'est jamais choisi dynamiquement selon la plausibilité de la valeur reconstruite.

## Vérifications

- aucune logique active ne compare MSW/LSW et LSW/MSW pour retenir la valeur « plausible » ;
- aucune convention alternative n'est appliquée à certains champs `uint32` ;
- aucune valeur métier n'est utilisée pour inférer l'ordre des mots ;
- les tests actifs considèrent V1 comme source de la convention.

## Critère d'acceptation

Le décodage est déterministe et identique pour tous les `uint32` : N = MSW, N+1 = LSW.

Toute absence de règle normative nécessaire est classée `NON DÉFINI / À ARBITRER` plutôt que compensée par une heuristique.
