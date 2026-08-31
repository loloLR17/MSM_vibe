# TT-ACC-06-B5-002 — Bloc 5 — ecriture_hors_plage_basse

## Objectif
Vérifier qu’une requête ecriture invalide est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Bloc de référence : 5
- Plage valide du bloc : 5000..5019
- Adresse requête : 4999
- Longueur requête : 1

## Justification du scénario
Adresse 4999 avant début du bloc 5 (5000..5019)

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- accès Modbus opérationnel
- état mémoire stable avant tentative

## Étapes
1. Lire la zone de référence utile avant tentative si l’opération est une écriture.
2. Exécuter la requête ecriture invalide à l’adresse `4999` sur `1` registre(s).
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
