# V2-B — Proposition de gouvernance et d'autonomie Codex

**Statut : PROPOSITION POUR REVUE — non applicable.** Branche : `test/echange-chatgpt-codex-20261008`. Ce document n'altère ni `AGENTS.md`, ni `main`, ni `~/.codex/config.toml`.

## 1. Références contrôlées

- `AGENTS.md` sur la branche expérimentale : blob `864c9f062f1b0636e5c92c29e4b0d83bc68ff341`, gouvernance V1, sections 1 à 16.
- Pilotes d'échange 001 et 002 : rapports publiés et relus ; le deuxième rapport porte `mission_id: TR2-20261008-EXCHANGE-002`.
- Observations d'audit antérieures à confirmer avant toute généralisation : `workspace-write` + `approval_policy=never` a bloqué une écriture par connecteur nécessitant une approbation ; un clone isolé et un push ont ensuite réussi lors d'une session plus permissive. Ce résultat ne qualifie pas une politique permanente de sandbox.

## 2. Analyse des écarts et propositions précises

| Section V1 | État V1 | Proposition V2-B | Risque / garde-fou |
|---|---|---|---|
| §1 Contrat | Permissions par mission | Exiger des champs explicites `allow_commit`, `allow_push`, `allow_flash`, `allow_debug`, `target_branch` ; valeurs par défaut `false` | Aucune permission implicite |
| §2 Source de vérité | `git pull --ff-only origin main` obligatoire avant toute modification | Distinguer travail sur `main` et mission sur branche dédiée ; `fetch`, `status`, HEAD et ancêtre vérifiés ; pas de pull de `main` dans une branche de test isolée | Préserver travail sale et éviter fusion accidentelle |
| §7 Push | Autorisation explicite utilisateur par push | Autorisation explicite **dans le contrat de mission validé par l'utilisateur**, couvrant des push répétés sur la seule branche autorisée ; pas de push hors périmètre | Pas de force push, contrôle du diff indexé, état distant avant/après |
| §8 Autonomie | Tests/build/corrections autonomes | Ajouter cycles autonomes Git autorisés, rédaction/publication de rapports et vérification distante | Escalade si divergence, permissions, conflit ou échec non résolu |
| §9–12 Matériel | Flash et debug après qualification explicite | **Conserver les protections** ; formaliser identifiants de cible, outil, artefact, commande qualifiée, périmètre, conditions d'arrêt et traces ; GDB interactif autorisé seulement après qualification | Mass erase, Option Bytes, boot persistant et actions physiques toujours sous accord spécifique |
| §14 Rapport | Rapport final technique | Ajouter `mission_id`, SHA du code évalué, branche, commit de rapport et preuve de relecture distante ; distinguer opérations tentées, réussies et prévues | Rapport DONE seulement après publication vérifiée |
| Nouvelle annexe | Aucune procédure d'échange | Référencer `PROTOCOLE_ECHANGE_V2.md` sans copier ses règles dans `AGENTS.md` | Une seule source par règle |

## 3. Politique de permissions proposée (à discuter)

- **Git** : une mission autorisant explicitement `allow_push: true` peut permettre à Codex de pousser ses propres commits sur `target_branch` après contrôle des tests et du diff ; interdiction d'inclure des fichiers préexistants hors mission. `main` reste protégé par validation de mission et garde-fous Git.
- **STM32** : `allow_flash: true` ne vaut que pour la procédure standard **documentée et qualifiée** et pour la cible identifiée ; l'autorisation de flash ne couvre ni erase étendu ni Option Bytes.
- **GDB** : `allow_debug: true` autorise une session persistante et des commandes interactives dans le périmètre documenté après qualification de la chaîne ; une reprogrammation depuis GDB suit les règles de flash.
- **Interventions physiques** : exclusivement humaines ; aucune déduction d'une autorisation logicielle.
- **Blocages** : ne jamais contourner un contrôle d'approbation ou une restriction du sandbox ; rendre compte, puis adapter la configuration avec décision humaine.

## 4. Configuration Codex : matrice de qualification, sans modification

| Essai | Sandbox | Politique d'approbation | Objet | Critère |
|---|---|---|---|---|
| C1 | `workspace-write` | `on-request` | Lecture Git, clone isolé, push limité à branche test | Réussite sans blocage inattendu ; noter approbations |
| C2 | `workspace-write` | `never` | Lecture et outils non sensibles seulement | Identifier limites réelles, sans demander de contournement |
| C3 | `danger-full-access` | `on-request` | Même pilote Git isolé | Mesurer gain fonctionnel et surface de risque |
| C4 (optionnel) | `danger-full-access` | `never` | **Pas de test matériel** ; seulement commandes de diagnostic non destructives | Qualifier comportement des outils, sans retenir ce mode par défaut |

**Préférence provisoire** : le mode le moins permissif permettant le travail requis, avec autorisations explicites par mission. Le fait qu'une commande Windows fonctionne en accès complet n'autorise pas à rendre ce mode permanent.

## 5. Conditions d'adoption

1. Revue par l'utilisateur de la présente proposition et des changements ciblés d'`AGENTS.md`.
2. Test isolé de la matrice, preuves et nombre d'interruptions documentés.
3. Vérification de la procédure de flash et de debug réellement qualifiée **avant** autorisation matérielle autonome.
4. Préparation d'un diff `AGENTS.md` V2 sur branche expérimentale, revue avant fusion.
5. Intégration à `main` et modification de la configuration Codex **uniquement après décision explicite**.

## 6. Décisions attendues

- D1 : accepter le principe du push autonome limité à la branche et au contrat de mission.
- D2 : conserver les autorisations flash/GDB conditionnées à la qualification documentée de la chaîne et à une mission explicite.
- D3 : valider une phase de tests comparatifs de configuration sans changer les paramètres permanents.
