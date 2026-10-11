---
mission_id: TR2-20261011-M014-C-READINESS
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# M014-C — Préparer et qualifier le GO pour une lecture .NET réelle sur HP

Lire AGENTS.md, protocole V2, rapports M013-J/M014-A/M014-B, procédure HP, code diagnostic actuel et état courant sur main. Vérifier HEAD et préserver stashes/preuves.

Faits confirmés par l'utilisateur sur HP : ZIP M014-A SHA256 conforme 51e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0 ; exécution diagnostic OFFLINE avec COM7/115200/8E1, réponse OFFLINE no port opened/no transmission/no log created et code retour0. L'empreinte de l'EXE extrait, l'inventaire PnP COM7 et les 370 fichiers du manifeste ne sont pas encore confirmés par un préflight intégral. Ne pas prétendre le contraire.

Architecture : HP Laptop 15-db1xxx seul maître Modbus sur COM7 via FT232R ; T1000 développement/GDB STM32. Cible STM32U575 firmware diagnostic connu, unité1 FC03 registre1000 quantité1, réponse attendue UNKNOWN0. M013-I et J ont établi une campagne antérieure conforme ; ne pas la rejouer. Aucun accès distant HP supposé.

Objectif de cette mission : établir les prérequis et les instructions exactes d'une seule lecture physique future .NET, sans transmettre de requête Modbus dans cette mission.

1. Auditer le code diagnostic .NET M014-A, les paramètres, le journal exclusif, les cas d'erreur, timeout/retry0, les commandes PowerShell et le préflight HP M014-B. Vérifier l'absence d'accès port sans --read-once.
2. Préparer un contrôle HP complémentaire, simple, sans ouverture COM7, portant sur SHA256 EXE, manifeste des 370 fichiers, identité PnP du convertisseur COM7, état du processus maître et dossier journal neuf. Distinguer port présent de port libre ; aucune méthode intrusive de vérification de disponibilité.
3. Sur T1000, si l'accès GDB à la cible est disponible, vérifier par lecture seule l'identité et l'état récent du STM32/observer et les IRQ sans CubeProgrammer HOTPLUG, reset, flash ou transmission COM7 ; comparer au dernier état connu, sans perturber un essai HP. Si indisponible, signaler clairement le manque de preuve.
4. Préparer la procédure d'une seule lecture FC03/1000/1, 115200 8E1, timeout borné, retry0, --read-once, journal CreateNew exclusif ; critères de succès et arrêt immédiat en cas de timeout, erreur ou réponse ambiguë. Ne pas exécuter cette lecture dans M014-C-READINESS. Ne pas relancer Python ni M013-I.
5. Publier un verdict de préparation GO/NO-GO motivé, les prérequis restant à vérifier sur HP, une commande d'essai réservée à une autorisation explicite ChatGPT après vérification de ces prérequis ; actualiser DERNIER_RAPPORT.md, ETAT_COURANT_TR2.md et procédure, commit/push main et relecture distante.

Aucun changement V1, gels ou firmware. Aucun flash ni reset ; GDB seulement en lecture hors trafic. Développement et Git autonomes selon AGENTS.md. L'utilisateur reste responsable de l'exécution sur HP. Ne pas demander de refaire les contrôles électriques acquis.
