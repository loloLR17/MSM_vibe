---
mission_id: TR2-20261010-DEV-RTU-INTEGRATION-004
status: READY
base_ref: main
base_sha: 142452b6171bc13f078eace64ae57e9e25565e3c
created_at_utc: 2026-10-10T16:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2 — Intégration fonctionnelle Modbus RTU STM32, tranche élargie

## Intention et autonomie
Poursuivre concrètement la construction du firmware industriel TR2 après la réception RTU de la mission 003. **Mission volontairement large : Codex décide et enchaîne de façon autonome les sous-étapes cohérentes**, avec tests, corrections, commits et push sur `main`. Pas de micro-missions ni de demandes d'accord intermédiaires pour le travail logiciel courant. L'utilisateur ne doit intervenir que pour déclencher Codex et pour les manipulations physiques ; escalader un vrai blocage ou une décision changeant le contrat normatif. Autorisation permanente de travail logiciel consignée dans `AGENTS.md`.

## Contexte de départ
Session Codex **native Debian WSL**, dépôt canonique `/home/lolo/dev/msm/projects/MSM_vibe`. Lire `AGENTS.md`, `Echanges_Codex/PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, freezes P12-E/F/G et spécification V1, ainsi que le code/tests utiles. HEAD distant de départ observé : `142452b6171bc13f078eace64ae57e9e25565e3c`. Mission 003 terminée : boucle STM32 consomme les événements UART, récepteur RTU/CRC et compteurs diagnostiques, mais pas encore de serveur PDU de production ni réponse RS-485. Ne pas requalifier T1000 ou refaire les audits déjà validés. Préserver le stash historique 002 et les travaux locaux préexistants.

## Objectif de livraison — résultat intégré, pas une étude
Amener le firmware STM32 de son état « réception RTU diagnostique » à une **chaîne Modbus RTU applicative cohérente**, aussi complète que possible sans matériel et sans inventer des paramètres électriques/pinout. Codex doit examiner les composants portables existants (serveur RTU, registre, services, runtime, transports, tests) et les **réutiliser**, non les dupliquer.

1. Composer dans le harness/runtime STM32 les objets et services de production nécessaires à la réception et au traitement d'une requête Modbus RTU valide, conformément aux spécifications V1 gelées et aux contrats P12 existants. Gérer proprement adresse esclave, CRC, fonctions prises en charge, exceptions, erreurs, trames invalides et états de service, sans comportement improvisé.
2. Mettre en place la voie de réponse (encodage ADU, émission par transport série existant, arbitrage TX/RX et contrôle DE//RE) **uniquement si** la documentation matérielle et les abstractions du dépôt permettent de la réaliser sans supposition. Si DE//RE, câblage ou configuration cible ne sont pas établis, isoler proprement cette dépendance, livrer et tester toute la chaîne portable possible et signaler précisément le prérequis physique ; ne pas produire de fausse implémentation.
3. Renforcer la testabilité des nouvelles compositions et chemins d'erreur avec des tests hôte ciblés, y compris cas de trame correcte, CRC invalide, adresse non destinée au TR2, exception et défaut transport lorsque ces cas sont applicables. Ajouter les tests d'intégration proportionnés. Corriger les anomalies directement liées, sans modifier les freezes.
4. Préparer un plan d'essai physique RS-485 court, exécutable ensuite sur TR2 avec matériel disponible, comprenant câblage/prérequis vérifiés, outils, commandes, observations et critères de réussite. **Ne pas flasher, déboguer ni effectuer de manipulation physique dans cette mission.**
5. Mettre à jour `ETAT_COURANT_TR2.md` pour les seuls progrès avérés ; maintenir **E4 microSD/power-loss ouverte**, sans travaux E4 opportunistes.

## Liberté d'exécution
Codex peut organiser plusieurs commits fonctionnels cohérents, corriger et retester de manière autonome, puis publier sur `main` avec `allow_commit: true` / `allow_push: true`. Vérifier brièvement l'état Git initial et distant, respecter les spécifications et préserver les travaux préexistants. Compiler STM32 avec le SDK Linux déjà qualifié, exécuter les tests hôte nécessaires et contrôler les régressions pertinentes. Ne pas relancer par défaut les 466 tests de supervision ni les campagnes sans lien avec la tranche. Pas de requalification T1000.

## Critères de fin et preuve
Livrer **du code intégré, des tests pertinents, des commits publiés et un rapport distant**. Les parties non réalisables sans preuve matérielle doivent être explicitement distinguées du travail logiciel terminé. Ne pas présenter cross-build comme preuve de communication réelle. Si la réponse matérielle est bloquée, terminer et publier le périmètre portable utile plutôt que bloquer l'ensemble. Si un invariant gelé est incompatible, s'arrêter sur ce point et rapporter les éléments concrets ; ne pas réécrire silencieusement la spécification.

Codex est seul rédacteur de `Echanges_Codex/DERNIER_RAPPORT.md` ; y publier le rapport de **`TR2-20261010-DEV-RTU-INTEGRATION-004`**, SHA du code, fichiers, tests effectivement exécutés, décisions techniques, limites matérielles, prochaine étape et vérification de publication distante selon V2-B3. ChatGPT lira ce rapport directement sur GitHub. Pas de copie manuelle à demander à l'utilisateur.

## Garde-fous
Pas de flash/debug, manipulation physique, effacement, changement d'Option Bytes, force push, suppression destructrice ou changement des spécifications gelées. Les droits d'accès du processus restent ceux effectivement accordés par la plateforme ; ne pas les contourner.
