# Qualification de l'accès GitHub depuis ChatGPT — 2026-10-08

Projet : MSM — Capteur de vibration TR2.

## Faits vérifiés
- Dépôt : `loloLR17/MSM_vibe` ; branche principale : `main`.
- Accès en lecture confirmé par récupération de `AGENTS.md` sur `main`.
- Accès en écriture vérifié par création de la branche de test `test/echange-chatgpt-codex-20261008` et du présent fichier via le connecteur GitHub.
- La relecture du présent fichier sur la branche de test est le contrôle de bout en bout.

## Convention de travail
ChatGPT peut consulter les sources du dépôt et, lorsque la mission le demande, créer ou modifier des fichiers et des commits GitHub. Toujours vérifier l'état courant de `main` avant une écriture et distinguer les opérations réellement exécutées des opérations simplement disponibles. Ne pas supposer que les messages Codex arrivent automatiquement dans ChatGPT : l'utilisateur doit demander la lecture des transmissions publiées.

## Portée
Cette qualification ne démontre pas l'exécution de commandes locales WSL, le flash STM32, ni le fonctionnement d'une notification automatique. La branche de test n'affecte pas `main`.
