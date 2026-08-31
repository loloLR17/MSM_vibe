# TT-ACC-06-B4-011 — 4055→4056 réservé vers RW

- **Générique** : TT-ACC-06-GEN-002
- **Plage FC16** : 4055-4056
- **Accès** : `reserved_4C` (réservé, mot final) → `campaign_context_id` (RW, mot initial)

## Attendu
Exception Modbus appropriée. `campaign_context_id` reste inchangé. `config_state` reste inchangé. Aucun effet interne ni exécution partielle. Répétition déterministe.
