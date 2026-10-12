# M015-E — Fiche de mesure logique préparée, non exécutée

**But** : obtenir la première preuve fil↔RAM d’un readbackCTRL2simple incorrect pendant alimentationFIFO, encadré par unburstcontrôle correct et unétatfigé correct. Un seul événement discriminant suffit ; une capture sans anomalie reste inconclusive, pas autorisation de rejouer.

## Faisabilité à établir avant tout branchement

Instrument disponible **NON VÉRIFIÉ**. Relever modèle, tension maximale/seuils compatibles3,3V,4entréeshaute impédance, fréquence simultanée etprofondeur. Préférer≥25Méchantillons/s sur4canaux à625kHz ;10MHz donne un décodage plusgrossier. La résolution40ns à25MHz ne permet pas certifiersetupCS5ns/hold20ns : si lacause paraît analogique/timing, unoscilloscope etsondes adaptés serontnécessaires, pas conclusion automatique«timings conformes».

Confirmer sur carte réelle le repérage/accès despoints, la massecommune logique3,3V etcourtesliaisons. Lesnoms ci-dessous sont lesréférencesversionnées, pas despointages physiquesnouvellement vérifiés. Instrument inconnu, masse incertaine ou charge non maîtrisée :STOPavantconnexion. Aucune action physique ni demande de câblage enE.

| Canalentrée | Signal documenté | NUCLEO selonpinoutversionné | Adaptateurcapteur selonmêmeschéma |
|---|---|---|---|
|0|CS PC9|CN8-4 /D44|JP2-21 /CS|
|1|SCK PC10 AF6|CN8-6 /D45|JP2-22 /SCL|
|2|MOSI PC12 AF6|CN8-10 /D47|JP2-23 /SDA|
|3|MISO PC11 AF6|CN8-8 /D46|JP2-24 /SDO-SA0|
|GND|Masse logique capteur/MCU|CN8-11 ou13 selonrepérage confirmé|GND adaptateur|

Source : [pinoutqualifié projet](CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md). VDD/VDDIO3,3V documentés, pasmesurés enE. Utilisateur seul raccorde lesentrées etlamasse confirmée aprèscontrat/preflight ; ne pas connecter unealimentation, sortieactive oumasse departieRS485isolée. PasINT1/2 ajouté.

## Acquisition envisagée sous contrat distinct

Garder lesparamètresD mode3/MSB-first/625kHz etlecontrôleRAM desoctetsbruts. N’appliquer aucuncorrectif ni nouvellesvariantes pendantcette mesure. Tracerune seule séquence :configuration puisFIFOalimente, commandes88/87/BA, arrêtCTRL1, comparaisonfigée. AucunDATA_OUT/XYZ ouoverrunvolontaire.

Si l’instrument peutdéclencher surla commandeMOSI `0A 01` (activationFIFO), garder≥30msaprèsdéclenchement ; à25MHz,750000samples,3Mbits pour4canaux(375000octets si compactés), encodageinstrument àvérifier. Sinon ilfautcapturerlaconfiguration etla pause100ms :viser≥150ms,3,75Msamples,15Mbits(1,875Mo compactés), oustreaming/segmentation réellementdisponibles. Pas présomption deprofondeur/trigger. Lecontratfutur peutréduire lesgroupes sansrefairecampagneD ;arrêter dèsévénement discriminant encadré etrestauration sûre.

Exporter latracebrute native plusdécodageCS/clocks/MOSI/MISO/temps, paramètresinstrument, hashes etRXRAMde lamêmeexécution. Aligner parordre descommandes etphase :write `0A01`→groupesactifs, write `1000`→pause2ms→groupesfigés. LescyclesDWT n’ont pas lamêmeorigine temporelle que l’analyseur ; ne pas aligner parUTC supposée ni utiliserlesanciennes tracesD comme contemporaines.

## Traces attendues et décision

`88 00` :16clocks, donnéeCTRL2 attendue81 au secondRXoctet. `87 00 00 00 00` :40clocks, RXutile00/81/0A/01. `BA 00 00` :24clocks, LSBniveau puisstatut/bitshauts. IgnoreRXcommande, CScontinu, pasdummy supplémentaire. PourétatfigéDhistorique, CTRL2single81 etstatutA100 ; cesvaleurs sont attenduessousmêmeconfiguration, pas mesurefuture déjàfaite.

| Observation de lamêmeexécution | Conclusion autorisée | Suite |
|---|---|---|
|MOSI ouclocks/CS incorrects|framing/générationdemande àexaminer|correctif uniquementaprèsdéfautmesuré, puiscontrôle minimal|
|MOSIcorrect etMISO81 maisRAM80/02/00|défaut entrefil observé etRAM (réceptionMCU/HAL/tampon)|instrumenter lacouchelocalisée ;pas attributionexclusiveHAL|
|MOSIcorrect,MISOmauvais,RAMconcordant|anomalie déjàsurfil ;signal ouinterface/readbackcapteur|vérifierqualitéanalogique/timing avantaccusercapteur|
|Pasdedéfaut reproduit|capture inconclusive pourcause|publier,s’arrêter sansretry automatique|
|Trace illisible/chargemodifiée/ressourcesinsuffisantes|mesure nonexploitable|STOP,sauvegarder limites,aucunequalification|

Critèreclôturefutur :au moinsune divergence réelle88 vs87 contextualisée, correspondancefil/RAMsansambiguïté etlocalisation justifiée ;ou qualification d’unevoie bornée aveccritères explicitement distincts. Qualifier FIFO/fluxV1 demande ensuite sesproprespreuves. PréserverM014, FRAM etSD ;chainestandard etrestauration complète ducontratfutur obligatoires avantcontroleminimal dereprise. Rien n’est exécuté par cettefiche.
