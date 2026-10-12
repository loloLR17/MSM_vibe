# Preuves M015-R — hors ligne

`analysis.py <racine_du_worktree>` reconstitue le budget depuis les bruts Q versionnés, sans réseau ni matériel. `service-intervals.csv` fournit les19intervalles DWT exacts ; `summary.json` distingue durées, estimations ODR/bilan et horloges. `status-timeline.json` conserve les niveaux et flags associés. `offline-checks.json` contrôle les identités binaires et la somme du budget ; `initial-preservation.json` relève Git/worktrees/stashes.

Les sources Q sont référencées et hashées dans summary.json ; les erreurs/refus/cleanup Q restent dans leur corpus immuable. Aucun essai nouveau, test firmware ni build répété. MANIFEST.sha256 couvre les fichiers de ce dossier à l'exclusion de lui-même. La publication/relecture sera consignée séparément ; pas de qualification4096 ou causeCPU démontrée.
