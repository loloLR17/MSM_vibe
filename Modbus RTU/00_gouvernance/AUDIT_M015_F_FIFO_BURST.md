# M015-F — Faisabilité FIFO en bursts128

## Profil et critères préparés

SPI3 mode3/2,5MHz candidat, noyau160MHz/div64. [DS13737 SPI3](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf) borne la transaction à1024 ; [AN5444 §§6.1/6.2/6.4](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) donne wrap0x7E→0x78, mots7octets, tagXL2/parité paire/compteur+1mod4. Burst128 =commandeF8+896zéros,897RX conservés dontRXcommande ignoré ;CSbas pour toutHALduplex. Pas instrument externe nécessaire ou supposé ;multimètre disponible seloncontrat, aucun geste demandé.

Une capture finie256XYZ :FIFOmode1, STOP_ON_WTM=1,seuil256,BDR26.667/±4g/BDU. ConfigurationFIFO vérifiée enburst07..0A, jamais parCTRL2single durant activité ;cette voie ne résout pas le défautD etne doit pas lemasquer. Préexistantnonattendu/routecapteur/FIFO nonvide refusés. Septcontrôles sauvegardés/restaurés ;profilOFF/exclusif avantmédia/UART/production, triggerunique4D313546.

Stabilisation100ms enbypass, remplissage ≤20ms/32polls etpollspacés1ms. Refus surrecul/flagsOVR_IA-LATCHED/mauvaiseconfiguration ou niveau non256. EntréeCTRL1 coupée, FIFOconservée, pause2ms ;config etniveau256 reverifiés. Deuxbursts128, niveaux256→128→0, capteurfigé. Pascontinumode/overrunvolontaire/DMA/FRAM/SD/B5.

Acceptationseulement256mots/typeXL/parité/255transitions+1mod4 (ycomprisfrontière desbursts),aucuneerreurHAL/flags/niveaux/CS/compteursrésiduels,restaurationréussie. Parité protègetag seulement ;compteurmod4 nevoitpas pertesmultiples4 ;absenceCRCpayload/calibration/buswire reste réserve. Saturation rapportée paraxe, pasrejettrompeur fondésurimmobilitéou1g.

BrutsRAM conservés ycompriséchecpartiel ;décodage horsligne sans correction. HorodatagesDWT encadrent lecture, pasconversions niUTC. Borne897 pour TX/RX statiques,2×897RX conservés,2métadatas52octets,32polls16octets,info164octets. PasfenêtreV1 oubackendfictif.

## Conséquences V1 sans réduction silencieuse

ÀODR26667,256mots ne représentent que9,59988ms nominaux :6,25%de lafenêtremin4096 et0,78125%de32768 (durées153,598ms et1228,785ms). Capture interrompue/fill-stop-drain :aucunecontinuité au-delà de ces256mots, passtore-and-forward ;M014configUNAVAILABLE/READY0/stockage0 inchangés. Aucunmapping/validator/specification modifié.

À2,5MHz,897octets :2,8704ms idéales parburst ;128mots+statut3octets :2,88ms,minimum1,500019Mbit/s pourODR26.667. Débitmoyen d’unevidange figée n’est pas une qualificationde servicecontinu/latencemaximale. LaFIFO conserve deséchantillons produits àODRnominale, sansprouver l’ODRmesurée, la bande6kHz ou un spectre/calibration. FFT/nonFIFOdécimation nonajoutées.

Pour fenêtres :raw3int16=24576..196608octets,3int32=49152..393216 ;mémoireDMA/traitement/latences etressourcesSDowned restent àqualifier. LaV1 exige26667Hz,4096..32768,continuité etstockagedurable :un succèsborné ne satisfait pas ces exigences. Prochaineétape conditionnée auproofF, pasdéclarationM015DONE.

## Logiciel validé, physique encore en attente

Test neuf1/1 :nominal/deuxbursts/transitioninterburst,parité/gap/timeout/overrun refusés,préexistantsanswrite,restauration. Cross-builddéfaut etprofil réussis ;core/driverinchangés,A-E/114firmware/539.NET nonrejoués. Première compilation refusée pourdeux-Werror=misleading-indentation :instructions séparées, aucuneoptiondésactivée, logséchec conservés. WarningsNewlib conservés. Unecampagne seulement aprèspréflight ;restaurationM014 byte-identique etcontrôleHPminimal prévus.
