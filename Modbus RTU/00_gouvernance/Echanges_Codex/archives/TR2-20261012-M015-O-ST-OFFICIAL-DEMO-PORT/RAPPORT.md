---
mission_id: TR2-20261012-M015-O-ST-OFFICIAL-DEMO-PORT
status: BLOCKED
base_ref: main
base_sha: f14183c81c9ff744ebb1d2c6d5f61537e9c1ddb0
initial_head: 4272a9d9a5a5e71dd1cda67f3cb9e9d176e8d9d1
result_sha: 906de1ad85f6b3d7d6737bc699f29a482590614d
created_at_utc: 2026-10-12T21:56:13.321120+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-O — fonction FIFO ST réellement exécutée, aucun XYZ obtenu

**Le portage appelle maintenant la fonction `iis3dwb_fifo()` publiée par ST,
pas une reconstruction via `bench_run()`.** Le témoin lit deux timestamps ;
la correction locale du getter lit 258 timestamps, en mode3 comme en mode0,
avec payloads byte-identiques. Aucun XYZ. Tous les transferts et cleanups passent.
Statut **BLOCKED** : provenance/portage/tests acquis, qualification FIFO XYZ absente.
Les deux variantes autorisées sont consommées ; confirmation512XYZ non engagée.
Aucun capteur du TR2 principal emprunté, aucun défaut du capteur présumé.

## 1. Provenance officielle et limites de version

Main initial réellement récupéré `4272a9d9a5a5e71dd1cda67f3cb9e9d176e8d9d1`, contrat M015-O READY relu avec
AGENTS.md, protocole et journal ; worktree isolé codex/m015-o. Mission/rapportN
archivés exactement depuis f14183c, preuves historiques K..N préservées et relues.
Permissions commit/pushmain/flash/debug exercées, sans fusion/rebase/reset/force.

Référence imposée :[fichier ST pinné](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/1f848c38e84b4144d55c66fb5411e78bdb9f96f1/iis3dwb_STdC/examples/iis3dwb_fifo.c),
repo STMicroelectronics/STMems_Standard_C_drivers, commit master réellement relu
`1f848c38e84b4144d55c66fb5411e78bdb9f96f1` ; fichier byte-identique à celui deK,
SHA256 `f8650edb8a1364624214ff0a3bc8132ec8f2d93a81c19b6a967769696acb34ec`.
Auteur ST Sensors Software Solution Team, licence BSD3 conservée.
C'est un **exemple constructeur officiel publié**, pas un exemple indépendamment
prouvé sur matériel ST ni une validation ST de l'ensemble NUCLEO+kit.

Le gitlink driver du superprojet est `d995549f0e5a17e57c81b0eafc367fbdc0043e1b`.
Nouveau fetch explicite :«not our ref». Le driver exécuté est le master officiel
ST accessible `f83e74fb9945fc4dde4e058e7fcd2bdbded357b5`, déjà pinné K..N et
vérifié contre SOURCES.json. **Différence de version déclarée**, aucune équivalence
byte-identique au sousmodule inaccessible revendiquée. Aucun fichier vendor altéré.

Plateformes annoncées :STEVAL-MKI109V3/D, hôtes d'évaluation. Le kit MKI208V1K
comprend MKI208V1/IIS3DWB, nappe12 et MKIGIBV2, comme le banc utilisateur ;
seul l'hôte est différent. [DB4012 Rev4 ST](https://www.st.com/resource/en/data_brief/steval-mki208v1k.pdf).
Les projets/README/_prj_MKI109V3.ioc sont conservés dans les preuves. IOCV3 :
CPU72MHz/PCLK1 36MHz/div4=SPI9MHz calculés, **CPOLhigh/CPHA2edge=mode3**.
K..N employait mode0/CPU16MHz/SPI2MHz. V3 utilise une alimentation3,3V pilotée ;
MKI109D source annonce1,8V via macros LSM6DSV16X, incompatibles avec le VDDminimum
IIS3DWB2,1V :ces valeurs ne sont pas reproduites. Notre VDD/VDDIO3,3V restent
des confirmations utilisateur, pas une nouvelle mesure Codex.
[Datasheet DS12569](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) pertinent
consulté lors deN ; réglages BDR0A/XL_EN5/stream6/DEC8/TSON conformes. AN5444
reste inaccessible, aucune information de cette note inventée.

## 2. Portage minimal, différences traçables et comparaison àN

[Audit et tableau d'équivalence](../../../experiments/iis3dwb_st_reference/official/AUDIT.md).
Le générateur fail-closed utilise la référence inchangée, produit source/patch/hash,
et vérifie le retour exact à la fonction originale après retrait du correctif de
format. Le fichier généré est réellement lié ; bench_run n'est pas lié au portage.
La séquence constructeur, constantes, branche watermark, num, lecture multiraw,
tag decoding et impression des premiers/derniers restent celles de ST.

Adaptations explicites :

- Horloges/SPI3/CS PC9 et broches10..12 du banc U5 ; CPU16MHz/SPI2MHz calculés,
  O2 HALSPI seul. Témoin mode3 comme IOCV3, contre mode0N. Rails externes fixes,
  attente plateforme1000ms puis BOOT_TIME10ms constructeur.
- Remplacement des cinq callbacks plateforme. Erreurs ignorées par le main ST
  interceptées explicitement, sortie bornée via longjmp ; deadlines500ms,
  reset100ms, refusWHO/IO/overflow/capacité/len0. Trigger unique, aucun UART/USB
  console émis :texte ST conservé en RAM, float printf activé.
- Format timestamp %ld/castlong corrige Werror=format sur ARM, sans désactiver
  le contrôle. Le label ST «TIMESTAMP [ms]» est conservé mais la valeur est
  un compteur brut ; aucune conversion en millisecondes inventée.
- Tableau fifo_data256→512 pour protéger toute profondeur possible ; watermark
  reste256. Capacité brute cumulée1024mots, console2048octets. Segmentation SPI3
  TSIZE à896octets/128mots, CS réasserté entre segments, longueurs exactes gardées.
- Ordonnanceur1ms après statut, qui modifie le rythme des polls, pas leur décision.
  Observations après la sortie de la fonction ST, puis cleanup ST XLoff/bypass/
  TSoff et readbacks. Réserve12traces/24snapshots/32octets :3observations+9actions,
  exactement24snapshots et24octets ; aucune preuve effacée pour faire passer.

Les différences vsN sont donc la fonction de contrôle ST réellement exécutée,
son getter unique, sa quantité num, son affichage, les adaptations ci-dessus et
le modeSPI. Les réglages capteur écrits restent les mêmes. Pas reprise des profils
N STATUS10/word7 sous une fausse étiquette constructeur. Le correctif level10
est un alias local séparé :il appelle le getter ST puis restaure les10bits depuis
**le même échantillon brut**, sans seconde transaction ni patch vendor.

Les getters du driverf83 restent défectueux :niveau masque03F, BDR case10 au lieu
0A dans un champ4bits. Le commentaire12.5Hz contredit l'argument26k7 réellement
exécuté. Ces défauts sont distingués du problème physique d'absenceXYZ.

## 3. Tests et checkpoint avant accès cible

14 scénarios natifs passent en exécutant la vraie fonction ST, témoin et alias :
XYZ synthétiques, timestamp seul, WHO incorrect, IO, FIFOoverflow, budget saturé
avec cleanup disponible, profondeur300 protégeant le nouveau buffer. Mocks seulement.
Quatre cross-builds réussis :témoin,level10,mode0,confirmation3paquets préparée.
Validation proportionnée au portage autonome :pas de buildTR2/test.NET ni ancien
essai physique inchangé rejoué. HALSPI/hooks/réservesM/N conservés ; SDK inchangé.

Erreurs conservées :assertion de génération sur une séparation de newline corrigée
sans modifier la fonction, erreur de format ST réellement corrigée, injectionIO
initialement située sur un write au lieu du read attendue déplacée après inspection
(critères de refus inchangés). Aucun test supprimé/affaibli, aucun warning masqué.

Checkpoint `906de1ad85f6b3d7d6737bc699f29a482590614d`, message «M015-O: compile the actual ST FIFO example with
isolated NUCLEO platform adapters» ; push normal vers main puis fetch/relecture
intégrale de100fichiers byte-identiques **avant** matériel.

## 4. Trois campagnes uniques effectivement réalisées

Seconde NUCLEO uniquement,SN`0032001F3235510C37333439`,
UID`001B00463842501820333157`,ID482/RevU,3,28V CLI. Identité avant chacun des
3flashs, hashesBIN/ELF, NORMAL/SWD/08000000/verify/readback exact ; resetGDB après
dernierCLI puisGDBseul, bootready1/core16MHz/faults0. Untrigger parimage,
detach pendant exécution ; tous GDB/serveurs exit0. PrimaireSN00350046…/UID004A003D…
refusé, aucun accès primaire. Aucun nouveau geste physique ni mesure VDD/VDDIO.

| Étape | Différence unique | Résultat | Traces | Cleanup |
|---|---|---|---:|---|
| Témoin officiel/driverf83,mode3 | Getter ST intact | Niveau brut258→API2 ; 2timestamps,0XYZ ; result0 |79|0|
| Diagnostic1 level10,mode3 | Correction locale des10bits uniquement | 258timestamps/0XYZ,1806octets reçus |81|0|
| Diagnostic2 level10,mode0 | SPI mode0 uniquement vsdiag1 | 258timestamps/0XYZ,1806octets byte-identiques au mode3 |81|0|

La référence n'est pas requalifiée en réussiteFIFO :la fonction ST termine un
paquet borné, mais son getter tronque258 en2 et aucune entréeXYZ n'est obtenue.
Le diagnostic1 corrige le comptage, pas l'absenceXYZ. Le diagnostic2 ne démontre
aucune correction par changement de phase :mode3etmode0 rendent exactement les
mêmes258mots. Aucune quatrième campagne ; imageconfirm construite mais **jamais
flashée ni déclenchée**, fauteXYZ/intégritéverts. Aucun essai512XYZ/4096.

Registres observés après l'exemple et avant cleanup :07..0A=00/01/0A/86,
CTRL1=AC,CTRL3=44,CTRL10=20,WHO7B,flagsFIFO81. Finalcleanup=0C/80/00
(XL_EN0,modebypass,timestampoff) dans chaque étape. CFG2 RXmode3=`87440000`,
mode0=`84440000`, delta03000000 prouve l'application CPOL/CPHA ; pas mesure wire.
Toutes transactions HAL_OK/error0/preclose complet, aucunoverflow/refus de journal.
Les deux longs segments896 enmode3 durent3,6312/3,6342ms ;mode0 3,6312/3,6309ms,
plus14octets finaux. Témoinlecture14octets seulement. La segmentation conserve
les mots7entiers ; bruts recomposés byte-identiques aux buffers de l'exemple.

## 5. Intégrité, limites et diagnostic

Témoin :deux tags21h/type4, timestamps748/772. Chaque diagnostic :258tags21h/type4,
parité correcte, compteurs0, timestamps748→6916 avec257pas24ticks. AucunXYZ,
aucune classification de zéros BSS. L'analyseur N indépendant rejette pourno_XYZ
(et partial_window pour le témoin2vs258). Zéro gapXYZ avec0XYZ n'est pas continuité ;
`global_loss_zero_proven=false`,aucune mesure vibratoire acceptée.

Champs BenchResult `status10`, `polls` et `st_batch` ne sont pas renseignés par le
main ST :leurs0 ne décrivent pas l'alias ni un getterBDR mesuré. Les profils compilés,
console num258, rawlevel, transactions et official_ctrl10 donnent les observations.
Les vrais mots sont dans `official_words`, pas dans words[] BSS de BenchResult.
GPIO/ticks/snapshots ne remplacent pas une observation électrique indépendante.
Les statuts discrets sansFULL/OVR ne prouvent pas zéroperte pendant toute la campagne.
Pas de readback avant chaque burst ajouté au mainST ;observations avantcleanup
uniquement, différence àN explicitée. Le niveau aprèsdrain n'est pas mesuré.

**Établi** :ce problème persiste avec la véritable fonction ST publiée, une version
ST accessible explicitée, les mêmes réglages capteur, mode3 et mode0. La simple
reconstruction du contrôle dansN n'est pas une explication suffisante ;le getter
tronqué explique le comptage, pas le contenuTS. **Non établi** :défaut capteur,
cause électrique, incompatibilité du kit, cause interne du batchingXL, équivalence
au drivergitlink inaccessible, preuve matérielle indépendante du système ST original.

Prochaine preuve sous nouveau contrat :résoudre la version driver exacte auprès
ST et/ou observer indépendamment SCK/CS/MOSI/MISO, écritures09/10/0A/19 et lectures
FIFO sur le même module. Vérifier l'initialisation/alimentation réellement appliquée
jusqu'au module si nécessaire, avec geste utilisateur précis et justifié ;pas
emprunter le capteur du principal ni multiplier les essais à l'aveugle. Les deux
variantes autorisées sont consommées ;aucune validation intermédiaire nécessaire
à la présente publication. M015 resteINPROGRESS/FIFOXYZ BLOCKED.

## 6. État final et preuves

Secondecarte laissée sur mode0, triggerconsommé puis WFI ;capteur XLoff/bypass/
TSoff,CFSR/HFSR0,debuggersfermés. Aucun backupFlash2MiB/restaurationinitiale,
conformément àla règle du banc jetable ;aucunmass erase/Option Bytes/RDP/bootpersistant.
TR2principal/M014-H/FRAM/microSD/Modbus/HP/COM7 jamaisaccédés. Worktrees/HEADs/stashes
etarchivesN vérifiés préservés. Journal etroadmap actualisés,règlesV1inchangées.

[Preuves brutes et provenance](preuves/TR2-20261012-M015-O/README.md), originaux
`/home/lolo/dev/msm/reports/TR2-mission015-O-20261012T214009Z` :copiesréférence/IOC,patches/génération/hashes,tests/builds,
commandes/imagehex/readbacks,raw/CSV/console réelle etcomparetousmotsmode3/mode0.
ABIresult4040/Trace40/Preclose76,little-endian ;official_words count×7seulement.

| Image | TailleBIN | SHA256BIN | État |
|---|---:|---|---|
| official | 54716 | `8712d55c6e74ff3af98326650e7f1324fbad4803d58279ff1f495def44b250d9` | FLASH_VERIFY_READBACK_EXACT |
| level10 | 54780 | `ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5` | FLASH_VERIFY_READBACK_EXACT |
| mode0 | 54764 | `ef39869a03b7a79e3a71ee88bede887eea3b40e3a81b49c54a3b2ee11e8b5921` | FLASH_VERIFY_READBACK_EXACT |
| confirm | 54780 | `7ce311a82a5c2fc70ea7cd143590de1216906b385aead41f4f7f9f86ce85fd9f` | BUILD_SEUL |

Résultats publiés au `d46a3d24e310b506313be71fb3a3eb01446e7c69`, message
«M015-O: preserve real ST demo runs and separate FIFO getter and SPI-mode findings».
Push normal `HEAD:refs/heads/main` réussi ; fetch puis **128 fichiers intégralement
relus et byte-identiques**. Cette dernière publication documentaire consigne la
preuve ; son SHA sera communiqué après contrôle, sans autoréférence. Worktree O
propre après publication, original main historique82169af conservé propre,
stashes et anciens worktrees préservés. Aucun merge/rebase/force/reset.
BLOCKED technique maintenu :publication du rapport ne qualifie pas le FIFO XYZ.
