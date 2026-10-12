# M015-O — portage de la fonction ST publiée

Source imposée [iis3dwb_fifo.c](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/1f848c38e84b4144d55c66fb5411e78bdb9f96f1/iis3dwb_STdC/examples/iis3dwb_fifo.c),
master relu =1f848c38e84b4144d55c66fb5411e78bdb9f96f1. Fichier byte-identique au
vendorK, SHA f8650edb8a1364624214ff0a3bc8132ec8f2d93a81c19b6a967769696acb34ec,
BSD3 conservé. Publication officielle, pas preuve indépendante d'essai physiqueST.
Le kit MKI208V1K comprend MKI208V1/IIS3DWB,nappe12 et MKIGIBV2 (DB4012Rev4) ;
MKI109V3/D sont des hôtes d'évaluation, pas les breakouts possédés. Notre hôte
NUCLEO-U575ZI-Q diffère ; le kit est pertinent, aucun échange avec TR2principal.

Limite de version :gitlinkdriver d995549f… inaccessible («not our ref» confirmé).
Driver officielST master f83e74fb… conservé, même versionK/N et hashesSOURCES.json.
Il ne peut être qualifié byte-identique au sousmodule inaccessible. Ses getters
niveau03F/BDRcase10 restent inchangés. L'exemple annonceMKI109D/V3 ; le .iocV3
configure SPI2 **mode3**, CPU72MHz/PCLK1 36MHz/div4=9MHz calculés. K/N utilisait
mode0/CPU16/SPI2MHz : différence plateforme importante, pas cause prouvée ici.
MKI109D source fournit1,8V par macrosnomméesLSM6DSV16X, contraireauVDDminimum
IIS3DWB2,1V ; ne pas reproduire cette alimentation. NotreVDD/VDDIO3,3V confirmés
parutilisateur, alimentationfixe, délai1000ms commeV3 ;pasplateformevalidéeparST.
DatasheetDS12569Rev8 utilisé dansN reste sourcepertinente ;AN5444 inaccessible.

## Équivalence vérifiable

`generate.py` produit le fichier compilé depuis le vendor pinné, avec patch/hash.
La fonction `iis3dwb_fifo()` est réellement liée/appelée ; `bench_run()` ne l'est
pas. Seule modification interne : formatconsole timestamp %ld/castlong pour
corriger Werror=format surARM. Le reversepatch redonne exactement la fonctionST.
Allocationfifo_data256→512 protège le buffer pourtoutniveauphysique ;watermark256
et logique num/k/tags restent identiques. Aucun réglagecapteur reconstruit.

| ST original | Portage | Transactions attendues |
|---|---|---|
| platform_init,delay |Horloges/GPIO/SPI U5,railsexternes3,3V,1000ms+boot10ms |CPU16MHz,mode3/SPI2MHzcalculés |
| reset→BDU→FS8g→WM256→BDR26k7→stream→XL26k7→DEC8→TSon |FonctionST inchangée |12=44,10=AC,07..0A=00/01/0A/86,19=20 |
| fifo_status_get / watermark / num |Getterofficiel intact dans témoin |Niveautronqué03F visible,0request refuseexplicitement |
| fifo_out_multi_raw_get |AppelST intact, plateforme segmente à896octets/128mots |SPI3TSIZElimité,CSréasserté entresegments,0erreurtolérée |
| tagsXYZ/TS etprintpremiers/derniers |FonctionST etcalculsFS8g,printf_floatactivé,consoleRAM |Capturebrutecomplète etlogoriginal |
| boucleinfinieAPI sansvérifierretours |Sortieplateforme bornéevia longjmp,erreurspréservées |500ms/100msreset,overflow/capacité/WHO/IO/len0 stoppent |
| aucuncleanup |SettersST off/bypass/TSoff+readbacks,12tracesréservées |3observationsprécleanup+9transactions,24snapshots/24octets |

Autres adaptations plateforme :1ms aprèslecturestatut (ordonnanceur sans modifier
les décisionsST),untriggerexplicite,stockageborné1024mots/2048consoleoctets,
refus au-delà512motsparappel/1024cumulés,post-observationsconfig etcleanup.
Durées/logs ne sont pas identiques auxhôtesST ; ces différences sont explicites.

## Campagne prévue avant flash

1. Témoin :fonctionST+driverf83 officiels,getter inchangé,mode3,unpaquetmax.
   Préservermêmeunnumtronquéoulen0 ;pas qualificationdugetter à partir deXYZ.
2. Diagnostic1 si nécessaire :seulecorrectionlocale du niveauFIFO en10bits,
   alias degetter au build, mêmeéchantillon brut, pasprofilN ni nouveausensorinit.
3. Diagnostic2 siXYZ enmode3 ou absencepersistante :mode0 seul vsdiag1,
   mêmefonction/correction/horloges/réglages. Comparaisoncausaleisolée SPIphase.
4. Confirmationmode3/troispaquets seulementaprèsXYZ etintégritéverts ;<=1024mots,
   cible512XYZconditionnelle,pas4096. STOP touteanomalienouvelle,cleanup sûr.

Calculcapacités :500pollsmaxà1ms≤500traces,init≤25,segments≤8,
post-observations+cleanup12 ;marge avant1012/2024.1024mots7168octets+console2KiB,
payload64KiB etRAM768KiB suffisants ;logcapacityrefuseavanttransaction etbudget
cleanup12/24/32 conserve3readbacksinitiaux et9actionsfinales. Les guardsHAL
etprecloseO2 restentceuxqualifiésM/N, erreursjamaismasquées.
14scénariosmockfonttourner lafonctionST réelle, témoin/alias,motsXYZ/TS,
IO/WHO/overflow/capacité/cleanup etniveau300 (>ancienbuffer256). QuatreARMbuilds.
