---
mission_id: TR2-20261012-M015-A-ACQUISITION-BRINGUP
status: BLOCKED
base_ref: main
base_sha: 762c459839439f7d27b42c11d12e309e65622a7f
initial_head: b147a918df1a699c428b6304f455ba76571798d3
result_sha: null
created_at_utc: 2026-10-12T09:04:10.476231+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-A — Point intermédiaire logiciel validé

BLOCKED porte sur les critères physiques et publication distante encore à obtenir, pas une demande de validation intermédiaire. Poursuite autonome autorisée. Audit et profilRAM isolé développés, driver réel réutilisé ; 114/114tests + cross-builddéfaut et profilcapture réussis. Aucun essai matériel M015 encore réalisé. Résultats code de cette tranche committés aprèsvalidation ; result_sha physique sera précisé dans rapportfinal.

Acquisitioncampagne interdite sans stockage selonV1 etsnapshotactif ; FRAM/READY/STORAGE préservés. Auto-test mécanique interne ST±4g prépare variationautomatisable ; capturepolling horodatéDWT ne revendique pasODR complet. Voir AUDIT_M015_A_ACQUISITION.md et preuves/TR2-20261012-M015-A/software-validation.txt.

ArchivesM014-H exactes depuis762c459 ; dépôtinitialmain82169af, worktreeH762c459 et deuxstashes conservés. WorktreeM015 isolé surb147a918, destination explicitemain ; aucun merge/rebase/reset/force. BackupsH etimagepermanenteb462c29a préservés. Suite : dump/identitécible, capturebornée, preuvesbrutes, restaurationimageexacte, contrôleproduction, journal/roadmap etrapportfinal, relecturedistante complète. Aucun accèsSD, aucuneFRAMwrite oucampagneB5.

Preuveslocales : /home/lolo/dev/msm/reports/TR2-mission015-A-20261012T085627Z. Erreurinitiale déclarationdriver corrigée puisvalidationcomplète réussie, aucun testaffaibli. Suite.NET539inchangée nonrelancée. Permissions exercées àcestade : commits/pushlogiciels ; flash/debug nonencoreexercésM015.

## Première capture et correction en cours

Code610f6c2 :2048 échantillons réels, WHO7B/readbacksA8/44,512parphase ;3281polls dont1229NOT_AVAILABLE,0erreurSPI,1saturation. DWT160MHz/SPI2.5MHz, lecturesmoyennes13.3kHz (médiane83.63µs), ODR26.667kHzconfiguré seulement. Normerepos≈1.54g/distribution bimodale prochefacteur2 : cohérenceinsuffisante, aucunevalidationphysiqueconclue. Captureintégraleetanalysespubliées, aucunevaleurécartée.

ImagepermanenteM014 restaurée dès aprèscapture : Flash2Mio entièrementbyte-equal àbackup ;bootgen12/configUNAVAILABLE/IRQ49et66/faults0. Diagnostic justifié : HAL SPI_CloseTransfer désactiveSPI entreTransmitaladresse etReceive, CSresteactif ; hypothèsedécalagebit à cettefrontière. Correctif une seuleHAL_SPI_TransmitReceive commande+dummy/data sousunCS ; testsbornage/framing/discardcommand/errorCS et114/114+crossdéfautréussis. Nouvellecapturesurcodechangé nécessaire pour vérifier ;aucunessaireussiinchangérejoué.

## Hypothèse TX/RX non confirmée ; vitesse conservative préparée

Capturecode9d47f1f en transactionduplexunique à2.5MHz :2048samples,3350polls/1298NOT_AVAILABLE/0erreurSPI/3saturations, normerepos≈1.53g encorebimodale. Le changement detransaction ne résoutpas l'anomalie ; nepasprésenterSPI_CloseTransfer comme cause démontrée. Captureintégrale publiée. Productionpermanente restaurée secondefois : Flash2Mioexacte/bootgen12/santéMCUconfirmés.

Profilcapture seulement abaissé prescaler256→625kHz ; productionconserveprescaler64 etBINpermanentexact. Cross-builddéfautetcapturelente réussis ; cœur/driverinchangésdepuisvalidation114, nonrelancés. Essailentjustifié par anomalie persistante etvitessemodifiée ; aucun format/FRAMwrite. ContrôlefinalB0/B1préparé sousnouveaunomunique physical-m015-a-restored, exactement2FC03/retry0 ; paquetHpréexistant réutilisé, garde fraîcheuretidentité inchangées.
