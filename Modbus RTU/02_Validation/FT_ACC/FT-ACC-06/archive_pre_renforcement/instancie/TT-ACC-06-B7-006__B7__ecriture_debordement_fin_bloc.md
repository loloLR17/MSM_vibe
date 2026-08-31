# TT-ACC-06-B7-006 — Bloc 7 — ecriture_debordement_fin_bloc

## Objectif
Vérifier qu’une requête ecriture invalide est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Bloc de référence : 7
- Plage valide du bloc : 7000..7015
- Adresse requête : 7015
- Longueur requête : 2

## Justification du scénario
Écriture à partir de 7015 sur 2 registres, débordant hors du bloc 7

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- accès Modbus opérationnel
- état mémoire stable avant tentative

## Étapes
1. Lire la zone de référence utile avant tentative si l’opération est une écriture.
2. Exécuter la requête ecriture invalide à l’adresse `7015` sur `2` registre(s).
3. Contrôler la présence d’une exception Modbus explicite.
4. Si l’opération est une écriture, relire la zone de référence.
5. Vérifier l’absence de modification mémoire.

## Résultat attendu
- exception Modbus explicite obligatoire ;
- aucune exécution partielle ;
- aucune modification mémoire.

## Critères d’acceptation
- refus clair ;
- comportement déterministe ;
- absence d’effet mémoire.

## Traces à conserver
- trame de requête ;
- exception retournée ;
- snapshot mémoire avant/après si écriture ;
- verdict.

## Criticité
P0
