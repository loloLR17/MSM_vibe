# M015-P — comparaison forensique I/J/O

## Références et preuve indépendante

I :code6c504a9d4035c2922d186a03350963bdac33379b,qualification physique2cebe602…,
[traces et bruts](../../Modbus%20RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261012-M015-I-PHYSICAL/).
BIN760aab56…/ELF7893fda8… rehashés surdisque ;aucune lacune du dumpFIFO :1794octets,
deuxtrames897,octetRXcommande puis128×7octets. RecompositionXYZCSV byte-identique,
256tags11/12/14/17h (64dechaque),type2,paritébonne,compteur0..3continu modulo4.
La réussiteI estFIFOXYZ,bien distincte de la lecturedirecte/pollingA/L.

O :code906de1ad85f6b3d7d6737bc699f29a482590614d,rapport d46a3d24… relu,
fonctionST1f848c38 exécutée/driverSTf83 différentgitlinkd995 inaccessible.
[Provenance/limitesO](official/AUDIT.md). Mode3et0 :258mots byte-identiques,
tag21h/type4,counter0,timestamps748..6916/pas24ticks ;0XYZ. Témoingetter03F,
258→2 ;aliaslocalniveau10distinct. Aucun tag corrigé ou reclassifié artificiellement.

Décodage datasheet :((tag>>3)&31),contre hypothèseexterne>>2 :
I11/12/14/17→4/4/5/5 ;O21→8. Ce décalage ne transforme pas O en XYZ.
DS12569Rev8 [ST](https://www.st.com/resource/en/datasheet/iis3dwb.pdf),§4.3/9.4-5/
9.11/9.39 :mots7,BDR0A,CTRL1bitsXL_EN=5,FSbits2:3,CTRL10TSbit5,
FIFOCTRL2STOP_ON_WTMbit7,CTRL4mode1vs6/DEC8 ;pasfaitinventé depuis l'expert externe.

## Tableau de différences avec force de preuve

SourceI figée :[stm32_iis3dwb_burst_capture.c](https://github.com/loloLR17/MSM_vibe/blob/6c504a9d4035c2922d186a03350963bdac33379b/Modbus%20RTU/05_Firmware/platform/stm32/stm32_iis3dwb_burst_capture.c).
SourceO figée :official/port.c et vendor/st/examples/iis3dwb_fifo.c au906de1a.
Transaction-comparison.csv publie chaque transactionI/O,avec préfixeRX/commande
observéI etdataO ;lescommandesO sontreconstruites, pasdes mesuresMOSI.

| Élément | I (source et traces) | O (source et traces) | Force / réserve |
|---|---|---|---|
| Cible/module |SN00350046…/UID004A003D…,premierTR2 |SN0032001F…/UID001B0046…,secondkitMKI208 |Matérielsdifférents,aucun échange ni équivalence présumée |
| CPU/SPI |160MHz/2,5MHz/mode3 |16MHz/2MHz/mode3 ou0 |Clocks/cfg réellement lus,fréquencescalculées ;paswire |
| Initialisation |baseline0/0/0/0/0/4/0etWHO7B,registresextrasaudités,aucunresetcapteur (I99..133) |WHO→softreset→BDU→FS8g→WM→BDR→stream→XL→DEC8→TS (ST146..199) |Séquencesréelles etwrites,étatpréexistantdifférent |
| CTRL3_C/BDU/IF_INC |12=44 (I128-133),IF_INCvérifié (I82-94) |12=44 aprèsreset ;IF_INCreset4 conservé |Concordant auxlectures,deux champsactifs |
| CTRL1_XL/FS |10=A8/FS4g,XLen5avantFIFO,puis00avantdrain (I129,157) |10=AC/FS8g,XLen5restantactif |Deux facteurs distincts,nepasdéduirecausalité |
| FIFO_CTRL1/2 |07=00,08=81(WM256+STOP_ON_WTM) |07=00,08=01(WM256,sansSTOP) |Octetsréels,stop/configdifférente |
| FIFO_CTRL3 |09=0Aetreadbackgroupé |09=0AsetterRMWetreadback,BDRgetbugcase10 |Mêmebitsattendus,sampleSPIpaspreuveindépendante du stockage interne |
| FIFO_CTRL4 |bypass0→FIFO1 (I136),sansTS |stream6puisDEC8→86 |Régimesdifférents ;Jstream6a toutdemême desXYZbruts |
| CTRL10_C |19=0vérifié,défautconservé ;pasdansliste7writesI |19=20setterTimestamp |Inefaitpasunwrite19 fictif ;timestampsréellementdésactivésdansI |
| CTRL5_C |14=0écrit/auditéI |resetdefault0dansO |Mêmevaleur,séquencesdifférentes |
| Délai/ordre |XLenavantFIFO,bypass100ms,I134 ;freezeXL0+2msavantdrainI157 |FIFOactifavantXL,aucunsettling100ms,capteuractifau drain |Hypothèsechronologiepossible,nonisoléeparlestraces historiques |
| Commandes/transfert |F8+896zeros ;TXRX897enunappelHAL,CSbas continu(I39..74) |TXcommande1puisReceive896,CSbasmaisCloseTransferentreappels(platform.c) |Différencelogicielleprécise,transportSTF4paséquivalenceU5 démontrée |
| Capture RX / offset |Octetcommandeexplicitementstocké,payloadcommence+1 |Phasecommandenoncapturée,payloadseuloffset0 |Replaysvalidés,aucunmélangeoffset |
| Statuts/croissance |0,53,106,158,212,256 ;drain256→128→0 |0→258àwatermark,rateTS3333/snominal ;niveaufinalnonmesuré |Monotoniediscrète,sansODRmesuréeindépendamment |
| Tags/XYZ/TS |256tags2,nonsaturés etdistincts ;aucunTS |258tags4/pas24ticks ;aucunXYZ |Bruts/PARITÉ/CSV,forceforte surcontenu,pasraisoninterne |
| Reconfigurationenburst |XLdéjàoff ;aucunwriteavantfin2bursts,puisrestoration7regs |Aucunwritependantpaquet,cleanupaprèstexteST |Liretransactions,pas supposer un setterinterne invisible |
| Parsing |tag>>3==2,parité/counter/XYZ(int16LE),accept256 |switchSTtypes2/4,analyseindépendante refuseXYZ |CompteursTSconstants nequalifientpascontinuité |

## Cas J précisément recadré

Jcode24ccec00698cd2e7f55297acc2bad0d22746e744,source stm32_iis3dwb_continuous.c,
registre07..0Apostburst attendu80/00/0A/06,reçu80/00/14/0C,trace25RX
00 80 00 14 0C,premierrefusreg09=14. Patternarithmétique×2observé,**pas
preuve des bits au fil ni d'une corruption permanente du registre**. Transport
TXRX surpremierTR2,streamactif :128motsreçus ont tous tag2/parité/countersbons,
mais restentREJETÉS parcequeconfigpostdrainéchoue. «0XYZacceptéJ» ne signifie
pas«0XYZreçusJ». Le streamou l'absencefreeze ne suffisentdoncpas commeexplication
unique de l'absenceXYZ O ;matériels/timestamps/clocks restentconfondus.

## Hypothèse sélectionnée et expérience autorisée

Hypothèseprincipale :framingHAL séparécommande/payload surU5 affecte l'échange
effectif (y compriswritesinitialisation),comparé au duplexunique deI/J.
Ce n'estpasune causeprésumée niune preuve de glitchSCK. Matrice :

| Hypothèse | Observation existante | Discrimination |
|---|---|---|
| Décodage>>2erroné |Réfutéparbruts I/O |Horscible,sansnouveau testphysique |
| GetterlevelO |03Fexplique258→2,aliasdonnetoujoursTS |DéjàisoléO,nepasrejouer |
| ModeSPI |0et3O donnentmêmespayloads |DéjàisoléO,nepasrejouer |
| FramingSPI/APIHAL |I/JTXRXréelXYZ vsOséparéTS ;carte/moduleconfondus |TémoinetcausalP surmême secondbanc,seuleAPIchange |
| Ordre/délaiXL/FIFO |I/JpréactiventXL,OpréactiveFIFO |Possible,sanspreuveisolée ;pasdevariantedeplusdansP |
| Timestampbatching/stop/full |Nno-tsFIFOvide,modes/STOPdifférents |Nonrésolu ;paspatchregsheuristique |
| Matériel/alimentation/versions |Différentescartes/modules,gitlinkd995inaccessible |Mesureindépendanteultérieure,pas«capteurdéfectueux» |

Pimporteuniquement leframingI, pasprétendre avoircloné toute sa séquence,
car changer simultanémentFS/STOP/mode/TS/horloges empêcheraitl'isolementcausal.
DeuximagesmêmefonctionST/mode3/level10/CPU16/SPI2/FS8g/BDR0A/stream6/DEC8/TSON,
unevariableBENCH_DUPLEX0/1 seulement. ContrôleretrouveTS,puiscausaluntrigger.
Prédictionprincipale :duplexunique reçoitXYZetconfigconforme ;siTSseulsourefus,
hypothèsenonsuffisante/refutée danscecadre,STOP aucunréglageadditionnel.
Snapshotsactualsize n+1 pourduplex,incluantRXcommande capturé séparément ;
payloadcopiédepuisRX+1,parser/tagshorsligne identiques. AppelHALTXRX897max sous
limiteSPI3 ;O2HAL_SPIinchangé,20ms,cleanup/réserves12/24/32 conservés.
ÉcarttimeoutI10→P20 estgardelevelPconstantdansles2images,pasfactor changé.
