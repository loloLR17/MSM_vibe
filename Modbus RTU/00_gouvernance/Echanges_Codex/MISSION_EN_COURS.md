---
mission_id: TR2-20261011-DIAG-VDDIO2-RS485-007
status: READY
base_ref: main
base_sha: 4a0858725e6e562167ee585622f6e5e7687d2ab1
created_at_utc: 2026-10-11T12:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 007 — Diagnostic VDDIO2 et sécurisation du premier essai RX RS-485

## Contexte et preuves nouvelles de l'utilisateur

Mission 006 terminée DONE, rapport et livrables publiés et contrôlés sur main (HEAD de référence ci-dessus). Sur le banc réel STM32U575 NUCLEO-U575ZI-Q, MIKROE-3863 ADM2867E, T1000 WSL et HP COM7 :
- Logement microSD **vide** actuellement (confirmation utilisateur).
- OpenOCD 0.12.0, STLINK V3J15M7, cible Cortex-M33, tension cible 3,288416 V, GDB port 3333 : connexion réelle réussie.
- GDB `target extended-remote localhost:3333` puis `monitor halt` réussis ; lecture Flash de 157816 octets à 0x08000000, SHA256 `8869e369664da06a707d8d73633fd9fcf1e7940e985b963004c9da73d7b0f480`, **différent** du BIN 006. Firmware chargé **non identifié**, ne pas utiliser les symboles 006 pour interpréter sa RAM.
- BIN 006 local `build-stm32-p11c/tr2_stm32_p11c.bin` : SHA256 `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d`, identique au rapport 006.
- Lecture GDB `x/wx 0x46020810` → `0x46020810: 0x00000000`, donc bit29 IO2SV à 0 **dans ce contexte de lecture**. Ce constat n'est pas une mesure électrique ni une preuve que le futur firmware possède le même état.
- Aucun flash, émission HP ou validation RX physique réalisé. L'utilisateur a été invité à `monitor resume; detach; quit`, mais **son exécution n'a pas été confirmée**. Ne pas présumer de l'état actuel du CPU/serveur.

## Objectif

Lever par analyse logicielle/documentaire rigoureuse les incertitudes relatives au domaine VDDIO2/IO2SV, au pinout PG7/PG8 LPUART1 et à la direction DE//RE du montage RS-485, préparer les corrections logicielles minimales nécessaires et la séquence sûre de qualification physique. Ne pas déclarer prêt un essai sur la seule lecture du registre. Conserver la séparation entre faits, hypothèses et mesures physiques à réaliser.

## Travail autonome autorisé

1. Relire AGENTS.md, protocole V2-B3, rapport 006, PREPARATION_RS485_RX_006.md, gouvernance pinout, schémas et code réellement courant sur main ; contrôler git status/HEAD/branche/origin, préserver les stashes/travaux existants.
2. Vérifier dans les **sources officielles STM32U575/STM32CubeU5** la signification exacte de PWR_SVMCR.IO2SV, la configuration VDDIO2, les conditions électriques et la pertinence pour PG7/PG8 et l'AF LPUART1 ; distinguer état logique de configuration, tension mesurée, mode secure/nonsecure, lecture GDB et comportement au boot. Ne pas inventer de correspondance pinout ni d'API HAL. Identifier si l'absence de `HAL_PWREx_EnableVddIO2` est réellement un défaut dans ce montage, et ce que la lecture 0 permet de conclure.
3. Inspecter l'initialisation GPIO, UART, direction DE et /RE et les schémas MIKROE-3863 (révision physique inconnue) ; analyser les risques de niveaux incompatibles et d'isolation. Vérifier précisément les éventuelles broches de contrôle disponibles et l'état sûr RX. Ne pas supposer que PG4/PG5 sont câblés. Si l'architecture matérielle manque de preuves, fournir les mesures minimales localisées et les critères de sécurité ; ne pas faire de pontage ou d'activation à l'aveugle.
4. Si une correction logicielle peut être établie et rendue **sûre sans supposer une alimentation physique non vérifiée**, implémenter le changement minimal, avec garde-fous et tests appropriés. Sinon, documenter le blocage matériel précis et préparer un patch conditionnel non activé par défaut ou une procédure de décision. Ne pas activer silencieusement les sorties RS-485, le TX ni le serveur de production. Conserver les freezes et la dette E4 microSD/power-loss.
5. Préparer une procédure courte, exacte et non destructive pour la prochaine séance utilisateur : identification du firmware en Flash, vérification préalable du matériel/alimentations et VDDIO2, procédure éventuelle de flash **seulement après autorisation explicite distincte**, contrôles avant/après, une seule émission COM7 ultérieure. Examiner la procédure ST déjà qualifiée et les risques liés au boot. Ne pas présumer que l'utilisateur a détaché GDB.
6. Valider logiciellement par cross-build ARM et tests proportionnés (validation complète si cœur/interface partagé touché), inspecter diff/secrets/index, commit/push normal main et relire les livrables distants.

## Permissions et interdictions

`target_branch: main`, `allow_commit: true`, `allow_push: true` : développement, documentation, tests, commits et push normaux autorisés selon AGENTS.md, sans validation répétitive.

`allow_flash: false`, `allow_debug: false` : **aucun accès cible OpenOCD/GDB, aucun flash, reset, changement d'Option Bytes, aucune émission physique COM7 ni manipulation électrique** par Codex dans cette mission. L'utilisateur réalise les opérations physiques et prend les décisions importantes. Pas de force push/reset destructif/écrasement de travaux.

## Critères d'acceptation et sortie

Livrer : (a) analyse fondée sur références officielles de IO2SV et pertinence PG7/PG8, (b) état vérifié du code et de DE//RE, (c) correctif minimal compilé si réellement sûr, sinon blocage clairement documenté, (d) plan de mesures physiques **minimal** et séquence guidée du prochain essai, (e) validation réelle et limites. Distinguer VALIDÉ logiciel, COMPILÉ STM32, NON QUALIFIÉ électrique, NON QUALIFIÉ RX. Ne pas réclamer à l'utilisateur de réaliser des tâches Git ou tests que Codex peut faire.

Publier `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` sous `mission_id: TR2-20261011-DIAG-VDDIO2-RS485-007`, puis vérifier intégralement le résultat distant et fournir le SHA final conformément à V2-B3.
