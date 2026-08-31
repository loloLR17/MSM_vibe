# TT-STR-05-GEN-001 — Identification exhaustive des zones réservées

## Objectif

Vérifier que toute zone déclarée réservée par V1 est représentée exactement une fois dans le mapping dérivé et qu'aucune zone réservée héritée ou inventée n'est active.

## Préconditions

- V1 disponible ;
- GEL-MAP-V1 disponible et gelé.

## Procédure

1. Recenser dans V1 les champs/plages explicitement réservés.
2. Rechercher leur représentation dans GEL-MAP-V1.
3. Comparer nom, bloc, plage, taille et type documentaire.
4. Rechercher les doublons et anciens alias.

## Critères

- couverture exhaustive ;
- une représentation logique par zone normative ;
- aucun doublon de plage ;
- aucun alias historique actif ;
- aucune zone non réservée reclassée comme réservée.

## Hors périmètre

La valeur réellement lue et les droits d'écriture ne sont pas démontrés par ce test documentaire.
