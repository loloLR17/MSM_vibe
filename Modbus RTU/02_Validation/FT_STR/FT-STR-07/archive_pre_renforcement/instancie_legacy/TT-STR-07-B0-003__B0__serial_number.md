# TT-STR-07-B0-003 — Bloc 0 — serial_number

## Objectif
Valider la stabilité bit à bit du champ ASCII fixe `serial_number`.

## Référence mapping
- Adresse début : `8`
- Adresse fin : `15`
- Taille : `8` registre(s)

## Préconditions
- capteur en état stable
- accès Modbus opérationnel

## Étapes
1. Lire le champ ASCII 20 fois consécutivement.
2. Comparer les séquences brutes registre par registre.
3. Vérifier que la zone utile ne varie pas.
4. Vérifier que le padding `0x00` ne varie pas.
5. Comparer lecture 1 et lecture 20.

## Résultat attendu
- champ ASCII identique bit à bit sur toute la séquence.

## Critères d’acceptation
- 0 variation sur zone utile ;
- 0 variation sur padding ;
- identité bit à bit lecture 1 / lecture 20.
