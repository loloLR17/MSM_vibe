# TT-STR-01-GLOBAL-001 — Cohérence globale inter-blocs

## Objectif
Valider la cohérence structurelle globale de l’ensemble des blocs Modbus couverts par le mapping unifié.

## Référence couverture
- Nombre de blocs couverts : `8`
- Première adresse observée : `0`
- Dernière adresse observée : `7015`

## Préconditions
- Toutes les vues de couverture par bloc sont disponibles
- Mapping unifié brut validé
- Les blocs 0 à 7 sont figés au niveau spécification

## Contrôles à effectuer
- ordre croissant des blocs selon leurs plages d’adresses ;
- absence de chevauchement inter-bloc ;
- identification explicite des éventuels trous inter-blocs.

## Étapes
1. Lister les portées d’adresses de tous les blocs.
2. Trier les blocs par adresse de début.
3. Vérifier que deux blocs successifs ne se chevauchent pas.
4. Identifier les éventuels intervalles non couverts entre deux blocs successifs.
5. Vérifier que la structure globale reste déterministe et non ambiguë.

## Résultat attendu
- structure inter-blocs cohérente ;
- aucun chevauchement inter-bloc observé.
- trou inter-bloc entre bloc 0 et bloc 1 : `21..999`
- trou inter-bloc entre bloc 1 et bloc 2 : `1020..1999`
- trou inter-bloc entre bloc 2 et bloc 3 : `2016..2999`
- trou inter-bloc entre bloc 3 et bloc 4 : `3048..3999`
- trou inter-bloc entre bloc 4 et bloc 5 : `4176..4999`
- trou inter-bloc entre bloc 5 et bloc 6 : `5020..5999`
- trou inter-bloc entre bloc 6 et bloc 7 : `6064..6999`

## Critères d’acceptation
- aucune ambiguïté de portée entre blocs ;
- aucun chevauchement inter-bloc ;
- tout trou inter-bloc est explicitement connu et acceptable au regard de la spécification.

## Classification
- Famille : `FT-STR-01`
- Sous-famille : `Conformité structurelle`
- Niveau : `instancié`
