---
mission_id: TR2-20261010-DEV-RS485-002
status: READY
base_ref: main
base_sha: 36c33b7981c61376dae45586509327b899e5f3b8
created_at_utc: 2026-10-10T14:15:00Z
author: ChatGPT
target_branch: main
allow_commit: false
allow_push: false
allow_flash: false
allow_debug: false
---

# TR2 — Reprise développement RS-485 dans l'architecture T1000 qualifiée (révision 002)

**Cette mission remplace TR2-20261010-DEV-RS485-001. Ne pas exécuter simultanément les deux missions. Si 001 a déjà modifié des fichiers, conserver et reprendre ces travaux sans écrasement ni doublon.**

## Architecture imposée (déjà qualifiée ; ne pas auditer à nouveau)

Lire `00_Environnement_developpement/T1000/ARCHITECTURE_FINALE.md` et `00_Environnement_developpement/T1000/REPRISE_QUOTIDIENNE.md`. Le **seul clone actif** est `/home/lolo/dev/msm/projects/MSM_vibe` dans **Debian WSL**, SDK `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`. L'ancien `/home/lolo/projects/MSM_vibe` est archivé, non actif. L'exécution attendue est **Codex natif dans Debian** via le lanceur `tr2-codex` (depuis Debian, ou raccourci/lanceur T1000 prévu). Ne pas lancer Codex Windows comme opérateur de développement ni orchestrer chaque commande avec `wsl -d Debian --cd ...` depuis une session Windows. Ne pas déplacer/recloner/réinstaller le dépôt ni modifier les configurations permanentes.

**Contrôle unique et court de contexte** : avant toute modification, confirmer le système d'exécution effectif (Linux Debian), `pwd` dans le clone canonique, branche et `git status --short`. Si la session Codex tourne côté Windows, **STOP : signaler qu'il faut rouvrir Codex natif via `tr2-codex`** ; ne pas tenter de corriger en utilisant un pont WSL pour chaque commande. Ne pas relancer un audit de migration.

## Développement à réaliser

Lire `AGENTS.md`, `Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md`, les spécifications gelées et les seuls sources/tests utiles. Identifier la prochaine lacune concrète de l'intégration firmware STM32/UART/RS-485/Modbus RTU à partir du code actuel, diagnostics déjà présents compris. Implémenter **une tranche logicielle réelle et ciblée**, sans refaire l'existant ni inventer pinout/API/comportement physique. Corriger les erreurs dans le périmètre et exécuter les tests/builds proportionnés. Mettre à jour le journal seulement si progrès significatif. Conserver la dette E4 (microSD/power-loss) comme ouverte, sans audit ou travaux E4 opportunistes.

## Préservation, autorisations et résultat

Préserver absolument les modifications locales déjà présentes, y compris celles éventuellement issues de la mission 001. Ne pas faire de pull sur un arbre sale ni de reset/clean/restore destructif. Si la mise à jour Git est déjà réalisée et sans divergence, ne pas la répéter inutilement. Aucun flash/debug, manipulation physique, changement de câblage ou microSD. Pas de commit/push dans cette mission ; ne pas créer de nouvelle branche sans nécessité. Si l'exécution est bloquée par le contexte Windows, arrêter **avant de modifier du code**.

Rapport concis dans la session Codex : environnement effectif, fichiers développés, résultat des validations ciblées, état Git et prochaine action. **Pas d'audit général, pas de campagne de requalification T1000.**
