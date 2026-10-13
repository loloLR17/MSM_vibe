---
mission_id: TR2-20261013-M016-V1-VIBRATION
status: READY
base_ref: main
base_sha: 25406aff35a769fc2bcf2dd77b33bf0571b832a9
created_at_utc: 2026-10-13T17:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Traitement et publication vibratoire Modbus RTU V1

## Objectif
Raccorder l'acquisition physique IIS3DWB déjà démontrée (M015-V et A04-02) aux traitements métier et aux registres vibratoires du mapping Modbus RTU V1 gelé. Livrer des mesures physiques réelles cohérentes, fraîches, qualifiées et invalidables, sans compromettre la FIFO, la persistance, le transport RTU ou les acquis M014/M015.

## Références et démarrage
Lire l'état courant réel de main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, PROTOCOLE_ECHANGE_V2.md, roadmap, DETTE_TECHNIQUE_A04.md, spécification/mapping unifié V1 (notamment B3, configuration B4 et commandes B5), architectures firmware A–J, sources cumulatives, rapports archivés M015-V/A04-01/A04-02 et leurs preuves, audit A-05 et matrice de tests. Vérifier HEAD, branche, état Git, provenance de l'image et travaux préexistants. Préserver les profils sûrs A04-01 et la protection B5 A04-02.

## Périmètre technique
1. Inspecter le propriétaire unique des données FIFO, le scheduler, les fenêtres et traitements existants, les champs B3, la configuration active autoritative, les états acquisition/cleanup, les générations et la supervision existante. Réutiliser les composants existants avant toute création.
2. Implémenter une chaîne bornée échantillons physiques → fenêtres métier validées → calculs/indicateurs strictement définis par V1 → projection atomique/cohérente vers registres B3, avec unités, échelles, saturation, axes et ordre de mots conformes au mapping gelé.
3. Définir explicitement validité, fraîcheur, transitions de configuration, génération, reset, timeout, défaut FIFO, timestamp discontinu, perte de source, fin/annulation d'acquisition et absence de données ; ne jamais publier comme fraîche une valeur obsolète, ni inventer une mesure. Les lectures B3 doivent rester compatibles avec le service Modbus et la politique A04-02.
4. Analyser et respecter les budgets CPU/RAM/stack et de service FIFO, la cadence/timestamps (A04-05/A04-06), les chemins bloquants (A04-07) et les risques de coexistence. Choisir la solution incrémentale la plus simple ; pas de RTOS, refonte des drivers, FFT coûteuse, allocation SD ou changement du format V1 sans justification. Si un champ V1 exige un traitement non soutenable ou une décision normative, documenter précisément le blocage au lieu de produire des valeurs fictives.
5. Conserver ActiveConfiguration, idempotence et durabilité B5, recovery, B0/B1, communication M014, acquisition M015 et refus B5 pendant capture A04-02. Pas d'activation implicite du stockage bulk microSD, ni de modification silencieuse des spécifications gelées.

## Critères d'acceptation et preuves
- Traçabilité champ par champ des sorties B3 au mapping V1, aux sources réelles et à la configuration active ; tests de décodage, axes, échelles, saturation, limites et cohérence des fenêtres.
- Tests des transitions valid/invalid/stale, erreurs et changements de configuration/génération ; aucune publication partielle ni stale présentée comme valide.
- Tests hôte pertinents et cross-build cumulatif ARM sûrs ; non-régressions Modbus/FRAM/FIFO, A04-01/A04-02 et cas A-05 adaptés. Ne jamais affaiblir des tests pour obtenir un vert.
- Si banc sûr et accessible : flash/debug autorisés et qualification physique bornée repos/excitation contrôlée **sans demander de manipulation physique non disponible** ; lectures Modbus B3 réelles, cohérence avec acquisition et B0/B1, contrôle FIFO FULL/OVR, délais, erreurs UART, FRAM et état final. Séparer observations, estimations et garanties ; ne pas affirmer métrologie ou absence globale de pertes sans preuve.
- Produire SHA code et BIN réellement exécutés, profil/toolchain, commandes, logs, données décodées, limites et dettes résiduelles ; conserver les résultats d'essais infructueux utiles.

## Autonomie et garde-fous
Codex inspecte, choisit l'implémentation proportionnée, développe, corrige, teste, compile, flashe et débogue si pertinent, commit/push normalement sur main, puis publie DERNIER_RAPPORT.md et les preuves avec relecture distante. Les autorisations permanentes AGENTS.md s'appliquent ; pas de validation répétitive. Avant intervention STM32 : identifier TR2-001, vérifier image/profil/empreinte, état du banc, récupérabilité et préservation Flash/FRAM/microSD/identité ; utiliser procédure qualifiée. Pas de mass erase, Option Bytes/RDP, déprotection, opération destructive inhabituelle ou écriture SD non autorisée. Préserver worktrees/stashes, aucune opération Git destructive.

Arrêter/escalader uniquement si risque matériel non maîtrisé, contradiction normative V1, décision structurante ou intervention physique indispensable. Si l'intégration complète n'est pas sûre dans une tranche, livrer un checkpoint stable, un rapport explicite et une suite bornée sans revendiquer M016 DONE.

## Livrables et clôture
Commits de code et tests, preuves reproductibles hôte/ARM/physique si réalisées, cartographie B3 et états invalides, budgets/limites, état matériel final et rapport Codex DERNIER_RAPPORT.md avec statut réel et SHA vérifiés sur main. Ne pas autoqualifier M016 ni créer de tag A-01. ChatGPT décidera de la qualification et de la mise à jour des jalons après contrôle indépendant. Le rapport A04-02 est archivé dans archives/TR2-20261013-A04-02-FRAM-MODBUS/RAPPORT.md ; ne pas l'écraser.
