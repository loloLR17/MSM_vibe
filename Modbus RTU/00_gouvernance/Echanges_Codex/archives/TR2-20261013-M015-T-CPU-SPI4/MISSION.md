---
mission_id: TR2-20261013-M015-T-CPU-SPI4
status: READY
base_ref: main
base_sha: 79c52f2882a63f4a12a2cf713c978c28253baf24
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-T — CPU accéléré, SPI3 maintenu à 4 MHz

Objectif : test discriminant unique du service RX HAL après l'échec M015-S (CPU16MHz/SPI4MHz, OVR+timeout, 63/896 octets consommés). Vérifier si davantage de cycles CPU, à SCK inchangé, permettent 4096 XYZ intègres sans débordement FIFO. Ne pas présumer de la cause.

Lire AGENTS.md, gouvernance, ETAT_COURANT_TR2, preuves et rapport S, budget R/Q ; vérifier origin/main HEAD, état Git, worktrees et stashes ; archiver contrat et rapport S à l'identique. Préserver les travaux préexistants.

Étudier clock tree STM32U575 et CubeU5 ; choisir fréquence CPU sûre, idéalement 160MHz si validée par la documentation. Ajuster indépendamment source et diviseur SPI3 pour maintenir SCK calculé à 4MHz. Contrôler RCC, SystemCoreClock, PCLK3, prescaler, latences flash et limites électriques. Ne modifier que la configuration d'horloge nécessaire ; garder SPI mode3, ODR, driver ST, parsing, buffers, instrumentation, contrôles et cleanup identiques. Documenter horloges calculées versus mesurées.

Compiler ARM et tester les paramètres d'horloge de façon ciblée. Maximum **une acquisition physique nouvelle** de 4096 XYZ à CPU accéléré / SPI4MHz, sans DMA ni refactoring ; aucun test 8MHz ou répétition des essais 2MHz/512. Mesurer DWT payload, cadence complète et marge temporelle, RXoffset et OVR, statuts FIFO FULL/overrun, XYZ/timestamps/tags, intégrité des octets bruts, faults, journal et cleanup. Stop immédiat sur anomalie, conserver les données rejetées sans les qualifier. Comparer honnêtement S et T et ne pas revendiquer perte zéro globale.

Flash/debug autorisés uniquement sur seconde NUCLEO : STLINK 0032001F3235510C37333439, UID 001B00463842501820333157, avec image identifiée, empreinte, programmation normale et vérification/relecture. Primaire exclu : STLINK 003500463235510B37333439, UID 004A003D3842501820333157 ; ne pas toucher M014-H, FRAM SPI1, SD, Modbus, HP. Pas de mass erase, Option Bytes/RDP, modification physique ou sauvegarde 2MiB injustifiée. Vérifier XL off, FIFO bypass, TS off, faults0 et detach.

Publier preuves reproductibles, matrice comparative CPU16/CPU accéléré à SPI4, conclusion bornée, ETAT_COURANT_TR2, roadmap et DERNIER_RAPPORT.md ; relire la publication distante et préserver archives, worktrees et stashes. Limiter essais et builds pour économiser le quota.
