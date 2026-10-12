# M015-N — audit causal et expériences prévues

Référence : M015-M 83f9cb8, images FIFO évaluées a7a2ebc. Traces O2 originales
reconstruites octet par octet dans preuves/M015-N/M015-M-reconstructed.csv ;
commandes reconstruites depuis registre/sens, pas capture de fils SPI.
Les deux payloads896 reconstruisent exactement les256wordsRAM, offsets propres,
little-endian explicite, tag=octet0>>3, tag21h/type4, parité correcte et compteurs0.
753..6873/pas24ticks : des timestamps réels de formatFIFO, aucune preuveXYZ.
Le nouvel analyseur indépendant reproduit no_XYZ/partial/cleanup_failed ;
ni découpage7, ni endianness, ni classification ne créent artificiellement le4.

Références officielles : [DS12569 Rev8 juillet2025](https://www.st.com/resource/en/datasheet/iis3dwb.pdf),
§4.3.7/p18, §9.4-5/p29, §9.11/p32, §9.28/p42, §9.39/p46 ; lecture web effective.
FIFO_CTRL3/09 BDR_XL=0A sélectionne26667Hz, CTRL1_XL/10 XL_EN=5 activeXL.
DEC_TS_BATCH=2 donneBDR/8 ; timestamp12,5µs/tick (ajustableINTERNAL_FREQ_FINE).
Tag2=XL,4=timestamp ; bits2:1 identifient un créneau modulo4, pas numéroglobal.
Les valeursM 07..0A=00/01/0A/86,CTRL1=AC,CTRL10=20 correspondent aux réglagesST.
Le commentaire exemple «12.5Hz» contredit l'argument26k7, pas le code exécuté.
Getters du pilote pinné : fifo_level masque03F contrechamp10bits datasheet ;
BDRget switch10 surchamp4bits dont valeur0A → défauts de cette version démontrés.
Vendor/exemples/sourceSDK conservés byte-identiques, pas patchsilencieux.
AN5444 référencée parST : accèsoutil et téléchargements timeout, contenu noncité.

SéquenceM effective : reset→BDU→FS8g→WM256→BDR0A→stream6→XL_EN5→DEC8→TSon.
Configuration vérifiée avantFIFO puis aprèschaqueburst ; statutstouteslectures
sansFULL/OVR jusqu'au refuscapacité, pas preuveabsenceultérieure. DémarrageXL
avantTSON peut décaler l'origine temporelle, pas expliquer à lui seul zéroXL.

| Hypothèse | État de preuve | Expérience discriminante prévue | Configuration attendue |
|---|---|---|---|
| Mauvais BDR/ODR | ValeursM relues conformes au datasheet, getterBDR erroné connu | Référence sûre, comparer readbacks avant/après |09=0A,10=AC,19=20 |
| Interaction timestamp batching / XL |256TS/0XL malgréBDR correct ;cause nonétablie |no-ts : seulDEC_TS_BATCH2→0,TSenable reste1 |0A=06,09=0A,10=AC,19=20 |
| Ordre activation du batching |STconfigureBDR avantXL ;pas preuve défaut |bdr-last : déplacer uniquement setterBDR aprèsXL/TSon |Mêmevaleursfinales00/01/0A/86/AC/20 |
| Lecture multiwords ne restitue queTS |BrutsparfaitsTS maisCPU/SPIlong réparé |word7 : seulburst128→1 ;observation limitée8entrées |RéglagesST inchangés,8readFIFO de7octets |
| Capture aprèsFIFOplein / pertes |AucunFULL/OVR dansM capturé,paritébonne ;paspreuveglobale |Statuts/niveaux/contrôles etcleanup de chaqueétape |Refus sur68/FULL/OVR,aucunpayloadaccepté |
| Parser/offset/endianness |BrutsRAM↔payload exacts et analyse indépendante |Audit horscible uniquement |256tags4 confirmés,pas nouveauflash |

Ordre campagne : référence512 entrées d'abord ; si seuleabsenceXYZ connue est
reproduite etcleanupcorrect, no-ts puis bdr-last puis word7 si nécessaire.
Chaqueprofil a son image/identité/trigger etpreuves, aucune modificationCPU/SPI/
FS/BDR/mode simultanée. La borne8 deword7 est une borne d'observation déclarée,
pas un réglagecausalcapteur : on compare ses premièresentrées àcelles dutémoin.
STOP anomalie nouvelle, pasenchaînement aprèsHAL/config/cleanuprefus imprévu.
SuccèsXYZ nécessite analyseintégrité verte avant touteextension512XYZ/4096XYZ.
Cesextensions conditionnelles ne sont pas préparées en aveugle ; leurs budgets
et tests seront dimensionnés seulement si une voieXYZ conforme existe.

## Capacité et cleanup

Max400polls,2lectures/poll,delay1ms ettempsglobal500ms. Pour512entrées,
initialisation<=40traces,4bursts×13transferts (burst+12contrôles),800polltraces,
9cleanuptransferts :<=901traces. word7/8entrées :<=953traces. Payloads<6KiB
référence, snapshots<=1906 ; capacités1024/64KiB/2048 respectées, margeexplicitée.
Acquisitionlimitée1012traces/65504octets/2024snapshots ;cleanupdispose12/32/24
réservés (besoin9/9/18),3transferts demarge. Chaqueappel réclame2snapshots avant
CSbas ; pasréutilisationoueffacement depreuve. Refusbitsdistincts1trace/2payload/
4snapshot ; débordementobservateur distinct. STOP conservepremièreerreur.
Clock.cleanup_begin ouvre uniquement lebudget réservé au début des settersST
XLoff/bypass/TSoff ;readbacks exigés. Une panneHAL réelle resteunrefuscleanup,
jamaisune réussiteinventée. ImagecleanupM qualifiée resteprocéduredesecours.
Agrégat6mots des polls (count/ticks/min/max/flagsOR) conservé en plus desbruts ;
aucune troncature silencieuse. Capacité ne prouve pas budgetCPU/absenceperte.
