# TT-ACC-03-B0-002 — firmware_version_major

## Référence mapping
- Bloc : 0
- Adresse : 3 → 3
- Type : uint16
- Registres : 1

## Test
1. Lire valeur initiale
2. Tenter écriture
3. Vérifier exception
4. Relire

## Résultat attendu
- exception Modbus
- aucune modification
