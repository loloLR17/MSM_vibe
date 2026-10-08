# Protocole d'échange ChatGPT ↔ Codex — V2-B3

Statut : **gouvernance documentaire V2-B3** intégrée sur `main` après revue V2-C. Aucune configuration permanente de Codex ni qualification STM32 n'est impliquée.

## 1. Objet et responsabilités

Le dépôt GitHub est le support asynchrone des missions et rapports. ChatGPT rédige et publie les missions ; Codex exécute les missions autorisées et publie les rapports ; ChatGPT contrôle indépendamment le résultat distant. L'utilisateur déclenche Codex puis demande à ChatGPT de consulter le rapport : **aucune notification automatique n'est démontrée**.

Le code et les règles de `AGENTS.md` en vigueur priment sur ce protocole. Ce document ne donne **aucune autorisation implicite** de push sur `main`, flash, GDB, effacement ou intervention physique.

## 2. Arborescence et propriété des fichiers

Dossier : `Modbus RTU/00_gouvernance/Echanges_Codex/`.

- `PROTOCOLE_ECHANGE_V2.md` : convention commune ; modifications sur mission dédiée.
- `MISSION_EN_COURS.md` : **ChatGPT seul rédacteur**, mission active et son identifiant.
- `DERNIER_RAPPORT.md` : **Codex seul rédacteur**, dernier résultat publié.
- `archives/<mission_id>/MISSION.md` et `archives/<mission_id>/RAPPORT.md` : copie immuable des échanges clôturés, si archivage nécessaire ; jamais écraser une archive existante.

Un seul pilote actif à la fois sur la branche partagée. La mission suivante n'est publiée qu'après vérification et clôture de la précédente. Les fichiers actifs ne constituent pas une archive historique.

## 3. Contrat de mission

En tête YAML, au minimum :

```yaml
---
mission_id: TR2-YYYYMMDD-NNN
status: READY
base_ref: <branche cible>
base_sha: <sha observé avant publication de la mission>
created_at_utc: <horodatage ISO 8601 UTC>
author: ChatGPT
target_branch: <branche explicitement autorisée>
allow_commit: false
allow_push: false
allow_flash: false
allow_debug: false
---
```

Corps : objectif, périmètre autorisé/interdit, critères d'acceptation, validation attendue, permissions explicites (commit, push, matériel), conditions d'arrêt et livrable. `base_sha` est un **point de référence**, pas nécessairement le parent immédiat du commit qui publie la mission : le comparer à l'historique, sans supposer une relation parent directe.

Les quatre permissions valent `false` si absentes ; seule une valeur `true` dans une mission validée par l’utilisateur autorise l’opération correspondante. `allow_push: true` ne dispense pas de `allow_commit: true` pour créer un commit. Les restrictions de plateforme, de sécurité, du sandbox et les exigences matérielles d’AGENTS.md restent prioritaires. L’autorisation expire à la clôture de la mission.

La mission `READY` reste figée pendant son exécution. Une correction significative impose un nouvel identifiant de mission ou une révision explicitement identifiée.

## 4. Contrat de rapport

En tête YAML, au minimum :

```yaml
---
mission_id: TR2-YYYYMMDD-NNN
status: DONE # ou BLOCKED / FAILED
base_ref: <branche de mission>
base_sha: <référence du contrat>
initial_head: <HEAD observé au démarrage>
result_sha: <SHA du code effectivement évalué, ou null si sans objet>
created_at_utc: <horodatage ISO 8601 UTC>
author: Codex
target_branch: <branche autorisée>
allow_commit: false
allow_push: false
allow_flash: false
allow_debug: false
---
```

Corps : modifications et chemins, commandes réellement exécutées avec succès/échecs, validations **réellement observées**, état Git initial/final, preuves et limites, problèmes, décision éventuelle attendue. Ne jamais présenter une opération prévue comme exécutée. Un `status: DONE` signifie critères de mission satisfaits, rapport publié et relecture distante effectuée, **pas** qualification physique ni adoption de gouvernance. Les champs de permissions du rapport recopient ceux du contrat ; le corps précise les opérations effectivement exercées.

Le SHA du commit contenant le rapport est fourni dans la réponse finale Codex après commit ; ne pas tenter d'inscrire dans le même commit son propre SHA. ChatGPT doit relever et vérifier ce commit indépendamment sur GitHub.

Cycle de clôture : publier d’abord un rapport provisoire `BLOCKED` si la preuve distante reste à obtenir, en précisant que ce statut porte sur cette attente. Après push, relire intégralement les livrables depuis la référence récupérée de GitHub et comparer leurs contenus aux fichiers publiés. Si tous les critères sont satisfaits, publier une mise à jour `DONE` consignant le SHA relu ; vérifier aussi cette dernière publication et fournir son SHA dans la réponse finale. Si un critère reste bloqué, conserver `BLOCKED`. Une présence distante seule ne démontre pas une relecture historique ; ne pas réécrire les archives pour compléter une preuve absente.

## 5. Procédure opératoire

1. **ChatGPT** lit la référence distante, contrôle la mission précédente, puis publie `MISSION_EN_COURS.md` avec nouvel identifiant et SHA de référence.
2. **Utilisateur** déclenche Codex par une courte instruction (« exécute la mission en cours »).
3. **Codex** lit `AGENTS.md`, la mission et le protocole à partir de la référence Git réelle ; vérifie `mission_id`, branche, historique, `git status` et les permissions.
4. Si le dépôt local contient des travaux préexistants, Codex les préserve ; utiliser un clone/worktree isolé lorsque cela est possible. Si Git/DNS/approbations empêchent la publication, rendre `BLOCKED` et décrire le blocage ; **ne pas contourner une restriction de sécurité**.
5. **Codex** exécute les seules actions autorisées, publie le rapport sur la branche explicitement prévue, puis vérifie sa présence sur la référence **distante** et son identifiant.
6. **Utilisateur** écrit « consulte le rapport » dans ChatGPT.
7. **ChatGPT** lit `DERNIER_RAPPORT.md` sur la branche correcte, vérifie `mission_id`, contenu et référence Git. En cas d'absence, de divergence ou de rapport périmé : statut non reçu ; ne pas conclure au succès.
8. Après clôture, archiver au besoin avant de remplacer les fichiers actifs.

## 6. Règles de sûreté et de traçabilité

- Pas de `reset --hard`, `clean`, `push --force`, écrasement de travail local ou résolution automatique d'une divergence.
- Ne jamais inclure de secrets, tokens, données personnelles ou logs volumineux dans ce dépôt public.
- Les écritures sur `main` nécessitent les autorisations actuellement applicables ; un pilote de transmission ne les remplace pas.
- Les accès matériels suivent intégralement `AGENTS.md` ; aucune opération matérielle dans les missions de qualification V2-A.
- Distinguer toujours lecture distante, écriture locale, commit local, push et **relecture distante**.
- Les codes de retour des commandes regroupées ne prouvent pas le succès de chacune des sous-commandes.
- Une erreur de réseau ou de permission doit être rapportée comme telle ; ne pas annoncer un push réussi sans preuve distante.

## 7. Critères de qualification V2-A

Deuxième mission pilote : ChatGPT publie une nouvelle mission, Codex récupère la bonne version, produit un rapport distinct du premier, publie et relit la référence distante ; ChatGPT constate indépendamment la correspondance de `mission_id` et la traçabilité. Aucune modification firmware, de `main` ou de configuration Codex.

**État documentaire au 08/10/2026** : les rapports des pilotes `TR2-20261008-EXCHANGE-001` et `TR2-20261008-EXCHANGE-002` sont archivés. Leur publication et relecture Codex sont rapportées dans ces documents ; cela ne suffit pas à affirmer un contrôle indépendant ChatGPT du second pilote. Les critères historiques V2-A ci-dessus restent la référence de cette qualification. La V2-B3 consolide le protocole et fait l'objet d'une intégration documentaire V2-C sur `main`. V2-B1 reste **BLOCKED** pour la qualification comparative des configurations ; aucune qualification STM32 n’en découle.
