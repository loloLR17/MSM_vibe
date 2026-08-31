# TT-ACC-06-B4-010 — 4046→4047 RW vers réservé

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4046-4047
- **Accès** : `alarm_hold_time_ms` (RW) → `reserved_4C` (réservé, mot initial)

## Attendu
Exception Modbus appropriée. `alarm_hold_time_ms` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
