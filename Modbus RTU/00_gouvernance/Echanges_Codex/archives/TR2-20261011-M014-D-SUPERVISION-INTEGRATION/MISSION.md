---
mission_id: TR2-20261011-M014-D-SUPERVISION-INTEGRATION
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-D — Integrer la supervision operationnelle jusqu'a la limite du quota Codex

Consulter AGENTS.md, gouvernance V2, roadmap M014, rapport M014-C DONE et etat courant reel de main. Verifier HEAD, arbre Git, stashes et preuves avant action. Ne pas rejouer l'essai unique M014-C ni la campagne M013-I.

Objectif : faire progresser en autonomie la connexion de la supervision .NET au TR2 physique, au-dela du diagnostic FC03/1000/1, jusqu'aux lectures de blocs systeme V1, polling maitrise, detection des pertes et reprise, sans modifier les gels V1. Regrouper les travaux coherents pour limiter les aller-retours.

1. Auditer les ecarts reels entre le firmware STM32 diagnostic actuellement flashe, le serveur Modbus RTU applicatif de production, les autorites runtime, le mapping V1 B0/B1 et le service .NET S0-S7. Distinguer fonctions developpees, simulees et effectivement qualifiees physiquement. Publier une matrice de dependances et des criteres de validation.
2. Implementer et tester les raccordements firmware/runtime et supervision necessaires dans le depot, par tranches internes autonomes. Privilegier les tests unitaires, integration, cross-build ARM et tests .NET existants ; ajouter des tests sur lecture de blocs, identification, validite, timeout, deconnexion/reprise, sans masquer les erreurs ni inventer READY ou DeviceId.
3. Autorisations : developpement, commits et push main autonomes ; flash et debug STM32 autorises dans le cadre AGENTS.md apres verification cible, image et sauvegardes pertinentes. Interdire mass erase, Option Bytes, deprotection, changement de cablage ou modification des gels. Ne pas flasher si le diagnostic qualifie risque d'etre perdu sans restauration controlee.
4. Le HP reste seul maitre physique sur COM7 et n'est pas accessible a Codex a ce stade. Ne jamais pretendre avoir execute un essai HP. Preparer les procedures d'essais HP minimales, non destructives et sans boucle de tentatives. Attendre la revue ChatGPT et l'intervention utilisateur pour les essais physiques HP ; ne pas reutiliser le marqueur ou la commande consommee M014-C. Ne pas faire de GDB pendant un essai HP.
5. Si un blocage materiel ou une decision structurante apparait, documenter clairement et poursuivre les autres travaux logiciels independants possibles. Eviter les micro-missions et les validations repetitives. La limite de quota Codex n'est pas un motif pour perdre des travaux : committer/pusher les tranches valides, conserver les preuves, noter les travaux incomplets et les risques.
6. Avant arret ou epuisement du quota, publier DERNIER_RAPPORT.md et mettre a jour ETAT_COURANT_TR2.md avec HEAD exact, tests reellement executes, fichiers modifies, statut M014 (ne pas declarer M014 DONE avant lectures periodiques physiques et reprise qualifiees), blocages et instructions de reprise. Verifier la publication distante. Si quota interrompt avant publication, ne pas affirmer la cloture et conserver un point de reprise local explicite.

La validation du diagnostic M014-C est acquise uniquement pour une lecture nominale. Les qualifications timing RS485, DE/TC, robustesse et industrielle demeurent ouvertes. Pas d'acces distant HP configure dans cette mission ; son etude est differee apres validation de la supervision.
