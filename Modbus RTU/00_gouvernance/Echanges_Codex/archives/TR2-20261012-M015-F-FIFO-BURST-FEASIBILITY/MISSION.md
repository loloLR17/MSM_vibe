---
mission_id: TR2-20261012-M015-F-FIFO-BURST-FEASIBILITY
status: READY
base_ref: main
base_sha: 1c062af564e3d4d45093b1c369128cb4cf6d0575
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-F — Faisabilité acquisition FIFO par bursts

Respecter AGENTS.md, gouvernance V2-B3, V1 gelée, état courant et roadmap. Vérifier main et rapport M015-E DONE ; archiver mission et rapport E. Préserver worktrees, stashes, firmware permanent M014-H, FRAM génération12 et microSD OFF. L'utilisateur dispose d'un multimètre, mais pas d'oscilloscope ; ne pas exiger d'instrument absent. Objectif : trouver une méthode fiable sans chercher à expliquer exhaustivement les échecs précédents. Aucune réduction silencieuse des exigences V1.

1. Auditer brièvement le code et les documents ST pour définir un profil RAM borné : IIS3DWB FIFO, bursts de128 mots (897octets, limite SPI3 1024), SPI mode3 à2,5MHz candidat, débit et latence, tags, flags, CS, STOP_ON_WTM et risques. Préparer critères d'intégrité et conditions d'arrêt. Si les préconditions de sûreté ne sont pas réunies, ne pas tenter le matériel.
2. Développer harness RAM réversible, test hôte ciblé et cross-build. Si preflight OK, UNE campagne physique courte maximum : lire FIFO_DATA_OUT en bursts, conserver octets et tags bruts, horodatages, niveaux et flags, mesurer débit et rejeter toute capture ambiguë. Ne pas traiter les lectures single CTRL2 fautives comme vérité ; vérifier configuration en burst, signaler cette limite. Ne pas prétendre qualifier un flux continu par un burst réussi.
3. Flash/debug autorisés : UID/image vérifiés, sauvegarde et hashes Flash2Mio, flash NORMAL qualifié, jamais HOTPLUG CubeProgrammer, mass erase, Option Bytes ou RDP. Restaurer octet-identique M014-H et vérifier la reprise minimale HP B0/B1, GDB détaché pendant trafic. Aucune écriture FRAM ni SD, partition LBA65536 protégée. Arrêt sur risque ou incohérence.
4. Publier traces, mesures, limites, état courant, roadmap et DERNIER_RAPPORT.md ; commit/push normaux et relecture distante. M015-F DONE seulement si la faisabilité RAM bornée est démontrée, sinon BLOCKED motivé. M015 globale IN PROGRESS.

Présenter les conséquences chiffrées de toute réduction éventuelle : ODR26667Hz, fenêtres4096..32768, continuité et stockage store-and-forward, bande fréquentielle, spectre, pertes, latence, mémoire et compatibilité V1. Ne modifier ni mapping, ni validator, ni spécifications gelées sans décision utilisateur.

Exclusions : recherche exhaustive de cause CTRL2, instrument externe supposé, DMA/production/flux continu, FFT, calibration, microSD, B5, Modbus vibration, Web, requalification des campagnes A-E. Limiter la dépense de quota et publier les acquis rapidement.
