---
mission_id: TR2-20261011-SSH-HP-001
status: READY
base_ref: main
base_sha: 78093692f2a2b0b6680d657ce1eb6a87ad2d472e
created_at_utc: 2026-10-11T19:54:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# SSH-HP-001 — Automatiser les essais distants sur la Brouette

## Contexte et référence
Lire AGENTS.md, PROTOCOLE_ECHANGE_V2.md, le HEAD réel de main, l'état Git local, ETAT_COURANT_TR2.md et le rapport M014-D archivé. M014-D demeure BLOCKED quant à la qualification physique de production : cette mission autonome n'en déclare pas la résolution. Le contrat précédent et son rapport ont été archivés avant remplacement.

Topologie confirmée par l'utilisateur : T1000 Dell Windows 11 (Codex CLI lancé depuis PowerShell, sources/compilations en WSL Debian, ST-LINK vers TR2) ; Brouette HP Windows 10 (supervision .NET, adaptateur USB-RS485 sur COM7 vers TR2). Les deux machines sont sur le même Wi-Fi privé. SSH Windows sur T1000 via alias `tr2-hp` vers `192.168.83.158`, utilisateur `Lolo`, clé dédiée déjà configurée ; `ssh tr2-hp hostname` et PowerShell distant sans mot de passe ont été testés manuellement avec succès. Le nom distant observé est `LAPTOP-3F6J7STJ`. Ces preuves utilisateur ne démontrent pas encore l'exécution SSH par Codex.

## Objectif
Permettre à Codex CLI sur T1000 de déclencher et analyser automatiquement, via SSH, des tests .NET/Modbus sur le HP sans copier-coller manuel. Infrastructure temporaire, minimale, sûre, réversible à l'arrivée du dock USB.

## Travail autorisé
1. Qualifier depuis l'environnement réel de Codex `ssh tr2-hp hostname`, l'exécution de PowerShell distant et les codes de retour, sans divulguer clé privée ou secrets. Vérifier identité distante, répertoire de travail, droits effectifs et accès réseau ; si le sandbox bloque SSH, rapporter précisément sans contournement.
2. Inventorier en lecture seule .NET, projet/artefacts de supervision et port COM7 sur HP, sans supposer que COM7 est disponible ou que les chemins historiques sont actifs. Vérifier les processus utilisant le port avant toute ouverture.
3. Concevoir, implémenter et tester dans le dépôt des scripts minimalistes d'orchestration T1000→HP (exécution PowerShell, transfert contrôlé d'artefacts si nécessaire, collecte logs, timeout, code retour, gestion d'erreur, exclusivité COM7). Privilégier les scripts déjà présents et les commandes non interactives ; documenter l'usage et le retrait. Ne pas installer un second Codex sur HP.
4. Commencer par tests de connectivité, de lecture et scénarios OFFLINE sans émission série. Toute campagne physique COM7 doit être explicitement bornée (nombre de requêtes, timeout, journal exclusif), non destructive, conforme aux procédures M014 et sans rejouer les commandes/jetons consommés M014-C. Ne pas déclencher une campagne Modbus physique si les prérequis et son autorisation spécifique ne sont pas établis ; préparer les commandes pour revue si nécessaire.
5. Vérifier sécurité du serveur SSH : ne pas élargir les droits Windows, ne pas modifier le pare-feu ou les services sans nécessité démontrée, ne pas enregistrer de secret. Signaler le risque du compte distant administrateur et proposer une réduction ultérieure sans imposer une refonte pour cette installation provisoire.
6. Exécuter les tests logiciels proportionnés, produire preuves réelles, conserver les données/logs volumineux hors Git. Préserver les gels V1, les stashes et tout travail existant ; aucune migration du dépôt ou changement de topologie.
7. Publier DERNIER_RAPPORT.md avec statut exact, preuves SSH/HP effectivement observées, commits et SHA, validations, limites et reprise ; vérifier la publication distante. Actualiser ETAT_COURANT_TR2.md seulement si nécessaire et sans présenter l'accès distant comme qualification physique Modbus.

## Autorisations et limites
Développement, documentation, tests logiciels, commits et push normaux sur `main` autorisés (`allow_commit: true`, `allow_push: true`). Flash/debug interdits dans cette mission. Aucun mass erase, Option Bytes, changement de câblage, reset cible, écriture Modbus, modification de firmware ou intervention physique. Respecter les restrictions de plateforme/sandbox et les conditions d'arrêt AGENTS.md. Ne pas demander de validations répétitives pour les opérations logicielles autorisées.

## Critères d'acceptation
SSH non interactif exécuté par Codex et preuves de retour ; inventaire distant vérifié ; scripts robustes et réversibles avec tests et documentation ; distinction explicite entre test OFFLINE, accès COM7 et qualification physique ; rapport publié et relu depuis GitHub. En cas d'impossibilité d'accès réseau ou de quota, statut BLOCKED honnête avec point de reprise exploitable.
