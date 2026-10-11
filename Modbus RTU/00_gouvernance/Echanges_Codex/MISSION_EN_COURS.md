---
mission_id: TR2-20261011-M013-H-HP-CAMPAIGN-PREFLIGHT
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-H — Préflight HP et GO conditionnel campagne RTU bornée

## Référence
Relire AGENTS.md, protocole, ETAT_COURANT_TR2.md, DERNIER_RAPPORT.md M013-G DONE, plan QUALIFICATION_TEMPORELLE_RTU_M013_G.md, script rtu_campaign.py et tests, rapports E/F, images et preuves conservées. Le premier échange HP unique E est corroboré F, sans qualification temporelle ni répétabilité. M013-G a publié outil Python OFFLINE/mock testé 10/10 sur Linux, non encore testé sur HP réel ; campagne maximum12 écritures (10 nominales, CRC invalide, unité étrangère), sans GO physique. Aucun instrument de chronogramme identifié ; qualification temporelle reste WAITING.

## Objectif
Préparer en autonomie la campagne HP, vérifier la compatibilité de l'environnement Windows et l'état sain du STM32, puis publier un **GO ou NO-GO documenté pour une seule future campagne**. Ne pas exécuter la campagne physique ni ouvrir COM7 dans cette mission. Éviter toute sollicitation utilisateur pour des opérations réalisables par Codex ; ne demander que la future commande HP indispensable et seulement après contrôle indépendant ChatGPT du GO.

## Exécution
1. Vérifier HEAD main, git propre/stashes, permissions et preuves G/F/E ; contrôler que l'outil et son plan sont bien ceux de main. Revoir les 12 cas et les critères d'arrêt, notamment gestion des silences attendus, absence d'écho confondu, délai inter-requête, réponse tardive, et conditions de récupération. Corriger seulement les lacunes avérées avec tests, commit/push.
2. Déterminer si Codex peut vérifier directement le HP Windows et Python/pySerial sans perturber COM7, en utilisant les accès réellement disponibles. Ne pas inventer la disponibilité du HP, ne pas installer ou modifier des logiciels système sans nécessité. Tester --offline et --mock sur l'environnement effectif si accessible, et vérifier l'absence d'ouverture série dans ces modes. Si le HP est inaccessible, préparer une commande de préflight non intrusive que l'utilisateur pourra exécuter ultérieurement ; publier NO-GO tant que prérequis essentiels non confirmés.
3. Avant tout éventuel futur GO, vérifier image exacte et santé MCU via GDB seulement, sans CubeProgrammer HOTPLUG : NVIC IRQ49/66, UART/RX armé, PG4 bas, observateur baseline avant campagne, sans reset/flash si inutile. Conserver snapshot brut et hashes. Aucun halt pendant trafic ; aucune émission sur RS485. Ne pas répéter contrôles matériels J1/J2/J3/RE-RST/VIO/SB28 déjà acquis.
4. Si environnement et état MCU sont confirmés, fournir commande physique Windows exacte, maximum 12 écritures, journal exclusif et condition d'arrêt ; préciser que l'utilisateur lancera cette commande **une seule fois** seulement après revue GO par ChatGPT. Si environnement non confirmé, publier NO-GO ou GO partiel limité aux préflights, avec une seule action minimale concrète. Aucune transmission HP/COM7, aucun test physique ni mesure temporelle dans H.
5. Publier DERNIER_RAPPORT.md, ETAT_COURANT_TR2.md, livrables/scripts utiles, commit/push normal main, relecture distante et verdict explicite. Conserver backups et stashes, ne pas modifier V1 ni prétendre qualification physique générale.

## Critères
Autonomie logicielle maximale, aucun accès COM7 ni trame émise en H, campagne strictement bornée, vérifications traçables, distinction GO/NO-GO. Autorisations commit/push/flash/debug accordées pour éviter les validations intermédiaires, mais flash/reset uniquement si besoin démontré et après précautions AGENTS ; pas d'Option Bytes, mass erase ou manipulations physiques.
