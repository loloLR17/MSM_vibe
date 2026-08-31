# TT-ACC-02-GEN-005 — Répétition d'une écriture nominale autorisée

## Objectif
Vérifier qu'une cible RW stable accepte de manière déterministe la répétition d'une écriture nominale non déclenchante.

## Préconditions
- cible RW sans sémantique de déclenchement ;
- valeur de test sûre ;
- état initial connu.

## Étapes
1. Écrire une première fois la valeur de test.
2. Contrôler l'acceptation.
3. Répéter la même écriture.
4. Contrôler l'acceptation et l'état attendu.
5. Restaurer si nécessaire.

## Résultat attendu
Aucun refus d'accès injustifié n'apparaît du seul fait de la répétition.

## Exclusion
L'idempotence des commandes du Bloc 5 relève de leur logique fonctionnelle, pas de ce générique.
