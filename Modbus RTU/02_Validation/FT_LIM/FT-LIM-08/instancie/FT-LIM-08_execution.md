# FT-LIM-08 — Procédure d’exécution

1. Lire les Blocs 0 et 1 sans écriture directe.
2. Vérifier le masque réservé de `device_capabilities`.
3. Capturer un snapshot Bloc 0 de référence puis le comparer après évolution normale d’état/acquisition.
4. Si un redémarrage autorisé est observé, comparer `device_id` avant/après et relever `last_reset_cause`.
5. Si plusieurs capteurs sont disponibles, comparer leurs `device_id`.
6. Vérifier les domaines de `last_reset_cause`, `storage_status` et `acquisition_state`.
7. Vérifier la monotonie de `uptime_s` sur une période sans reset.
8. En cas de reset observé, ne pas qualifier la baisse d’uptime comme défaut ; contrôler seulement les invariants explicitement normatifs.
9. Tester la persistance défaut/avertissement uniquement lorsqu’une condition et son indication sont identifiables sans extrapolation.
10. Pour les champs sans domaine normatif, consigner la valeur et classer TRACE_ONLY/NOT_DEFINED.

Les règles structurelles et d’accès restent déléguées à FT-STR et FT-ACC.
