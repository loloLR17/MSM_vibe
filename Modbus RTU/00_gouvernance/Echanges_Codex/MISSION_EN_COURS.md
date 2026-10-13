---
mission_id: TR2-20261013-M016-SDMMC-REFERENCE-BENCH
status: READY
base_ref: main
base_sha: 1c634e3a16c78feed3a5564fd59a6b2fd2298a93
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Banc SDMMC2 4 bits indépendant à partir d'une référence éprouvée

## Décision utilisateur
Suspendre les nouvelles tentatives de réglages empiriques et d'écriture sur la carte microSD du TR2 principal. Avant toute reprise du stockage de production, établir sur **la seconde NUCLEO-U575ZI-Q et le second breakout Adafruit 4682** un exemple minimal d'écriture puis relecture en **SDIO/SDMMC 4 bits**, en partant prioritairement d'un exemple officiel ST/Adafruit applicable. Cette mission est une tranche de diagnostic préalable ; elle ne qualifie ni ne remplace le stockage V1. Les spécifications gelées restent inchangées.

## Démarrage et références
Vérifier le HEAD réel, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md (règle références constructeur), le rapport M016 actuel, les preuves d'échecs et de recovery, les gels H3h, le pinout SDMMC2, la géométrie des médias et les stashes/worktrees. Préserver les travaux et preuves existants. Vérifier si Codex a encore des opérations actives ; ne pas concurrencer une session ou des écritures en cours.

## Objectif et autonomie
1. Rechercher dans les sources officielles STMicroelectronics (STM32CubeU5, exemples NUCLEO-U575ZI-Q ou U5 proches, HAL/BSP) et Adafruit 4682 une référence de lecture/écriture SDMMC 4 bits ; identifier les versions, liens, licences et écarts. Un exemple SPI seul n'est pas une validation du SDMMC 4 bits. Un exemple non disponible ou non directement portable doit être signalé, sans inventer de code constructeur.
2. Préparer un **projet de démonstration indépendant**, aussi proche que possible de la référence choisie, sans intégrer CampaignDataStore, FRAM, IIS3DWB ni Modbus. Adapter uniquement le strict nécessaire à SDMMC2 et au câblage réel de la seconde Nucleo. Conserver la provenance et les différences.
3. Définir un protocole reproductible : identification Nucleo/SD, bus 4 bits effectivement confirmé, initialisation, écritures bornées, relecture et comparaison octet à octet, répétition, horloge mesurée, statuts/erreurs et performances. Tester les hypothèses une à une, pas une succession aveugle de réglages.
4. Vérifier avec l'utilisateur le montage physique et la carte SD de test : **aucune écriture sur un support dont les données doivent être préservées**. Le choix/effacement éventuel d'un média de test appartient à l'utilisateur ; aucune préparation destructive implicite. Les opérations de câblage/insertion sont physiques et réalisées par l'utilisateur.
5. Si le banc est disponible, sûr, identifié et muni d'une zone d'essai autorisée, réaliser les flash/debug et essais bornés, enregistrer les preuves. Sinon terminer la préparation logicielle et rapporter précisément les interventions physiques nécessaires, sans prétendre avoir validé l'écriture.
6. Si le banc de référence fonctionne, comparer ensuite les différences pertinentes avec l'implémentation TR2 existante et proposer la correction minimale ; ne pas modifier ni reflasher le TR2 principal dans cette tranche.

## Critères d'acceptation
- Référence constructeur/communautaire traçable, statut exact et différences documentés ; code de démonstration isolé compilable.
- Si matériel prêt : preuve physique d'écriture/relecture 4 bits répétée, avec identifiants, erreurs et comparaison des données ; sinon statut BLOCKED/PENDING matériel explicite.
- Préservation vérifiée des données, firmwares et preuves du TR2 principal ; aucune nouvelle écriture dans les volumes M016 historiques.
- Tests/builds proportionnés, commits/push sur main et DERNIER_RAPPORT.md publié et vérifié ; conclusions séparant démonstration, diagnostic et qualification industrielle.

## Garde-fous et escalade
Autorisations : target_branch main, allow_commit/push/flash/debug true. Identifier formellement la **seconde** Nucleo avant tout flash ; interdire toute confusion avec TR2-001. Ne pas mass erase, modifier Option Bytes/RDP, formater ou repartitionner sans décision explicite ; aucune écriture hors zone d'essai autorisée. Préserver FRAM/microSD/identités et données existantes. Les gestes physiques restent à l'utilisateur. Escalader uniquement absence de matériel, incertitude sur les données à préserver, risques matériels non maîtrisés ou choix structurant.

## Suite
Une fois le banc indépendant démontré, ChatGPT contrôlera le rapport et décidera de la mission d'intégration minimale au stockage V1, puis de la reprise de M016-V1-VIBRATION.
