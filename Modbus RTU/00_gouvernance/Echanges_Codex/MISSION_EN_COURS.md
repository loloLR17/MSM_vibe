---
mission_id: TR2-20261014-P12H3H-CN8-DRIVER-DIFFERENTIAL
status: READY
base_ref: main
base_sha: b2f00a77250780a3ed62058cf4baca56325a78b3
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P12/H3h — Faisabilité CN8 et analyse différentielle des drivers SDMMC

## Objectif
Sortir des essais SDMMC répétitifs en évaluant deux pistes distinctes, sans nouvel essai matériel : (1) connexion du breakout microSD via CN8 pour contourner les soudures/connecteurs suspects, (2) comparaison causale du projet communautaire SDMMC1 avec le driver officiel ST effectivement utilisé dans les bancs historiques. Préserver l'objectif industriel d'enregistrement des vibrations ; aucune décision d'abandon du stockage ou de modification d'architecture.

## Références et méthode
Partir du HEAD réel de main et consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, ETAT_COURANT_TR2.md, les preuves M016/M017, SDMMC1 PoC, FIRSTFAULT, ROOTCAUSE et le dernier rapport permutation DONE. Archiver intégralement mission et rapport permutation avant remplacement, sans réécrire l'histoire. Distinguer SDMMC2 historique, SDMMC1 récent, configurations, versions HAL, broches, horloges, modes et provenance des résultats. Ne pas refaire les essais ni une analyse générique déjà réalisée.

### Axe A — CN8, faisabilité documentaire
À partir des schémas ST MB1549, UM2861 et de la configuration carte réelle, vérifier **si CN8 est bien un connecteur utilisable pour tous les signaux SDMMC 4 bits nécessaires** (CLK, CMD, DAT0–DAT3, 3V3 et GND), pour quelle instance SDMMC et avec quels AF, straps/solder bridges, conflits de périphériques, masses, niveaux et limitations. Ne jamais présupposer CN8 compatible. Produire une table pin-à-pin vérifiée, ou conclure explicitement impossible/incomplet ; distinguer ce que le déplacement contournerait réellement (soudures, câbles, contacts) et les nouveaux risques. Ne demander aucun branchement ni soudure à ce stade.

### Axe B — Différences projet communautaire / référence ST
Identifier exactement le dépôt, commit, licence et code du projet communautaire SDMMC1 utilisé dans le PoC, et les sources officielles ST / HAL STM32U5 / exemples pertinents, avec versions réellement utilisées. Comparer les implémentations et séquences **sur code source**, pas seulement les API : GPIO/AF, clocks, alimentation, init, CMD0/8/55/ACMD41, largeur 1→4 bits, ACMD6, ACMD13/SSR, CRC, FIFO/polling/DMA, gestion STA/ICR, timeouts, vitesse, fronts, bus width, retry, recovery, CMD12/CMD16 et interprétation des erreurs. Rechercher historique upstream, commits, issues et commentaires pour établir ce que la communauté a effectivement voulu corriger ; ne pas attribuer d'intention sans preuve. Construire une matrice différence → motif prouvé/supposé → mécanisme possible pour premier DCRCFAIL SSR → cohérence avec les réussites 4 bits et échecs 1/4 bits historiques → test discriminant éventuel. Mettre en avant les écarts réellement causaux et éviter les modifications opportunistes.

## Livrables et critères
Rapport dans DERNIER_RAPPORT.md, liens précis vers sources/commits/lignes et tableaux vérifiables, recommandation priorisée : (a) CN8 faisable ou non, (b) différence logicielle candidate justifiant ou non une correction minimale, (c) **un seul** prochain essai discriminant si nécessaire, avec résultat attendu selon les hypothèses et prérequis physiques. Si aucune différence causale plausible, le dire. Aucun changement firmware, aucune intervention sur la cible, aucun test SD, aucune répétition d'essai, aucun flash/debug. Publier les seules preuves documentaires nécessaires, archiver le contrat précédent et son rapport, commit/push normal sur main puis relecture distante. Statut DONE si étude complète, BLOCKED si sources essentielles inaccessibles.

## Autonomie, garde-fous
Travail documentaire/hors ligne uniquement ; préserver la Nucleo principale, la seconde Nucleo, FRAM, médias et firmware, sans reset, connexion cible, flash, debug, recâblage ni geste physique. Ne pas modifier les règles de gouvernance ou les spécifications gelées. Préserver le Git existant et ne pas écraser les travaux locaux. Les opérations physiques et la décision de renoncer au stockage restent à l'utilisateur. Aucune validation intermédiaire requise.
