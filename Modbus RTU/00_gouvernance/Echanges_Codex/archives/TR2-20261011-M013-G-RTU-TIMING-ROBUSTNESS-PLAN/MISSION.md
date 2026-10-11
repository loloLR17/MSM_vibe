---
mission_id: TR2-20261011-M013-G-RTU-TIMING-ROBUSTNESS-PLAN
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-G — Qualification temporelle et robustesse RS-485 : préparation instrumentée

## Référence et acquis
Relire sur main AGENTS.md, protocole V2, ETAT_COURANT_TR2.md, DERNIER_RAPPORT.md M013-F DONE et archives M013-D/E/F, code, procédures et preuves hors Git. Le probe HP E unique a renvoyé `01 03 02 00 00 B8 44` ; F a établi par lecture GDB seule 8 octets reçus, CRC correct, requête acceptée, réponse exacte préparée, TX started1/TC callback1, RX réarmé, sans erreur. Aller-retour fortement corroboré, mais pas de preuve temporelle physique ni répétabilité. L'essai E est consommé : ne pas le relancer. Éviter CubeProgrammer HOTPLUG en dernier accès MCU : cause NVIC reproduite en D. Aucun besoin de recontrôler le câblage/VIO/RE-RST déjà acquis.

## Objectif
Préparer une qualification **physique temporelle** de DE, UART TX/RX, trame différentielle A/B et retour RX, suivie d'une robustesse Modbus ciblée et mesurable. Livrer procédure et outillage reproductibles, sans affirmer que des mesures non faites sont acquises. Privilégier l'image diagnostic actuelle et les observateurs existants ; modifier firmware uniquement si une lacune instrumentale est prouvée, avec tests et garde-fous V1.

## Exécution autonome
1. Préflight HEAD Git, état/stashes, sauvegardes/identité de l'image et état courant. Analyser timing DE/TX/TC/T3.5, callback HAL et règles Modbus RTU 115200 8E1. Définir critères de réussite mesurables, tolérances et incertitudes de mesure. Distinguer temporisations internes de chronogramme physique.
2. Inventorier uniquement le matériel de mesure **effectivement connu/disponible** (oscilloscope, analyseur logique, sonde différentielle, canaux) ; ne pas présumer sa présence. Préparer un protocole sûr, connexions de mesure non intrusives et ordre des captures DE/TXD/RXD/A/B, précisant risques de masse et mesures différentielles. Si instrument manquant, séparer les essais réalisables sur HP des captures physiques reportées ; aucune improvisation de branchement.
3. Préparer et tester hors bus un script HP dédié à une campagne limitée, journalisation brute horodatée des réponses/CRC/timeout, aucune boucle illimitée, délais inter-requêtes sûrs, cas FC03 nominal répété, CRC invalide et unité étrangère, sans modifier le script historique E. Définir nombre maximal d'essais, conditions d'arrêt, protection contre échos et critères de qualification. Tester uniquement en mock/loopback logiciel isolé sans ouvrir COM7 ni injecter RS485.
4. Si instrumentation firmware indispensable, tests/build/cross-build et flash/debug standard autorisés après backups vérifiés ; respecter diagnostic isolé et gates, aucune production simulée. Tout accès CubeProgrammer doit être suivi de reset final GDB et contrôle NVIC sain. Ne pas modifier Option Bytes, mass erase, média ou câblage. Ne pas lancer de transmission HP, aucune requête synthétique sur le bus et aucune manipulation physique utilisateur dans cette mission.
5. Publier rapport avec distinction **READY pour essai logiciel HP** versus **WAITING instrumentation physique** selon les moyens réellement établis, scripts/protocole exacts, critères et garde-fous, et prochaines actions minimales. Mettre à jour DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md ; commit/push normal main, relecture distante.

## Acceptation
Aucune nouvelle émission physique pendant G. Critères objectifs pour la qualification temporelle et robustesse, outil HP hors bus testé, limites documentées, absence de revendication de validation générale. L'utilisateur reste responsable du branchement d'instruments et du lancement d'essais physiques futurs, après revue ChatGPT d'un rapport GO.
