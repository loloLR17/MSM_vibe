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

## Validation logicielle conservée

Test neuf1/1 :nominal/deuxbursts/transitioninterburst,parité/gap/timeout/overrun refusés,préexistantsanswrite,restauration. Cross-builddéfaut etprofil réussis ;core/driverinchangés,A-E/114firmware/539.NET nonrejoués. Première compilation refusée pourdeux-Werror=misleading-indentation :instructions séparées, aucuneoptiondésactivée, logséchec conservés. WarningsNewlib conservés. Unecampagne seulement aprèspréflight ;restaurationM014 byte-identique etcontrôleHPminimal prévus.

## Unique tentative physique : arrêt avant activation FIFO

Code571b0d80d8910500556fc2fcaec4006cb633511c, SPI mode3/2,5MHz et HCLK160MHz observés. Le garde refuse après les writes de préparation et le readback global `controls(actual)`, avant100ms de stabilisation et avant writeFIFOmode1. result9/INTERNAL,loss_flags4/configuration,HALerrors0 ;poll_count/burst_count/word_count/accepted=0. Aucun FIFO_DATA_OUT ou délai de vidange mesuré.

**Limite de télémétrie** : cette version n’exporte `configured[]` qu’après le freeze. Le mismatch de préparation sort avant cette étape ; `actual[]` est ensuite écrasé par la restauration. Le registre divergent, attendu et lu ne sont donc pas conservés. Le tableauconfigured tout0 est du BSS non renseigné, pas une lecture de configuration. Aucun diagnostic précis sur une fréquence, le capteur ou CTRL2 ne peut en être déduit. Les RX/metadata/polls tous0 ne sont pas des mesures ; aucun CSVXYZ artificiel publié.

Les sept contrôles sont relus restaurés0/0/0/0/0/4/0,restore_result0. M014 restauré, Flash2Mio entièrement identique au backup. Contrôle minimal B0/B1 de reprise réussi, sansretry ;journalHP complet collecté. Une seule tentative, pas de variante ni recherche exhaustive après le rejet.

**M015-F BLOCKED** : profil logiciel testé et réversible, faisabilité matérielle des bursts non démontrée. La présence de code et le flash réussi ne prouvent pas DATA_OUT. La capture prévue256mots n’a pas été acquise ;ODR, continuité, fenêtres, débit, latence, pertes, bande etspectre restent non vérifiés.

## Suite minimale proposée, non exécutée

Avant un prochain essai sous contrat distinct, exporter chaque readback de préparation ainsi que registre/attendu/observé au point de rejet. Puis viser une preuve RAM finie à une vitesse choisie, sansrelâcher le garde. Un candidat conservateur est625kHz/mode3 déjà observé pour A-D : une FIFO figée peut être vidée lentement (11,4816ms idéales parburst128), contrairement à un fluxODR continu. Cette variante n’a pas été exécutée et ne peut pas être vendue commeV1 : occupationidéale240% pour128+status à26667Hz.

ÀN256/ODRnominal, durée9,59988ms et grilleDFT hypothétique104,168Hz contre6,5105Hz pour4096 et0,813873Hz pour32768. Ce sont calculs de conséquences si une séquence uniforme était établie, pas un spectre calculé ni une bande qualifiée. Nyquistnominal13,3335kHz ne qualifie pas la bande6kHz du système. RéduireSPI ne réduit pas silencieusement l’ODR ; réduire la fenêtre ou interrompre la capture rompt la conformité V1 tant que4096..32768/continuité/store-and-forward ne sont pas démontrés. Aucun changement de validator/mapping/specification.
