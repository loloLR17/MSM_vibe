---
mission_id: TR2-20261013-A06-GOUVERNANCE-CONSOLIDATION
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Audit A / A-06 — Consolidation incrémentale de la gouvernance MSM TR2

## Objectif et principe intangible
Appliquer les décisions conjointes de l'audit A-01 à A-06 en **préservant le fonctionnement actuel**. Le circuit obligatoire reste ChatGPT → GitHub → Codex → GitHub → ChatGPT. Aucun changement de firmware, de matériel, de spécification V1 gelée, de configuration GitHub du dépôt, ni de méthode de travail structurelle. Privilégier les modifications documentaires minimales, proportionnées, traçables, sans multiplier les documents ou les obligations. Examiner d'abord l'existant et signaler les contradictions éventuelles.

## Préparation et préservation
Lire AGENTS.md et la gouvernance courante sur origin/main, le rapport M015-V et les documents de suivi. Vérifier HEAD réel, branche, git status, worktrees, stashes, éventuelle mission concurrente et état de la mission précédente. Archiver le contrat M015-V selon le protocole existant sans le modifier ni perdre sa traçabilité ; ne pas altérer DERNIER_RAPPORT.md tant que les preuves précédentes ne sont pas préservées. Travailler avec une zone Git propre et des commits/push normaux, jamais de force push ni d'écrasement de travaux existants. Ne pas modifier le présent contrat.

## Travaux à effectuer de manière autonome
1. Auditer la gouvernance et le protocole existants. Compléter **au plus juste** AGENTS.md et/ou les documents de gouvernance déjà présents ; n'ajouter un document que si une lacune précise le justifie.
2. Expliciter les responsabilités : ChatGPT définit avec l'utilisateur les objectifs, contraintes et critères d'acceptation ; Codex analyse les solutions et risques puis réalise les travaux autorisés de manière autonome ; Codex publie code/preuves/rapport ; ChatGPT vérifie indépendamment. L'utilisateur n'intervient que pour décisions structurantes, risques matériels non maîtrisés, opérations destructives inhabituelles et gestes physiques.
3. Rappeler à ChatGPT la lecture obligatoire de la gouvernance **courante de main** avant chaque analyse technique ou mission TR2, et la vérification distante de toute publication. Les instructions du projet ChatGPT dans l'interface ne peuvent pas être modifiées depuis ce dépôt : fournir en rapport un **texte bref prêt à copier** pour celles-ci, notamment la lecture GitHub obligatoire et **aucun recours à Work sans demande explicite de l'utilisateur**. Ne pas prétendre avoir modifié les paramètres ChatGPT.
4. Formaliser la gestion des interruptions : checkpoint intermédiaire avec état IN_PROGRESS distinct de DONE/BLOCKED/FAILED, SHA et état Git, tests effectués, éléments matériels, reste à faire ; commits/push intermédiaires proportionnés si autorisés ; reprise de la même mission sans inventer de progression ni de quota observable. Ne pas rendre obligatoire un checkpoint à chaque action.
5. Clarifier les rôles des documents existants (AGENTS.md permanent, MISSION_EN_COURS.md contrat, DERNIER_RAPPORT.md preuves récentes, ETAT_COURANT_TR2.md état synthétique, ROADMAP jalons), en limitant les répétitions. Ne pas entreprendre la réécriture massive des longs documents dans cette mission ; proposer seulement une simplification graduelle.
6. Préparer un mécanisme léger de qualification : tags Git **annotés** pour jalons réellement qualifiés, pointant sur le commit de code qualifié et créés après revue indépendante ChatGPT ; fiche de qualification concise avec preuves, limites, SHA et configuration exacte. Ne créer **aucun tag de qualification rétroactif** ni affirmer que M015-V a été qualifié indépendamment.
7. Consigner l'orientation A-06 « amélioration incrémentale maîtrisée » : conserver ce qui fonctionne, corriger les défauts démontrés et risques importants, pas de refonte préventive, pas de bureaucratie nouvelle.
8. Documenter en quelques lignes le prochain audit technique, **sans l'exécuter** : configuration de build STM32 canonique, budgets temporels FIFO/Modbus/FRAM/SD, drivers ST et dépendances, qualité des tests, reproductibilité, CI légère facultative. Codex sera sollicité pour proposer et réaliser les corrections minimales après analyse des risques, dans une mission distincte.

## Autorisations et limites
Développement documentaire, commit et push sur main autorisés, sans demande de validation répétitive. allow_flash=false et allow_debug=false car **aucune opération STM32 n'est nécessaire**. Pas de modification de code, de scripts de build, de CI ou d'instructions ChatGPT hors dépôt. Ne pas changer les permissions permanentes ni affaiblir les garde-fous matériels de AGENTS.md. Pas de réorganisation globale ni de suppression d'archives. Escalader seulement un conflit normatif insoluble, une décision structurante ou un risque exceptionnel.

## Acceptation et livraison
Diff documentaire restreint et motivé ; absence de contradiction avec AGENTS.md et le protocole ; contrats précédents et preuves préservés ; vérifications documentaires adaptées ; état Git et commits SHA publiés ; DERNIER_RAPPORT.md avec DONE, IN_PROGRESS, BLOCKED ou FAILED et preuves, limites et texte court d'instructions projet ChatGPT. Vérifier la présence réelle des commits sur origin/main après push. ChatGPT contrôlera indépendamment le rapport et les fichiers modifiés avant toute qualification ou nouvelle mission technique.
