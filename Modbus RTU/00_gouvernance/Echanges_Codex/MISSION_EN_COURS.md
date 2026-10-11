---
mission_id: TR2-20261011-M013-I-HP-CAMPAIGN-GO
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-I — Préparer GO campagne HP bornée

Relire AGENTS.md, rapport H, plan G, scripts campagne et état courant sur main. L'utilisateur confirme que COM7 est disponible sur le HP Laptop 15-db1xxx. Préflight HP H réussi : Python 3.13.3, pySerial 3.5, environment_ready true, offline et mock OK, fichiers hachés conformes, aucune transmission. Ne pas refaire le préflight inutilement. Un premier échange RTU a été corroboré en F. La qualification temporelle physique reste non faite.

Objectif : préparer le GO ou NO-GO d'une seule campagne de 12 requêtes maximum, sans lancer de trafic pendant cette mission.

1. Vérifier HEAD main, état Git, rapports, sources et conditions de sûreté du script ; préserver stashes et preuves.
2. Contrôler avec GDB seulement l'image STM32 exacte, NVIC sain, UART RX armé, PG4 bas et baseline des compteurs ; ne pas utiliser CubeProgrammer HOTPLUG. Ne pas flasher ni reset sans besoin établi.
3. Revoir les cas nominaux, CRC incorrect et adresse étrangère, délais, gestion écho, journaux et arrêts. Confirmer la commande PowerShell HP exacte utilisant le dossier préflight existant : C:\Users\Lolo\AppData\Local\Temp\TR2-M013-H-696f8b55f09242cfaa38ff397de54256. Ne pas supposer d'autres équipements sur le bus. La disponibilité COM7 est confirmée par l'utilisateur ; la topologie maître unique reste une condition explicite.
4. Publier un GO ou NO-GO justifié, avec commande exacte prête à copier si GO. L'utilisateur exécutera la campagne une seule fois après contrôle indépendant ChatGPT du rapport. Aucun accès COM7 ni émission RS485 dans I.
5. Mettre à jour DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md, commit et push main puis vérifier publication distante. Aucune modification V1 ni revendication de qualification temporelle.

Autonomie autorisée pour développement, tests, commit/push, flash/debug selon AGENTS.md ; aucun essai physique ni écriture Option Bytes/mass erase. Limiter les demandes utilisateur aux gestes réellement nécessaires.
