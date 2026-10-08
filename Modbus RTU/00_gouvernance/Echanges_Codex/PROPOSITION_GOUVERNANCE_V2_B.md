# V2-B — Proposition de gouvernance et d'autonomie Codex

**Statut : V2-B3 candidate pour revue — aucune adoption dans main.** Branche : `test/echange-chatgpt-codex-20261008`. Ce document décrit la candidate `AGENTS.md` de la branche expérimentale ; il ne confère aucune permission et ne modifie ni `main` ni la configuration Codex.

## 1. Références contrôlées

- Référence historique main V1 : blob AGENTS `864c9f062f1b0636e5c92c29e4b0d83bc68ff341`. La branche expérimentale porte désormais la candidate V2-B3, sections 1 à 18 ; son contenu et ses SHA doivent être relevés à chaque mission.
- Pilotes d’échange 001 et 002 : rapports archivés, publication/relecture Codex rapportées ; aucune nouvelle preuve de contrôle indépendant ChatGPT du second pilote n’est produite ici.
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

## 3. Politique de permissions de la candidate (à revoir avant adoption)

- **Git** : tout commit exige `allow_commit: true`, absent ou false interdit le commit. Une mission autorisant explicitement `allow_push: true` peut permettre à Codex de pousser ses propres commits sur `target_branch` après contrôle des tests et du diff ; interdiction d'inclure des fichiers préexistants hors mission. `main` reste protégé par validation de mission et garde-fous Git.
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

## 5. Décisions distinctes et conditions d'adoption

1. **Adoption documentaire** : revoir AGENTS V2-B3, le protocole et cette proposition. Une mission d’intégration distincte doit autoriser explicitement `target_branch: main` et préciser la version et sa portée. Aucune fusion n’est autorisée par V2-B3.
2. **Configuration Codex** : V2-B1 reste **BLOCKED**. C1/C2/C3 ne démontrent pas les sessions indépendantes opérationnelles et leur attribution complète ; C4 reste limitée à sa portée rapportée. Compléter les preuves sous mission dédiée, ou décider explicitement de différer cette qualification pour une adoption documentaire séparée. Aucune configuration permanente n’est qualifiée ou modifiée ici.
3. **Qualification STM32** : qualifier et documenter séparément les chaînes flash/debug, puis obtenir les permissions matérielles explicites par mission. Les aides d’outils et les échanges Git ne qualifient aucune chaîne matérielle.
4. **Journal technique** : la section locale est reprise dans AGENTS §18 avec ses garanties. Le journal lui-même reste local et non versionné dans cette branche ; son éventuelle publication relève d’une autre mission, sans importer les travaux firmware locaux.

## 6. Décisions attendues

- Revue humaine de la candidate et décision séparée sur une éventuelle intégration dans main.
- Compléter ou différer explicitement la qualification comparative V2-B1, sans convertir son statut BLOCKED en succès.
- Décider séparément de la configuration permanente, de la publication du journal et de la qualification matérielle.

La priorité des restrictions de plateforme, sécurité, sandbox et des protections matérielles est explicite dans AGENTS §1. Le protocole ne peut étendre les permissions. Les push autorisés expirent à la clôture de chaque mission après contrôle distant.
