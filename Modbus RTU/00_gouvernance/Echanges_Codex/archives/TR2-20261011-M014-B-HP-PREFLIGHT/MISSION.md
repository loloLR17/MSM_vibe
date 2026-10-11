---
mission_id: TR2-20261011-M014-B-HP-PREFLIGHT
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# M014-B — Préflight HP de la supervision .NET

Consulter AGENTS.md, protocole de gouvernance, rapport M014-A et état courant de main. Vérifier HEAD, préserver preuves et stashes.

Contexte : M014-A est terminée, 502 tests réussis et application diagnostic .NET préparée en Windows x64 autonome sur T1000. HP Laptop 15-db1xxx sera le poste de supervision, seul maître Modbus sur COM7, 115200 8E1. T1000 reste le poste de développement. Dock T1000 non requis.

Objectif : préparer le déploiement HP et un préflight OFFLINE, sans trafic Modbus.

1. Vérifier intégrité, manifeste et SHA256 du paquet M014-A, ou le reconstruire si nécessaire. Ne pas prétendre que le ZIP local T1000 est déjà sur HP.
2. Documenter une méthode de transfert simple et vérifiable vers HP, sans supposer partage réseau, dock ou accès distant.
3. Préparer commandes PowerShell HP pour vérifier identité Windows, architecture, SHA256, présence COM7 sans ouverture, puis exécuter le diagnostic uniquement en mode OFFLINE. Aucun changement de configuration Windows ou installation de SDK requis.
4. Vérifier syntaxe et tests logiciels utiles, sans ouverture COM7, flash, reset, debug ni émission RS485.
5. Documenter les conditions GO/NO-GO d'un futur essai unique M014-C sur COM7, qui nécessitera une autorisation distincte après revue ChatGPT. Aucun essai physique dans M014-B.
6. Publier procédure, DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md ; commit et push sur main, puis vérifier la publication distante.

Autonomie logicielle et Git conformément à AGENTS.md. Préserver les gels V1 et la supervision existante. Ne solliciter l'utilisateur que pour le transfert ou les gestes HP nécessaires, sans répétition des contrôles électriques déjà validés.
