# TT-ACC-02-GEN-001 — Écriture d'un registre RW mono-registre

## Objectif
Vérifier qu'un registre unique explicitement déclaré `RW` accepte une écriture Modbus nominale autorisée.

## Préconditions
- FT-STR gelée ;
- cible mono-registre explicitement `RW` dans V1 et GEL-MAP-V1 ;
- valeur de test sûre et compatible avec le scénario ;
- état initial connu ou lisible.

## Étapes
1. Lire l'état initial utile.
2. Écrire la valeur de test sur le registre cible avec une fonction autorisée.
3. Vérifier l'absence d'exception Modbus.
4. Relire la cible lorsque sa sémantique V1 permet une image stable.
5. Restaurer la valeur initiale si nécessaire et sûr.

## Résultat attendu
- écriture acceptée ;
- prise en compte observable conformément à V1 ;
- aucun refus injustifié sur une cible RW.

## Exclusions
La validité métier et les effets fonctionnels ne sont pas jugés par ce test.
