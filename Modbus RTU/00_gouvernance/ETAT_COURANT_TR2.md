# État courant MSM TR2 — synthèse opérationnelle

**Dernière actualisation :** 17 octobre 2026 UTC, R5-AFIX : offsets B3 corrigés3002alarmes/3003sévérité, code29a6f4e ; preuve physique1095ADU/563B3 dont462V/F et51HELD acceptées par le PC strict en replaySQLite. Work809/service2116/gap2344µs sous budgets. M016 restauré11:41:55UTC/Flash2Mio c5bc…f5b26 ; FRAMfinale4e7fab…84bd/journal70/root83/epoch5/run10/config3 conservée sansrollback. R5-A historiqueBLOCKED préservée ; contrôle indépendant etR5-B/R6 distincts, aucunGO industriel. **État du second banc rapporté le14/10, non relu depuis :** SDMMC1 hors MSC : A PASS borné en1bit ; B premierCRC données capturé surSSR/ACMD13 en4bits, avantCMD17/CMD18. Protocole arrêté sansretry, cause électrique unique non isolée. SecondeNucleo laissée enprobeBterminal99 ; carte de test insérée/STLINKseul/CN15off selon utilisateur. **Source de vérité :** main de loloLR17/MSM_vibe.

> Ce document est une synthèse opérationnelle, non un journal d'essais. Le document intégral antérieur est conservé **sans modification** dans [Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md). Il s'agit d'un état reconstruit à partir des preuves disponibles, et non d'une restitution du journal HP historique introuvable.

## 1. État matériel et logiciel

- **Cible principale vérifiée le17/10/2026UTC (R5-AFIX) :** TR2-001/NUCLEO-U575ZI-Q, SN003500463235510B37333439, UID004A003D/38425018/20333157. M016 CommissioningReadOnly WAIT restauré11:41:55UTC ; Flash2097152octets exacte c5bc…f5b26, FRAM262144octets4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd entière relue après restauration. Journal70/root83/epoch5/run10/config3gen3/STOP5/11COMPLETED/effect45phase3. Historique/tail exacts ; WHO7B/ODRoff/FIFObypass/TSoff ensuite, GDBdétaché/COM4fermé. Aucunrollback/RENEW/retraite ; ancienSTARTED4/5/run5 archivé demeure indécidable. Pas de produitLIVEcomplet laissé sur carte.
- **Code A04-02 historiquement exécuté par Codex :** `8bcb8436822ce7444074865617cb3696f877a6ad`, BIN169948octets SHA `b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b`. **OBSERVÉ PHYSIQUE BORNÉ** :32768XYZ/4096TS,6readsHP sans retry, B5 retiré pendant capture/relu après cleanup ; qualification ChatGPT A04-02 acquise dans ce périmètre borné le 13/10/2026.
- **Code M015-V déclaré testé par Codex :** `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae` ; commit documentaire de preuves `92c0966d5a4f84ee4033490825e3f7cc6dc6737a`. SHA-256 BIN : `8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480` (rapport M015-V). **Revue indépendante A-01 et tag `tr2/M015` encore à réaliser.**
- **Supervision PC :** socle S0–S7 développé ; M014-H a démontré sur le TR2 réel les lectures B0/B1, le polling et la perte/reprise dans une portée bornée. Pas de preuve de mesure vibratoire V1 affichée.
- **STM32/Modbus :** B0/B1 fonctionnels dans l'essai M015-V ; correction de préparation B5 inutile sur lecture FC03 B0/B1. A04-02 retire B5 par exception04 avant I/O pendant capture ; réponse fraîche après cleanup. I/O B5 reste synchrone au repos (max144,138ms observé), boot bloquant non corrigé.

## 2. Preuves acquises et limites

- **M015-V — mission `DONE`, acquisition physique bornée :** une capture continue de **32 768 XYZ + 4 096 timestamps**, préfixes 4 096/8 192/16 384/32 768, durée environ **1,247 s** ; 304 bursts, FIFO max 158, aucun FULL/OVR observé sur essai corrigé, nettoyage final conforme. Deux lectures Modbus B0/B1 durant l'acquisition, sans erreur UART signalée.
- Contrôle RAM/CRC par bursts concordant ; bruts SPI intégraux reconstruits seulement pour les derniers 4 096. Une première tentative échouée (pause ~141 ms, FIFO FULL/OVR) est conservée séparément ; correction de la reconstruction B5 non demandée étayée par comparaison.
- **Non qualifiés :** perte globale zéro, endurance, calibration/métrologie, FFT/indicateurs, stockage de campagne V1, SDMMC 4 bits, I/O B5/FRAM/SD exécutée durant acquisition (A04-02 observe seulement son refus borné), publication vibratoire V1 et chaîne capteur→Web complète. Les tests hôte/ARM ne valent pas preuve physique.
- **Jalons :** M014 `DONE` dans sa portée ; M015 `DONE` dans sa portée d'acquisition bornée selon Codex, revue indépendante A-01 en attente ; **J3 EN COURS** ; M016–M020 à faire.

## 3. Préservation et reprise

- FRAM : génération 12 et domaine actif préservés dans les essais rapportés ; sauvegardes M014-H vérifiées et réutilisées. MicroSD non initialisée/accédée durant M015-V. Option Bytes non modifiés selon procédure rapportée.
- Après A04-02 (état historique), carte laissée avec firmware cumulatif, capteur au repos après cleanup, débogueur détaché. Vérifier **l'état réel actuel** avant toute intervention physique ultérieure ; dernier contrôle A04-02 : cleanup0C8000/READY, CRCFRAM avant/après concordant et logiquegen12 byte-identique, UART sans erreur ; pas une télémétrie en direct.
- Préserver les backups FRAM/Flash, l'identité, les médias et les archives de tests. Opérations physiques à l'utilisateur ; Codex peut flasher/déboguer dans les limites de `AGENTS.md` et de la mission.

## 4. Prochaine étape

1. **A-02 achevée** et **A-03 harmonisée** ; réaliser la revue indépendante A-01 de M015 (fiche et tag uniquement si les preuves le permettent).
2. **A04-01/A04-02 qualifiés et statuts de dette actualisés le 13/10/2026** ; conserver les limites de budget/couverture/endurance, A04-04 à A04-08 et la revue A-01 M015 distincte.
3. Exécuter **M016** : traitements et publication des vibrations dans le mapping Modbus V1 gelé, puis M017–M020 selon la roadmap.

## 4 bis. Décisions après audit A-04

- Audit A-04 livré par Codex (`TR2-20261013-A04-AUDIT-FIRMWARE`, `DONE`) ; revue documentaire indépendante effectuée, **sans qualification physique nouvelle**. Le build cumulatif reproduit le SHA-256 BIN M015-V selon les preuves Codex.
- **Registre de dette A-04 :** [DETTE_TECHNIQUE_A04.md](DETTE_TECHNIQUE_A04.md), neuf constats ouverts ou différés, avec priorités et critères de clôture. Ne pas effacer ces constats lors du passage A-05.
- Ordre décidé : **A-05 tests et preuves → A04-01 build sûr → A04-02 FRAM/Modbus → A04-03 intégration M016**. A04-08 avant activation SD ; autres dettes suivies par ID.
- La mission A-04 est clôturée par son rapport `DONE` ; le contrat `READY` historique ne doit pas être relancé.
- **13/10/2026 — A04-01, VALIDÉ LOGICIEL par Codex :** sélection explicite des profils, refus du harness SD implicite, BIN cumulatif byte-identique à M015-V. [Rapport de correction](Echanges_Codex/archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md), code `c9ee22292ae69cc6f333aaab3527d18556ee8272`. **Qualifié logiciel par ChatGPT le 13/10/2026 ; dette A04-01 clôturée dans cette portée** ; registre inchangé, aucun matériel accédé ni état actuel relu. A04-02 reste hors de cette correction.

- **13/10/2026 — A04-02, VALIDÉ LOGICIEL / OBSERVÉ PHYSIQUE BORNÉ par Codex :**126tests et cross-build ; FIFOmax293, FULL/OVR0, gapDWT5,855ms, réserve nominale7,30ms/marge1,445ms. B3 métier reste indisponible ; B5exception04 pendant capture, journal frais après cleanup. [Rapport archivé A04-02](Echanges_Codex/archives/TR2-20261013-A04-02-FRAM-MODBUS/RAPPORT.md). **Qualifié logiciel et physique borné par ChatGPT le 13/10/2026 ; dette A04-02 clôturée dans cette portée.** A04-03/M016 à exécuter.

## 5. Références durables

- [Roadmap](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md) : jalons et dépendances.
- [Rapport Codex M015-V](Echanges_Codex/archives/TR2-20261013-M015-V-COOPERATIVE-CLOTURE/RAPPORT.md) : tests, empreintes, preuves, limites et publications.
- [Historique complet pré-A-02](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md) : missions M013 à M015-V et reconstruction T1000 ; préservé byte-identique.
- [Gouvernance ChatGPT](GOUVERNANCE_CHATGPT_CODEX.md), `AGENTS.md` à la racine, et fiches A-01 dans `Modbus RTU/02_Validation/Qualifications/`.


## Décision de pilotage du 13/10/2026

ChatGPT a qualifié A04-01 (logiciel) et A04-02 (logiciel + essai physique borné) sur rapports, commits et preuves publiés. La qualification ne suppose pas une reproduction indépendante de tous les tests. Rapports durables : `Echanges_Codex/archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md` et `Echanges_Codex/archives/TR2-20261013-A04-02-FRAM-MODBUS/RAPPORT.md`. La qualification A-01 et le tag M015 demeurent distincts et en attente. M016 est la prochaine mission ; ne pas confondre DONE Codex et qualification future M016.

## M016 — 13/10/2026, précondition métier à arbitrer

- **VALIDÉ HORS LIGNE** : record de configuration conservé A04-02 CRC/décodage conformes (gen1/config1,±2g,4096,storage1/limit1MB). Le validateur C réel refuse l'environnement production `{false,0}` ; mode0/limite0 interdits parV1. Recovery UNAVAILABLE ne publie aucune ActiveConfiguration ; démarrage métier refusé.
- **BLOCAGE DE DÉCISION** : M016 sans SD/stockage disponible ne peut publier une mesure métier sous configuration autoritative avec les politiques actuelles. Choisir une voie de stockage explicitement attribué/qualifié, ou une évolution explicite de la politique de supervision sans stockage ; aucun contournement appliqué. Cartographie26champs/48registres et détails dans le rapport M016. A04-03 reste ouverte ; M016 non terminée.
- **NON VÉRIFIÉ sur cible pour M016** : aucune opération matérielle/Modbus/FRAM/SD/reset/flash/debug. Firmware conservé, dernier état documenté A04-02 inchangé ; pas une télémétrie actuelle. Aucun test/cross-build antérieur répété, aucun code fonctionnel modifié. Reprise après décision publiée, puis glueFIFO/configuration/fenêtres/validité/B3 et qualification bornée.

## M016 — stockage suspendu et banc de référence, 13/10/2026

- **OBSERVÉ PHYSIQUE antérieur** : deux écritures techniques bornées échouées, préfixes récupérables131072 et655360octets conservés dans volumesdistincts. FRAMcomplète et MBRprefix8192 identiques A04-02 ; partitionintégrale non hashée. Tentative3 préparée c4a91c1 **non exécutée**. Causematérielle indéterminée. Derniercodeflashé38c0273, probeisolé5MHz/1bit, recoveryreadonly aprèsreset, pasdeconfig disponible ; debuggerdétaché, capteurODRnon démarré. CumulatifA04-02 **non restauré** ; historique d'état antérieur à nepas traiter comme télémétrie courante.
- **DÉCISION publiée** : mission TR2-20261013-M016-SDMMC-REFERENCE-BENCH suspend les writesprincipal et interdit sa modification/reflash. Volumes/allocation/backups et deuxstashes préservés ; aucune nouvellewrite ni accèsdebugprincipal dans cette mission. Repriseproduction seulementaprès banc et nouvellemission.
- **PRÉPARÉ / VALIDÉ LOGICIEL** : projetisolé experiments/sdmmc_st_reference, STU575I-EV DMAlinkedlist/v1.9.0 adaptéSDMMC2/4bit, readonlydefault, guardUIDsecondecarte, zone8secteurs+CID+triggerRAM et3rounds préparés. DeuxbuildsARM et gardeCMake/isolationELF réussis ; pasdetestsanciensrejoués. RéférenceexacteNUCLEO nontrouvée, différencesdocumentées. Aucunequalification4bit/écriture/débit physique acquise.
- **OBSERVÉ PHYSIQUE, mission M016-SDMMC-REFERENCE-PHYSICAL** : secondeSN0032001F3235510C37333439/UID001B00463842501820333157, carte de test sacrifiable confirméeutilisateur, CID03534453/44333247/858BF092/3C01927A,62333952secteurs512. Init4bit24MHzOK ; deuxessaiswrite/readLBA2048..2055 échouésCRC données+commande précédente àlalecture,0cyclecomplet. PortDMA revenuà2nodesST/code d9a223e, pascauseprouvée. Readonly4bit4MHzCRCfail ; readonly1bit4MHz **4096octets exacts/error0 aprèsreset**, pasqualification4bit/endurance/débit.
- **BLOCAGE PHYSIQUE** : vérifiersecondDAT1/PB15/CN12-26, DAT2/PB3/CN12-31, DAT3/PB4/CN12-27, contacts/connexionspartagées/3,3V/GND horsalimentation. Questionenvoyée ; aucun nouveauwrite depuisdeuxessais. Dernierétatsecond : firmware2nodes au reposstage2/trigger0, debuggerdétaché, configurationSD temporaire4bits4MHz/rising, dernièreerreurDATA_CRC_FAIL02 ; bootdefault4bit24MHz sansautowrite. Principalnonaccédé, probeSTORAGE2 historique inchangé ; restaurationdistincte. Gels/dettes/stashes inchangés. Rapport/proofs publiés pourreprise aprèsretourphysique.

- Après retourutilisateur « le câblage semble correct », diagnosticreadonly4bits4MHz/rising encoreCRC02 ; GPIOAF conformes, DAT0–3hautsau repos, tracePB3off et UCPDdeadbatterydésactivé. Aucun nouveaushortcut/write. 0x1002 initialementmalinterprété : CRC données+commandeprécédente, **pasRXoverrun** (0x20), correctionpreuvesrapport explicite. Mesurecontinuité/conditionsélectriques nécessaire pourpoursuite ; causeexacte nondémontrée.

## M016 — GPIO DAT1/DAT2/DAT3, 13/10/2026

- **CONTRÔLÉ MCU / MESURES DC RAPPORTÉES** : firmware isolé STGPIO NUCLEO-U575ZI-Q, code9b0ad67 ; cross-build/Flashreadback conformes, SDMMCoff. PortMSPPWR corrigé après arrêt initial avantGPIO, preuves conservées. MicroSDretirée/ligneslibres/bancalimenté confirmés utilisateur avantflash.
- **Mesures utilisateur BM869s au breakout** : tout-bas=0/0/0V ; DAT1haut=3,3/0/0V ; DAT2haut=0/3,3/0V ; DAT3haut=0/0/3,3V. Septphases contrôléesGPIOODR/IDR, intervalleminimumobservé48433ms, pas de hautsimultané ; conformité statique étayée, aucune mesureohmique ou qualificationSDMMC4bit dynamique.
- **État final secondeNUCLEO** : stage4/phase6/command0/error0, trois sorties basses, SDMMCoff, debuggerdétaché. Garder microSDretirée tant quefirmwareGPIO présent, même tout-bas. Principal nonaccédé/nonrestauré, stashes et preuves antérieures préservés. Rapport pour contrôleindépendant ChatGPT et choix d'investigation dynamique ; CRC4bit reste inexpliqué.

- Mission GPIO clôturée **DONE dans sa portée statique**, après publication et relecture distante des résultats ; contrôle indépendant ChatGPT à réaliser. Ce statut ne clôture pas le diagnostic dynamique SDMMC4bits.

## M016 — 1 bit LOAD-ONLY, résultat du seul essai

- **PRÉPARÉ / VALIDÉ LOGICIEL** : profil ST indépendant code `c6c56b63e02cb78f176f278195dc114a6b7cbe76`, BIN SHA-256 `2639dee58fd27da7471c513336a29bcc7493ff92a51c65b5fb9a02ac087791f1`, cross-build réussi. Arrivée 426672 B/s, ring réel 512 KiB, chunks 64 KiB, cible 600 s ; HAL officiel inchangé. Transition sûre GPIO→SD avec retrait avant flash puis insertion hors alimentation confirmés par l'utilisateur, readback Flash conforme, PB15/3/4 INPUT. Carte et contrôleur 1 bit, kernel 160 MHz/div8=10 MHz selon registres.
- **OBSERVÉ PHYSIQUE, ÉCHEC CHARGE** : un seul RUN du 13/10 à 22:02:59 UTC, arrêt après 4,164454 s sur ring overflow (`stage99/error3`). Générés1769888, acceptés1769472, écrits1310720 octets ; pertes416, encore458752 en SRAM. Débit utile314739,94 B/s, backlogmax524288 ; vingt latences, P95 285,588 ms, P99/max323,973 ms. HAL/DMA/CMD/faults0. Dix minutes non validées ; cause détaillée de lenteur non isolée, aucun correctif spéculatif ni second RUN.
- **INTÉGRITÉ PRÉFIXE OBSERVÉE** : vingt lectures de 64 KiB de LBA2048..4607, continuation du même essai sans reset/write/retry, comparaison Python de tous les1310720 octets : zéro différence, hashes par chunk publiés. Compteur embarqué verified0 conservé (branche erreur), résultat externe distingué. Pas de qualification runtime, stockage V1, coexistence ou sept jours.
- **ÉTAT LAISSÉ / SUITE** : seconde SN0032001F3235510C37333439/UID001B00463842501820333157, CID03534453/44333247/858BF092/3C01927A, carte de test insérée ; firmware SD sûr au terminal99, producteur arrêté, aucun autowrite, GDB détaché, R1=900/HALerror0. Principal/FRAM/Modbus non accédés, worktrees/stashes préservés. Nouvelle mission nécessaire pour attribuer le déficit de débit et corriger une cause démontrée ; ne pas augmenter aveuglément le tampon. [Rapport et preuves](Echanges_Codex/DERNIER_RAPPORT.md) publiés sur main et intégralement relus au commit `5d5d9a894f13a0a7b5b5f6bf707ca9a5a0173be5` (58 fichiers) ; statut final FAILED, contrôle indépendant ChatGPT à réaliser.

## M017 — qualification 1 bit, 14/10/2026

- **VALIDÉ LOGICIEL / OBSERVÉ PHYSIQUE BORNÉ** : harness/runner code65ff1fa, HAL officiel intact ; seconde SN0032001F3235510C37333439/UID001B00463842501820333157, CIDtest03534453/44333247/858BF092/3C01927A,1bit10MHz déduits carte/registres. Diagnostic sans producteur8DMA+8polling, chaque64KiB relu exact, HALerror0 ; plateau≈190ms et READY1–3ms. Ni producteur ni DMA seuls n'expliquent l'écart C2c55ms, identité du média C2c non établie dans son rapport.
- **ÉCHEC DE CHARGE MESURÉ** : RUN1 du13/10 à22:23:22UTC, arrivée426672B/s/ring128KiB/chunks64KiB, stopoverflow après0,402315s. Générés131440, acceptés131072, écrits65536, pertes368, resteSRAM65536 ; latence248,141ms dont244,116ms dans transfert, HAL/DMA/CMD0. Tout le préfixe écrit relu conforme avant puis après réinitialisation contrôlée. Pas de600s/3runs ni qualificationV1/endurance ; répétitions arrêtées selon contrat.
- **HYPOTHÈSE NON RETENUE** : ACMD23 avant CMD25 accepté,16transferts/relectures exacts, mais latences encore≈190ms avecpause359,590ms. Aucun correctif suffisant démontré, aucun RUN2 spéculatif ; patch/source/image conservés séparément et retirés du harness actif. ÉcartC2c reste partiellement inexpliqué ; établir identités/méthodes et blocages intra-transfert avant correction/choix de média. Compteur CPU cumulé32bits à élargir avant futur600s ; n'affecte pas la preuve de l'échec court.
- **ÉTAT LAISSÉ** : seconde carte imageexpérimentaleACMD23 sûre, source65ff1fa+diff/hashes publiés, stage2/producteurdisabled/trigger0, SD1bit10MHz/carteinsérée, DAT1–3INPUT, HALerror0/R1=900/GDBdétaché ; pas d'auto-write ni GPIOrestauré. Principal/FRAM/Modbus/donnéesprincipales non accédés, stashes/worktrees préservés. [Rapport M017](Echanges_Codex/DERNIER_RAPPORT.md) publié et intégralement relu au commit76717ac408c922c63bd8779a794263a1cf498449 (112fichiers) ; statutfinalFAILED, revue indépendanteChatGPT à réaliser, trois600s non satisfaits.

## P12/H3h — SDMMC1 communautaire, 14/10/2026 (distinct de M018)

- **VALIDÉ LOGICIEL / FLASH** : sourceVILLO88/NUCLEOU5751e3c9183bad023ebf7d234134823d9270a0e8b64 inchangée, HAL1.6.2/TinyUSB0.20.1, wrapperCMakeLinux seul. DebugO0 puis ReleaseOs d'origine, BINRelease31437 SHA2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a, flashverify/readback conformes secondeSN0032001F3235510C37333439/UID001B00463842501820333157. Card32GoCID03534453/44333247/858BF092/3C01927A/62333952secteurs512 ; insertionconfirméeuser avantflash. CN11/CN12 etSB19/SB22pontés rapportésuser ; aucune soudure/correctionparamSD.
- **OBSERVÉ PHYSIQUE BORNÉ** : HALInit/WideBusHAL_OK, CLKCR4002/SSRwidth2 ⇒4bits, kernelHSI48/div2=12MHz déduits, CPU160MHz. CâbleUSB data prouvéGalaxy surmêmeport ; Nucleo0483:572B et disque32Go reconnusCN15. JeuA16writesGDB×8192 puisreadcompletUSB aprèsreset exact131072 ; jeuB16writesUSB alignées+flush/readcomplet exact131072, sanssourcefirmwarechangée. ChronohôtejeuBwrite3,1787s/read0,1874s, pasqualificationcadenceTR2/FS/checkpoint/7jours.
- **ÉCHEC FIABILITÉ / NON RÉSOLU** : CRC0x02 enDebug etRelease ; recovery nécessaire carteDATA5 aprèsmulti-read, original sansCMD12 surCRCavantDATAEND. NoMedia revient mêmeaprèsles succès ; derniererror2005/R1B00, USBconfig1, WindowsSize0/NoMedia, GDBdétaché. CausepremierCRC nonisolée, pasdecorrectifSD/bus/fréquence appliqué. PermissionsWindowsrawdiskrefusées, volumeRAWsansFS, aucuneformat ; hosttoolaligné corrigé etpreuvepréservée, aucuneélévation/ACLconfigpersistante.
- **ÉTAT / SUITE** : seconde laisséeenimageoriginaleReleaseSDMMC1, carte etUSBnatif/STLINK connectés selonuser ; n'estpasunfirmwarequalifié. Principal/FRAM/média512Go/Modbus nonaccédés, stashes/worktreespréservés. Rapport finalFAILED ; checkpoint `51da6b09306ad942cb365b8c1a5196297f76fcdb` publié surmain et134fichiers intégralement relus/byte-identiques,127empreintes vérifiées ; ChatGPT doitcadrerdiagnosticCRC/récupération et éventuellecorrectiondistincte, sansdéclarerM018 ou4bitsproductionvalidés. JournaletM017archiveactualisés, gels/dettesintacts.

## P12/H3h — comparaison fils 10 cm, 14/10/2026

- **CONFIGURATION PRÉSERVÉE / OBSERVÉE** : changement physique utilisateur 20→10cm seulement ; seconde SN0032001F3235510C37333439/UID001B0046/38425018/20333157 et CID32Go/62333952secteurs vérifiés. Readback31437octets byte-identique SHA2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a ; pas de build/flash. CLKCR4002/RCC HSI48 indiquent4bits/12MHz calculés, CPU160MHz. SSRcarte4bits acquis précédemment, non rejoué.
- **FAILED, AMÉLIORATION NON DÉMONTRÉE** : Windows NoMedia/Size0 avant tout debug ; erreur initiale3005. Un reset logiciel autorisé seulement, huit secondes puis état2005/R1B00 et NoMedia final. Zéro cycle de données/écriture opérateur, aucun benchmark ; arrêt selon contrat, pas de reset/retry supplémentaires. Effet causal des fils non isolé, pas de rejet du contrôleur ni qualificationTR2.
- **ÉTAT / SUITE** : image originaleRelease inchangée seconde carte, GDBdétaché, USBnatifs/STLINK et carte test connectés selon contrat, média non disponible ; principal/FRAM/512Go/Modbus nonaccédés, protections et stashes préservés. Rapport finalFAILED ; preuves publiées/relues byte-identiques surmain au commit `309c8654f7b8f54dedf10d958c21c885fe247e64` (33fichiers/28empreintes). Nouveau diagnostic/récupération ou campagne relève d'une mission distincte ; aucune mission suivante lancée.

## P12/H3h — COLDSTART10CM, 14/10/2026

- **FAIT UTILISATEUR / OBSERVATION** : coupure complète STLINK+USB natif puis reconnexion déclarée dans contrat, microSDtest32Go insérée et fils10cm inchangés. Windows11:04:23UTC, avanttoutGDB/reset : USB0483:572BOK mais NoMedia/Size0.
- **FAILED, ARRÊT CONTRACTUEL** : UIDseconde/CIDhandle/capacité/configuration vérifiés, readback31437octets identique SHA2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a,4bits/12MHz calculés inchangés. ErrorCode1007/R1B00/STA81000, HALREADY/sd_ready1 ne prouvent pas disponibilité. Windows11:04:31UTC resteNoMedia ; zéro cycle/écriture/reset/flash Codex, aucun benchmark ni correction. Pasdeconclusion sur efficacité des fils, causeCRC nonisolée.
- **ÉTAT / SUITE** : imageoriginaleRelease conservée secondecarte, GDBdétaché, média indisponible. Principal/FRAM/512Go/Modbus/protections nonaccédés, travaux/stashes préservés. Rapport finalFAILED ; preuves publiées/relues byte-identiques surmain au commit `8253835ef0adbe990db7842e847ae6ea12e7629d` (20fichiers/15empreintes) ; touteinvestigation/campagne suivante exige contratdistinct, non démarrée.

## P12/H3h — ROOTCAUSE-REVIEW, 14/10/2026

- **REVUE DOCUMENTAIRE** : source tiers1e3c9183 inchangée. MSP AF12/PULLUP/HIGH et schéma Adafruit avec47kΩ,10µF et100nF : absence générale de pull-ups contredite. MSC s'exécute dans main, FatFs n'est pas actif sur ce chemin. ST signale les stubs SB19/22 fermés ; signaux, rail et population physique non mesurés.
- **DÉDUCTION ÉTAYÉE / LIMITE** : CMD12 conditionné par DATAEND ; certaines sorties CRC reviennent HAL_READY sans arrêter le multibloc. CMD13/GetCardState est actif et accumule les erreurs ; ASC3A peut annoncer NoMedia malgré la présence. Premier CRC non isolé ; NoMedia avant GDB après coupure rapportée ne prouve pas VDD carte0V. Électrique, polling/USB et état initial restent à départager.
- **PREUVES PC ANTÉRIEURES** :1GiB exact,12,47Mo/s write et66,68Mo/s read ; random4K2311read/555writeIOPS avec flush final sans write-through ; pausewrite64KiB max1,08s. Manifest local vérifié, copies documentaires préparées ; aucun nouvel essai ou certificationA1/TR2. Carte dans lecteur PC selon dernier utilisateur, état non relu.
- **SUITE PROPOSÉE, NON EXÉCUTÉE** : lecture horsMSC1bit puis4bits12MHz,512/8192octets en zone connue ; premier record RAM avant clearflags, pas de CMD13 après erreur ou halt pendant polling. Nouveau contrat et gestes physiques à encadrer par ChatGPT. Revue DONE ; publication relue au commit `d107a4307c4f83900633bc41835fc550c563b29b` (19fichiers/14empreintes) ; aucune cause unique ou qualification matérielle annoncée. Principal, protections, stashes et archives préservés.

## P12/H3h — READONLY-FIRSTFAULT, checkpoint 14/10/2026

- **PRÉPARÉ / VALIDÉ LOGICIEL** : probe A isolé 1bit/12MHz, build ARM réussi code42e8dd2 (réappliqué identique8d3db40 après autorisation utilisateur). BIN21852 SHAfb2843742056093610f979268c004bb9c66455cf540069ea5d8f55531a252fb1. Capture RAM première erreur avant clearflags, pas de MSC/FatFs/write lié ; WAIT sans accès SD, oracle512/8192 préparé. B ni construit ni exécuté. Aucun ancien essai rejoué.
- **RETOUR UTILISATEUR / SÉCURITÉ** : bleuCN11-20/GND, aubergineCN11-16/3V3 confirmés hors tension ; correction du repérage erroné CN11-18=5V dans contrat initial. Maine02f7c4 suspend le matériel ; ST-LINK déconnecté, banc hors tension, SD non insérée selon retour utilisateur. CN15 explicitement déconnecté et préflight encore à confirmer ; aucune mesure de continuité/VDD0 prétendue.
- **NON EXÉCUTÉ / SUITE** : zéro flash/debug cible/reset/commande SD. Préparer ST-LINK seul sans SD, identifier cible/image, charger A au WAIT puis insertion hors tension/départ électrique unique utilisateur. A physiquePASS complet requis pourB. Principal/FRAM512Go/Modbus/protections inchangés ; pas de nouvelle qualification. Checkpoint et preuves à publier/relire surmain avant poursuite matérielle.

- **FLASH / OBSERVATION DEBUG, pas validation SD** : utilisateurSTLINKseul/SDabsente/CN15off confirmé. PréflightUIDseconde et imageoriginaleSHA2a2fe744… conformes, backup conservé. ProbeAflashverify/readback21852 exactSHAfb284374…, WAIT1/trigger0/run_count0 observés puis detach. Reset flash avant insertion uniquement ; aucun RUN A ni commandeSD opérateur. En attente coupure/insertion/départSTLINKseul utilisateur ; VDD0 à mesurer si possible ou limite à conserver.

- **OBSERVÉ PHYSIQUE BORNÉ A PASS** : seulRUN A/1bit12MHz horsMSC, VDD0,003V avantinsertion rapportéutilisateur, UID/CID/62333952secteurs512 conformes. Init/SSRwidth0/read512/read8192 au506304 conformesHALerror0 ; comparaisonintégraleembarqué+Python0différences etSHAoracles512/8192 exacts. CMD13init/postreadsTRANSFER4, premièreerreurvalid0/terminal100/run_count1/GDBdétaché. Pasdewrite/retry/resetpendantRUN. B construit seulementaprèsA PASS, mêmesoverlays/FF_PROFILE4 ; physiqueB nonexécuté, coupureutilisateurentreprofilsencorenécessaire. PasdequalificationTR2/endurance/causalitéUSB.

- **B PRÉPARÉ / FLASH VÉRIFIÉ** : buildprofile4/BIN21860 SHA fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829, imageA précédenteconforme/terminalPASS avantflashB. Readbackexact/WAIT1profile4/trigger0/run_count0, debuggerdétaché, aucunaccèsSDavanttrigger. EnattentecoupureSTLINK/VDD0 si possible/reconnexionutilisateur pour uniqueRUN B ; A acquis ne sera pasrépété.

- **B FAILED / PREMIÈRE ERREUR ISOLÉE, arrêt 14/10/2026** : cycleSTLINK confirméutilisateur, VDD0Bnonrapporté. Init et sélection4bitsHAL_OK/CLKCR4002 ; SSR64/ACMD13HAL_ERROR2. RecordRAMphase4/overlayL3401 avantclear : STA2/DCRCFAIL, RESPCMD13/R1=920/ARG0, DCTRL62/DLEN64/DCOUNT0, IPSR0, HALerrorencore0 ; pasRXOVR/timeoutdanscapture. SSRwidthBnonvalide (0initialisé), aucunCMD17/CMD18/CMD13harness ensuite, run_count1/terminal99. HALoriginalenvoieCMD16/512aprèsclearavantretour, limiteexplicite«aucunecommandepostfault» ; aucunecommandeajoutée/recovery/retry.
- **DÉDUCTION / SUITE** : CRCreproduithorsUSB/MSC etavantmultibloc ; USB/recoveryCMD18non nécessaires àcedéfaut précis. Sensibilitéprofil4bitsétayée, D1–3/débitFIFO/rail/médianonisolés. Nouvellemission pourvérificationdynamiqueciblée oucomparatifunevariable ; aucunecorrectionspéculative.
- **ÉTAT FINAL OBSERVÉ** : secondeimageB SHA fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829 au99/trigger0/run1/HALREADY/error2/Context0, debuggerdétaché, aucunautotraficSD ; carteinsérée/STLINKseul/CN15off. A PASSpréservé, pasrestaurationUSBautomatique ; principal/FRAM512Go/Modbus/protections/stashesintacts. RapportFAILEDsignifieéchecqualificationB, diagnosticpremierCRCacquis ; preuvesàpublier/relire, contrôleChatGPTdistinct.

- **PUBLICATION VÉRIFIÉE** : rapport final FAILED et preuves A/B publiés sur main au commit `fc737cd02a5b3702c685c5922ddd34a40f7c301a`, puis 86 fichiers relus intégralement/byte-identiques et 74 empreintes vérifiées. Archives et documents protégés, dépôt historique, stashes et upstream préservés. Protocole arrêté sur premier CRC B, aucun nouvel accès matériel ; contrôle indépendant ChatGPT et future mission distincts.

## A-06 — découplage supervision / enregistrement, 15/10/2026

- **DÉCISION UTILISATEUR PUBLIÉE** : la supervision vibratoire doit pouvoir progresser et fonctionner sans média de campagnes ; enregistrement industriel conservé pour réintégration après qualification. La V1 gelée n'est pas modifiée par cette décision ni par l'audit.
- **AUDIT DOCUMENTAIRE / PROPOSITION** : [étude A-06](ETUDE_A06_SUPERVISION_SANS_STOCKAGE.md), référence `45ee898`, parcours des 2 167 fichiers spécification/validation et inspection des dépendances réelles. Validateur et recovery exigent encore une capacité de campagne pour l'active ; calcul et archivage SQLite ont des responsabilités distinctes. Recommandation : extension versionnée, configuration live autoritative en FRAM, commandes propres, aucune fausse campagne. Encodages, migration et états à arbitrer par ChatGPT avant implémentation.
- **NON IMPLÉMENTÉ / NON QUALIFIÉ** : aucune modification du firmware, mapping ou FT/TT ; aucun test hôte/ARM/physique répété, aucun accès matériel. État matériel hérité inchangé, pas relu ; M016/A04-03 et dettes existantes restent ouverts. Prochaine étape : contrôle indépendant de l'étude, publication du contrat normatif puis intégration/qualification live bornée ; stockage séparément, sans abandon.

## R0 — norme LIVE successeur, 15/10/2026

- **SPÉCIFIÉ / NON IMPLÉMENTÉ** : [profil LIVE R0](../01_Specification_source/LIVE_R0/README.md), protocole 2, B4/B6 version 2, commandes 13/14, tuple LIVE 2/0/0 avec durée 0 continue ; héritage transactionnel B5 V1.1 explicite. Pas de redémarrage implicite après reboot. Les propositions A06 de durée bornée sont remplacées pour le successeur, archives intactes.
- **CONTRÔLÉ DOCUMENTAIREMENT** : mapping 187 champs / 390 registres, 106 scénarios et 384 contextes START/STOP spécifiés, trois vecteurs préparés/actifs vérifiés dont V1 CRC5207CCFC inchangé. Layout historique calculé depuis le code ; partition LIVE disjointe prévue [122880,245760), disponibilité physique non vérifiée. Identité persistée et règle UNAVAILABLE du recovery identifiées comme dépendances R1.
- **SUITE** : contrôle indépendant ChatGPT avant R1 ; disponibilité/attribution FRAM, migration réelle, implémentation epoch, budgets des commits complets, tests et physique restent à réaliser. Aucun accès matériel, firmware/FRAM/microSD inchangés et non relus. Dettes/jalons historiques conservés, pas de qualification nouvelle.


## R1 — configuration LIVE / FRAM, 15/10/2026

- **IMPLÉMENTÉ / VALIDÉ LOGICIEL** : backend indépendant de la SD, validation LIVE R0, codecs configuration/identité format 2, projection B4 en mémoire, activation transactionnelle format logique 3. Migration explicite dans [122880,245760), contrôle de propriété obligatoire, provenance SHA256/génération/UID ; historique et tail préservés. Tests sur vrais stores derrière un transport mémoire, 814 budgets de coupure, corruptions/refus et erreurs de lecture ; 129 tests hôte et cross-build cumulatif. Détails, preuves et limites : rapport R1 et [LIVE_R1.md](../05_Firmware/LIVE_R1.md).
- **LIMITE D'INTÉGRATION EXPLICITE** : composition production protocole 1 conservée. Identité protocole 2 préparée sans bits acquisition/SD/LIVE ; B4 schema 2 non raccordé au RTU. Ancre epoch 1 réservée seulement ; aucun moteur 12/13/14, auto-START, campagne nouvelle ni calcul B3 LIVE. R1 refuse les états futurs qu'il ne sait pas interpréter. Le contrat R1 acte la réception documentaire R0 pour cette entrée en développement ; contrôle indépendant de l'implémentation reste ChatGPT.
- **RESSOURCES / SUITE** : candidate partagée 52298 octets, service ARM 576, second média 96, soit +1072 octets hors alignement par rapport à l'ancienne autorité seule. Commit 52426 octets écrits, synchrone et relu ; ni timing électrique ni marges R4 qualifiés. R2 doit raccorder la vraie autorité epoch/journal et les commandes avant exposition protocole 2, puis R3/R4/R5/R6 selon roadmap.
- **MATÉRIEL NON TOUCHÉ / NON RELU** : aucun flash, debug, reset, écriture FRAM, accès SD ni émission RS-485. Propriété réelle de la zone LIVE non déterminée, aucun provisioning autorisé par supposition. État matériel hérité conservé ; HP retiré selon gouvernance §10, futur banc unique T1000 à inventorier avant usage. Worktrees/stashes et dettes historiques préservés.
- **PUBLICATION R1 VÉRIFIÉE** : code `f4697febfd5cea267a3c4fa21a1f1e0071845665`, premier rapport/preuves `450ca809929213b4eff13f21e576ad8b75b7196d` sur main ; 49 fichiers relus depuis GitHub, byte-identiques et empreintes contrôlées. Clôture logicielle Codex, contrôle indépendant ChatGPT distinct ; matériel non accédé.


## R2 — moteur epoch / commandes LIVE, 15/10/2026

- **VALIDÉ LOGICIEL** : vrais stores R1 + journal4/TR2Q/TR2E, machine RESERVED/STARTED/reçu/COMPLETED, commandes12/13/14 et APPLY1 ; B5 dédié29registres, pas de campagne ni accès SD. 384 décisions R0, 126 admissions réelles/slots STOP126 et renew127, 1561 budgets de coupure et barrières recovery28/29/9 sans redispatch. Régression130tests, sanitizers, cross-build et lien ARM ciblé ; preuves/limites dans rapport et [LIVE_R2.md](../05_Firmware/LIVE_R2.md).
- **CORRECTION DÉMONTRÉE** : conserver le dernier terminal durable pendant la réutilisation du slot de renouvellement ; les deux copies logiques s'appuient sur l'enveloppe atomique, pas une troisième réservation. Run_id réservé dès RESERVED, jamais réutilisé après abort ; horodatage historique, y compris zéro valide, non redaté.
- **COMPOSITION NON RACCORDÉE** : production toujours V1/B5 20registres ; aucune annonce LIVE/protocole2 sur RTU. Port causal STM32 continu absent, FIFO de qualification borné conservé ; B1/B6/B7 successeur, calcul/B3 R3 et effets hérités2..11 à composer avant exposition. Sans port complet ou budget explicite, START refusé. Moteur livré logiciel avec ces limites, pas firmware LIVE opérationnel ni jalon physique qualifié.
- **RESSOURCES / SUITE** : ARM moteur480/store9656/config584/média96/candidate52298 ; design autonome63210octets hors alignement, +10144 sur R1. START nominal :209704octets écrits/1255664lus dans le modèle ; quatre commits synchrones. STOP/réserves numériques vérifiés, marges FIFO/Modbus/FRAM physiques R4 non qualifiées. Compléter composition et R3 sous contrôle indépendant ChatGPT avant usage physique.
- **MATÉRIEL NON TOUCHÉ / NON RELU** : zéro flash/debug/reset/FRAM/SD/RS485 ; disponibilité/propriété réelle de la zone LIVE non établie, aucun provisioning. État matériel hérité conservé sans restauration supposée. HP retiré ; futur banc T1000 à inventorier. Worktrees, stashes, corpus R0, archives et dettes préservés ; R1 mission/rapport archivés intégralement.
- **PUBLICATION R2 RELUE** : code59d3757e88c6fadbc36a2de1cb17c6b67a15186a, premier rapport/preuves b78e7105a9a7da7ed6ba6fe3d57b0bfbc6cf0204 ;45 livrables relus depuis GitHub byte-identiques,29 empreintes vérifiées. Clôture de livraison logicielle avec limites de composition explicites ; revue indépendante ChatGPT et raccordement complet restent distincts.


## R3-A — propriétaire continu et architecture,15/10/2026

- **VALIDÉ LOGICIEL** : owner FIFO continu sans SD/tampon896octets, parser/statut réutilisés sans changement de corps, port R2 causal avec readback, binding transport STM32 qualifié et claim unique par boot. Intégration moteur/store R2 sur SPI simulé, >4M XYZ modèle/quatre FS, fautes/STOP et131tests hôte+ARM/sanitizers. Aucun RMS/B3 ni protocole2 publiés ; composition canonique V1 conservée. Détails [LIVE_R3A.md](../05_Firmware/LIVE_R3A.md).
- **REVUE ARCHITECTURALE** :407modules/interfaces/tests inventoriés ; références CMake/symboles ELF, pas de code déclaré mort. Avant R1/R2/R3-A :30415/31768/31999lignes C/C++ propriétaires, tests45422/46516/46620, docs140951/141714/142059 au commit code ; pas d'assimilation LOC et ajouts Git. Parsing/transport mutualisés, deux contrats B5 conservés, aucun retrait arbitraire.
- **SUITE / RISQUES** : owner ARM1064octets, adaptateur1088 ; canonique text170916/data480/bss602232. FRAM synchrone R2 et budgets STOP/FIFO non qualifiés R4 ; fenêtres/calcul/projections/RTU2 et politique axes R3-B à compléter. Pas de preuve physique de cadence ni endurance.
- **MATÉRIEL PRÉSERVÉ** : énumération STLINK seule SN003500463235510B37333439, aucun accès cible/flash/debug/reset/FRAM/SD/RS485 ; état hérité non relu. Carte/connexion/vue électrique et propriété zoneLIVE non établies ; pas de restauration supposée. Mission/rapport R2 archivés, worktrees/stashes/gels/dettes préservés.

Publication R3-A : code 0c6f002, preuves/rapport a33066d publiés sur main et relus intégralement le 2026-10-15 (2332 fichiers protégés et stashes préservés). Clôture logicielle uniquement ; qualification physique et contrôle indépendant restent distincts.


## R3-B — métrologie LIVE/B3, 16/10/2026 (date locale)

- **VALIDÉ LOGICIEL** : code e80e252 puis1c5368f (fin de fichier seule), fenêtres N/P continues, mg/isqrt partagés V1, masques et saturation conservatrice, qualité/âge/UTC, alarmes/hold/latch/compteurs saturés. Binding causal R3-A et notification R2 de perte d'autorité ;133tests,448oracles indépendants,>4MXYZ supplémentaires sur SPI simulé, ASan/UBSan et ARM. [Détails/limites](../05_Firmware/LIVE_R3B.md). Aucun RTU2 ni B3 physique annoncé.
- **R4 PRIORITAIRE / DÉDUCTION** : chaque phase FRAM R2 lit313916/écrit52426octets ; à2,5MHz calculés dans main.c,>=1,172s utiles versus17,066ms FIFO vide nominal. Pas mesure électrique ni budget approuvé. Proposition quantaFRAM+CRC/hash coopératifs sans réentrance et STOP prioritaire, à instrumenter/qualifier. Provider temps capteur observé/calibré encore requis, aucune datation physique inventée.
- **ARCHITECTURE / SUITE** : accumulateur et conversion extraits avec corps identiques, calculateur/projection métrique réutilisés ;414modules/interfaces/tests et187champs de readiness documentés. Résultat32341lignes C/C++ propriétaire/47329tests/142428docs au commit code. Métrologie720octets ARM, adapter1096/engine488 ; mémoire et pile physiques non qualifiées. Composition B0/B1/B4/B5/B6/B7/effets2..11, temps et R4/R5/R6 restent à terminer avant exposition LIVE.
- **MATÉRIEL / PRÉSERVATION** : aucun accès cible/flash/debug/reset/FRAM/SD/RS485 ni inventaire matériel actuel ; état hérité conservé, pas relu/restauré. Worktrees/stashes et2334fichiers protégés inchangés, R3-A archivée intégralement. Écart méthodologique signalé : générateur oracles créé après première rédaction du service, avant compilation ; attendus indépendants non ajustés. Contrôle indépendant ChatGPT distinct.

Publication R3-B vérifiée : code1c5368f, preuves/rapport faf0f94 sur main relus intégralement depuis GitHub le15/10/2026UTC (16/10local) ;44empreintes et2334fichiers protégés contrôlés. Clôture logicielle Codex, contrôle indépendant ChatGPT et qualification physique distincts.


## R4 — coopération et blocage causal,16/10/2026

- **VALIDÉ LOGICIEL** : codef3ae3a6, quantaFRAM/CRC/SHA/recovery et contrôle de réentrée/ownership, formats R0/R1/R2 conservés.136tests/ARM PASS,74,52s ; ASan/UBSan4suites PASS,141,52s ;1561oracles R2 préservés. Modèle q32 prolongé624,225s/13859117XYZ/39033RTU, FIFOmax19/RXqueue8/overflow0. Pas des temps cible.
- **OBSERVÉ PHYSIQUE BORNÉ** : TR2-001/UID et ST-LINK attendus confirmés, double backup initial. Préfixe FRAMreadonly/FIFO180826XYZ, niveau18, gap601us, read132us/service461us/arrêt235us ; RUN entier interrompu par GDB pendant contrôle à l'arrêt, nonPASS. Premier-mot11ms, politique initiale distincte du gap5ms ; timeoutHAL1ms corrigé2ms, V1inchangé. R1 migration/config2 réussies ; harness init réordonné avant mount sans répéter ces écritures.
- **CORRECTION CIBLE** : R2 dépasse1030us au seuil1000us, arrêt sûr, START non confirmé. Pile readonly situe le bloc entre copies/décodages journal ; points de service ajoutés, seuil conservé. Recovery physique corrigé PASS6,275s/workmax907us/0write, FRAM inchangée ; statut9/STARTED indéterminé conservé, aucun replay. Ce succès ne qualifie pas écritureR2+FIFO.
- **BLOCKED / DÉCISION CHATGPT** : R0 interdit redispatch/effacement et reçu inventé. Définir maintenance/réconciliation ou reprovisionnement LIVE expressément autorisé pour cette trace archivée, puis reprendre la campagne START/RENEW/STOP/APPLY sans refaire validations inchangées. WCET fautesSPI3/IRQ/STOP, RTU électrique (T1000seulCOM3), CPU/pile et provider temps calibré restent à qualifier ; pas RTU2/B3 physique ni budget production.
- **MATÉRIEL FINAL VÉRIFIÉ** :07:50:12UTC, Flash2Mio intégralement identique à l'état initial, M016 commissioning WAIT/GDBdétaché ; pas nouveau cumulatif LIVE.14pages reprogrammées normalement, ni mass erase ni OptionBytes. FRAM finale SHA c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561,566octets modifiés seulementLIVE ; historique/tail inchangés. Backups locaux reports/TR2-r4/hardware. Aucun essai SD/RS485 R4.
- **PRÉSERVATION / PUBLICATION** :53worktrees, stashes et2336fichiers gels/dettes/archives préservés, ancienmain82169af propre.421modules inventoriés ;32868lignesC/C++/47665tests au codef3ae3a6. Journal/roadmap et rapport mis à jour ; bilanff40f8e publié et relu intégralement (286fichiers/256preuves), statut finalBLOCKED pour décision de maintenance STARTED. Contrôle indépendant ChatGPT distinct de la clôture Codex.


## R4-M — 16/10/2026, campagne arrêtée conformément au contrat

- **OBSERVÉ / PRÉSERVÉ** : UID/STLINK vérifiés ; doubles backups frais Flash2Mio/FRAM262144 conformes R4. Ancien STARTED reste indécidable, archive intégrale c814e9cc…271561 et échanges R4 conservés, historique/tail inchangés.
- **VALIDÉ LOGICIEL** : maintenance isolée CLOSED/RETIRING,142coupures+4phasesRENEW ; checkpoints recovery additionnels après premier dépassement.137tests complets PASS56,30s/sanitizer ciblé1,38s/ARM ; BINcanonique V1 inchangé6a2150af…15a9. Code physique final3606942.
- **OBSERVÉ CIBLE** : isolation unique puis1075us>1000us avant RENEW ; pont c868950f…af950 sauvegardé deux fois. Reprise explicitement liée à ce pont, sans réisolation : vraiRENEW1->2 COMPLETED4/0, ancienneclé refus24/0write. START(epoch2,tx1,run2) échoue1241us, nouveau STARTED9,0XYZ ; STOP réel observé236us, FIFOarrêté. Aucun essai/cleanup supplémentaire après cette nouvelle indétermination.
- **RESTAURÉ / BLOCKED** : Flash initiale exacte2Mio c5bc344d…f5b26, FRAM finale f2bd7a99…8a0c1 conservée sans rembobinage, M016 CommissioningReadOnly enattente/GDBdétaché. STOP de commande/APPLY/coexistence FIFO/reboot final/faute injectée non exécutés ; aucune qualif R6/B3/RTU2.
- **À TRANSMETTRE À CHATGPT** : nouveau verrou impose l'arrêt du contrat ; ne pas réutiliser run2 ni retirer le verrou sans nouveau plan explicite. Qualifier d'abord hors écriture les quanta du port START/observe ; cause exacte1241us non attribuée. Rapport et snapshots sans perte publiés pour contrôle indépendant, aucune décision normative R0 modifiée.
- **PUBLICATION VÉRIFIÉE** : bilan e0d87711e65c85919d5ca1e2bc7c2bd599181f99 relu intégralement depuis GitHub (151fichiers,133preuves SHA) ; worktree propre/HEAD=origin/main. Contrôle indépendant ChatGPT distinct ; statut BLOCKED technique maintenu.


## R4-D — 16/10/2026, diagnostic et correction de la tranche temporelle

- **OBSERVÉ CIBLE** : START905–907us +OBSERVE102us dépassent1000us sans yield ; garde1012us et arrêt sûr reproduits sans admissionR2/writeFRAM. Trace36/3/9callbacks, surcoût mesuré1,44–1,57us/callback. Codec authentique227us, total≈1235–1237us cohérent avec1241us historique, causalité historique exacte non reconstruite.
- **CORRIGÉ / VALIDÉ LOGICIEL** : code7a6a8b9, checkpoints avant effet/entre effet et observation/après observation, causalité et budgets inchangés. Nouveau test échoue avant/passe après ;138tests PASS56,80s,4suitesASan/UBSan PASS16,06s, ARM et BINcanonique V1 inchangé. Warning longjmp hors tranche bloque le premier build sanitizer global ; suites ciblées seulement déclarées validées.
- **OBSERVÉ PHYSIQUE BORNÉ** : exercice volatil des frontières corrigées workmax911us/service2us, marge nominale89us ; faute logicielle unique avant callback11, nettoyage/relectures conformes. Aucun moteurR2 complet ni FIFO/FRAM sous charge, aucun WCET faute électrique ou qualificationR6.
- **PRÉSERVÉ / RESTAURÉ** : doubles backups frais conformes R4-M, Flash initiale restaurée exacte2Mio c5bc344d…f5b26, FRAM inchangée f2bd7a99…8a0c1 après les deux probes et restauration. STARTEDepoch2/run2 intact ; M016CommissioningReadOnly/GDBdétaché. Archives R4-M,54anciensworktrees et deuxstashes préservés.
- **À TRANSMETTRE À CHATGPT** : R4-D clôturée DONE dans sa portée après publication/relecture ; campagne R2/R4 reste suspendue au verrou persistant et à un contrat distinct. Pas de nettoyage automatique ni de réceptionR6/B3/protocole2.
- **PUBLICATION RELUE** : bilan85fb51e7aca24d210b60fb88d656a824617c6c49,104fichiers comparés et92preuves SHA depuis GitHub ; clôture documentaire, contrôle indépendant ChatGPT distinct. Worktree propre/HEAD=origin/main au bilan.


## R4-P — 16/10/2026, coexistence FRAM/FIFO observée dans le vrai moteur R2

- **OBSERVÉ PHYSIQUE BORNÉ** : code9f429e513faba26ce89b760361c9cba08057bc24, correction R4-D conservée ; vraiRENEW2→3, START3/run3, RENEW3→4 sousFIFO, RUN60,000989s, STOP/APPLYconfig3/START4/run4 tous COMPLETED4/0.6454507XYZ/806814TS, niveau25, pas d'erreur FIFO/stop signalée ; work913us<=1000/service566us<=2500/gap822us<=5000.1310650octets/40975commandes écrits uniquementLIVE, scans complets protégés278528octets.
- **DÉFAUT HARNESS CORRIGÉ / PREUVES CONSERVÉES** : isolation logique unique52426octets puis premier arrêt1075us avant RENEW, aucun nouveau STARTED. Pile readonly démontre3840itérations de saut LIVE sanscheckpoint ; correction O(1), test139/sanitizer/ARM PASS. Contrôle physique readonly work342us/0write et pont713a inchangé avant reprise explicite sans réisolation. Ancien STARTED2/run2 demeure historiquement indécidable et archivé ; aucune réadmission/reçu inventé, ancienneclé refus24.
- **ARRÊT / RECOVERY OBSERVÉS** : faute logicielle8 après COMPLETED, arrêt235us/réaction242us ; pas faute électrique/WCET. Reset logiciel/recoverylectureseule READY/0write/0autostart, epoch4/previous3, dernier START4/4/run4/config3, racine37. Snapshot complet finala13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9 et CRC audités ; historique/tail exacts.
- **RESTAURATION VÉRIFIÉE** : Flash initiale2Mio exactec5bc344d…f5b26, M016 CommissioningReadOnly WAIT/GDBdétaché ; FRAM finale relue après restauration et conservée sans rembobinage.55anciensworktrees/deuxstashes/2830fichiers protégés préservés, R4-D archivée byte-identique. Aucun accèsSD/HP/RS485/OptionBytes ni interventionphysique.
- **SUITE / LIMITES** : R4-P livrée pour contrôle indépendant ChatGPT ; R4/R6 complets non qualifiés.87us de marge nominale ne bornent pas les fautes électriques ; CPU/pile/IRQ/TX/imagesRAM/providertemps/B3 et compositionLIVE restent à qualifier par contrat distinct. Pas de perte globale zéro ni endurance annoncées ; premiermot mesuré depuis admission inclut les commits, distinct de Ton. [Plan et preuves](../05_Firmware/LIVE_R4P.md), rapport courant et snapshots publiés selon protocole.

- **PUBLICATION R4-P VÉRIFIÉE** : bilan13e5c840c29ca896ae0e532fbcca38b86c4e069f relu intégralement depuis GitHub à13:21:03UTC (170fichiers/155preuves SHA). StatutDONE de livraison R4-P publié après cette relecture ; contrôle indépendant ChatGPT distinct, R4global/R6 non qualifiés. Worktree propre/HEAD=origin/main après le bilan, ancienmain et stashes préservés.


## R4-Q — 16/10/2026, composition B3/RTU préparatoire et probe readonly

- **VALIDÉ LOGICIEL** : code6e813501a973fb25bd41bf3b0713435763414779, vrai owner FIFO/R3-B→snapshots RAM B3→RTU, B4staging/APPLY immuable et B5capture différée/R2 hors callback. Trois captures +réserveSTOP, nonréentrée/quanta/refus sans retry automatique ; âge à chaque lecture/copie sans callback, UTC absente visible.140tests PASS61,23s/ARM et sanitizers ciblés, quatreN. Modèle1554683XYZ/7019RTU, FIFO26/RX6/work484/service498/gap613us : simulation, pas mesures cible.
- **OBSERVÉ CIBLE BORNÉ** : probe NON-PRODUCTION10,000754s readonly FRAM+RTUloopbackRAM ;872réponses (218B0/B3/B4/B5), work341us/service1074us/PDU637us/publication378us. Capteur arrêté,0XYZ/0commande/0writeFRAM ; B3inactif1/flags4097/V0/F0, B0préparé2/cap12. Aucun UART/RS485/IRQ physique testé, gap0 ne qualifie pas FIFOinactive.
- **PARTIAL / SUITE** : logiciel de composition partielle livré, pas firmware LIVE complet. Fournisseur de temps événement STM32 qualifié toujours absent : aucune cadence supposée exacte/valeur vibratoire physique fabriquée. B1/B2/B6/B7/effets hérités et capacités produit à compléter, puis physique FIFO+calcul+RTU/IRQ/TX/STOP sous charge/fautes ; pas R6/R5/qualification industrielle ou perte globale zéro. [Contrat technique](../05_Firmware/LIVE_R4Q.md).
- **PRÉSERVÉ / RESTAURÉ** : préflight UID/SN/WHO7B/Flash2Mio/FRAM complets conformes R4-P, backups vérifiés réutilisés. Flash M016 initiale restaurée exacte2Mio à15:00:46UTC (17pagesnormales), FRAM entière inchangéea13b3401…a2ef9/historique+tail exacts ; epoch4/run4/config3 et reçus conservés, aucun rollback.56anciensworktrees/deuxstashes/2349fichiers protégés/3745preuves et archives R4-P préservés. Aucun SD/HP/OptionBytes/interventionphysique.
- **V1 / REVUE** : profil/recette V1 conservés, handler NULL par défaut ; nouveau BIN85b1be60…c0f5 différent par runtime/disposition,140tests pour nonrégression de comportement, pas de byte-identité annoncée. Ce canonique n'a pas été flashé. Rapport/preuves publiés selon protocole pour contrôle indépendant ChatGPT, limites R4-P/R4/R6 conservées.

- **PUBLICATION R4-Q RELUE** : bilan30c6b710d50bd3eda09c81249432b6925190c59d,110fichiers comparés intégralement/94preuves SHA256 vérifiées depuis GitHub. Livraison DONE logiciel dans le périmètre incrémental, couverture physique PARTIAL inchangée ; contrôle indépendant ChatGPT distinct. Worktree propre/HEAD=origin/main au bilan, ancienmain préservé.


## R4-R — 16/10/2026 UTC, BLOCKED après dépassement de service

- **OBSERVÉ / PARTIEL** : code0e808f3 ; diagnostic complet2,000064s/52894XYZ/6612TS,
 TS avantXYZ mêmecompteur/delta24,0anomalie/niveau13/poll401us/gap425us.
 Calibration197brackets DWT12,54054us/tick/résidu22us/bracket33us, pasUTC/SI.
 Provider8XYZ ancré surTS réels testé ; premierdiagnosticinterrompuGDB conservé.
- **BLOCKED R2 / B3 NON QUALIFIÉ** : vraiSTART4/5/run5,118XYZ/15TS/112datés,
 service2715>2500us/work922us/gap2141us/niveau64, STOPsûr235us.3660RTU RAM
 contrôlés/0B3valide, pasfenêtre4096/HELD/STOPcommande/10sreadonly/dérivelongue.
 STARTEDgen26/INTENT15/phase1/racine39 CRCvalides ;run5 jamaisréutilisable.
- **CANDIDATE LOGICIELLE** :83d4fbd, publication avantrequête et-O2 profilbanc,
 ARM/sanitizers-O2/provider+448oracles PASS, **nonflashée/nonchronométrée**.
141tests/ARM PASS, canoniqueV1 BIN85b1be60…c0f5 inchangé/nonflashé.
- **RESTAURÉ / PRÉSERVÉ** :20:57:06UTC FlashM016 exacte2Mio c5bc344d…f5b26,
18pages normales ; FRAMcomplète14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9
 inchangée aprèsrestauration,113octetsdifférents seulementLIVE/104852écrits.
 Historique/tail exacts ;M016CommissioningReadOnlyWAIT/capteurarrêté/GDBdétaché.
57worktrees/deuxstashes/4021fichiers protégés etR4-Q archivée préservés.
- **DÉCISION CHATGPT / SUITE** : nouveaucontrat sursnapshot14fb, qualifierd'abord
 servicecorrigé sansadmissionR2, puis traitementexplicitementautorisé deSTARTED
 avantnouveauSTART. Aucunreplay/effacement/reçuinventé/rollback. R4global/R6 ouverts.
[Plan etpreuves](../05_Firmware/LIVE_R4R.md). Checkpoint5e4fde6 relu113fichiers/97preuves.

- **BILAN DISTANT RELU** :444f01acc07ec4eb1389b508b634ead7710a1923 récupéré de
 GitHub,155 fichiers comparés intégralement/138 preuves SHA vérifiées ; statut
 BLOCKED technique maintenu. Publication et restauration démontrées, B3 physique
 et correction candidate non qualifiés ; contrôle indépendant ChatGPT distinct.


## R4-S — phase 1 en cours, 16/10/2026 UTC

- Préflight principal conforme M016 c5bc et FRAM14fb/STARTED5, sauvegardes vérifiées.
 Premier probe readonly :31787XYZ/3974TS, work335/service2322/gap2457us,96RTU
 cohérents et fenêtre4096 diagnostique contrôlée horsligne ; V/F toujours0.
- Arrêt sûr après1,206563s sur écart d'affine fixe109>100us ; FRAM complète inchangée,
 aucune admission R2. Suivi d'horloge continu préparé, même garde100us,141tests/ARM/
 ASan+UBSan-O2 ciblés PASS. Seconde qualification volatile àpoursuivre avantphase2.
- État : probe arrêté/GDBdétaché, restaurationfinale M016 àfaire aprèsqualification ;
 aucun isolement/RENEW/START, run5/epoch4 conservés. Pas encore de B3 LIVE autoritatif.

- **R4-S phase1 PASS** : troisièmeimage,10,001045s/265669XYZ/33209TS/5fenêtres
 diagnostiques/808RTU V0/F0. Work333/service1804/gap1902us/niveau58, gardehorloge
100us conservée/99contrôles/résidumax10us.4096bruts oracleRMS959/peak998mg global.
 Correctionprofilseule : feuillesstatistiques/calculateur-O2 ; premieraffinefixe
 etsecondservice2511us conservés. FRAMcomplète14fb exacte, aucunewrite/admission.
 Capteurarrêté/probedédié/GDBdétaché ; phase2 horsligne àdémontrer avantmutation.


## R4-S — bilan final du 16/10/2026

- **OBSERVÉ PHYSIQUE BORNÉ / phase1 PASS** : code c2395d9,10,001045s,265669XYZ/33209TS ; work333/service1804/gap1902µs, seuils1000/2500/5000 conservés, FIFOmax58/erreurs0. Cinq fenêtres diagnostiques,808RTU RAM cohérents, oracle4096 exact ; autoritéfalse, aucun B3 valide autoritatif. Suivi compteur/DWT :99contrôles,résidumax10µs/seuil100. Deux essais antérieurs échoués conservés séparément (dérive109µs, puis service2511µs) ; causes corrigées et attribuées.
- **VALIDÉ LOGICIEL** :141tests/main,ARM,profil/O2/ASan/UBSan/oracles PASS. Feuilles de calcul-O2 seulement profilR4S ; V1/R2 inchangés. **BLOCKED phase2** : retrait R4-M/R4-P sur copie authentique fait génération journal26→0→4 aprèsRENEW ; runfloor5 préservé/epoch4→5. Anciennes gardes refusent14fb. Décision structurante requise sur « aucun reset des compteurs », aucune exception supposée. Étude locale86d0f4b isolée/nonpoussée ; image04 jamais flashée. Phase3 nonexécutée ; R4global/R6 ouverts.
- **RESTAURÉ / PRÉSERVÉ** : M016 et FRAM intégrales exactes aux SHA ci-dessus ;0écritureFRAM/0admissionR2, historique/tail exacts, aucuneSD/RS485/HP. Reprise après décision publiée et preuve d'applicabilité sur copie ; ne pas rejouer la phase1 inchangée. Rapport/preuves R4S publiés pour contrôle indépendant ChatGPT ; aucune qualification de jalon par Codex.


## R4-T — 16/10/2026 UTC, préparation de la retraite monotone

- **VALIDÉ LOGICIEL / physique en préparation** : retraite slot3 + vrai RESERVED RENEW27 dans une unique enveloppe, puis steps28/29/30 ;568coupures/reboot/anti-rejeu et sanitizer ciblé PASS, ARM canonique/probe PASS. Cœur R2/R0/V1 inchangés ;141tests R4-S acquis non répétés. Aucune mutation physique à ce checkpoint.
- **PRÉFLIGHT OBSERVÉ22:00:20UTC** : UID/SN/capteur arrêté conformes ; FlashM0162Mio c5bc…f5b26 et FRAM14fb…2aa9 exactes. Suite autonome sous gardes du [plan R4-T](../05_Firmware/LIVE_R4T.md) publié avant mutation ; pas de B3 autoritatif encore démontré.

- **R4-T première tentative22:03:55UTC** : refus R2 5/7 par budget I/O non raccordé ;0write/0admission, snapshot FRAM entier14fb inchangé. Probe01 terminal/capteur arrêté/GDBdétaché, M016 restaurable depuis backup. Raccordement du prédicat existant corrigé, contre-test de refus PASS ; image02 distincte préparée, poursuite autonome sans réisolation durable.

- **R4-T maintenance physique PASS22:06:09UTC** : RENEW réel4→5, journal26→30/root39→43, runfloor5/INTENT/config3 conservés, FRAM f035749d…e312 exacte au modèle, historique/tail intacts. Ancienneclé refus24/0write. Phase5 FIFO/B3/R2 lancée sur image02/code79ba06a ; résultat en attente, pas de qualification anticipée ni rollback.


## R4-T — résultat final du16/10/2026UTC (17/10 local)

- **OBSERVÉ PHYSIQUE PASS BORNÉ** : code79ba06a, retraite slot3+vrai RENEW atomique sans pont vide, journal26→30/epoch4→5/runfloor5 ; suite START6/STOP réels,3918509XYZ/489814TS/74fenêtres4096/13548RTU RAM cohérents dont11989V/F. Oracle première4096exact ; UTCabsente, STOP/HELD observés. Work915/service1964/gap2203µs, FIFOmax66/erreurs0,1465contrôles horloge/résidumax13µs.
- **VALIDÉ LOGICIEL / LIMITES** :568coupures/contre-tests/sanitizers nouveaux PASS ;141tests R4-S sur cœur inchangé réutilisés, pas de suite143complète annoncée. Premier refus5/7 sans write préservé, raccordement budget I/O corrigé sans affaiblissement. N supérieurs non couverts par collecte4096/SRAM ; pas de qualification R4global/R6, IRQ/RS485, WCET faute, dérive SI/thermique ou endurance.
- **RESTAURÉ / PRÉSERVÉ** : FlashM0162Mio exacte c5bc…f5b26 ; FRAM371a9d16…6634 entière exacte au modèle, journal38/root51/epoch5/run6/config3,12commits/629112octets uniquement LIVE. Historique/tail,6497fichiers protégés,60autres worktrees/stashes intacts. Restauration22:10:24UTC, capteur arrêté/GDBdétaché, aucun rollback/SD/HP. Prochaine étape : contrôle indépendant ChatGPT puis couverture produit/collecte N supérieurs/RS485/R5/R6 distincte ; ne pas réappliquer la retraite.

- **PUBLICATION R4-T RELUE** : bilan5b2b84fbfa7eb49a9ed439f56e04a28b39d88902,139fichiers/125preuves SHA256 relus depuis GitHub à2026-10-17T05:48:14.796611+00:00. Clôture DONE dans la portée physique bornée N4096 ; aucune qualification globale/R6 ni tag, contrôle indépendant ChatGPT à réaliser. Reprise de session limitée à documentation/publication, aucun essai physique répété.


## R4-U — 17/10/2026, composition préparée avant cible

- **PRÉPARÉ / TESTÉ HÔTE** : runtime opt-in propriétaire unique/service FIFO+UART sans réentrée R2, B6 UNAVAILABLE honnête ; B1/B2/B7 non supportés.143anciens tests PASS, nouveau144 PASS ciblé après correction oracle readonly ; sanitizer/cross-build PASS. Aucun firmware produit LIVE annoncé.
- **PRÉFLIGHT OBSERVÉ09:30:14UTC** : M016 Flash entièrec5bc…f5b26, FRAM371a…6634, journal38/root51/epoch5/run6 terminal, capteur arrêté ; aucune mutation. COM4 FTDI confirmé raccordéTR2 par utilisateur, COM3STLINK exclu. Plan [LIVE_R4U](../05_Firmware/LIVE_R4U.md) : RAM puis UARTreadonly puis vrais B5 seulement si budgets conformes, sans retraite ni rollbackFRAM. Résultats physiques en attente.

- **R4-U PHYSIQUE READONLY OBSERVÉ09:56UTC** : image04/code806d967 duplex opt-in,265441XYZ/10s et79réponsesCOM4CRCexactes ; work294/service1259/gap1263µs,0erreurFIFO/0write, FRAM371a inchangée. Variante split image03 avait arrêté sûrement sur tag décalé d'un bit après131050XYZ ; bruts conservés, cause non prouvée. B5durable pas encore engagé, suite conditionnelle autorisée par les gardes acquises ; aucun produit LIVE complet/R4global/R6.


## R4-U — résultat final physique du17/10/2026UTC

- **OBSERVÉ BORNÉ / code806d967** : runtime opt-in propriétaire FIFO unique, datation observée/R3-B/R2/R1 et vraiUART-RS485 COM4 ; START5/3/run7 etSTOP5/4terminaux4/0, retry sans effet/collision27.3045698XYZ/380713TS,58fenêtres N4096,1632réponsesCRCexactes dont731V/F et49HELD. Oracle4096 exact, UTCabsente. B0/3/4/5 exposés, B6UNAVAILABLE honnête, B1/2/7non supportés, capabilityLIVE16absente.
- **MESURÉ** : work810/service1669/gap1897µs, bornes inchangées1000/2500/5000 ; FIFOmax56/0erreur, UART0drop/0erreur. CallbackSTART805/STOP203µs ; STOPB5 terminal63,249s, ACK→premierHELD55,757s. Priorité de file validée sur modèle seulement, pas de STOPindustriel temps réel ni WCETIRQ isolé. Variante split arrêtée sur bitshift/parité, voie duplex existante réussie bornément ; cause électrique/IRQ non démontrée, aucune garde affaiblie.
- **RESTAURÉ / PRÉSERVÉ** : M016 Flash2Mio c5bc…f5b26 ; FRAMentière95dab790…9af41 exacte au modèle/journal46/root59/epoch5/run7/config3/effect27,8commits419408octets LIVEseulement ; historique/tail exacts. WHO7B, ODRoff/FIFObypass/TSoff après restauration, COM4fermé/GDBdétaché, aucunSD/HP/rollback.6626fichiers protégés/61autresworktrees/2stashes inchangés.
- **LOGICIEL / LIMITES** :143anciens tests PASS, nouveau144 PASS ciblé après correction oraclereadonly, sanitizers et ARMdédié/canonique PASS ; BINcanonique85b1…c0f5 inchangé. Nsupérieurs physiques nécessitent collecte adaptée, B3run/config internes non exposés dans nouveaux registres réservés. Prochaine étape : contrôle indépendantChatGPT puis contratR5/capabilities/corrélation et décision sur latenceSTOPdurable, sans GO R4global/R6/industriel. Preuves [R4-U](Echanges_Codex/preuves/TR2-20261017-R4U/physical-final-summary.json).

- **PUBLICATION R4-U RELUE** : bilan941e8de6fcd147c0ff88d8987dfb6d17237394d4,259fichiers/245preuves et178artefacts relus/vérifiés depuisGitHub à2026-10-17T10:05:44.626187+00:00,1632ADUparsées. Clôture de livraisonDONE portée bornée N4096, archivesR4U ajoutées ; pas de qualification indépendanteChatGPT ni GO R4global/R6, aucun nouvel essai après restauration.


## R4-V — 17/10/2026, diagnostic de latence préparé

- **PRÉPARÉ / TESTÉ LOGICIEL** :145tests+ARM/sanitizers PASS ; télémétrie causalitéMCU/service/I/O préparée, baseline0 puis candidatFIFO250µs, UART/deadlines/barrièresR2 inchangés. Contre-test erreur-service puis retraitB3invalide corrigé. Hypothèse de sur-service non encore prouvée matériellement.
- **PRÉFLIGHT OBSERVÉ10:18:02UTC** : M016 Flash2Mio c5bc…f5b26, FRAM95da…9af41 entière/epoch5/run7/journal46/root59 exacts, capteurarrêté. Aucun flash/Modbus/writeFRAM R4V encore ; nouvelles identités préparées5/5..9 uniquement après gardes. [PlanR4V](../05_Firmware/LIVE_R4V.md), aucune qualification industrielle.

- **R4-V BASELINE OBSERVÉE10:30UTC** : codef8bf7ca, START5/5run8/STOP5/6terminaux, FRAMfce8…a682 exacte au modèle/journal54/root67/epoch5/config3 ; probe0terminal/capteurarrêté/GDBdétaché, M016restaurationfinale àfaire. STOPeffet56,628s/terminal64,319s(DWTnominal),82,14% service/6,980sI/O parcommande ; work815/service1624/gap1852µs. Correctifminimal espacementaprèsdrain et retraitFRESHimmédiat aprèsSTOP enrevalidation, pasencorephysique. AucunrollbackFRAM.


## R4-V — résultat final du17/10/2026UTC

- **OBSERVÉ PHYSIQUE BORNÉ** : baselinef8bf7ca puis candidat8db2f67/pacing250µs aprèsfinFIFO, START5/7/run9/STOP5/9terminaux4/0/retry/collision27. STOP5/8budgetrefus5/7 sansI/O/commit/effet, étatjournal58/root71inchangé.1833115XYZ/229140TS,1075ADUCRCexactes/447V-F/51HELD, oracle4096exact. Work814/service2063/gap2274µs sous1000/2500/5000, FIFOmax68/0erreur/UART0drop0erreur.
- **DIAGNOSTIC PROUVÉ** : sur-serviceFIFO dominant82,14%référence→71,41%candidat, I/O6,980s/commande ; quatrebarrières inchangées. STOPcapture→effet56,628→33,028s/terminal64,319→40,859s, ACK→HELDmaître56,355→32,866s ; callback205µs/HELDpublication429µs après. TempsDWTnominal/maître distincts, pas deWCETindustriel ni seuilnouveau. R2/formats/anti-rejeu inchangés ; cacheFRESHaprèsSTOP et retraitinvalideaprèsfault corrigés,145tests/ARM/4sanitizersPASS déjàacquis/nonrejoués.
- **RESTAURÉ / PRÉSERVÉ** :10:52:16UTC M016Flash2Mio c5bc…f5b26 exacte/19pagesnormales, FRAMentière70e273…73587 exacte au modèle et aprèsrestauration, journal62/root75/epoch5/run9/config3/effect39phase3. Historique/tail exacts, WHO7B/ODRoff/FIFObypass/TSoff, GDBdétaché/COM4fermé.6873fichiersprotégés/62autresworktrees/2stashes intacts, aucunSD/HP/OB/rollback.
- **SUITE CONTRÔLE CHATGPT** : diagnostic/correctif minimal bornément démontrés ; latence résiduelle importante, décisionexplicite du besoinSTOPsécurité et optionsCPU/service/journal/arrêtindépendant àprévoir sansdéplacerbarrièreimplicitement. R5profil/capabilities/B1B2B7/SQLite/Web etR6nonqualifiés, aucunevalidationindustrielle. [BilanR4V](../05_Firmware/LIVE_R4V.md), preuvesetrapport àrelire surGitHub ; aucunessai rejoué aprèsinterruption.

- **PUBLICATION R4-V RELUE** : bilanff469924b3ff624f1c54b148e307b780e801b996,168fichiers comparés intégralement/156preuvesSHA/114artefactsBUNDLE,2730ADU parsées depuisGitHub à2026-10-17T10:56:14.671782+00:00. DONE diagnostic/correctif borné, latence résiduelle importante maintenue explicite ; restaurationFlash/FRAM vérifiée, aucune qualificationindustrielle/R4global/R5/R6 ni tag. ArchivesR4V ajoutées aprèsrelecture, contrôleindépendantChatGPT distinct.


## R5-A — 17 octobre 2026 UTC, logiciel PC livré / conformité firmware bloquée

- **VALIDÉ LOGICIEL** : code `1efc4f1500f8401a5996951f8798a8c977019ec2`, lecteur strict protocole1/2 et diagnosticpartiel explicite, B3 normatif48, observations/segments/qualité/source, migrationSQLite7 additive sans rétroattribution, APIv2LIVE/pageS7/replay horsligne.551tests.NET/0skip, puis201testsService après correction du chemin statique,7scénariosUI sousDOMsimulé PASS ; HTTP200 réel/52observationsSQLite dont1rejetéeV/F0. Pas de navigateur graphique ni de preuve physique nouvelle.
- **BLOCKED FACTUEL** : capturesR4-U/V CRCexactes +sourcefirmware `live_metrology.c:203–204` prouvent sévéritéécriteà3002/alarmeà3003, inversion du mapping3002alarmes/3003sévérité.2068B3/2217 ont sévéritéréservée240 et sont quarantainés ;149états sansmesure valide. Aucun échange silencieux côtéPC ni modification de norme. Les chiffres RMS/FIFO/latence antérieurs restent observés, mais ne qualifient pas l'intégralité B3. [NoteR5-A](../Supervision%20TR2/LIVE_R5A.md).
- **PRÉSERVÉ / SUITE** : aucunCOM4/flash/debug/reset/FRAM/SD.7855fichiersprotégés/63autresworktrees/2stashes intacts, archivesR4-U/V etdettesA04inchangées. Nouveaucontrat nécessaire pourcorriger/qualifier projecteurSTM32 avant repriseR5-A/R5-B ; R5/R6 nonqualifiés. JournalmatérielR4-V inchangé, nonrelu surcible.

- **PUBLICATION R5-A RELUE** : bilan `e8c2834cafd997e5882a21a78f22eabc6de15d83` récupéré de GitHub à2026-10-17T11:27:56.417490+00:00,60fichiers comparés intégralement/41preuvesSHA/40artefactsBUNDLE. TRX distants551PASS puis201ServicePASS ; diagnostic2068B3 nonconformes etfuméeHTTP/SQLite vérifiés. StatutBLOCKEDfirmware maintenu, pas deDONE/R6/qualificationindépendante ni nouvelaccèsmatériel. Code1efc4f1 reprenable, correctionprojecteur dansnouveaucontrat àautoriser.


## R5-AFIX — préparation du17/10/2026UTC

- **VALIDÉ LOGICIEL** : code29a6f4ecf83c97499055b58fc73d631763cf5813, correctif2destinationsB3/3002alarmes3003sévérité, neuforaclesindépendants rouge/vert.146testsfirmware/ARMcanoniqueetprobe,4sanitizersO2,202testsServicePASS ; V1BIN85b1…c0f5inchangé. AnciensADU R5-A restentrefusés, neufADUmodèlepositifsacceptés/SQLite.
- **PRÉFLIGHT OBSERVÉ11:36:20UTC** : FlashM0162Mio c5bc etFRAM70e R4-V entières conformes, capteurarrêté, COM4FTDIidentifié/exclusifàprendre ; aucunflash/Modbus/writeFRAMencore. Suitepréparée paireuniqueSTART5/10run10/STOP5/11 sousgardes/budgets, restaurationM016 ensuite ; pasdequalificationphysiqueanticipée.


## R5-AFIX — résultat du17/10/2026UTC

- **OBSERVÉ PHYSIQUE BORNÉ** : code29a6f4e, une paireSTART5/10run10/STOP5/11 terminaux4/0 ;1095ADUCRCexactes/563B3/462V-F/51HELD, offsetsnormatifs240/3enV-F et240/0enHELD.1784856XYZ/223107TS/FIFOmax70/0erreur, work809/service2116/gap2344µs. Oracle4096RMS954/953/10/27mg/peaks996/996/34/73 exact, pas calibration. Aucunretry/collision/RENEW physique supplémentaire.
- **VALIDÉ LOGICIEL** :146testsfirmware+ARMcanonique/probePASS,4sanitizersO2PASS,202Service.NETPASS puis2testsR5AFIX avecnouvellecapturePASS.563observationsSQLite sansrejet enreplay, anciens2068ADUinversés toujoursrefusés. HeadersB4/B6replay simulés/étiquetés, pas R5-B. Correctif2destinations etoraclesneufétats, aucuneautreinversionadjacentedémontrée ; norme/R2/PCproduction inchangés.
- **RESTAURÉ / SUITE** :11:41:55UTC M016Flash2Mio exacte c5bc…f5b26/19pagesnormales, FRAMfinale4e7fab…84bd entièreexacteaumodèleetaprèsrestauration, journal70/root83/epoch5/run10/config3/effect45phase3. WHO7B/ODRoff/FIFObypass/TSoff, GDBdétaché/COM4fermé ;8077fichiersprotégés/64autresworktrees/2stashes préservés. R5-A historiqueBLOCKED restearchivée ; défautB3levédansR5AFIX, revueChatGPTpuisR5-BmaîtrePC→SQLite/Web distincts. AucunGO R5/R6industriel. [Bilan](../05_Firmware/LIVE_R5AFIX.md).
