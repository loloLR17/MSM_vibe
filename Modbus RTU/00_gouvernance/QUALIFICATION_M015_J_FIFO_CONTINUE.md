# M015-J — Fenêtre FIFO continue4096, protocole borné

Profil OFF/exclusif `TR2_STM32_CONTINUOUS_FIFO`, aucunmédia/UART/production. Réutilise les types et vérifications groupées I, sans changer le harness validé ni le cœur/mapping/validator. `AcquisitionService` et `VibrationSource` per-sample existants inspectés et inchangés : le nouveau propriétaire RAM diagnostique ne s’y substitue pas en production.

## Configuration et continuité à vérifier

[ST DS12569 Rev8 §§4.3.3/9.5](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) : FIFO_MODE110=continu, pouvant écraser d’anciens mots si pleine ; choix6, pas mode1 figé. STOP_ON_WTM désactivé, watermark128 (07=80/08=00), BDR_XL26,667 (09=0A), CTRL1=A8/CTRL3=44/CTRL5=00. Préparation enbypass puis une activation06 ; entrée A8 reste active pendant **tous** les32bursts128, sans freeze ni arrêt intermédiaire. Groupes07..0A et0F..14 avant/après chaqueburst, WHO/IF_INC/voisins ettous attendus exacts, HAL_OK/error0/compteurs0/CS1→0→1 obligatoires. Aucun vote/fallback/substitution, lecture simple ou reset capteur.

Chaque status3A/3B est horodaté, niveaux entre deuxlectures sansvidange doivent être monotones. Rejet sur FULL/OVR/OVRlatched (mask6800), niveau≥384 (marge128mots surFIFO512), timeout/guard/stop/capacité/tag/parité/gap. La relation `production = niveau_après − niveau_avant +128` est comparée aux timestamps encadrants avant/après leburst, avec une **enveloppe de garde déclarée ±5% de l’ODR configurée et2mots de phase/latching**. Ce choix conservateur ne change pas l’ODR V1 et ne la mesure pas indépendamment ; refuser horsenveloppe. Productionobservée strictementpositive prouve la progressionpendantvidange selon cesstatuts ethorodatages ; modulo4 seul ne prouve pas touteslespertes. Les artefacts delecture/flags nonobservés restent une limite.

Bornes :4096XYZ,32bursts de897octets chacun souslimiteSPI3 1024,192status maximum,384traces,250ms depuisactivationFIFO, un seultrigger4D31354A. API stop volatile consommée sansreprise ; aucuneauto répétition. Fenêtre EMPTY→ARMED→FILLING→COMPLETE seulement si4096mots etrestauration réussies ; touteanomalie→INVALID, pas de fenêtre partielle acceptée. Tags/counter/parité conservés, XYZ int16 etsaturation comptée. Horodatages deburst etstatus, pas instants deconversioninventés. Première coupureCTRL1 aprèsla fenêtrecomplète seulement ; les surplusFIFO à l’arrêt ne constituent pas une perte àl’intérieur des4096mots.

## RAM/stack et ownership

RAM768KiB=786432. XYZ24576octets, tags4096, rawRX28704,384traces35328,192polls3072,32metadata2688. Buffers statiques exclusifs au diagnostic, pasheap niwriter concurrent ; validité globale requise, bornes etindex contrôlés avant chaquechunk. Headers/interfaces V1 inchangés. Linker `_Min_Stack_Size`2048/_Min_Heap_Size512, .bss101536/.data12 (GNUsize inclutinit-array/heap-stack :data20/bss104100), espaceBSS→sommetpile684884octets ; marge très supérieure au min. ProloguesARM :run456/io152/controls40/HAL_SPI64octets, pas tableau volumineux surpile. Ce relevéstatique n’est pas une mesure high-water runtime.

Étendre **tous** cesbuffers/telemetry×8 pour32768 consommerait787712octets avantautres globals/stack : ne tientpas en l’état. XYZ+tags32768 seuls229376octets tiennent ; proposition future :XYZfenêtre détenue ou consommateursegmenté, deuxchunks128/ring borné etmetadata compactes, rawdiag nonretenu intégralement, backendstockageowned sousmission distincte. Rienécrit surmédia, aucune tailleV1 supprimée, pasarchitectureproduction adoptée par cetteproposition.

## Validation logicielle et campagne

18scénarios neufs : producteur actif pendantvidange/window4096, SPIincomplet, niveauincohérent/overrun/timeout, absenceproduction, parité/gap, stop, gelproducteur pendantlecture (refus), backpressure, wrapDWT, limitesRAM, configuration/WHO/NVIC/DWT etdeadline. 1/1CTest etcrossdéfaut/profilréussis ;profil concurrent refusé. AucuntestI oucampagneacquise rejoué. Sourcehorsprofil OFF ; uniquement harness/main/CMake touchés, validation cibléeproportionnée. WrapperHP nouveau nomJ, Steady-only/deuxlectures, parser/rejets horsport validés.

Préflightfresh M014-H/UID/STLINK/Flash2Mio/RAM requis avantflash, NORMALverify/reset puisGDB seul ;dumpavantrestauration enfinally, Flash2Mio exacte etdeuxFC03HPdereprise sansdebugger. Un seulessai physique court aprèscheckpointpoussé/relu, stop au premierrefus. Cette qualificationvise4096enRAMpendantproduction ; autresfenêtres/endurance/stockage/FFT/calibration restentouverts.
