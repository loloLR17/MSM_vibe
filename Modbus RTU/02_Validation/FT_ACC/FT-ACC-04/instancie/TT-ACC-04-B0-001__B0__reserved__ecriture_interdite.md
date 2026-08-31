# TT-ACC-04-B0-001 — B0 — reserved

## Référence mapping
- Adresse : 20
- Longueur : 1 registre
- Accès : réservé / écriture interdite
- Générique : TT-ACC-04-GEN-001

## Test
1. Capturer l’état de référence.
2. Tenter une écriture sur 20.
3. Exiger une exception Modbus standard appropriée.
4. Relire et vérifier l’absence de toute modification ou effet interne.

## Résultat attendu
Rejet total, aucune modification, aucune exécution partielle, comportement déterministe.
