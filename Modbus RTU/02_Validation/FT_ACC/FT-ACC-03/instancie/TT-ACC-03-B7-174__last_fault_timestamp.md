# TT-ACC-03-B7-174 — last_fault_timestamp

## Référence mapping
- Bloc : 7
- Adresse : 7004 → 7005
- Type : uint32
- Registres : 2

## Test
1. Lire valeur initiale
2. Tenter écriture
3. Vérifier exception
4. Relire

## Résultat attendu
- exception Modbus
- aucune modification
