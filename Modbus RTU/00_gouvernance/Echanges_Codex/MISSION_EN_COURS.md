---
mission_id: TR2-20261010-DEV-RS485-003
status: READY
base_ref: main
base_sha: 4591e13a37e99724c35e88fc03105e5e082c90b8
created_at_utc: 2026-10-10T14:20:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2 — Finaliser et publier le développement RS-485 / Modbus RTU

**Autorisation explicite de l'utilisateur : autonomie pour commits et push sur `main` dans le périmètre de cette mission, y compris le rapport.** Cette mission remplace 001/002 et reprend leurs éventuels travaux locaux ; ne rien écraser ni refaire inutilement. Codex est seul rédacteur du rapport.

## Travail à effectuer
Exécuter dans Debian WSL **Codex natif**, clone canonique `/home/lolo/dev/msm/projects/MSM_vibe` (voir `AGENTS.md`, `00_Environnement_developpement/T1000/ARCHITECTURE_FINALE.md` et `REPRISE_QUOTIDIENNE.md`). Si l'exécution est sous Windows, arrêter et demander de relancer `tr2-codex` ; ne pas piloter chaque commande par `wsl -d`. Ne pas migrer ni auditer l'environnement.

Vérifier brièvement l'état Git, préserver les changements locaux déjà effectués lors de 001/002. Lire les sources et tests ciblés ainsi que les spécifications V1 gelées. **Priorité absolue : finaliser et publier le travail RS-485 déjà réalisé dans la mission 002**, s'il existe. Sinon, réaliser une tranche logicielle concrète du firmware STM32 UART/RS-485/Modbus RTU. Corriger et tester de manière proportionnée, sans campagne d'audit. Garder la dette E4 microSD/power-loss ouverte et distincte ; aucune prétention de qualification matérielle.

## Publication et rapport — obligatoires
`allow_commit: true` et `allow_push: true` autorisent les commits et push normaux **sur `main`** pour les sources/tests/docs directement concernés et `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`. Respecter `AGENTS.md` : examiner diff, destination, secrets et état distant ; pas de reset/clean destructif, force push, merge ou rebase automatique. En cas de divergence ou de travaux non attribuables, préserver et signaler plutôt qu'écraser. Si l'état local exige de synchroniser avec la nouvelle mission, intégrer uniquement par opération sûre et non destructive.

Rédiger et **publier** `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261010-DEV-RS485-003`, résultats réellement obtenus, fichiers, tests, SHA, limites et état Git. Suivre le protocole V2-B3 pour preuve et relecture distante ; `DONE` uniquement si les critères sont remplis, sinon `BLOCKED` motivé. Ne pas demander à l'utilisateur de copier le rapport : ChatGPT le lira directement sur GitHub.

## Limites
Pas de flash, debug, câblage, manipulation physique, effacement ou changement matériel ; autorisations `allow_flash` et `allow_debug` restent `false`. Pas de changement des spécifications gelées ni de refonte hors périmètre. Les validations de migration T1000 restent acquises. N'interrompre l'utilisateur que pour un vrai blocage, une décision d'architecture ou une intervention physique indispensable.
