---
mission_id: TR2-20261017-R4V-RTU-START-STOP-LATENCY
status: IN_PROGRESS
base_ref: main
base_sha: 1a9cf6a34bd36a695fb0410ead4a65f38b948a1f
initial_head: 8010d0101d70170a13731ca928833ce8c95774c3
result_sha: f8bf7ca80a7dad385dd84d00ffecea7e3dea125b
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

## Baseline instrumentée acquise — pas encore de candidat physique

Codef8bf7ca publié/relu, profile0 flashé/hashé et relu. START5/5/run8 etSTOP5/6 terminaux4/0, retry/collision27,1655réponsesCRCcorrectes, FRAMentière fce8e1f9330989e02f7139d10cb066383bca1c58447a00a9852a8ca6c4b4a682 exacte au modèle, journal54/root67/run8/epoch5/config3, historique/tailintacts. Capteurarrêté/debugdétaché ; M016restaurable, probe0terminal laissé en attendant suite.

ChronologieMCU(DWTà160MHz nominal), depuis captureobservée : STARTeffet7,678087s/terminal64,280557s ; STOPeffet56,628081s/terminal64,318544s. Le service occupe82,13/82,14% des spans R2 contre6,980s d'I/O par commande ; cause dominante de sur-service prouvée sur ce banc. Work815/service1624/gap1852µs souslimites inchangées. TempsmaîtreSTOPACK→HELD56,354535s ; domaines horloge et pointsd'origine distincts, pas de synchronisation SI prétendue.

Contre-test supplémentaire démontre cacheFRESH possible jusqu'àpublicationpériodique10ms après arrêt : échec avant/passe après publicationimmédiate sur changementrunning/error. Le candidat250 est précisé comme délai minimal après FIN du callbackFIFO, pour éviter qu'un callback coûteux rende immédiatement le suivant dû. UART/deadlines restent à chaque checkpoint. Ces deux corrections locales sont en revalidation, non encore flashées ; prochainephase aprèstests et commit. Pas de déplacement debarrière ni décisiond'arrêt industriel.
