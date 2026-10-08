# Protocole d'échange ChatGPT ↔ Codex — V2 (pilote)

Statut : **pilote sur branche de test**, non intégré à `main`.

## Objectif

Transmettre des missions et des résultats via GitHub sans copier-coller de longues sorties de terminal. Le dépôt réel fait foi ; chaque transmission est identifiée et vérifiable.

## Fichiers

- `MISSION_EN_COURS.md` : mission rédigée par ChatGPT, destinée à Codex.
- `DERNIER_RAPPORT.md` : compte rendu de Codex, destiné à ChatGPT.
- `archives/` : archives utiles uniquement, sans journaux volumineux systématiques.

## Contrat minimal

Chaque transmission contient : `mission_id` (identifiant unique), `status`, `base_ref`, `base_sha`, `created_at_utc`, `author` et le périmètre. Le rapport mentionne `result_sha` (commit de code évalué) et, si différent, `report_commit_sha` dans le message de retour Codex ; le SHA du commit qui contient le rapport ne peut pas être inscrit dans le rapport avant création de ce commit.

Statuts de mission : `READY`, `IN_PROGRESS`, `BLOCKED`, `DONE`, `FAILED`. Statuts de rapport : `DONE`, `BLOCKED`, `FAILED`. Ne jamais confondre rapport publié et validation technique acquise.

## Déroulement

1. ChatGPT vérifie la branche et le HEAD réels, puis publie une mission avec identifiant unique et critères d'acceptation. Il ne modifie pas le code de développement sans mission explicite.
2. L'utilisateur signale à Codex qu'une mission est disponible ; Codex fait `git fetch origin`, vérifie branche, SHA, statut Git et lit la mission. Sur une branche de test, ne pas faire de `git pull origin main` sans raison ; respecter la gouvernance Git existante.
3. Codex exécute uniquement le périmètre autorisé, conserve les modifications locales préexistantes, documente commandes, codes de retour, preuves, anomalies, décision attendue et HEAD réellement testé.
4. Codex publie son rapport et communique brièvement `mission_id`, statut, branche et commit du rapport. Il ne pousse pas de code sur `main` tant que les autorisations d'AGENTS.md en vigueur ne sont pas satisfaites.
5. L'utilisateur indique à ChatGPT : « Consulte le rapport <mission_id> ». ChatGPT lit directement le rapport et vérifie les commits et fichiers nécessaires.

## Gestion des conflits

- Un seul rédacteur par fichier de transmission à un instant donné ; ChatGPT rédige la mission, Codex rédige le rapport.
- Refuser une mission si `mission_id` ne correspond pas ou si `base_sha` n'est pas cohérent ; ne jamais interpréter un ancien rapport comme un nouveau.
- Si le dépôt local est sale, ne jamais écraser le travail préexistant.
- Aucun `reset --hard`, `push --force` ou résolution automatique de divergence.
- Les logs complets sont exclus du dépôt par défaut ; ne publier ni secrets, ni identifiants, ni données sensibles.

## Limites

GitHub n'envoie pas automatiquement les messages dans la conversation ChatGPT. Le déclenchement reste manuel (« consulte le rapport »). Le protocole pilote ne modifie pas les règles de flash, de debug, de push ou de validation d'AGENTS.md.
