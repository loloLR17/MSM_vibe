# Preuves de reprise M015-T

`final-comparison.json` et `independent-audit.py` reconstruisent les captures4/8 depuis les bruts : XYZ/TS, parité/counters/ratio, statuts, snapshots avantHALclose, horloges etcleanup. `*-words.csv` et `*-service.csv` exportent les données et intervalles DWT. Une seule capture8 nouvelle, pas de répétition4/S/2/512.

**Ancienne analyse locale conservée** : `timing-comparison.json` et `service-intervals.csv` importés deT contiennent un comptage erroné des attentes (post-statuts inclus). Ne pas utiliser leur marge. `m015-t-timing-audit.json` explique75→40 ; `final-comparison.json` et lesCSVparcapture donnent les valeurs corrigées. Les bruts ne changent pas. Le préflightToriginal GDB7 pour mauvaiseadresseSPI3 est conservé ; script corrigé46002000 avant unique trigger. Aucun défautcible masqué.

Les .bin.hex encodent exactement les octets ; .log.original.hex préservent les originaux avant normalisationUTF8/LF. `artifacts.json` identifieBIN/ELF/map, `resume-build-commands.json` donne l'unique nouveau build8 ;ancienslogsT/S ne signifient pas testsrelancés. `preservation.json` relève lesworktrees/stashes ethashesoriginaux. `resume-checkpoint-readback.json` documente publication4avant8. MANIFEST.sha256 couvre les fichiers sauf lui-même. Unequalificationsurfenêtre n'estpas qualificationindustrielle/perteglobalezéro.
