# Preuves M015-Q

La matrice `capture-matrix.json` distingue les trois témoins réussis, la fenêtre512 réussie et4096 rejetée. Pour chaque capture : image/Flash.hex, identité/reset/trigger/capture GDB avec invocation et retour, bruts result/traces/payload/preclose/official-words, contrôles et analyses/CSV. Les .bin.hex encodent exactement les octets originaux ; chaque .log.original.hex préserve les logs avant normalisation UTF8/LF/blancs finaux.

`artifacts.json`, `commands.json` et logs compilations/CTest/intégrations sont les validations effectives. `initial-failed-*`, `analysis-correction.json`, `test-correction.json`, `window4096-qualification-original.json` et `analysis-time-correction.json` conservent les erreurs/corrections hors ligne. Aucune relance physique n'en découle. Le résultat4096 reste refusé ; duration null car last_tick firmware0 sur échec, délai au statut114ms dans matrice.

`checkpoint-*-readback.json` documentent les publications successives ; `preservation.json` contrôle worktrees/stashes/archives et sessions. `MANIFEST.sha256` couvre tous les fichiers sauf lui-même. Originaux locaux et ELF/maps dans le dossier `/home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z`. Aucun défaut historique ni cause O/P réinterprétés.
