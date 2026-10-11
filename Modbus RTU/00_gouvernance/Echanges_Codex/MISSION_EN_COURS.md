---
mission_id: TR2-20261011-M013-J-POST-CAMPAIGN-AUDIT
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-J — Audit STM32 après campagne Modbus HP

Consulter AGENTS.md, protocole, rapport I, état courant et firmware sur main.

Le HP, seul maître Modbus selon l'utilisateur, a exécuté une campagne physique unique sur COM7. Journal utilisateur M013-I-physical.jsonl : 12 écritures de 8 octets, 10 réponses nominales exactes 01 03 02 00 00 B8 44, une requête CRC invalide sans réponse, une requête adresse 2 sans réponse, complete passed true, durée 13,3657 secondes. SHA256 annoncé du journal : 23f463fb241c7378c46a79af0a43bce122511799cd1066836a787687c629bed4. Le journal est joint à la conversation ChatGPT et non présumé accessible sur T1000. Aucun nouvel essai HP.

Mission : vérifier indépendamment les compteurs et l'état STM32 après campagne, puis documenter la concordance avec le journal HP.

1. Vérifier HEAD main, gouvernance, rapport I et baseline GDB conservée, stashes et sauvegardes.
2. Lire la cible avec GDB uniquement, sans CubeProgrammer HOTPLUG, reset, flash, ni émission COM7. Vérifier firmware exact, IRQ49/66, UART RX armé, PG4 bas, absence de fautes et observer brut.
3. Comparer les deltas à la baseline I : RX96 octets, 12 trames, CRC valides11, CRC incorrect1, adresse étrangère1, acceptées10, TX démarrées et complétées10, erreurs/drop/failsafe0. Si divergence, expliquer sans inventer. Préserver logs et dumps.
4. Publier rapport de confrontation, limites de mesure temporelle, verdict de clôture M013 et proposition M014 supervision PC si concordant. Actualiser DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md, commit/push main, vérification distante.
5. Aucune modification V1 ni nouveau test physique. Autorisations de développement, commit/push, debug et flash selon AGENTS, mais privilégier exclusivement GDB en lecture et documentation.

Travailler en autonomie, sans validation intermédiaire pour les opérations Git et logiciels.
