---
mission_id: TR2-20261013-A04-02-FRAM-MODBUS
status: READY
base_ref: main
base_sha: ba6f34c4e536f430be74b2e6c27f6c8c9cb873e2
created_at_utc: 2026-10-13T15:11:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# A04-02 — Maîtrise des opérations FRAM/Modbus bloquantes

## Objectif
Corriger la dette A04-02 identifiée par l'audit cumulatif A-04 : maîtriser les chemins FRAM/Modbus susceptibles de compromettre le service coopératif de la FIFO IIS3DWB. Préserver la persistance, les garanties transactionnelles et la spécification Modbus RTU V1 gelée. Correction minimale, robuste et mesurable, sans architecture imposée.

## Références et état initial
Lire sur la branche main courante AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, PROTOCOLE_ECHANGE_V2.md, ETAT_COURANT_TR2.md, DETTE_TECHNIQUE_A04.md, les rapports archivés A-04/A-05, la matrice A-05 proposed-test-matrix.csv, le rapport et les preuves A04-01, les spécifications V1 et les sources du firmware cumulatif M014/M015. Vérifier HEAD distant, branche, git status, travaux préexistants et éventuelles divergences avant toute modification. Conserver les protections de profils de compilation A04-01.

## Analyse technique
Caractériser les chemins FRAM/Modbus, notamment lectures B0/B1/B3, commandes B5, scans, commits, persistance/recovery, défauts média, scheduler et traitement FIFO. Distinguer les opérations au boot de celles possibles pendant capture. Documenter les conditions de déclenchement et les coûts. Les références historiques (~141 ms de blocage et ~17 ms de budget FIFO depuis vide) sont des éléments de cadrage, non des mesures physiques nouvelles.

## Correction ciblée
Analyser puis implémenter la solution proportionnée la plus simple : traitement borné ou coopératif, projection/cache cohérent, politique explicite d'indisponibilité ou ordonnancement compatible FIFO, selon ce que les faits justifient. Ne pas imposer RTOS, DMA généralisé, refonte du stockage ou refactoring sans nécessité démontrée. Ne pas traiter A04-03/M016 dans cette mission.

## Invariants protégés
Préserver ActiveConfiguration et son autorité, durabilité/atomicité/recovery après coupure, idempotence B5 (pas d'ACK de succès avant durabilité), validation de configuration, sémantique et codes Modbus RTU V1, gestion des défauts, acquis M014/M015 et isolation des profils A04-01. Pas de nouveau code Modbus hors V1, pas de succès mensonger, pas de donnée obsolète exposée comme courante.

## Validation et acceptation
Employer en priorité les scénarios A-05 IO-01 à IO-05, avec instrumentation proportionnée :
1. B0/B1/B3 ne déclenchent aucun scan/snapshot B5 FRAM inutile en l'absence de commande B5 ; réponses et fraîcheur conformes.
2. B5 présente une vue autoritative cohérente, avec génération et invalidation en cas de défaut ; indisponibilité/refus conforme aux mécanismes V1 existants.
3. Avec coûts d'I/O représentatifs, les interactions scans/commits/Modbus/FIFO sont bornées, différées ou refusées explicitement, sans masquer les coûts par des mocks instantanés.
4. Durabilité, retries, collisions, corruption, coupures et recovery préservent les dernières générations valides et l'idempotence ; réutiliser les tests existants, compléter seulement les transitions touchées.
5. Sur STM32 si matériel et conditions sûres disponibles : essai borné de coexistence FIFO + B0/B1/B3/B5, mesures scheduler/DWT, niveaux FIFO, gaps, FULL/OVR, pertes UART, CRC, générations et erreurs, avec traces et identité du banc.

Mesurer ou justifier les budgets et hypothèses avant de fixer des seuils ; distinguer résultats hôte, ARM, mesures physiques et estimations. Ne pas déclarer garantie de continuité globale si la preuve est bornée. Exécuter les tests pertinents, le cross-build cumulatif et les non-régressions proportionnées selon AGENTS.md. Ne désactiver, affaiblir ou contourner aucun test. Documenter explicitement toute limite empêchant une qualification.

## Permissions et garde-fous STM32
target_branch: main ; allow_commit: true ; allow_push: true ; allow_flash: true ; allow_debug: true.
Flash/debug autorisés si utiles, sans confirmation répétitive. Avant flash : vérifier cible TR2-001, origine/empreinte et profil cumulatif de l'image, état matériel et sauvegarde des données/firmware si nécessaire ; utiliser la voie de programmation qualifiée et préserver FRAM, microSD, identité, Option Bytes/RDP et récupérabilité. Aucun mass erase, changement Option Bytes/RDP, déprotection ou écriture destructive inhabituelle. Arrêter en cas de risque non maîtrisé ; les manipulations physiques restent à l'utilisateur.

## Autonomie, arrêt et livrables
Codex inspecte, choisit, implémente, compile, teste, instrumente, flashe/débogue si pertinent, corrige les erreurs, commit/push normalement sur main et publie les preuves et DERNIER_RAPPORT.md. Pas de validations répétitives. Préserver travaux préexistants, stashes et worktrees ; aucune synchronisation Git destructive. Escalader seulement décision structurante, contradiction V1, opération destructive inhabituelle, risque matériel non maîtrisé, intervention physique nécessaire ou blocage réel.

Le rapport A04-01 a été archivé préalablement sous archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md ; ne pas l'écraser. Publier corrections ciblées, tests, commandes/résultats, mesures/bornes et hypothèses, risques résiduels, SHA et rapport DERNIER_RAPPORT.md. Vérifier les publications distantes. DONE signifie livrables publiés et relus, non qualification indépendante. ChatGPT contrôlera ensuite les commits, preuves et limites et décidera de la clôture A04-02. Ne pas engager A04-03/M016.
