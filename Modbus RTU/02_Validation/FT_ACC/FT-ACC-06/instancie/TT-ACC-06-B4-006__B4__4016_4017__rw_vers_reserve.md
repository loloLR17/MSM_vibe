# TT-ACC-06-B4-006 — 4016→4017 RW vers réservé

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4016-4017
- **Accès** : `sampling_frequency_hz` (RW) → `reserved_4B_0` (réservé)

## Attendu
Exception Modbus appropriée. `sampling_frequency_hz` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
