---
mission_id: TR2-20261008-EXCHANGE-001
status: READY
base_ref: test/echange-chatgpt-codex-20261008
base_sha: 2279dc382b45163082963ffd92b2457d7bae8531
created_at_utc: 2026-10-08
author: ChatGPT
---

# Mission pilote — retour Codex via GitHub

## Objectif

Qualifier le circuit Codex → GitHub → ChatGPT en publiant un rapport **sans toucher au firmware**.

## Périmètre

Sur la branche `test/echange-chatgpt-codex-20261008` uniquement : lire ce document et `PROTOCOLE_ECHANGE_V2.md`, relever le HEAD réel et créer `DERNIER_RAPPORT.md` dans ce même dossier. Ne pas modifier d'autre fichier. Aucune compilation, aucun flash, aucun GDB, aucune modification de configuration. Aucune action destructive.

## Critères d'acceptation

- Le rapport mentionne `mission_id: TR2-20261008-EXCHANGE-001`, `status: DONE`, branche, HEAD initial, commandes réellement exécutées et leurs résultats, et commit de code de référence (aucun changement de firmware).
- Rapport committé et poussé **uniquement sur la branche de test**, dans le cadre du GO donné pour ce pilote ; ne pas pousser sur `main`.
- Codex communique seulement le statut, la branche et le SHA du commit contenant le rapport. ChatGPT vérifiera le contenu sur GitHub.

## Arrêt / escalade

En cas de modifications locales préexistantes, de divergence Git, de branche incorrecte ou d'ambiguïté, arrêter sans écraser de données et rendre compte. Ne pas utiliser `reset --hard`, `clean`, `push --force` ou `checkout --`.
