---
mission_id: TR2-20261008-EXCHANGE-002
status: READY
base_ref: test/echange-chatgpt-codex-20261008
base_sha: 20e695e75181b6738d16f9d10bcd2b73f91a724a
created_at_utc: 2026-10-08T18:00:00Z
author: ChatGPT
---

# V2-A — Deuxième qualification du circuit de transmission

## Objectif
Vérifier qu'une nouvelle mission remplace la précédente sans confondre son rapport avec le rapport historique ; confirmer l'archivage et la vérification distante du livrable.

## Préconditions
Lire `AGENTS.md`, `PROTOCOLE_ECHANGE_V2.md`, cette mission, les deux fichiers d'archives `archives/TR2-20261008-EXCHANGE-001/` et l'ancien `DERNIER_RAPPORT.md`. Contrôler les identifiants et la branche. `base_sha` est la référence distante avant publication de la présente mission, et non son parent présumé après d'autres commits.

## Périmètre et autorisations
- **Seul fichier modifiable** : `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` sur la branche `test/echange-chatgpt-codex-20261008`.
- Autorisation explicite : créer un commit et pousser ce rapport **uniquement sur cette branche de test**, puis vérifier la lecture distante.
- Interdits : modification de `main`, du firmware, de `AGENTS.md`, des archives, du protocole, des paramètres Codex ; aucun build, flash, GDB ou intervention physique.
- Préserver tous les travaux locaux ; privilégier un clone isolé si nécessaire. Pas de force push, reset destructif ou contournement d'une approbation requise.

## Livrable
Remplacer `DERNIER_RAPPORT.md` par un rapport V2-A propre avec en-tête YAML :
`mission_id: TR2-20261008-EXCHANGE-002`, `status: DONE` si et seulement si la publication distante est vérifiée ; `base_ref`, `base_sha`, `initial_head`, `result_sha` (ou `null` si sans objet), `created_at_utc` ISO UTC, `author: Codex`.

Décrire les preuves effectives : HEAD initial, état local, méthode de publication, commandes exécutées et résultats, vérification de la branche et de la relecture distante, absence de changement firmware. L'ancien rapport reste archivé et ne doit pas être modifié.

## Critères d'acceptation
1. Le rapport distant porte le **nouvel identifiant** et non celui du pilote 001.
2. Les archives 001 restent présentes et inchangées.
3. La branche `main` et le firmware restent intacts.
4. Codex indique dans sa réponse finale le statut, la branche et le SHA du commit du rapport.

En cas de blocage, ne pas déclarer DONE ; indiquer précisément la restriction et les éléments déjà réalisés.
