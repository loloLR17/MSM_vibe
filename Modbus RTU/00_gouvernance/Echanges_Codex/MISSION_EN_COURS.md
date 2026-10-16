---
mission_id: TR2-20261016-R4Q-LIVE-B3-RTU-COMPOSITION
status: READY
base_ref: main
base_sha: 48805fe638151a33f2e44f17eb6da01a00e91efa
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-Q — Composition incrémentale LIVE B3 / transport RTU (préparation R6)

## Objectif
Après R4-P DONE (coexistence physique bornée FRAM/FIFO), raccorder progressivement le propriétaire d'acquisition continue et la métrologie LIVE R3-B au modèle de registres B3 et au chemin Modbus RTU successeur, sans perdre les budgets, la sûreté transactionnelle ni les acquis V1. Démontrer d'abord logiciellement le flux cohérent acquisition -> calcul -> image RAM B3 -> lecture RTU et l'ingress B5 différé. Effectuer seulement les validations physiques accessibles et sûres ; ne pas annoncer R6 bout en bout, PC/SQLite/Web ou qualification industrielle.

## Préconditions
Lire AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, ETAT_COURANT_TR2, roadmap A-06, normes LIVE_R0, implémentations R1/R2/R3/R4 et preuves R4-P, et état réel de main. Le rapport R4-P est DONE même si son ancien contrat reste READY : ne pas relancer R4-P. Examiner les interfaces et la composition réelle avant de choisir la solution minimale ; préserver les contrats gelés V1 et la configuration M016 de production. Préserver le snapshot FRAM physique final epoch4/run4 SHA256 a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9 et la Flash initiale M016 SHA256 c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26.

## Travaux attendus
1. Faire une analyse d'écarts ciblée : producteur FIFO, temps événement et UTC, métrologie/fenêtres R3-B, publication B3, image RAM cohérente, B0 version/profil, B5 transactionnel, ordonnanceur coopératif, UART IRQ et transport RTU. Définir une frontière claire entre acquisition/FRAM et réponses Modbus ; aucune lecture FRAM synchrone depuis callback RTU ou IRQ, aucune réentrée du moteur R2.
2. Implémenter par incréments le raccordement nécessaire et démontrable. Publier les fenêtres B3 à partir d'instantanés RAM atomiques/cohérents avec validité, fraîcheur, qualité, run/config/génération et temps observé, sans valeur inventée. Garder la cadence et les invalidations de R3-B, la version LIVE distincte de V1 et les limites du mapping R0. Les lectures RTU doivent exploiter des snapshots publiés, ne pas bloquer la FIFO. Capturer les commandes B5 et les faire traiter hors ISR dans le contexte propriétaire, avec admission différée, idempotence, reçus durables et STOP prioritaire. Si un raccordement est impossible sans choix normatif, limiter le lot, documenter BLOCKED ou PARTIAL et proposer une décision précise plutôt que d'inventer un contrat.
3. Qualifier par tests hôte ciblés : images B3 cohérentes sous mises à jour, lecture multi-registres sans déchirure, profil B0 et V1 inchangés, fenêtres et horodatage, B5 répété/perte/reprise, overflow UART, files saturées, déconnexion, STOP et absence d'accès FRAM en contexte IRQ/RTU. Tester la coexistence charge RTU+FRAM+FIFO par modèle de budgets ; ne pas confondre simulation et cible. Exécuter builds ARM, tests de régression pertinents, sanitizers proportionnés ; ne pas contourner les défauts.
4. Si le raccordement logiciel est stable et si le banc physique TR2-001 est disponible sans intervention utilisateur, mener un essai matériel borné selon AGENTS : vérifier identité UID/STLINK, capteur arrêté, backups vérifiés Flash/FRAM et empreintes actuelles avant flash ; utiliser un profil explicite NON-PRODUCTION, image hashée et méthode qualifiée. Privilégier lecture/diagnostic sans nouvelle transaction FRAM ; toute commande réelle qui écrit sur FRAM exige état journal vérifié et protection anti-rejeu, avec arrêt au premier indéterminé. Mesurer DWT/HCLK, quanta de travail <=1000us, service <=2500us, gap FIFO <=5ms en continu, niveaux/pertes, IRQ UART/TX, latences de réponse et STOP. Si RS-485 physique non raccordé, ne pas prétendre tester l'électrique : essais host/loopback seulement, report explicite à R6. Ne pas modifier le câblage ni solliciter une action physique sauf indispensable.
5. Restaurer l'image initiale vérifiée après probe non-production et préserver FRAM finale réelle (pas de rollback des epochs/runs) ; audit des partitions historiques/tail, journal et CRC. Si aucun essai physique sûr n'est possible, livrer la tranche logicielle et ses preuves sans forcer une qualification matérielle.

## Critères d'acceptation
Code et tests montrent un chemin LIVE B3/RTU cohérent et borné, commandes B5 non réentrantes, invariants transactionnels conservés, V1 bit-à-bit ou démonstration de non-régression adaptée ; métriques et limites chiffrées, preuves des échecs aussi. La qualité du temps et les données invalides sont visibles, non masquées. Tout dépassement, perte non signalée, risque de réécriture FRAM, panne SPI/IRQ non maîtrisée ou ambiguïté normative stoppe le sous-essai concerné. Distinguer explicitement DONE logiciel, PARTIAL, BLOCKED et qualification physique R6 non acquise. Aucun SD, HP, réécriture normative R0, refonte générale ni extension PC R5 dans cette mission.

## Livrables et autonomie
Commits et push normaux sur main, preuves brutes/mesures et rapport DERNIER_RAPPORT.md, état courant et roadmap factuels mis à jour ; revue indépendante ChatGPT après publication. Permissions allow_commit/push/flash/debug true selon gouvernance, mais flash/debug seulement si nécessaires et sous garde-fous matériels. Codex choisit les solutions techniques minimales et exécute de façon autonome ; préserver worktrees/stashes et archives R4-P. L'utilisateur ne réalise que les éventuelles interventions physiques ou décisions structurantes.
