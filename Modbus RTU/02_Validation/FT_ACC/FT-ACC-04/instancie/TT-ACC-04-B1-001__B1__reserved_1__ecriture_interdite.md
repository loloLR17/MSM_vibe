# TT-ACC-04-B1-001 — B1 — reserved_1

## Référence mapping
- Adresse : 1017
- Longueur : 1 registre
- Accès : réservé / écriture interdite
- Générique : TT-ACC-04-GEN-001

## Test
1. Capturer l’état de référence.
2. Tenter une écriture sur 1017.
3. Exiger une exception Modbus standard appropriée.
4. Relire et vérifier l’absence de toute modification ou effet interne.

## Résultat attendu
Rejet total, aucune modification, aucune exécution partielle, comportement déterministe.
