---
mission_id: TR2-20261012-M015-N-ST-FIFO-XYZ-QUALIFICATION
status: BLOCKED
base_ref: main
base_sha: 83f9cb80bc292bd24dda878a8e99b85085f1d664
initial_head: ddd292fb06aed40e06fe39dfc7ab6230344cdabe
result_sha: 64f55db78dfb5cfc1392939fd688691a6977b578
created_at_utc: 2026-10-12T21:22:09.324310+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-N — harness corrigé, absence de batching XYZ discriminée mais non résolue

**Référence complète512 et cleanup sûr désormais observés. Les trois expériences
prévues n'obtiennent aucunXYZ :512timestamps en référence, FIFOvide sansbatching
TS,512timestamps avecBDRprogramméaprèsXL,8timestamps en lectures7octets.**
La cause précise de l'absenceXYZ reste inconnue :statut **BLOCKED** technique,
aucune qualificationFIFOXYZ/continuité/V1. Campagne terminée aux critèresd'arrêt,
aucun quatrième diagnostic ni extension512XYZ/4096XYZ fautepréconditionsvertes.

## A — Audit causal indépendant

AGENTS/contrat/protocole/journal lus depuisGit réel ;main distantddd292fb06aed40e06fe39dfc7ab6230344cdabe,
baseM83f9cb8 conservée. WorktreeN isolé,originalmain82169af et anciensworktrees
préservés. Archivesmission/rapportM exactes au83f9cb8,preuvesM nonaltérées.
Reconstructionoctet paroctet de1024transactionsM :commande reconstruite depuis
registre/sens,payload/offset/count ;deuxbursts896 identiques aux256wordsRAM.
Analyseurindépendant confirme256tags4/timestamps,paritébonne,counter0,aucunXYZ.
Pas erreurendian/classification/découpage7 identifiée ;les commandes reconstruites
et observationsRAM ne sont pas une capturewire.

[Audit/matrice/budgets](../../../experiments/iis3dwb_st_reference/AUDIT_M015_N.md).
SourcesvendorSTpinnéesetdatasheetofficiel[DS12569Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf)
consultés :champsBDR_XL0A/XL_EN5 conformes,mode6/DEC8/TSON vérifiés parles
readbacks requis. Gettersversionpinnée défectueux :niveau03F contrechamp10bits,
BDRcase10 impossible danschamp4bits etpas0A. Aucunpatchvendor. Le commentaire
exemple12.5Hz n'est pas l'argument26k7 réellement exécuté. AN5444 n'a pas pu
être lue(timeoutoutil etdownload),aucune information de cette note inventée.

Hypothèsesavantflash :mauvaiseconfiguration,interactionbatchingTS/XL,ordreBDR,
lecturemultiwords,captureaprèsFIFOplein,parser. Matriceprévoit la référence et
exactementtroisdiag,no-ts→bdr-last→word7,sansmodificationsCPU/SPI/FS/ODR/BDR/mode
simultanées. Tous lesréglages constructeur restentinchangésdansréférence ;
no-ts etbdr-last sontdesvariantes déclarées, pasune démoSTstrictementidentique.

## B — Correction durable du harness et validations

- **Réserve cleanup démontrée** :acquisitionlimitée1012traces/65504octets/
  2024snapshots ;cleanupdispose12traces/32octets/24snapshots,besoin9/9/18.
  Refusdistincts1trace/2payload/4preclose,débordementobservateur séparé ;
  aucun effacement ou recyclage depreuve pour fairepasser la suite.
  Clock.cleanup_begin ouvrelebudgetréservéuniquementàl'entrée ducleanup.
  SettersST XLoff/bypass/TSoff, puis3readbacks,échec explicite etpremièreerreur
  acquisition conservés. Une pannephysiqueHAL peutencore empêcher lecleanup :
  la garantie porte sur la saturationdujournal,pas surtoutdéfautmatériel.
- **Polls bornés** :400max/500ms/delay1ms ;agrégatcompact6mots,count/ticks/min/max/
  flagsOR,enplusdestracesbrutes. Calculréférence<=901traces,word7/8<=953,
  <6KiBpayload et<=1906snapshots ;marge vérifiée, paslimite augmentéeauhasard.
- **Transport** :O2HALSPI seul,CPU16MHz/SPI3mode0/PCLK3div8=2MHzcalculés,
  timeouts20ms inchangés ;hookCloseTransfer etgardescomplets896 conservés.
  SDKHALoriginal/vendorST byte-identiques, copieinstrumentée debuild explicitéeM.
- **Tests réellement exécutés** :5/5CTest,diagnostic14scénarios+8budgets,
  intégrationinitiale41scénarios puis43aprèsajoutdescascleanup/capacité ;
 14testsPythonqualificateurindépendant,2instrumentationHAL,5outilsanciens.
  Positifs/négatifs couvrenttagsmixtes,TSseuls,parité,SPI,compteurwrap/gap,
  FIFOoverflow,tempswrap/recul,ratioTS/XYZ,partiel,troncature,cleanupimpossible.
  WHO/pollingnonrégressésdanslesmocks ;pasrépétitionphysiquedeleursacquisL.
- **Builds/statiques** :4cross-buildsARM réussis,référence/no-ts/bdr-last/word7 ;
  GCC-fanalyzerciblébench.c sanswarning/erreur. AvertissementsNewlibusuelsde
  linkageconservés. Validationproportionnéeauprojetautonome modifié,aucuncœurTR2
  touché :pastr2_validate.sh/testrégressionproduction/.NET inutiles.

Checkpointlogiciel `64f55db78dfb5cfc1392939fd688691a6977b578`,message
«M015-N: reserve cleanup budgets and qualify FIFO independently before hardware»,
pushnormalmain puisfetch/relectureintégralede38fichiers identiques **avant**
campagne. AucuneacquisitionMrejouéeàl'identique :nouveauharness sûr etnouveaucontrat.

## C — Qualification physique progressive effectivement réalisée

SecondeNUCLEO-U575ZI-Qseulement,SN`0032001F3235510C37333439`,
UID`001B00463842501820333157`,ID482/RevU,tensionCLI3,28V.
Chaqueimage/SHA/ELF etUID relusavantflash,NORMALSWD/SNexplicit/freq8000,
08000000/verify/readbackexact ;dernierCLI suiviresetGDB puisGDBseul.
Bootready1/UIDexact/core16MHz/faults0,untriggerparprofil,détachépendantexécution.
Lesquatresessionsdecapture et tousserveursGDB se terminentavecretour0.
Câblage/VDD/GNDconfirmés antérieurementparutilisateur, pasnouvellemesureCodex
ni manipulationhumaine requisedanscettecampagne. IdentitéprincipaleSN00350046…/
UID004A003D…refusée ;aucunaccèsMCUprincipal.

| Profil unique | Paramètre causal | Résultat réel | Traces / polls | Cleanup |
|---|---|---|---:|---|
| Référence ST + STATUS10 |Réglagescapteur STidentiques |512TS/0XYZ,4×896complets ;firmwareresult0 |285/93 |0 ;0C/80/00 |
| Diag1 no-ts |DEC_TS_BATCH2→0 uniquement,TSONreste1 |FIFO0 sur500lecturesstatut/500ms ;0entrée,result4TIMEOUTduharness |543/250 |0 ;0C/00/00 |
| Diag2 bdr-last |SetterBDR déplacé aprèsXL/TSon, valeursfinalesidentiques |512TS/0XYZ,4×896complets,result0 |285/93 |0 ;0C/80/00 |
| Diag3 word7 |Burst128→1 ;borneobservation8déclarée |8TS/0XYZ,8×7complets,result0 |257/51 |0 ;0C/80/00 |

AucunHALtimeout/OVR/error :toutestransactionsenregistréesvalides ;tousRXphase2
ontRxRemaining0/offset=taille/CSbas/absenceOVR avantcleanup. Aucune saturation
trace/payload/preclose,drapeauxrefus0. no-ts n'aaucunburstFIFO :sonTIMEOUT est
celui del'attentefenêtre500ms, **pas** HAL_SPI_TIMEOUT. Cette issue étaitprévue,
préservée puisanalysée avantpassage àl'expériencesuivanteautorisée.
Config07..0A/CTRL1 :réf/bdr-last/word7 `00/01/0A/86/AC`,no-ts `00/01/0A/06/AC` ;
CTRL10avantcleanup20 pourtous. Contrôlesavantdrain etaprèschaqueburst passent,
STBDRgetter0télémétrie conservée,jamaisutiliséàlaplace duBDRbrut0A.
word7changeaussi la borneobservation512→8pourlimiterle diagnostic ;ceparamètre
nechange pas lesréglagescapteur et nepermetpas conclure sur512motslecturecourte.

## Intégrité réellement démontrée et limites

Chaqueprofil :count×7populédeRAM reconstruitbyte-identiqueauxpayloadsSPI ;
aucuneclassificationdeBSS. Référenceetbdr-last :512tags21h/type4,parité512/512,
counters0,timestamps751→13015,511pasde24ticks. word7 :8tags21h/type4,
parité8/8,counters0,timestamps751→919,7pas24ticks. Registrestemporels/datasheet
permettentinterprétationnominale12,5µs/tick ;pas mesureindépendanteODR ouSCK.
256TSM etnouveauxTS restent séparés ;aucunedonnéeXYZ fabriquéeenconvertissantTS.

Niveauxobservésmin/max :référence0..260,bdr-last0..261,word70..344,no-ts0..0.
AucunflagFULL/OVR/latch danslesstatutscapturés ;lesvaleursSTLEVELtronquées
restentvisibles,STATUS10identifié. Readbacksconfigaprèschaqueburstconformes,
mais pasnouveaustatutFIFOaprèsledernierburstavantcleanup :nepas transformer
leslecturesdiscrètes enpreuvecontinue d'absencepertes/overflow. Misebypass
confirméeparreadback,inférenceFIFOvide selonmode ;pasmesureultérieure duniveau.
Bursts896 :3,6324..3,6328ms ;7octets :48,125..48,375µs,paspreuveswire.

L'analyseurindépendant rejettechaquefenêtrepour`no_XYZ` (no-tsaussiautresrefus).
ZérodiscontinuitéXYZ est **vacuement** comptéavec0XYZ ;compteursTSconstants ne
qualifientaucunecontinuité. `global_loss_zero_proven=false` etaucune fenêtreXYZ
acceptée,quelquesoitlefirmwareresult0ouuneparitébonne. Extension512XYZ/4096
**nonengagée**,préconditionXYZverteabsente ;pasvalidation32768/V1/FFT/production.

## Diagnostic final et prochaine expérience

**Établi** :le défautdecapacité/cleanupM estcorrigé ;letransport reçoitles
longueurs complètes sansOVR ;le contenuTSne provientpasd'unerreur deparsing ;
désactiverseulement leurbatching rendleFIFOvide. ProgrammerBDRaprèsactivationXL
neproduitpasXYZ ;lecture7octetsn'enmontreaucun dansles8premièresentrées.
**Déduction** :pisteprioritaire restante,cheminbatchingXL effectif ou étatcapteur/
chaîned'écriture,malgréregistre09relu0A ;pascauseracinesilicon/HAL/câblageattribuée.
Gettersdéfectueux sontconnus maisn'expliquentpas cecontenu reçu/parsé indépendamment.

Les troisexpériencesautorisées sontconsommées ;aucunflashsupplémentaireaveugle.
Prochainepreuveàcontractualiser :observerindépendamment SCK/CS/MOSI/MISO et les
écritures09/10/0A/19 puis lecturesFIFO,avec analyseurlogique3,3V identifié,
ou prévoirunautretémoin matériel/transport àunseulfacteurexplicitementautorisé.
Vérifier identité/révisionduMKI208 réel etjustifier toutpowercycle si étatcapteur
résiduelhypothétisé ;gestesphysiquesrestentutilisateur. Aucun contrôlephysique
redemandéiciau titre d'une hypothèse. ComparaisonTR2n'estpasexécutée tantque
FIFOXYZsurce bancn'estpasqualifié ;M014-Hprincipalresteintouché.

## État final, preuves et publication

La secondecarte restesurl'imageword7 endormieaprèstriggerconsommé,XLoff/FIFObypass/
TSoff,CFSR/HFSR0,détachée ;aucunfirmwareinitial restauré ni backupFlash2MiB répété.
Aucun masserase/OptionBytes/RDP/bootpersistent,aucunTR2principal/HP/COM7/RS485,
aucunFRAM/microSD/Modbus. Worktrees/HEADshistoriquespropres etdeuxstashes
vérifiésidentiques. ArchivesM immuables,preuve64f55db maintenue. Journal/roadmap
actualisés avecfaits/réserves/prochaineétape,aucunespécificationgeléemodifiée.

[Preuvesbrutes/index](preuves/TR2-20261012-M015-N/README.md),
originaux `/home/lolo/dev/msm/reports/TR2-mission015-N-20261012T210706Z` :commandes/SN/UID/ELF/BINhash,readbacks,logsGDB,
result4040/Trace40/Preclose76octets little-endian,CSV,payload/words/imagehex,
manifestes etanalyseurs. Les petiteserreursoutillage (curlabsent,timeoutdoc,
quotagehelperPython corrigéavantconnexionMCU) n'ontdéclenchéaucuneacquisition
supplémentaire ;aucuneerreurtest/build cachée. Sourcescodeévalué64f55db exactes.

| Image | TailleBIN | SHA256BIN |
|---|---:|---|
| reference | 31392 | `df6e49076ea57d22e8c305e11fa7645900a1153fdecbf47d4f28df80003911ce` |
| no-ts | 31392 | `62ef50677fb049461784c25d30a48e3d3514c525e9f2237e132a1a9ab9ddabb5` |
| bdr-last | 31392 | `425090e8299102aa9b4c64b4e122a2f28eb463fa2191e04a0833c8b8b6d1a4fa` |
| word7 | 31392 | `8ece594a3102d2c79be27fe3c06e5a20daf67ecce252fd613dc3c9e25d48a545` |

Git :branchlocalecodex/m015-n,parentmainréelddd292f,pushnormauxHEAD:refs/heads/main,
aucunmerge/rebase/reset/force. Preuvesetrapportfinalpréparés pourpublication ;
relecturedistante àconsigner ensuite. BLOCKEDporte surFIFOXYZnonqualifié et
causepréciseencoreindéterminée,pas surunevalidationintermédiaireattendue.
