# TT-ACC-06-B4-008 — 4027→4028 RW vers réservé

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4027-4028
- **Accès** : `storage_limit_mb` (RW, mot final) → `reserved_4B` (réservé, mot initial)

## Attendu
Exception Modbus appropriée. `storage_limit_mb` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
