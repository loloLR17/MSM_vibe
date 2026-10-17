---
mission_id: TR2-20261017-R4V-RTU-START-STOP-LATENCY
status: IN_PROGRESS
base_ref: main
base_sha: 1a9cf6a34bd36a695fb0410ead4a65f38b948a1f
initial_head: 8010d0101d70170a13731ca928833ce8c95774c3
result_sha: null
created_at_utc: 2026-10-17T10:26:57.400490+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-V — checkpoint logiciel, qualification physique encore à faire

Contrat actuelR4V READY, autorisations explicites vérifiées ; worktree isolé à8010d01, ancienmain82169af/deuxstashes/62autresworktrees et6873fichiers protégés préservés. AGENTS/gouvernance/V2, journal intégral/roadmap, R0/B5/R1/R2/R4P/T/U et preuves relus. R4U déjà archivée, aucune archive réécrite.

[Plan et chronologie à instrumenter](../../05_Firmware/LIVE_R4V.md). CapturesR4U réanalysées sansrejeu,63s ne sont pas attribuées aveuglément à la FRAM ; hypothèse surcoût de service à mesurer.145tests complets PASS/cross-build,4suites sanitizersO2PASS6,50s. Pacing0par défaut et candidat250µs borné/configurable seulement arrêté ; UART/deadline inchangés. Contre-test de faute service prouve un défaut de retrait duB3retenu : correction viafaultR2 existant+publicationinvalide, sans reçuFRAMfabriqué. OrdreR2/formats/anti-rejeu/commits inchangés.

Instrumentation MCU passive, modèles métadonnées et profils de probe séparés préparés, pas encore flashés. Préflight10:18:02UTC M016Flash2Mio c5bc…f5b26 etFRAM95da…9af41 exacts, UID/SN/WHO7B/arrêt conformes. COM4FTDI identifiéT1000, maîtreC# compilé/selftest sansémissionPASS. Aucun nouvelSTART/writeFRAM/flash/ModbusR4V pour le moment.

Suite : checkpointGitHub, baselineinstrumentée (5/5STARTrun8,5/6STOP), auditFRAM/modèle ; puis candidat250 surétat exact neuf (5/7STARTrun9,5/8STOPbudgetrefus sansmutation,5/9STOP), chronologie comparative/budgets inchangés. Si erreur ouindétermination, arrêt sûr/pasreplay. RestaurationM016/relectureentièreFlash+FRAM, rapports/proofs et relectureGitHub finale. Pas de qualification industrielle/R4global/R6. Journal actualisé pour état préparé, roadmapfinale aprèsrésultats.
