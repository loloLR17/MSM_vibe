---
mission_id: TR2-20261011-QUALIF-RS485-RX-006
status: READY
base_ref: main
base_sha: 9753b3d6b46243f0472a0d33c5fa82b509dcea71
created_at_utc: 2026-10-11T10:56:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2 — Préparation autonome du premier essai physique de réception RS-485 (mission 006)

## Objectif et contexte confirmé

Préparer une procédure courte, reproductible et instrumentée permettant à l'utilisateur de démontrer ou d'infirmer la réception d'une trame RTU sur le STM32U575 réel, sans confondre détection du ST-LINK, présence de COM, réception d'octets, CRC valide et réponse Modbus applicative. Cette mission est **logicielle et préparatoire** : aucun flash, debug sur cible ou transmission physique n'est autorisé à Codex dans ce contrat. L'utilisateur fera les manipulations matérielles après relecture de la préparation.

Banc réellement disponible et observé le 11 octobre :
- T1000 Windows/WSL Debian, clone canonique `/home/lolo/dev/msm/projects/MSM_vibe`, SDK STM32CubeU5 v1.9.0 ; ST-LINK V3 USB 0483:374e attaché à WSL via usbipd ; OpenOCD 0.12.0 détecte `STLINK V3J15M7`, cible `stm32u5x.cpu` Cortex-M33 r0p4, tension 3,288 V, avec `openocd -f interface/stlink.cfg -f target/stm32u5x.cfg -c "init; targets; shutdown"`. Ceci valide l'accès SWD, **pas** le firmware actuellement programmé ni la liaison RS-485.
- Ancien HP Windows : adaptateur FT232R USB–RS485 identifié **COM7**, réglages visés **115200, 8E1**. Ancien test d'ouverture du port effectué sur un autre PC, sans émission ; aucune réception physique encore observée.
- TR2 actuel : STM32U575 NUCLEO-U575ZI-Q et carte MIKROE-3863 **ADM2867E** (ne pas réintroduire l'erreur ADM2587E). Câble blanc A+ → borne A, vert B− → B, noir GND → GND, rouge +5 V non raccordé et isolé. Montage alimenté depuis USB ST-LINK raccordé au T1000 pour essais ; auparavant powerbank. Mesures TR2 hors tension : continuité A↔Y et B↔Z, résistance A↔B ≈110 Ω. Les niveaux, VDDIO2, DE//RE, polarités, autorité du firmware et réception ne sont pas qualifiés par ces seules observations.
- L'utilisateur a dû retailler un tournevis pour le bornier ; câblage actuellement réalisé. Ne pas lui redemander de recâbler sans raison étayée.
- Mission 005 clôturée DONE : code `66edf2d17c5183003b3e5fcd7b5fe29a0bde4784`, rapport final `9753b3d6b46243f0472a0d33c5fa82b509dcea71`, 104/104 tests hôte et cross-build ARM. Production binding désactivé par défaut ; diagnostic RX prévu, aucune réponse Modbus de production garantie. E4 microSD/power-loss reste ouverte.

## Travail autonome demandé à Codex

1. **État réel et pinout.** Vérifier HEAD distant, état Git local, `AGENTS.md`, protocole V2-B3, mission 005 et documents de pinout en gouvernance ; inspecter le code STM32 actuel, l'UART LPUART1, PG7/PG8, les éventuels GPIO DE//RE PG4/PG5, les alimentations et la révision MIKROE-3863. Séparer preuves documentaires, mesures rapportées et hypothèses ; ne pas déduire un câblage d'une photographie seule.
2. **Diagnostic réellement observable.** Déterminer comment identifier de façon non destructive le firmware actuellement chargé et comment observer sur cible les compteurs `tr2_rtu_rx_*` ou autre preuve fiable. Si nécessaire, implémenter une instrumentation minimale compilable, à état sûr par défaut, permettant une observation via ST-LINK/OpenOCD/GDB ou une autre voie déjà existante. Ne pas supposer que le binaire actuel possède cette instrumentation ni que des symboles sont disponibles ; établir la méthode et ses limites. Préparer une procédure de flash/debug pour une **future autorisation humaine distincte**, sans l'exécuter dans cette mission.
3. **Émission côté HP.** Fournir un outil/script ou une procédure Windows concrète, avec dépendances explicites, pour ouvrir COM7 en 115200 8E1 et transmettre une trame RTU **déterministe et CRC correct**, dérivée des spécifications/adresses réelles du dépôt. Fournir hex attendu et méthode de vérification hors ligne. Prévoir les délais RTU et un timeout interprété correctement : firmware RX diagnostique ne répond pas nécessairement. Ne pas émettre pendant cette mission.
4. **Critères de preuve.** Définir observations avant/après côté STM32 (compteurs octets/trames/CRC/erreurs et limites), contrôle éventuel de l'état DE//RE, conditions de sécurité et diagnostic différentiel (aucun octet, CRC KO, trame valide, réponse absente). Distinguer validation SWD, RX UART, ADU RTU, serveur Modbus et TX RS-485. Donner à l'utilisateur **une séquence très courte de gestes/commandes** une fois la mission relue, sans demander des séries de tests exploratoires.
5. **Livraison autonome.** Implémenter seulement les changements nécessaires ; tests hôte proportionnés et cross-build ARM selon `AGENTS.md`, corriger les régressions, documenter la préparation et mettre à jour l'état courant avec les preuves réelles. Préserver toutes spécifications gelées, configuration de production et dette E4. Ne pas activer par défaut le TX ni un runtime de production non provisionné.

## Permissions, limites et arrêt

`target_branch: main`, `allow_commit: true`, `allow_push: true` : Codex est autorisé à développer, tester, documenter, committer et pousser normalement ses propres livrables sur main, sans validation répétitive ; contrôler branche, périmètre, secrets, divergence et relire les livrables distants conformément à V2-B3. Préserver tous les travaux/stashes préexistants. Pas de force push, reset destructif, réécriture des freezes, effacement, Option Bytes ou action matérielle.

`allow_flash: false`, `allow_debug: false` : **aucun flash STM32, aucune session OpenOCD/GDB avec la cible, aucune émission série physique, aucun geste matériel dans cette mission**. Si la preuve finale nécessite ces opérations, livrer la préparation et indiquer précisément l'autorisation/l'intervention attendue ; ne pas marquer la liaison physique qualifiée.

Si une ambiguïté électrique réelle rendrait un essai dangereux, documenter la condition bloquante et sa mesure minimale plutôt que d'improviser. Ne pas interrompre la mission pour une question que le dépôt, les docs officielles ou les tests logiciels permettent de résoudre.

## Acceptation et rapport

Livrables attendus : instrumentation/outil ou justification vérifiée de leur existence, procédure courte et exacte de l'essai HP COM7 ↔ STM32, validation logicielle réellement observée, tableau des preuves restantes et décision claire « prêt / non prêt pour essai physique ». Rapporter `VALIDÉ hôte`, `COMPILÉ STM32`, `PRÉPARÉ essai` et `NON QUALIFIÉ physique` séparément.

Publier `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` sous `mission_id: TR2-20261011-QUALIF-RS485-RX-006` conformément au protocole V2-B3 ; relire le résultat distant, fournir le SHA final et laisser l'utilisateur demander à ChatGPT « consulte le rapport ».
