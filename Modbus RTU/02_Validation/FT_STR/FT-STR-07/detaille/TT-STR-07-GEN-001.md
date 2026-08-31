# TT-STR-07-GEN-001 — Répétabilité des données réellement statiques

## Objectif

Vérifier qu’une donnée dont la stabilité est explicitement garantie reste identique tant que son contexte de validité ne change pas.

## Procédure

1. Justifier pourquoi la cible est statique : règle V1, nature d’identité, zone réservée, ou scénario maîtrisé.
2. Effectuer une série de lectures valides de la même cible.
3. Comparer les valeurs bit à bit.
4. Journaliser tout événement susceptible d’expliquer une variation.

## Critères

- aucune variation inexpliquée ;
- aucune cible dynamique n’est reclassée arbitrairement comme statique ;
- une variation expliquée par un changement de contexte ne constitue pas un échec FT-STR-07.

## Exemples de cibles

- informations d’identité du Bloc 0 ;
- registres réservés soumis à une valeur structurelle fixe ;
- configuration explicitement figée pendant le scénario.

## Interdiction

`current_time`, `uptime_s`, compteurs, mesures ou états dynamiques ne doivent pas être déclarés non conformes parce qu’ils évoluent normalement entre deux requêtes.