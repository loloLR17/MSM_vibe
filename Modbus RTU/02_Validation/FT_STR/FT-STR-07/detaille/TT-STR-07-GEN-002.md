# TT-STR-07-GEN-002 — Atomicité des `uint32` en évolution contrôlée

## Objectif

Démontrer qu’un `uint32` est toujours lu comme une valeur atomique cohérente, sans mélange MSW/LSW provenant de deux instants différents.

## Procédure

1. Sélectionner un `uint32` capable d’évoluer de manière contrôlée ou observable.
2. Organiser des lectures répétées couvrant simultanément ses deux registres.
3. Faire évoluer la valeur de façon maîtrisée ou exploiter une évolution naturelle suffisamment observable.
4. Reconstruire chaque valeur avec la règle MSW puis LSW.
5. Rechercher toute valeur impossible ou incohérente avec la séquence attendue.

## Critères

- aucune valeur déchirée ;
- aucune combinaison MSW ancien / LSW nouveau ou inverse ;
- l’ordre MSW/LSW reste conforme à FT-STR-03.

## Preuve insuffisante

Répéter 20 fois la lecture d’un `uint32` immobile ne démontre pas son atomicité. Si aucune évolution contrôlée ou observable n’est disponible, le résultat doit rester **NON DÉMONTRABLE PAR L’INTERFACE SEULE / INSTRUMENTATION REQUISE**.