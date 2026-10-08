---
mission_id: TR2-20261008-CODEX-GIT-PREP-001
status: READY
base_ref: test/echange-chatgpt-codex-20261008
base_sha: 43e72c0c87c1f5b38e8565fd3f3cb876eef40135
created_at_utc: 2026-10-08
author: ChatGPT
target_branch: test/echange-chatgpt-codex-20261008
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P1 — Sauvegarde sécurisée et préparation isolée du firmware local

## Contexte et objectif
Après la fusion V2-C sur main (19c39d6a7a9c3d8fef4330a64be427563ed108cb), l'audit TR2-20261008-CODEX-GIT-AUDIT-001 a constaté un dépôt principal local sur 18c9871481e714e05c93ee60d982c7293f488ba3, avec 6 fichiers suivis modifiés et 2621 non suivis, dont 2609 fichiers de build. Objectif : protéger intégralement cet état par une sauvegarde privée vérifiée, puis préparer une tranche firmware diagnostique dans un clone de travail isolé à partir de main distant, sans publication du firmware ni modification du dépôt principal.

## Autorisations et restrictions
L'utilisateur autorise cette mission P1. Dans le dépôt principal local : lecture seule stricte ; ne jamais exécuter pull, fetch, add, commit, push, stash, reset, clean, checkout, switch, restore, merge, rebase, ni modification de fichier. Pas de modification de main distant. Pas de flash/debug ni configuration permanente. Pas de suppression de fichiers. Ne jamais inclure les répertoires de build ou des secrets dans des commits.

**Sauvegarde privée** : autorisée uniquement dans un chemin local privé distinct, hors du dépôt principal, hors d'un répertoire synchronisé/public et hors des dépôts GitHub. Choisir un chemin sûr et consigner le chemin exact dans le rapport ; si la confidentialité ou l'espace disponible ne peuvent être vérifiés, arrêter avant la copie et rapporter BLOCKED. Sauvegarder la totalité du dépôt principal (y compris .git, fichiers suivis, non suivis et ignorés, BIN/ELF et documents), en préservant les données et sans écraser une sauvegarde existante. Vérifier la copie par inventaire et empreintes de fichiers, y compris les fichiers générés ; contrôler quelques récupérations. Une sauvegarde non vérifiée ne suffit pas. Ne jamais pousser cette sauvegarde sur GitHub.

**Clone isolé** : après sauvegarde vérifiée seulement, créer un clone distinct sur le SHA main distant revérifié. Importer dans une branche locale de préparation dédiée uniquement les changements firmware identifiés lors de l'audit : les 5 fichiers firmware suivis modifiés, 4 fichiers diagnostics STM32 (.c/.h), 2 tests unitaires et 3 procédures de gouvernance. Examiner les dépendances et contrôler les contenus/sensibilités avant intégration. Ne pas importer AGENTS.md local V1, ni les répertoires de build, ni le PDF/XLSX, ni le journal local ETAT_COURANT_TR2.md sans décision séparée. Les originaux restent intacts. Documenter le statut strictement diagnostique du harness, qui court-circuite le runtime normal, sans promotion en production.

## Validations
Inspecter diff, fichiers ajoutés, état Git, cohérence CMake et références de la version STM32CubeU5 ; vérifier les prérequis avant tout build. Si faisable sans toucher au dépôt principal ni au matériel, lancer dans le clone la validation pertinente (tr2_validate.sh, toolchain STM32) et consigner exactement commandes, résultats et limitations. Ne pas prétendre à une validation physique. Si le clone ne peut pas accéder aux prérequis, le signaler sans contourner les restrictions. Ne pas utiliser git add global. Aucune publication de firmware, aucun commit ou push de la branche de préparation vers GitHub dans cette mission.

## Rapport et clôture
Seule écriture GitHub autorisée : DERNIER_RAPPORT.md sur la branche expérimentale depuis un clone de publication isolé, avec commit/push normal après contrôles. Les permissions allow_commit/allow_push valent **exclusivement pour ce rapport**, pas pour le firmware ni main. Rapport : mission_id, status DONE/BLOCKED, références exactes, inventaire et sauvegarde (destination privée, taille, manifestes, hash et vérification), préparation locale (chemin et branche), fichiers importés, validations réellement exécutées, écarts, risques, critères d'arrêt et étapes proposées avant publication. Ne pas inclure de données secrètes. DONE seulement si sauvegarde intégrale vérifiée ET clone isolé préparé ; sinon BLOCKED avec état réel des travaux. Relire le rapport publié sur GitHub, vérifier le SHA distant, préserver toutes les archives et ne pas modifier la mission active.
