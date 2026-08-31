# TT-ACC-06-B4-012 — 4095→4096 RW vers réservé

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4095-4096
- **Accès** : `sea_state_code` (RW) → `reserved_4D` (réservé, mot initial)

## Attendu
Exception Modbus appropriée. `sea_state_code` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
