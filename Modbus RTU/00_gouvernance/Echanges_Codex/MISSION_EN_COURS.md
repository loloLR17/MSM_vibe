---
mission_id: TR2-20261011-M013-C-FIRST-RTU-RESPONSE
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-C — Premier flash diagnostic et reponse Modbus physique

## Fait nouveau determinent, confirmation utilisateur

L'utilisateur confirme explicitement apres verification repetee : **broche 2 RE/RST du Click MIKROE-3863 rev1.02 libre de toute connexion externe, y compris via breakout/support ; aucune liaison vers NUCLEO ni autre sortie**. Ne pas redemander ce controle ni une nouvelle mesure statique sans nouvel indice contradictoire. J1 3,3V, VIO 3,27V, J2/J3 fermes, DE=0V, /RE=0V au repos ; PG4 seul pilote de direction. Ces observations sont des declarations/mesures utilisateur, non une mesure instrumentee Codex. Le NO-GO specifique contention RST de M013-B est donc leve sur la base de la topologie confirmee, sous reserve des autres verifications de securite.

## Etat source

Rapport M013-B status BLOCKED uniquement pour RE/RST : build diagnosticON TXON unit1 reussi, 3/3 tests cibles ; aucun flash/debug/TX. Candidat reconstruit BIN 48240 octets SHA256 47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8 ; ELF SHA256 762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a. Carte dernierement observee BIN008, RX011 un ADU valide. Preflight et rapport M013-B, AGENTS.md, protocole V2-B3, ETAT_COURANT_TR2.md sont references obligatoires. Ne pas supposer les artefacts inchanges sans verification.

## Travail attendu

1. Verifier HEAD reel main, etat Git/stashes, gouvernance et documents ci-dessus ; confirmer la route electrique PG4-DE-J2-/RE-J3-RE/RST libre et l'absence d'autre source active d'apres les observations utilisateur et schema rev1.02. Enregistrer formellement la levee du blocage RST, sans faire repeter l'inspection.
2. Verifier le candidat diagnostic isole et ses garde-fous : adresse1 explicite, FC03/1000/1 seul disponible UNKNOWN0, aucun acces aux medias, aucune autorite production fictive ; verifier build/test proportionnes et empreintes exactes.
3. Si tous prealables restent satisfaits, proceder de facon autonome au flash STM32 : cible STM32U575 NUCLEO-U575ZI-Q, ST-LINK SN 003500463235510B37333439, image diagnosticON/TXON unit1 issue du HEAD reel, methode ST qualifiee. Avant flash sauvegarder et verifier image Flash courante 2MiB, verifier identite et empreintes ; flash normal 0x08000000, verify/readback exact, ELF correspondant pour debug. Aucune mass erase, Option Bytes, force push, reconfiguration de bus ni formatage FRAM/SD. Si prealable nouveau echoue, NO-GO motive.
4. Confirmer boot du diagnostic et absence de TX spontanee. COM7 du HP est inaccessible depuis T1000 : ne pas simuler un envoi HP. Preparer pour l'utilisateur **une seule** requete COM7 115200 8E1 sans retry : 01 03 03 E8 00 01 04 7A ; reponse attendue 01 03 02 00 00 B8 44. Produire instructions minimales pour lancement et capture reponse, avec methode de preuve CRC et etat DE/TC/retour RX lorsque accessible. Si aucune observation physique de la reponse n'est possible, marquer TX PHYSIQUE NON VERIFIE et attendre retour utilisateur, sans qualifier J1.
5. Publier dans DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md : GO/NO-GO, identite cible, image exacte, sauvegarde et readback, tests, boot, TX physique, limites, action utilisateur minimale. Documenter la levee du blocage RE/RST ; commit/push normaux main et verification distante. Preserver archives et stashes.

Interdictions : operations destructives, repetition de controles physiques acquis, affirmation d'une reponse sans trame reellement capturee, confusion diagnostic avec runtime production, changements de specification V1.
