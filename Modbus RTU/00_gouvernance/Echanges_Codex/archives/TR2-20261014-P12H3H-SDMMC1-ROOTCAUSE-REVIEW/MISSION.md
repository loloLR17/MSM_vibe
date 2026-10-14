---
mission_id: TR2-20261014-P12H3H-SDMMC1-ROOTCAUSE-REVIEW
status: READY
base_ref: main
base_sha: 6e9f0a48023288a5ed1e3cf4029ebc1f6c143ccd
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P12/H3h — Revue contradictoire documentaire des causes SDMMC1 avant nouvel essai

## Objectif
À la suite des missions SDMMC1 de référence, comparatif 10 cm et COLDSTART10CM (toutes FAILED pour la fiabilité), et de la qualification PC de la SanDisk 32 Go rapportée par l'utilisateur (1 Gio écrit/relu conforme sur lecteur natif, **rapport local non encore publié sur GitHub**), effectuer une **analyse du code réel et du schéma**, sans intervention STM32. Intégrer comme hypothèses à vérifier les observations de l'expert indépendant Claude, qui reconnaît n'avoir pas pu consulter le code ou les rapports : marge électrique 4 bits, pull-ups, GPIO speed, alimentation, séquence d'init, contexte USB/SDMMC, récupération multibloc incomplète. Ne pas traiter ces hypothèses comme des faits.

## Périmètre
1. Lire AGENTS, gouvernance, HEAD main, rapport actuel et archives/éléments probants des trois missions ; inspecter directement le firmware communautaire épinglé `VILLO88/NUCLEOU575@1e3c9183bad023ebf7d234134823d9270a0e8b64`, sa configuration IOC, HAL MSP, SDMMC, TinyUSB MSC et FatFs, et les schémas officiels NUCLEO-U575ZI-Q / breakout Adafruit 4682 accessibles. Citer fichiers/lignes, distinguer sources inspectées et hypothèses.
2. Contrôler GPIO `PUPDR`/pull-ups CMD/D0-D3, `OSPEEDR`, alternate functions, SB19/SB22 déjà vérifiés, topologie alimentation et découplage si documents disponibles, horloges SDMMC/USB, NVIC/priorités et contexte d'appel des transferts, choix du bus 4 bits et repli 1 bit, CMD12/CMD13/états HAL et logique « No Media ». Signaler clairement ce qui exige mesure électrique.
3. Construire une **chronologie raisonnée** : première erreur potentielle vs accumulation et récupération défaillante, sans attribuer une chronologie à partir des seuls bits ErrorCode accumulés. Vérifier l'observation cold-start No Media avant GDB, qui ne démontre pas que la carte elle-même a été à 0 V.
4. Proposer un **unique protocole ultérieur court et discriminant**, d'abord initialisation/lecture seule hors USB MSC si techniquement réalisable, puis comparaison 4 bits/1 bit à fréquence identique, sur même zone connue, avec contrôle de l'état initial, instrumentation minimale de première erreur et limites causales. Ne pas reprendre directement les 20×64 Mio suggérés par Claude. Définir conditions d'arrêt, preuves et interprétation de chaque issue, et préciser les interventions physiques minimales à demander (carte actuellement dans lecteur PC, confirmation VDD=0V si mesure indispensable). Ne pas présumer le matériel connecté.
5. Classer les hypothèses avec niveau de confiance et faits pour/contre ; préciser si une lecture du schéma suffit à trancher les pull-ups et si une mesure au multimètre est nécessaire. Examiner les risques de CMD13 intrusif après erreur et de perturbation par debugger.

## Contraintes
**Mission strictement documentaire, sans matériel** : aucun flash, reset, GDB, écriture sur SD, accès au lecteur PC, modification firmware tiers, ni opération physique. `allow_flash: false`, `allow_debug: false` parce qu'aucune intervention STM32 n'est demandée. Ne pas modifier les règles de gouvernance, les gels ni les documents de spécification. Préserver worktrees/stashes et état local. Autorisations commit/push pour rapport et preuves documentaires uniquement.

## Livrable
Publier sur main `DERNIER_RAPPORT.md` avec mission_id présent, revue indépendante du code et références vérifiables, hypothèses hiérarchisées, proposition de test **non exécuté**, besoins physiques et limites, statut DONE si revue documentaire effectivement accomplie, BLOCKED si sources indispensables inaccessibles. Ajouter preuves documentaires minimales si utile. Commit/push et vérification distante. ChatGPT effectuera le contrôle indépendant avant d'autoriser une mission de test physique.
