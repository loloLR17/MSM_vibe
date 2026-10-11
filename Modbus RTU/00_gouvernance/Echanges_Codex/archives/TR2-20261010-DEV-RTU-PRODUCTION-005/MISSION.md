---
mission_id: TR2-20261010-DEV-RTU-PRODUCTION-005
status: READY
base_ref: main
base_sha: ff19cf40a017233b967b95fea3365bad0f631e11
created_at_utc: 2026-10-10T15:30:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2 — Raccordement runtime de production et préparation qualification RS-485

## Mission large, autonome, orientée résultat

Poursuivre la mission 004 (rapport DONE, code `92c75f6a`, 103/103 tests et cross-build) en livrant une tranche fonctionnelle substantielle du firmware TR2, sans demander de micro-validations. Codex choisit les sous-étapes et leur ordre, code, teste, corrige, commit et push sur `main` de façon autonome. L'utilisateur ne doit intervenir que pour lancer cette mission, puis pour les gestes physiques ultérieurs. Appliquer `AGENTS.md` et le protocole d'échange V2-B3.

## Environnement et faits matériels vérifiés

Codex natif Debian, clone canonique `/home/lolo/dev/msm/projects/MSM_vibe`, SDK STM32CubeU5 déjà qualifié. Vérifier HEAD distant et état local sans audit T1000, préserver tout travail préexistant.

**Inventaire utilisateur désormais consigné dans `Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md` :** TR2 de test équipé d'un **ADM2587E sur carte MIKROE-3863** ; un seul exemplaire de cette carte, plus deux exemplaires du second modèle nommé **ADM2867E** dans le projet pour les futurs deux TR2/supervision multi-TR2. **Câble USB–RS485 reçu, jamais connecté au PC ni au TR2.** Vérifier les désignations exactes et les documentations constructeur avant de fixer une interface ; ne jamais supposer que les deux modèles ont le même pinout, tension ou commande DE//RE.

## Livrables de développement attendus

1. **Runtime STM32 durable :** faire progresser concrètement le boot et la composition applicative de production vers un `SystemRuntime` persistant en mémoire, avec autorités et services réellement disponibles, sans recycler comme objets durables les fixtures temporaires du harness. Réutiliser les services, contrats et architectures gelés ; choisir un incrément réellement compilable et testable, plutôt qu'une architecture théorique.
2. **Commandes B5 :** intégrer les mécanismes existants de validation, admission, rejet, persistance et dispatch conformément aux spécifications V1 ; combler les raccordements manquants de façon ciblée, notamment soumission invalide/ID 0/code 14 et annulation, sans simuler une acceptation. Ne pas inventer de comportement ni modifier les freezes. Si une dépendance de production empêche l'activation, isoler explicitement cette limite et continuer le travail logiciel utile.
3. **RS-485 sur montage réel :** examiner les documents et fichiers de conception pertinents au montage **ADM2587E / MIKROE-3863**, ainsi que le transport STM32 existant (PG7/PG8, candidats PG4/PG5, VDDIO2). Établir une matrice « prouvé / à mesurer / inconnu » pour alimentation, niveau logique, pinout, DE//RE, isolation, terminaison, polarisation, masses et sens A/B. Ne pas confondre ADM2587E avec ADM2867E. Développer des abstractions et tests de pilotage direction/TX/RX uniquement si les prérequis sont établis ; sinon ne pas activer de sortie dangereuse ni inventer de câblage.
4. **Préparation des essais :** fournir une procédure courte, progressive et exécutable pour identifier et tester d'abord le câble USB–RS485 **sur le PC seul** (identification USB, driver, port, paramètres), puis préparer un second jalon de raccordement électrique au TR2 après vérification du montage réel. Aucune connexion, émission, flash ou manipulation physique pendant cette mission. Préparer également la future validation avec deux TR2, sans présumer qu'ils existent déjà.
5. **Tests et livraison :** tests hôte ciblés, tests d'intégration pertinents et cross-build ARM, correction autonome des régressions, journal `ETAT_COURANT_TR2.md` mis à jour selon preuves. Ne pas relancer sans motif la supervision .NET. Maintenir la dette **E4 microSD/power-loss** ouverte.

## Critères de sortie et publication

La mission doit livrer du code réel, tests et résultats, plus le document de qualification matérielle/préparation. Distinguer `VALIDÉ hôte`, `COMPILÉ STM32`, `PRÉPARÉ matériel` et `NON QUALIFIÉ physique`. Si un composant ne peut pas être intégré sans mesure ou décision matérielle, livrer les autres éléments autonomes et signaler précisément le prérequis.

Permissions : `target_branch: main`, `allow_commit: true`, `allow_push: true` pour développement, tests, docs et rapport. Commits/push normaux, sans force push ni opération destructrice ; préserver stash et travaux locaux. `allow_flash: false`, `allow_debug: false` ; pas de câblage, effacement, Option Bytes, manipulations matérielles ou changement des spécifications gelées.

Codex publie `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` avec `mission_id: TR2-20261010-DEV-RTU-PRODUCTION-005`, résultats, tests, SHA, limites et preuve de relecture distante selon V2-B3. ChatGPT récupérera le rapport directement sur GitHub. Ne pas demander à l'utilisateur de le recopier.
