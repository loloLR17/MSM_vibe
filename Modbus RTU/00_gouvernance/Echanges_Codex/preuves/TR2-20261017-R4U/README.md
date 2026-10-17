# Preuves R4-U

Sources/fichiers évalués au commit806d967b912080872a44f51c1f75887448c62ea3, image04.
Les images01..03 et leurs échecs restent distincts. Aucun test après restauration.

- `physical-final-summary.json` : métriques physiques/oracle/ADU/monotonie.
- `hardware/live05-master.jsonl.zlib` : toutes requêtes/réponses physiques (un seul maîtreCOM4), timestampsUTC/elapsedms/CRC.
- `hardware/live05-phase.log` : GDB ready avantSTART et terminal aprèsSTOP, aucune interruption dans l'acquisition.
- `hardware/restoration-summary.json`, `restored-sensor-stopped-readonly.log` : M016 complet/FRAMconservée/WHO7B/arrêt.
- `hardware/*before.bin.zlib`, `live05-fram.bin.zlib`, `fram-after-restoration.bin.zlib` : snapshots complets sans rollback.
- `images/image04.{bin,elf}.zlib` et profils : artefact exact/provenance.
- `model-final.bin.zlib` : modèleRAM des métadonnées, pas une acquisition simulée présentée comme physique.
- `initial-state.json.zlib`/`preservation-final.json` : conservation des6626fichiers et autres worktrees/stashes.
- `full-validation-02.log` :143tests existants PASS/erreur oracle nouveau144, puis `host-readonly-fix-test.log` PASS ; ne pas masquer ce premier échec.
- `readonly-failure-analysis.json`/brutsro04 : bitshift réel de la variante split, pas de données corrigées utilisées par le firmware.

`BUNDLE.json` fournit empreintes brutes et stockées des gros artefacts ; SHA256SUMS couvre tous les fichiers de preuve. Décompression sans modifier la source : `raw = zlib.decompress(Path("capture.zlib").read_bytes())`. Les logs brutsWindows gardent CRLF/encodage d'origine. NativeC# utilise la bibliothèque SerialPort Windows et CRC connu ; aucune ExecutionPolicy modifiée.

Les scriptsGDB contiennent des tokens de déclenchement matériel : ce sont des preuves historiques, pas des autorisations de réexécution. Le snapshot95da…9af41/epoch5/run7 remplace371a ; ne jamais rejouer les clés consommées5/1..4 ni la retraiteR4T.
