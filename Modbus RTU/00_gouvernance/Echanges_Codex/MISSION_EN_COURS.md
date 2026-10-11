---
mission_id: TR2-20261011-M013-D-UART-IRQ-ROOT-CAUSE
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-D — Isoler la cause RX UART / IRQ sans nouveau probe HP

## Contexte vérifié
M013-C terminé BLOCKED, rapport distant relu : firmware diagnostic exact flashé/vérifié, unique requête HP COM7 115200 8E1 sans réponse ; après essai UART RXNE+ORE, HAL BUSY_RX, buffers et file RTU/TX vides, NVIC via SWD ISER=0 et ISPR=FFFFFFFF anormaux. Code contient EnableIRQ et vecteurs LPUART1/TIM6 attendus. Ce sont des observations, non une preuve de cause racine. Le premier essai est consommé. Aucun nouveau probe HP, aucune modification physique demandée. RE/RST broche2 du Click est libre de toute connexion externe, point définitivement acquis sauf preuve contradictoire nouvelle.

## Objectif et autonomie
Diagnostiquer et, si cause démontrée, corriger le défaut de traitement des interruptions RX LPUART1/TIM6 du diagnostic isolé ; instrumenter de manière passive la chaîne IRQ→HAL→callbacks→file→RTU→TX, sans émission RS485 ni requête synthétique sur bus. Préserver le contrat V1 gelé et les garde-fous diagnostic (FC03 registre1000 valeur UNKNOWN0 seulement, aucune fausse autorité production). Travailler sur main courant, relire AGENTS.md, DERNIER_RAPPORT.md, ETAT_COURANT_TR2.md, code/HAL et preuves M013-C. Permissions permanentes applicables ; ne pas solliciter de validations logicielles répétitives.

## Exécution attendue
1. Préflight : vérifier HEAD distant/local, état Git/stashes et identité cible ; préserver l'intégralité des dumps RAM, logs, backup Flash2MiB et preuves M013-C avant toute action qui réinitialise la RAM. Aucun reset/reflash avant analyse et archivage.
2. Établir l'explication des valeurs NVIC anormales : comparer lecture SWD aux lectures exécutées par le cœur (instrumentation passive si nécessaire), distinguer banques secure/nonsecure, état des interruptions, mapping IRQ et limitations de debug ; inspecter startup, VTOR, TrustZone/attribution et init HAL sans déductions non prouvées. Analyser pourquoi RXNE/ORE restent non traités alors que HAL RX est armé.
3. Priorité au diagnostic non intrusif ; ajouter instrumentation minimale et tests ciblés, compteurs monotones IRQ UART/TIM6, callbacks RX/ERROR, états ISR/erreurs, réception octets, progression RTU et éventuelle préparation TX, sans log émetteur sur UART RS485. Si une cause logicielle est établie, apporter la correction minimale, tests et cross-build proportionnés ; ne pas masquer les symptômes ni affaiblir tests.
4. Flash/debug standard autorisés si nécessaires et si les préconditions qualifiées restent satisfaites : cible STM32U575 et STLINK SN 003500463235510B37333439, sauvegarde vérifiée, image exacte et readback ; pas de mass erase, Option Bytes, déprotection, effacement média, changement câblage. Tout reflash effaçant l'état RAM doit être précédé de sa conservation. Après flash, aucune émission active sur bus, aucun faux résultat physique. Ne pas relancer le script HP ni demander de transmission dans cette mission.
5. Documenter observations brutes, hypothèses éliminées, cause racine prouvée ou non, modifications, preuves tests/build/flash éventuel, limitations et critères objectifs d'un futur essai HP instrumenté. Mettre à jour DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md ; commits/push normaux main, vérifier relecture distante. En cas de blocage réel, statut BLOCKED argumenté, sans extrapoler une validation TX physique.

## Critères d'acceptation
Diagnostic discriminant IRQ/NVIC/TrustZone fondé sur preuves ; instrumentation passive et/ou correction vérifiée ; aucune nouvelle requête HP ni TX provoquée ; état Git et sauvegardes préservés ; rapport vérifiable et publiable. La qualification Modbus bidirectionnelle reste NON VALIDÉE jusqu'à capture réelle d'une réponse lors d'une mission ultérieure.
