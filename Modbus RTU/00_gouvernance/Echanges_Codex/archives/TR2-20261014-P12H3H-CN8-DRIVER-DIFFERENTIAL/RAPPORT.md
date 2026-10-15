---
mission_id: TR2-20261014-P12H3H-CN8-DRIVER-DIFFERENTIAL
status: DONE
base_ref: main
base_sha: b2f00a77250780a3ed62058cf4baca56325a78b3
initial_head: ea860be2223223ed59a3e3d748bdf1fad2115bb4
result_repository: https://github.com/VILLO88/NUCLEOU575
result_sha: 1e3c9183bad023ebf7d234134823d9270a0e8b64
created_at_utc: 2026-10-14T17:04:29.091643+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# CN8 faisable en SDMMC1 ; aucune correction différentielle SSR démontrée

**CN8 fournit tous les signaux nécessaires au bus SDMMC1 4 bits, 3V3 et GND. Il peut contourner les contacts/soudures des headers morpho, mais ne supprime pas les dérivations PCB quand SB19/SB22 restent fermés.** Aucun recâblage, changement de strap ou essai effectué.

**Le projet communautaire utilise le driver SD officiel ST 1.6.2 sans modification.** La HAL 1.6.3 du SDK historique ne diffère que par deux zones d'identification dans `hal_sd.c` ; LL, SD_ex et les trois headers SD sont identiques. Les chemins SSR, CRC, polling, DMA, flags et recovery comparés sont identiques. Aucune différence de driver démontrée ne justifie un patch minimal censé résoudre le premier CRC SSR. Une faiblesse commune du HAL reste possible ; cette comparaison ne qualifie aucun driver ni montage.

Recommandation prioritaire : **un seul futur essai du profil B existant par CN8**, même firmware et paramètres, sans rejouer A. Protocole proposé ci-dessous, **non exécuté** ; il exige une mission matérielle distincte et les prérequis physiques. L'enregistrement industriel des vibrations reste l'objectif, sans renoncement au stockage ni changement d'architecture.

Statut DONE pour l’étude documentaire publiée et relue ; aucune qualification matérielle nouvelle. Zéro flash/debug/reset/connexion cible/commande SD, accès lecteur PC ou écriture mémoire persistante.

## 1. Sources et acquis réutilisés

Main récupéré à `ea860be2223223ed59a3e3d748bdf1fad2115bb4` ; AGENTS, gouvernance et protocole inchangés depuis la mission précédente, versions vérifiées. Journal intégral relu. Rapports/preuves M016/M017, PoC, FIRSTFAULT et ROOTCAUSE examinés, [revue permutation archivée](archives/TR2-20261014-P12H3H-SDMMC-DAT-PERMUTATION-REVIEW/RAPPORT.md) réutilisée sans refaire ses calculs ni validations physiques. [Périmètre de la revue](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/review-scope.json).

Source communautaire propre : [VILLO88/NUCLEOU575@1e3c9183…](https://github.com/VILLO88/NUCLEOU575/tree/1e3c9183bad023ebf7d234134823d9270a0e8b64), même HEAD main confirmé via API GitHub. HAL déclaré1.6.2 dans `Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c`, projet .ioc CubeU5 1.8.0 ; release Os/g3 déjà évaluée dans PoC. Aucune licence globale du projet trouvée à la racine épinglée ; licences de composants présentes, HAL BSD-3-Clause. Aucune image/source complète tierce redistribuée ici.

Référence officielle réellement utilisée : [STM32CubeU5 v1.9.0 / STM32U575I-EV SD_ReadWrite_DMALinkedList](https://github.com/STMicroelectronics/STM32CubeU5/tree/d88042df24f16799957c24f00c2b234c9e306188/Projects/STM32U575I-EV/Examples/SD/SD_ReadWrite_DMALinkedList), SDK `d88042df24f16799957c24f00c2b234c9e306188`, sous-moduleHAL **1.6.3** `2552682ee5a0d6f826a6435750fc9267be65868b`. SDK1.9.0 n'est pas le numéro de versionHAL. Exemples SLA0044, HAL BSD-3-Clause ; [provenance et empreintes](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/provenance.json). Port historique Nucleo dans `experiments/sdmmc_st_reference`, distinct du main officiel EV.

Téléchargement des deux fichiers SD/LL depuis le tag officiel ST1.6.2 : **octet à octet identiques au projet communautaire**. Tag annoté `36cc28d890b600abc27f211d743c308fc5509ddf`, commit `2c5e2568fbdb1900a13ca3b2901fdd302cac3444`. [Preuve de correspondance](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/official-v162-source-match.json), [tag résolu](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/official-v162-tag.json). Le terme « communautaire » désigne surtout la configuration et l'application, pas une réécriture du driver SD.

## 2. Axe A — table CN8 vérifiée

[UM2861 Rev10, table16 p34, table20 p39](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) et [MB1549-C05, feuille des connecteurs](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf) consultés en texte PDF officiel. Révision physique, qualité des soudures et orientation réelle non inspectées. [Table documentaire et limites](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/CN8-documentary-map.json).

| Fonction breakout | Contact CN8 | MCU / AF | Contact morpho correspondant |
|---|---:|---|---|
|D0 / DAT0|2|PC8 /AF12 SDMMC1|CN12-2 viaSB19|
|D1 / DAT1|4|PC9 /AF12 SDMMC1|CN12-1 viaSB22|
|DAT2|6|PC10 /AF12 SDMMC1|CN11-1|
|D3 / DAT3|8|PC11 /AF12 SDMMC1|CN11-2|
|CLK|10|PC12 /AF12 SDMMC1|CN11-3|
|CMD|12|PD2 /AF12 SDMMC1|CN11-4|
|3V3|7|Rail3V3, pas une AF|CN11-16|
|GND|11, ou13|Masse commune|CN11-20|

CN8 est un socket2×8. **Pin1=NC ;3=IOREF ;5=NRST ;9=5V ;15=VIN** : ne pas les confondre avec alimentation3V3 ou masse. Pin14/PF3 et16/PF5 ne sont pas des DAT. IOREF dépend des straps, ce n'est pas le choix sûr à substituer à3V3. La microSD/Adafruit4682 utilise3,3V, pas5V. [Adafruit pinout](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts), netlist épinglée déjà revue : D0/D1/DAT2/D3 distincts deCMD ; aucun nouvel examen physique du breakout prétendu.

**Instance : SDMMC1 uniquement.** Le montage SDMMC2 historique utilise PD6/PD7 AF11 et PB14/PB15/PB3/PB4 AF12, pas les brochesSD présentes surCN8. Aucun portage d'instance requis pour le probeB SDMMC1 existant ; impossible de conserver le firmware SDMMC2 et simplement déplacer ses fils sur ces contacts CN8.

**Straps :** CN8 porte directement PC8/PC9 ; SB19 etSB22 raccordent leurs branches morpho. OFF déconnecte ces branches pour éviter les stubs ; ON maintient la dérivation et peut affecter la qualité. Leur fermeture est rapportée historiquement par l'utilisateur. CN8 reste utilisable avec cet état, **sans changement de strap nécessaire à sa faisabilité**. Déplacer les fils ne retire pas les pistes/headers morpho encore connectés. Ouvrir les ponts pourrait changer cette topologie pour D0/D1, mais serait un second facteur et une intervention physique distincte, **ni demandée ni recommandée automatiquement dans l'essai unique**. D2/D3/CLK/CMD restent aussi sur les réseaux partagés de leurs contacts morpho.

**Ce qui est contourné :** interface d'insertion et chemin série des broches/soudures de header CN11/CN12, éventuellement les embouts qui y étaient raccordés. **Ce qui subsiste :** mêmes MCU/AF, réseauxPCB, dérivations, rail, breakout/socket/carte et tout court-circuit ou charge parasite restant connecté au réseau. Une soudure ouverte en série peut être contournée ; un pont ou défaut qui charge le net n'est pas automatiquement isolé.

Avant une future opération, vérifier le repérage du socket, la révision et les tensions/masse, les embouts adaptés au socket femelle, l'absence de shield ou autre connexion sur ces pins, et le MCU en niveau compatible3,3V. Pas de jumper ajouté. CN8 ne constitue ni isolation galvanique, ni nouvelle instance de contrôleur. Éviter la boucle de retour créée par des masses redondantes ; une masse commune correctement identifiée suffit. La configuration réelle actuelle n'est pas certifiée par le tableau documentaire.

## 3. Axe B — comparaison du driver, pas seulement des API

[Diff intégral SD1.6.2→1.6.3](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/hal-sd-1.6.2-to-1.6.3.diff), [six fichiers et hashes](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/driver-file-comparison.json), [27 fonctions, lignes et empreintes](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/driver-functions.json) :

- `ll_sdmmc.c`, `hal_sd_ex.c`, `hal_sd.h`, `ll_sdmmc.h`, `hal_sd_ex.h` **identiques en bytes** entre les deux distributions.
- `hal_sd.c` : seulement deux hunks. `HAL_SD_InitCard` gère ClockDiv0 aux très basses fréquences et ajoute1 à la division initiale ; `SD_PowerON` teste `CMD_RSP_TIMEOUT` au lieu de `TIMEOUT` pour le repliCMD8 d'une carteV1.
- 25 des27 fonctions extraites sont identiques après normalisationLF ; les deux exceptions sont précisément celles des deux hunks. Rien n'est ignoré dans le fichierSD comparé. Cette analyse n'est pas un audit complet de tous les driversRCC/PWR du SDK.

**Diff1 — horloge d'identification :** à kernelHSI48,1.6.2 div60 donne400kHz ;1.6.3 div61 donne≈393,443kHz. L'ancienne valeur respecte déjà≤400kHz dans notre configuration48MHz. La correction couvre aussi les noyaux très lents selon les notes ST. Elle ne change pas leCLKCR4002/12MHz de la phaseSSR fautive. À noyau160MHz du banc1bit historique, div200→201,400→≈398,01kHz à l'identification, pas le débit nominal10MHz du RUN. Un effet indirect de séquence initiale n'est pas une impossibilité logique, mais n'est pas étayé par les observations initPASS/identité correcte ; pas un correctif démontré duCRC4bits.

**Diff2 — CMD8V1 :** code0x4 (`CMD_RSP_TIMEOUT`) contre 0x80000000 (`TIMEOUT`, timeout logiciel). Corrige une branche de compatibilité des cartesV1, décrite parST. La carte de test est identifiéeSDHC/V2, CMD8 accepté et initialisations conformes ; cette branche ne porte pas le premierDCRCFAIL SSR. [NotesST1.6.3 épinglées](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/blob/2552682ee5a0d6f826a6435750fc9267be65868b/Release_Notes.html). Rien n'autorise à attribuer à toute mise à jourHAL un bénéficeCRC.

### Chemin de commandes et flags partagé

[HALST1.6.3 source](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/blob/2552682ee5a0d6f826a6435750fc9267be65868b/Src/stm32u5xx_hal_sd.c), lignes correspondantes dansdriver-functions.json :

1. `HAL_SD_Init` appelle MSP puisInitCard, qui force identification1bit/rising/horloge≤400kHz, powerON et délai de clocks ; `SD_PowerON` envoie CMD0, CMD8 et boucleCMD55/ACMD41. Les constructions LL et traitements R1/R3 sont identiques, hormis le testV1 cité. Pas d'intention communautaire de remplacer ces transactions.
2. CID/CSD/RCA et sélection carte ; lectureSSR en1bit dansInit, puis `HAL_SD_ConfigWideBusOperation` selonInit.BusWide. `SD_FindSCR`, CMD55/ACMD6, reconfigurationCLKCR et les contrôles de fréquence sont identiques. PoC/FF demandent explicitement4bits aprèsInit1bit ; le bancST demande4bits dansInit, mais le driver identifie également en1bit avant le changement. Pas d'identification initiale à12/24MHz.
3. **SSR :** `HAL_SD_GetCardStatus` (communautaire2261, officiel2268) appelle `SD_SendSDStatus` (3272/3279) : CMD16/64, CMD55, DPSM64octets verscontrôleur, ACMD13, boucleFIFO par8mots, contrôleDTIMEOUT/DCRCFAIL/RXOVERR, vidange pendantDPSMACT, clear desflags. Corps identiques, pas voieDMA spéciale pourSSR, mêmeCRC matériel même quand les blocs applicatifs emploientDMA.
4. Première erreur FIRSTFAULTB capturée à l'overlay3401 surDCRCFAIL : STA2/RESPCMD13/DLEN64/CLKCR4002/IPSR0, avant leclear ; niCMD17/CMD18 niUSB/MSC avant le défaut. `GetCardStatus` d'origine effaceflags, accumuleErrorCode, revientREADY puis restaureCMD16/512 avant retour, **dans les deux versions**. Ce CMD16 ne provoque pas rétroactivement le premierCRC ; la limite d'arrêt originale reste documentée.
5. Lectures/écritures polling etDMA, LLdata/CRC/ICR, IRQ etSD_ex identiques dans les versions comparées. Les conditions d'arrêt et timeouts sont celles du HAL, pas unretry communautaire qui masqueraitSSR. LL peut classer un mauvaisRESPCMD commeCMD_CRC_FAIL ; ErrorCode est unOR, pas une chronologie. `COM_CRC_FAILED=0x1000`, `DCRCFAIL=0x2`, `RXOVR=0x20` restent distincts ; le décodage historique1002 corrigé est préservé.
6. Recoverymultibloc : CMD12 conditionné parDATAEND dans certains chemins d'erreur polling, READY logiciel ne signifie pas TRANSFER carte. Fragilité commune pouvant expliquer une propagationNoMedia après erreur ; **pas une différence corrigée par passer1.6.2→1.6.3**, et pas la cause nécessaire du premierSSR avantmultibloc.

## 4. Matrice causale des applications et configurations

| Écart / fait de code | Motif établi ou supposé | Mécanisme pour premierCRC SSR | Cohérence historique / décision |
|---|---|---|---|
|HAL1.6.2 vs1.6.3, identification|CorrectionsST de noyau lent et carteV1, établies|Influence initiale indirecte non démontrée ; pas de différenceSSR|Init/CIDPASS dans les deux bancs ; ne justifie pas unpatchSSR ou migration globale.|
|SDMMC1 vsSDMMC2, pins/routage|Adaptation obligatoire à deux montages distincts|Conditions électriques/contacts/charges différentes, plausibles|Les deux ont desCRC4bits et succès1bit. Aucun banc n'est une référence4bits fiable, donc pas de causalité aux seuls drivers.|
|PULLUP internePoC surCMD/D0–3 vsNOPULLST|Ajout explicitement voulu au commit9127ca7|Différence de bias/charges possible ; effet dynamique non mesuré|Pullups47k externes dans conceptionAdafruit, PoC réussit puiséchoue ; absence générale de pullups déjà contredite, pas modification aveugle.|
|GPIOHIGH, AF12/AF11|Même choixHIGH ; AF appropriées àinstance|Pas de différence de vitesseGPIO démontrée sur cheminSDMMC1|FIRSTFAULT registre conforme ; VERY_HIGH non proposé commecorrectif.|
|12MHz/risingPoC vs24MHz/fallingST ; M0171bit10MHz/PLL160|Réglages application différents ; auteur rapporte24MHz instable|Marge timing/signaux plausible, pas preuve de cause|CRC4bitsST persiste à4MHz surles deux fronts ;CRCFF à12MHz. Pas simplement « passer à12MHz suffit ».|
|CPU160MHz, kernelHSI48 surPoC etbanc4bit ; PLL160surM017|Horloges configurées selon profils|Jitter/rail non mesurés, paramètresPLLCPU différents ; pas débitKernel implicite|Registres historiques concordants48MHz dans4bits, différencePLL1bit ne prouve pascauseCRC4bits.|
|SMPS/voltageScale, rail externe|Initialisation PWR de cartes/exemples distincts|Alimentation réelle plausible mais non observée dynamiquement|Les appelsPWR configurentMCU, pas une preuve de décharge/tension au socket. Pas de changement de rail.|
|AppST init4bits ; PoC init1bit puisACMD6|Séquencevolontaire communautaire, HAL commun dessous|Transition avantSSR4bit pertinente, mais codeACMD6identique|SuccèsSSR4bitPoC antérieur etFFACMD6PASS. « Init1bit » n'est pasune correction unique duCRC.|
|PollingPoC/TIM17 vsDMAST/SysTick|Choix applicatif ; auteur déclareDMApluslent|SSR restepollingdans les deux ; différencesIRQ/CPU possibles|FFhorsUSB enmain/IPSR0 échoueSSR sansRXOVR danscapture. MigrerSSRversDMA n'est pas une correction justifiée. M017polling/DMA lents sansCRC en1bit.|
|USB TinyUSB/FatFs /bench isolé|PoC exposeMSC, auteur optimisebuffers4096→8192|Préemption/propagationNoMedia possibles après défaut, pas nécessaires auSSRFF|CRC reproduit hors USB ; FatFs nonactif dansmainPoC courant, pasdoublemontage démontré.|
|ExempleEV : high-speed, erase,24MB/circulaire ; portNucleo borné|Fonctiondémonstration et adaptations traçables|CheminsDMA/blocs changés ; paspatchSSR|ExempleEV n'a pas étéphysiquement qualifié ici ; ne pas prendre une différence deprogramme complet pourdriver corrigé. Aucun erase/high-speed recommandé.|
|Recovery/CMD12/CMD16 et «fallback1bit» PoC|Faiblesseslecture/cleanup communes ; fallback commenté nonvérifié|Peuvent aggraver aprèserreur, paspremierCRCcapturé|Ne pascorrigercleanup ouaffaiblir les gardes danscette revue.|

La matrice distingue lecture du code, déclarations d'auteur, observations historiques et hypothèses. Aucun mécanisme logiciel différentiel suffisamment démontré pour une correction minimale du premierSSR ; **cela n'exclut pas un défaut partagé du HAL ou du contrôleur**. Les conclusions de permutation et les contrôlescroisés déjà acquis sont référencés, pas recalculés.

## 5. Historique upstream et intentions vérifiables

[Historique compact/API](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/upstream-review.json). Le dépôt communautaire a six commits dans l'historique local complet ; mainAPI reste1e3c9183. Les fichiersHAL n'apparaissent que dans lecommit initial, aucune modification communautaire de leurs routinesCRC/SSR/recovery ensuite.

- [9127ca7](https://github.com/VILLO88/NUCLEOU575/commit/9127ca700e723ede6b0065ec065ba628a6f27689) : ajoutinit1→4bits, pullups internes, FatFs ; création de fichier déclarée réussie par l'auteur, pas preuve industrielle indépendante.
- [f5be9fe](https://github.com/VILLO88/NUCLEOU575/commit/f5be9feaacfa5600274056cc5834cd4291aec5cd) : div4→2 (6→12MHz), bufferMSC512→4096, objectif débit ; environ570KB/s déclaré, pas mesuresbrutes reproduites ici.
- [5d34516](https://github.com/VILLO88/NUCLEOU575/commit/5d3451617566bcca621e8ef1f60fd50e2106c217) : buffer8192, l'auteur déclare avoir conservépolling aprèsDMApluslent et24MHzinstable ; aucun patchHAL associé. Cela soutient une histoire de contraintes de vitesse/montage, sans prouver notrecauseCRC ou la comparabilité de sonbanc.

APIissues`state=all`,100max : **0entrée** à la date de consultation. Aucun commentaire GitHub sur les trois commits SD/MSC contrôlés (APIcount0 chacun), ni thread de bugSSR trouvé ; absence de thread n'est pas absence de problème. Huitcommits touchant`hal_sd.c` retournés par l'APIST, dernier2552682release1.6.3 ; la noteST correspond aux deuxdifférences, pas à uncorrectif CRC4bits. Les remarques/commentaires du code sur les pullups et le fallback ne remplacent pas des observations. Aucun accès privilégié ou source manquante indispensable : l'étude est complète avec ses limites.

## 6. Un seul futur essai discriminant proposé, non exécuté

**Facteur étudié : connexion du même banc SDMMC1 parCN8 au lieu descontacts morpho.** Garder SB19/SB22 fermés, mêmes AF/12MHz/rising/PULLUP/HIGH/HAL/horloges, même carte 32 Go et même firmwareprobeB existant, SHA256 `fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829`. Pas de mise à jour HAL, nouveau DMA, variation de fréquence ourepriseA en parallèle.

Prérequis d'une **nouvelle mission matérielle** : permissions explicitesflash/debug si utiles, identificationsecondeSN0032001F3235510C37333439/UID001B0046/38425018/20333157, imagepréservée/readback vérifié, carteCID03534453/44333247/858BF092/3C01927A. Utilisateurseul met hors tension, retire touslesliensmorpho vers lebreakout etconnecte les 8 liaisons du tableauCN8 ; aucun branchement double. Confirmerorientation/masse/3,3V, absencecourt/shield, carte hors tensionsi insertion, déchargeVDD si possible, puisSTLINKseul/CN15off. Aucun de cesgestes n'est demandé maintenant.

Les nouveaux embouts ou changement de retour GND nécessaires font partie du facteur « chemin d'interconnexion ». Consignerlongueur/géométrie/câbles pour ne pas attribuer un résultat au seul metalduconnecteur. Stubs encoreprésents avecstrapsON ; un essai identique élimine leur retrait commeexplication.

Un départélectrique, untriggerB, détachement avantpolling puis collecte terminale ; réutiliser le protocole FIRSTFAULT : Init/CID/ACMD6, SSR64, puisCMD17/512 etCMD18/8192 au506304 uniquement siSSRconforme, comparaisonsentières auxoracles. Aucunwrite/MSC, niCMD13/recovery/retry ajouté aprèséchec ; leCMD16interneoriginal est explicitementtoléré/consigné commeavant. **Au premierCRC/timeout/identité douteuse : arrêt total, aucune répétition.** Les builds etAinchangés ne sont pas rejoués. Aucunflash deconfort si imageBidentique ; le powercycle remet leprobe auWAIT.

| Résultat de l'unique essai | Interprétation attendue |
|---|---|
|SSR etlectures exacts, aucune premièreerreur|Indice favorable au chemininterconnexion/contact/retour GND ; pas preuve d'unesoudureparticulière, d'une fiabilité continue ou éliminationdéfinitive d'une panneintermittente.|
|DCRCFAIL SSR identique|Contournementdescontacts seul non suffisant ; stubs/rail/breakout/carte/défautpartagé restent possibles. Pas preuve que « drivercommunautaire » serait fautif.|
|Autreerreur/échecprérequis|Nouvelleconfiguration noncomparable, conserver les preuves et arrêter ; pas d'essaiannexe.|

Cette proposition est plus discriminante qu'une migrationHAL qui conserve justement le corpsSSR ; elle ne garantit pas le succès et ne remplacepas unequalificationTR2. Aucune campagned'endurance, formatage, effacement ou choixabandonstockage proposé.

## 7. Conservation, validation et publication

Revue permutation et rapport DONE archivés **intacts depuis b2f00a7** sous `archives/TR2-20261014-P12H3H-SDMMC-DAT-PERMUTATION-REVIEW/`. ArchivesFIRSTFAULT etstatuts administratifs antérieurs non modifiés. Contrat courant, gouvernance, journal, gels, dettes, code et schémas inchangés. Journal intégral relu ; constats transmis dans le rapport, publication limitée auxdocuments demandés, sans nouvel état matériel.

État du banc uniquement hérité, non relu : seconde carte, probe B au terminal99/trigger0/run1/error2, débogueur détaché ; SD insérée/STLINKseul/CN15off selon dernier utilisateur. Aucunaccès àprincipal/FRAM/microSD512Go/Modbus/OptionBytes. Aucune manipulationphysique requise pour clôturer cette étude.

Dépôt original propre82169af7e76797731358852bec83bfb0ceb54ccf préservé ; worktree isolé `MSM_vibe-CN8-driver-differential`, branche `codex/p12h3h-cn8-driver-differential`, baseea860be. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2/f3b829e6188f491920edf74a5f9ed0f559d08b87 et autresworktrees conservés. Aucun merge/rebase/reset/clean/forcepush.

Validation proportionnée : lecture/diff complet des six fichiers SD, hashes de 27 fonctions et correspondance avecST1.6.2 distant, pinoutST ethistoriqueupstream, inspections des liens/documents/diff. Pas de compilation, test firmware ni recalcul des validations historiques. Comparaison desfonctions = preuve statique, pas testphysique. `result_sha` désigne le code tiers examiné, pas unnouveau code flashé ; sources officielles séparément épinglées dans provenance.json. Permissions commit/push uniquement exercées, flash/debugfalse respectés.

Publication normale sur `main` du commit **`576a859cbc2dffc23db5fcf3f01d02973ae61cd7`**, message `P12 H3h: document CN8 routing and prove shared ST SDMMC driver paths`, réussie. Référence récupérée depuis GitHub ; **14 fichiers intégralement relus et identiques octet à octet, 10 empreintes de preuves vérifiées**. Archive permutation exacte, documents protégés et journal inchangés, sources comparées et hashes conservés, dépôt historique et stashes préservés. [Preuve de relecture distante](preuves/TR2-20261014-CN8-DRIVER-DIFFERENTIAL/remote-readback-576a859.json). Worktree propre au contrôle.

Cette finalisation DONE reste documentaire. Son push normal et sa relecture seront également vérifiés ; SHA final annoncé dans la réponse sans auto-référence. Contrôle indépendant ChatGPT et autorisation d’un futur essai distincts. Aucune correction fonctionnelle, intervention matérielle ou décision d’abandon du stockage.
