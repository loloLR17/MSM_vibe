# TT-ACC-06-B4-009 — 4039→4040 réservé vers RW

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4039-4040
- **Accès** : `reserved_4B` (réservé, mot final) → `supervision_enable_mask` (RW)

## Attendu
Exception Modbus appropriée. `supervision_enable_mask` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
