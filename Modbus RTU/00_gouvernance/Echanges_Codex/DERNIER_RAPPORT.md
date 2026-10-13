---
mission_id: TR2-20261013-M015-V-COOPERATIVE-CLOTURE
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 4faf74f42bd313efb9db5d17e0ea8b8ece370cd8
result_sha: null
created_at_utc: 2026-10-13T10:48:04.519178+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-V — Checkpoint logiciel avant coexistence physique

Service coopératif intégré à la boucle de production :Modbus pollé entre quantaFIFO bornés, SPI4 surHSI16, driver/parser/guards T-U réutilisés. Toutes taillesV1 prises en charge, continuité conservée aux frontières4096, aucune donnéeV1 fabriquée/publiée. RAM statique602144 et marges explicites. [Audit/stratégie](../INTEGRATION_COOPERATIVE_M015_V.md).

Validation complète125/125 et ARMdéfaut réussis ; ARMcooperatif4MHz réussi ; un testciblé renforcé ensuite sans rejouer la suite inchangée. Captures, erreurs et cleanup simulés ;aucune coexistence physique encore revendiquée.

TR2-001 seule sondeSN003500463235510B37333439,UID004A003D3842501820333157 vérifiés ;M014-H présent/gen12/configUNAVAILABLE/NVIC49-66/faults0. DoublebackupFlash2MiB exactSHA556fedf5… etBINhistorique exact, RAM sauvegardée. Aucun reset/flash à cecheckpoint. Contrat/rapportU archivés byte-exacts depuis1120314 ;worktrees/stashes préservés.

BLOCKED provisoire pour qualification physique etpublication/relecture finales. M015 IN PROGRESS,J3/M016nonaccomplis. Prochaine tranche :unecampagne32768 avecpréfixesV1 et B0/B1 pendantcapture, bruts/CRC/latences/nativeUART, puis décisionimagecumulative ouretourM014.8MHz seulement si mesure4insuffisante.


## Première campagne et correction avant reprise

Code2386654, unique tentative4MHz :0XYZ/0burst, FIFOstatus0000 à13630/13631/13632 puis00EA(niveau512/FULL/OVR/latch) à13773. FenêtreINVALID/result7,cleanup0/0C8000 ; toutes transactions effectuéesHAL_OK,aucun OVR SPI. B0/B1 passent,UART16octets/deuxTC/erreurs0 ;premierB0reçu pendantcapturemais sansdonnées, B1 aprèsinvalidation. Paspreuve decoexistence réussie.

Écart141ms entrestatuts. Lectureducode :refresh_context reconstruitB5mêmepourB0/B1 ;command_engine_snapshot→latest_completed scanne les256slots/deuxcopies parlecture physiqueFRAM. Hypothèsecausale forte,àconfirmer pardifférentiel corrigé. M014-H restauré :Flash2MiB exacte,gen12 ;deuxB0/B1deretour réussis,journauxcomplets récupérés.

Correctionminimale :prepare_request existant remplacepreparegénérique ;seules FC03valides entièrementB0/B1 évitent le snapshotB5 non demandé. Statutfrais,withdrawal,identité,mapping,CRC/adresse etcontrôlesB5restentinchangés. Testsneufs réponsesB0/B1 avecbackendB5 indisponible, uptimefrais,refusB5sansstale puisrepriseB5 ;validationcomplète125/125+ARMdéfaut etARMprofil réussis. PascommandeSPI8ni autreparamètrecapteurchangé.

Erreurcollecteur supplémentaire :dump d'un tableau vide refusé parGDB ;lesstatuts/controls/observer etbrutsSPI avaientdéjàété sauvegardés avantrestauration. Prochaincollecteur garde explicitement les tailles0 ;aucunessai matériel refait pour ce seulproblème.
