---
mission_id: TR2-20261016-R4D-START-TIMING-DIAGNOSTIC
status: READY
base_ref: main
base_sha: 71ee541d6f534d993d439475c32bec5b03d7856c
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-D — Diagnostic des temps de START, observation et STOP

## Contexte
R4-M a réussi l'isolation de l'ancien verrou et le renouvellement authentique epoch1 vers epoch2. Un nouveau START epoch2/run2 a cependant dépassé le budget de travail (1241 microsecondes pour une limite de 1000), puis laissé un journal STARTED indéterminé. La cause précise du dépassement n'est pas démontrée. La Flash initiale a été restaurée ; l'état FRAM et les traces sont préservés. Cette mission est une investigation ciblée, non une répétition de R4-M.

## Instructions
Consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, l'état courant, la roadmap, R0 et les documents LIVE_R1 à LIVE_R4M, le rapport R4-M et ses preuves. Vérifier HEAD et archives, archiver le contrat et le rapport précédents sans perte.

1. Examiner les traces GDB/DWT et le code des séquences START, OBSERVE, STOP, leurs callbacks, accès SPI, CRC, checkpoints et reprises. Décomposer les durées et distinguer hypothèse de cause et preuve.
2. Ajouter une instrumentation proportionnée à faible perturbation, avec mesure de son overhead, pour localiser la portion responsable du dépassement. Caractériser également les chemins d'erreur et les limites HAL SPI, sans présenter les mesures nominales comme WCET.
3. Corriger seulement un défaut démontré, par coopération ou découpage minimal. Conserver les limites work 1000us, service 2500us et gap 5ms ; préserver causalité, journal, anti-rejeu, STOP sûr et V1. Si un changement structurant s'avère nécessaire, exposer le choix sans modifier silencieusement R0.
4. Tester les scénarios START/OBSERVE/STOP, faults, coupures et recovery sur hôte, ASan/UBSan et cross-build ARM ; comparer avant/après et fournir des preuves reproductibles.
5. Sur T1000, privilégier des diagnostics lecture seule et des exercices volatils du capteur sans admission de commande R2 ni écriture FRAM LIVE. Avant toute intervention, vérifier identité, sauvegardes, SHA de FRAM et arrêt capteur. Les probes STM32 sont autorisés si les garde-fous sont réunis ; restaurer et vérifier la Flash initiale ensuite. Toute écriture dans la partition FRAM LIVE est exclue de cette mission, ainsi que tout nouvel effacement ou reprovisionnement du journal.
6. Préserver l'état STARTED epoch2/run2 et les données historiques, ne jamais inventer de reçu ou de PASS physique. Aucun essai SD/HP/RTU électrique.

## Acceptation
Diagnostic étayé du coût START/OBSERVE/STOP ou limite de preuve explicitée ; instrumentation et overhead ; correction minimale si justifiée, tests et non-régression ; aucun changement de norme ni relâchement de budget. Rapport sur les risques résiduels et proposition de reprise physique distincte si nécessaire.

## Autorisations et publication
allow_flash=true et allow_debug=true pour les probes sous garde-fous AGENTS.md, allow_commit=true et allow_push=true. L'utilisateur n'effectue que les gestes physiques éventuellement nécessaires. Codex publie commits, preuves, mise à jour factuelle état/roadmap et DERNIER_RAPPORT.md sur main puis vérifie à distance. ChatGPT contrôlera indépendamment.
