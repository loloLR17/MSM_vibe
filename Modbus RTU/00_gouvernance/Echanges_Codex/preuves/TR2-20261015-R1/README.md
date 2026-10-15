# Preuves R1 — configuration LIVE / FRAM

Code évalué `f4697febfd5cea267a3c4fa21a1f1e0071845665`, base mission
`b6bae88001a46527ccab5249fce0c2527f2672d2`. Aucun accès matériel.

- `initial-state.json` : état Git de départ, worktrees/stashes et 2576 empreintes protégées.
- `verification.json` : résultats vérifiés et limites ; `evaluated-firmware-sha256.json` : empreintes de tous les fichiers suivis du firmware évalué.
- `r1-live-validation.log` : premier cycle complet, 129 PASS hôte puis erreur ARM type-limits conservée. Ce log seul ne prouve pas le succès final.
- `r1-live-host-final-build.log`, `r1-live-host-final-tests.log` : build et 129 PASS après corrections finales.
- `r1-live-arm-final.log`, `build-profile.txt`, `artifact-sha256.txt`, `elf-inspection.txt` : cross-build final et contrôles de l'image non flashée.
- `r1-live-sanitize-{configure,build,tests}.log` : CMake avec `-fsanitize=address,undefined -fno-omit-frame-pointer`, build du test R1, exécution sans erreur rapportée.
- `resources/commands.json`, sources des probes, `sizes.txt`, `*.su` : tailles ARM comparées au header de base et cadres statiques GCC -O0. Ni test exécuté sur ARM, ni borne complète de pile.

Le test commité `05_Firmware/tests/unit/test_live_configuration.c` est le protocole
reproductible des modèles mémoire/coupures. 407 budgets par opération, pas chaque
octet de payload. Les préconditions de propriété n'ont valeur que dans ce modèle.
Les empreintes vérifient le contenu conservé, pas une qualification électrique.
Les paths absolus dans les logs désignent ce worktree/SDK, jamais une instruction
de déplacer le dépôt actif.
