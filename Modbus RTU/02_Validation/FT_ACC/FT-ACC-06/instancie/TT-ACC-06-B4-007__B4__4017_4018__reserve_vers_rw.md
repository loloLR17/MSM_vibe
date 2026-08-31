# TT-ACC-06-B4-007 — 4017→4018 réservé vers RW

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4017-4018
- **Accès** : `reserved_4B_0` (réservé) → `axes_enable_mask` (RW)

## Attendu
Exception Modbus appropriée. `axes_enable_mask` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
