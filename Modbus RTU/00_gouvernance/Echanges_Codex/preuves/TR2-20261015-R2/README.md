# Preuves R2 — moteur epoch / LIVE

Base091f5e8, code évalué59d3757e88c6fadbc36a2de1cb17c6b67a15186a.
Aucune opération matérielle. Production V1 conservée ; backend non raccordé.

- `initial-state.json` : Git, worktrees/stashes et2382 empreintes protégées.
- `verification.json` : synthèse chiffrée, périmètre et limites.
- `evaluated-firmware-sha256.json` : empreintes de tous les fichiers firmware suivis au code évalué.
- `r2-full-validation.log` : premier cycle complet hôte/ARM ; `r2-host-final-{build,tests}.log` et `r2-host-engine-last-test.log` : validation finale après correction du flag STOPPING.
- `r2-targeted*.log` : intermédiaires ; `r2-last-retention-failure.log` : défaut démontré puis corrigé de last. Ils ne représentent pas tous le code final.
- `r2-sanitize-{configure,build,tests}.log` : cycle final ASan/UBSan, aucun défaut rapporté.
- `r2-arm-final.log`, `build-profile.txt`, `artifact-sha256.txt`, `cumulative-elf-inspection.txt` : compilation V1 sans lien du nouveau moteur, binaire non flashé.
- `resources/commands.json`, probes C, `.su`, `sizes.txt` : ABI ARM/pile statique ; `arm_link_smoke.c` et `arm-link-smoke.log` : lien effectif des nouveaux modules dans un ELF sans board/startup/map production, **NE PAS FLASHER**. Pas d'exécution ARM.

Tests reproductibles : `05_Firmware/tests/unit/test_live_command_engine.c`, qui
réutilise les helpers de fixture R1 sans exécuter son ancien main ; R1 garde son
propre test. `live_context_vectors.h` copie384 attendus du CSV R0 inchangé, aucun
résultat calculé par l'implémentation testée.1561 budgets à10 étapes, payload par
pas4096 et octets de headers/publications ; pas un modèle électrique exhaustif.
Les fixtures de plafond et ABSENCE_PROVEN sont des autorités simulées explicites,
jamais des faits physiques déduits d'un reboot. Les logs conservent leurs chemins
de worktree/SDK, sans instruction de déplacer la racine active.

`remote-readback.json` atteste la relecture des45 fichiers au commit
b78e7105a9a7da7ed6ba6fe3d57b0bfbc6cf0204. Il n'auto-atteste pas le commit qui
ajoute le reçu ; la publication finale est vérifiée ensuite, SHA dans la réponse.
`SHA256SUMS.json` couvre les autres preuves, sans s'auto-empreinter.
