# TT-ACC-05-GEN-017 — Écritures répétées même registre

## Objectif
Valider l'absence d'effet de bord pour le scénario `Écritures répétées même registre`.

## Préconditions
- FT-STR validée
- FT-ACC-01 et FT-ACC-02 validées
- cible `RW`
- snapshot bloc complet disponible

## Étapes
1. Capturer le snapshot complet du bloc avant écriture.
2. Écrire sur la cible.
3. Capturer le snapshot complet du bloc après écriture.
4. Comparer avant/après.
5. Vérifier que seules les adresses ciblées diffèrent.

## Résultat attendu
- aucune modification hors cible ;
- comportement déterministe.

## Criticité
P0
