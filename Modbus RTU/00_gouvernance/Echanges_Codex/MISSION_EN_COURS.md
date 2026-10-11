---
mission_id: TR2-20261011-M013-B-PHYSICAL-TX
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-B — Validation preflight et premier dialogue Modbus physique

## Acquis utilisateur du 11 octobre 2026

Inspection physique de la carte MIKROE-3863 rev1.02 ADM2867E, communiquee directement par l'utilisateur : J1 position 3,3V ; J2 ferme ; J3 ferme ; VIO mesure 3,27V contre masse logique ; DE 0V ; /RE 0V au repos. RST non identifie sur breakout ; utilisateur rappelle qu'aucune liaison RST vers la NUCLEO n'a ete realisee. Ce sont des mesures statiques et observations utilisateur, pas une preuve de comportement dynamique. Ne pas redemander ces controles sans raison nouvelle. J3 ferme relie /RE au net RST mikroBUS sur le schema v1.02 : s'assurer par inspection de topologie qu'aucune sortie active ne le pilote sur le breakout, y compris une connexion implicite non visible.

## Contexte

M013-A rapport status BLOCKED preflight avant mesures : tranche diagnostic FC03 isolee implemente et testee, 108/108 tests hote, cross-build STM32 diagnostic TXOFF/TXON reussis ; aucun flash/TX. Rapport et PREFLIGHT_RTU_TX_M013_A.md font foi. BIN008 reste sur cible selon derniere preuve, RX011 valide un seul echange entrant. Diagnostic candidate unit ID1 FC03 register1000 qty1 request 01 03 03 E8 00 01 04 7A ; reponse attendue 01 03 02 00 00 B8 44 (UNKNOWN=0), jamais statut production.

## Objectif et methode

1. Lire AGENTS.md, protocole V2-B3, ETAT_COURANT_TR2.md, dernier rapport M013-A, preflight M013-A, schema Click rev1.02, code diagnostic/transport et tests. Verifier main reel et etat Git, preuves et candidat exact.
2. Actualiser matrice preflight avec observations utilisateur ci-dessus, evaluer RST/J3 et topologie breakout reellement connue. Ne pas transformer absence de fil NUCLEO en preuve absolue de net libre. Si une incertitude de contention reste non resolue sans inspection physique, consigner une seule question ou action minimale et maintenir NO-GO TX.
3. Si GO justifie : preparer build diagnosticON, TXON, unit1 a partir du code main reel, executer validation proportionnee, verifier ST-LINK/STM32, sauvegarder et verifier flash 2MiB courant, identifier BIN/ELF/SHA, programmer a 0x08000000 par procedure standard qualifiee, verifier/readback, garder trace et ne pas confondre ELF ancien/nouveau. Autorisations flash et debug explicites et autonomes, sans mass erase/Option Bytes/force push.
4. Avant TX : assurer que seul le maitre HP prevu est connecte et que le bus/direction ne presente pas de contention. Le COM7 HP n'est pas accessible depuis T1000 : ne pas pretendre lancer une requete depuis Codex. Si toutes les conditions sont reunies, preparer un unique essai HP sans retry, avec instructions minimales pour l'utilisateur, et verifier les preuves reelles de la reponse, CRC, timing DE haut puis bas sur TC et retour RX ; si l'essai n'est pas executable dans cette mission, distinguer firmware flashe et TX non verifie. Ne pas lancer de TX non sollicite.
5. Publier compte rendu avec verdict GO/NO-GO, etats distincts BUILD, TEST, FLASH, EXECUTION, TX PHYSIQUE, preuves, questions restantes et suite M013. Mettre a jour journal et documentation de preflight. Commit/push normaux main, rapport DERNIER_RAPPORT.md, verification distante.

Limites : aucune modification physique autonome, aucun changement de mapping V1 ou de configuration production fictive, pas de formatage FRAM/SD, pas d'operation destructive, stashes preserves. Toute condition de securite non etablie interdit activation TX.
