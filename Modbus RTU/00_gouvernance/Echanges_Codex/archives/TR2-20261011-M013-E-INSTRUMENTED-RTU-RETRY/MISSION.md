---
mission_id: TR2-20261011-M013-E-INSTRUMENTED-RTU-RETRY
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-E — Préparer le premier échange RTU instrumenté avec NVIC sain

## Source et conclusion M013-D
Relire sur main AGENTS.md, ETAT_COURANT_TR2.md, DERNIER_RAPPORT.md (M013-D DONE), diagnostic M013-D, sources et preuves hors Git. M013-D a reproduit une perturbation réelle du NVIC par CubeProgrammer Windows2.23.0 HOTPLUG : IRQ49/66 désactivées, ISPR=FFFFFFFF. Reset GDB rétablit l'état sain. L'image D courante est diagnosticON/observerON/**TXOFF**, et ne peut répondre. Tests 5/5 et cross-build passés, code instrumenté commité. Aucun nouveau probe HP depuis l'essai C unique sans réponse. Ne pas remettre en question sans preuve les contrôles acquis de RE/RST broche2 libre, VIO/J1/J2/J3/SB28 et câblage.

## Objectif
Préparer et qualifier sur la cible un candidat diagnostic isolé **observerON/TXON** pour un **unique futur essai COM7 115200 8E1**. L'essai physique HP reste à faire par l'utilisateur après GO documenté, et n'est pas à simuler ni déclencher depuis Codex. Le premier probe M013-C est clos ; celui-ci est une nouvelle tranche explicitement autorisée, un seul essai, sans retry.

## Exécution
1. Vérifier main réel, Git/stashes, image et preuves M013-D. Relire les risques, conditions de qualification et procédures. Sauvegarder et vérifier l'état Flash/RAM requis avant flash ; préserver les preuves. Vérifier identité NUCLEO-U575ZI-Q STM32U575 ID0x482, ST-LINK SN003500463235510B37333439.
2. Construire à partir du HEAD réel image diagnosticON/observerON/TXON/unité1, et vérifier gardes-fous FC03/1000/1 UNKNOWN0 seulement, absence d'autorités production/médias, gate TX explicite. Effectuer tests/build proportionnés ; empreintes BIN/ELF. Pas de modifications opportunistes.
3. Si GO technique, flash standard qualifié 0x08000000, verify/readback exact. **Ne pas laisser CubeProgrammer HOTPLUG en dernier accès** : après toute opération CLI sur cible, reset logiciel qualifié par GDB, detach, vérifier depuis le cœur via GDB ready1, polling, ISER IRQ49/66 actifs, ISPR cohérents, UART RX armé sans ORE/RXNE résiduels, PG4 bas, TX idle. Ne plus accéder à la cible via CubeProgrammer avant le probe. Ne pas déclencher d'émission ni d'injection synthétique sur RS485.
4. Publier GO/NO-GO et preuve d'état sain, image exacte et limites dans DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md, commit/push main et relecture distante. Si GO, laisser cible prête et donner instruction minimale pour **une seule** requête HP COM7 avec script existant `Send-Tr2RxProbe.ps1 -Transmit`, requête `01 03 03 E8 00 01 04 7A`, réponse attendue `01 03 02 00 00 B8 44`. Le lancement physique appartient à l'utilisateur, après lecture par ChatGPT du rapport GO ; ne pas attendre en bloquant la publication du rapport de préparation.
5. Après retour ultérieur utilisateur, une mission séparée analysera réponse/CRC, compteurs IRQ→RTU→TX/TC et éventuelle capture DE ; aucun succès physique ne peut être affirmé avant preuve.

## Sécurité et critères
Pas de mass erase, Option Bytes, déprotection, média, câblage ou force push. Pas de répétition des vérifications physiques acquises. Si les conditions d'accès sûr, d'activation IRQ et de réception armée ne sont pas remplies, NO-GO sans requête HP. Ne pas confondre GO préparation et validation physique. Respecter les autorisations permanentes AGENTS.md ; ne pas demander de validations intermédiaires logicielles.
