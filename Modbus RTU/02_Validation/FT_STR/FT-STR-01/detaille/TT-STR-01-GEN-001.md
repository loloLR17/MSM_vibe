# TT-STR-01-GEN-001 — Bornes et longueur d'un bloc

## Objectif

Vérifier génériquement qu'un bloc Modbus possède exactement les bornes et la longueur définies par la référence normative.

## Entrées

- borne de début normative `A_debut` ;
- borne de fin normative `A_fin` ;
- représentation dérivée du bloc dans le mapping.

## Contrôles

1. Le premier registre couvert est `A_debut`.
2. Le dernier registre couvert est `A_fin`.
3. La longueur de la portée vaut `A_fin - A_debut + 1`.
4. Aucun registre situé hors de ces bornes n'est attribué implicitement au bloc.

## Résultat attendu

Les bornes et la longueur dérivées sont strictement identiques aux valeurs normatives.

## Échec

Toute différence de borne ou de longueur constitue une non-conformité structurelle et ne doit pas être corrigée en modifiant silencieusement la référence supérieure.

## Instanciation

Ce test est instancié par `TT-STR-01-B0-001` à `TT-STR-01-B7-001`.
