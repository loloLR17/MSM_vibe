---
mission_id: TR2-20261014-P12H3H-SDMMC1-REFERENCE-POC
status: FAILED
base_ref: main
base_sha: a517e1cd8ce35d6e48bbe92e53f07c7289980ae5
initial_head: 3073d95b1fcf9d6e3e3ed5c35307a17364bdf3d5
result_repository: https://github.com/VILLO88/NUCLEOU575
result_sha: 1e3c9183bad023ebf7d234134823d9270a0e8b64
created_at_utc: 2026-10-14T10:26:06.069377+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# P12/H3h — SDMMC1 communautaire reproduit fidèlement, fonctionnement instable

**Résultat technique : FAILED pour un fonctionnement fiable du PoC.** Le firmware tiers inchangé compile et s'exécute. **4 bits effectifs prouvés**, USB natif reconnu, **deux jeux distincts de 128 KiB écrits puis intégralement relus sans différence**. Cependant des CRC et des passages « No Media » persistent ; la dernière observation montre de nouveau l'indisponibilité du média. Le succès borné ne masque pas ces échecs. Le statut final FAILED constate cette instabilité non résolue ; les succès bornés restent acquis et ne qualifient pas le stockage TR2.

Aucune source SDMMC, bus, fréquence, GPIO, driver ou middleware tiers n'a été corrigé. Aucun portage SDMMC2 ; mission P12/H3h distincte de M018 PC/SQLite. Il ne s'agit pas d'une qualification TR2 à426672 B/s ou sept jours.

## Provenance et compilation

Référence inspectée et clonée directement : [VILLO88/NUCLEOU575 au commit évalué](https://github.com/VILLO88/NUCLEOU575/tree/1e3c9183bad023ebf7d234134823d9270a0e8b64), checkout externe propre `/home/lolo/dev/msm/cache/VILLO88-NUCLEOU575-SDMMC1-PoC`. [Provenance et hashes sources](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/reference-provenance.json). HAL embarqué1.6.2 ; .ioc déclare CubeU5 package1.8.0 ; TinyUSB0.20.1 et FatFs du tiers conservés. SDK MSM1.9.0 non substitué. Buffer MSC source8192 octets, contrairement aux4096 annoncés dans README tiers ; utilisation de la source réelle.

Seule adaptation au build : [CMake Linux](../../../experiments/sdmmc1_community_reference/CMakeLists.txt), HEAD tiers et état propre obligatoires, sources/startup/linker d'origine, Cortex-M33/FPUfpv5-sp-d16/ABIhard/GCC14.2.1. Debug O0/g3 puis **Release Os/g3**, optimisation déjà définie dans .cproject tiers ; changement de profil de build, pas changement des paramètres SD. [Commandes reproductibles](../../../experiments/sdmmc1_community_reference/README.md), flags effectifs/logs et [artefacts Release](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/artifacts-release.json).

| Image | Taille BIN | SHA-256 |
|---|---:|---|
| Debug |60252|a2248ae4e35878703134b89c272d9c122e3aff1840e107744c762fe0b9256f94|
| Release laissée |31437|2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a|

Deux builds ARM réussis et flashs standards verify + readbacks intégraux concordants. Avertissements newlib/syscalls et fonction USB HAL inutilisée conservés. Aucun test hôte/firmware production inchangé rejoué : zone isolée, aucune interface de production modifiée. Les artefacts complets BIN/ELF/MAP restent locaux ; absence de licence globale trouvée et absence de nécessité de redistribuer l'image tierce : Git contient scripts/hashes/observations, sources publiques épinglées et notices composants dans checkout intact.

## Cible, câblage, insertion et largeur réelle

Seconde Nucleo seulement : **ST-LINK0032001F3235510C37333439 / UID001B0046/38425018/20333157**, vérifiés avant flash et dans chaque script. Ancienne image M017 readback/hash concordants ; retrait de SD indiqué au contrat, puis insertion32Go et alimentation ST-LINK confirmées utilisateur avant flash. Principal003500463235510B37333439 jamais sélectionné ; pas de FRAM, média512Go ou Modbus principal.

Carte **CID03534453/44333247/858BF092/3C01927A**, **62333952 secteurs512 =31914983424 octets** vérifiés avant toute write. Pas de formatage, mass erase, OB/RDP, nouvelle soudure ni modification de jumper. Câblage CN11/CN12 et pontsSB19/SB22 fermés **rapportés par l'utilisateur**, pas de mesure de continuité collectée ici.

[ST UM2861 Rev10 tables19/20](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) : D0/PC8→CN12-2 viaSB19 ; D1/PC9→CN12-1 viaSB22 ; D2/PC10→CN11-1 ; D3/PC11→CN11-2 ; CLK/PC12→CN11-3 ; CMD/PD2→CN11-4. Ponts ouverts déconnectent D0/D1 du morpho ; ponts fermés rendent les contacts disponibles mais ST signale le risque de stubs. L'hypothèse de ponts ouverts a été écartée après retour utilisateur ; aucune cause électrique précise n'est prouvée.

GDB observe **HAL_SD_Init=HAL_OK**, puis **HAL_SD_ConfigWideBusOperation(4B)=HAL_OK**. **CLKCR=0x4002** : WIDBUS4 ; **SSR DataBusWidth=2** côté carte :4 bits. `hsd1.Init.BusWide=0` conserve la valeur initiale1bit et ne décrit pas le registre actuel. Kernel **HSI48=48000000**, diviseur2 ⇒ **12MHz calculés**, CPU160MHz. Front montant/pull-ups/AF12 PC8..12 etPD2 conformes au code original ; fréquence déduite des registres/HAL, pas mesurée électriquement. [Observation Release](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/release-community-init-observation.log).

## Exécution, erreurs et intégrité

| Étape | Observation réelle |
|---|---|
| Debug O0 |init et4bits OK ; read8192 puis512 échouées CRC0x02 ; RAM de read ratée non utilisée comme donnée valide|
| Recovery après multi-read CRC |carte observée DATA5/R1B00 alors que handleHAL READY ; reset identique nécessaire|
| Release Os |read8192 àLBA2048 entièrement conforme au motif M017 antérieur, HAL0|
| Jeu A : callbacks originaux viaGDB |16writes×8192 =131072 àLBA506048..506303 réussies ;2reads16384 exactes, CRC auchunk2 ; aucune intégrité entière prétendue à ce stade|
| USB natif |CN15 branché par utilisateur ; câble/mêmeport validé avec GalaxyA16 détecté ; Nucleo0483:572B/serial123456 et disque32Go reconnus|
| Read jeu A après reset explicite |USB via handle du volume de test :131072 octets exacts, SHA43290e316d188d8736115ab1b92516e0b28d82f3a69edf8312be2c0ba9c3bf12|
| Première write USB FileStream |échec API hôte, pas un résultat de write SD qualifié|
| Lanceur Win32 aligné initial |garde refusé avant exécution : WindowsNoMedia/Size0 ; aucune write alignée lancée|
| Recovery explicite mêmeimage |identité/CID/4bits/capacité vérifiés, aucune modification SD|
| Jeu B : Win32 aligné sanscache |16writes×8192 =131072 àLBA506304..506559, flush réussi ; read intégral131072 exact, SHAc8a5f1e01ab61ebcace803eb63d5cf360035b17a8aaaae309369c3922a611df4|
| Dernier contrôle |SDerror2005/R1B00, USB configuré mais WindowsNoMedia revient : instabilité non résolue|

[Jeu A intégral](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/usb-integral-after-reset.json), [Jeu B résultats](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/native-aligned-recovered-result.json), [comparaison indépendante Python jeu B](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/native-aligned-recovered-verification.json). **262144 octets distincts écrits/vérifiés**, mêmes sources originales, aucun compteur forcé ; resets et tentatives explicitement distingués, aucun retry opérateur caché. Retries internes Windows/USB non instrumentés, donc pas de claim « zéro retry interne ».

Le jeu A a été écrit après CID/capacité gardés, réutilise le callback original sans modification du firmware. GDB restaure seulement le payload dans la RAM MSC ; aucune pause à l'intérieur du transfert, pauses entre chunks, **ce n'est pas un benchmark de débit**. Les données de read512 ratée (367 octets différents en RAM) ne prouvent pas une corruption durable sur carte, puisque le HAL avait refusé la lecture.

## USB, permissions et méthode hôte

CN15 USB-C natif distinct du ST-LINK est nécessaire au MSC. L'utilisateur a testé le même câble/port avec Galaxy ; Windows reconnaît USB composite/WPD puis Nucleo USB0483:572B. [Preuve câble sans numéro de série du téléphone](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/usb-phone-data-capability.json), [énumération effective](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/usb-enumeration-settled.json). Les premières probes négatives n'établissaient ni câble de charge ni panneUSB ; le filtre et le contrôle ont été complétés à partir de l'étatTinyUSB configuré. Aucun patch GPIOUSB n'a été appliqué.

Windows refuse PhysicalDrive2 avec droits courants. Le volume de test E:, partition16GiB àoffset1048576, est RAW/non exploitable comme filesystem ; aucun formatage. Son handle de partition autorise les accès ciblés sans élévation : origine relative504000 secteurs pour jeu A et504256 pour jeu B, traduits en LBAs carte506048 et506304. Garde obligatoire modèleSTM32/USB/serial123456/taille31914983424/partition correspondante, pour ne jamais toucher un autre disque.

Lanceur hôte Win32 propre : [NativeAligned.cs](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/NativeAligned.cs), tampon VirtualAlloc aligné4096, requêtes8192 multiples de512/4096, NO_BUFFERING+WRITE_THROUGH, FlushFileBuffers, fermeture puis nouvelle ouverture/relecture. [Méthode Microsoft](https://learn.microsoft.com/en-us/windows/win32/fileio/file-buffering). Adaptation de l'outillage Windows uniquement, pas de contournement SD. Refus et erreurs API sont conservés, pas d'ACL/politique persistante ni demande d'admin. Fichiers PowerShell refusés par politique locale lors du contrôle téléphone ; cmdlets inline de lecture seules utilisées ensuite sans changer cette politique.

## Performances limitées et diagnostic

Chronométrage hôte natif, **aucun debugger connecté pendant les transferts mesurés**, payload131072 à16chunks8KiB et flush. Jeu B : write+flush **3178,7446ms**, read **187,4372ms**, soit environ **41234B/s écrit** et **699285B/s lu** dans cette petite séquence. Jeu A readaprèsreset **194,6563ms**. Ce sont des durées applicatives hôte/USB/volume brut, incluant leurs coûts ; pas une mesure intrinsèque SD ni un débit soutenu. Pas de P95/P99 de longues campagnes déduit d'un seul agrégat.

Le readme tiers annonce environ570KB/s ; cela n'a pas été reproduit comme benchmark comparable ici. M017 utilisait SDMMC2/1bit10MHz/HAL du SDK CubeU5 v1.9.0/DMA/ring/producteur426672B/s ; ici SDMMC1/4bit12MHz/HAL tiers/polling/USB/buf8KiB sans producteur. **Aucune attribution causale du gain/perte aux seuls4bits**, aucun transfert de qualification M017 ou runtimeTR2.

**Faits :** CRCdata0x02 en Debug et Release, puis erreurs accumulées2005 (CMD_CRC_FAIL1+CMD_RSP_TIMEOUT4+ILLEGAL_CMD2000) avec média devenant indisponible. Le HAL original quitte une multi-read CRC sans CMD12 lorsque DATAEND manque ; la carte a été observée DATA5 et le handle logiciel READY. Le callback retourne erreur, puis TEST_UNIT_READY peut annoncer notready ; un reset récupère temporairement. Cela explique une séquence de récupération insuffisante, **pas la cause initiale des CRC**. `sd_ready=1` seul n'est pas une preuve de disponibilité.

**Hypothèses non départagées :** signal/routage/stubs, comportement support, contraintes du polling/IRQ et effets d'observation. Release améliore certains accès mais ne résout pas les CRC. Aucun changement de bus/fréquence/HAL/cleanup pour rendre la mesure artificiellement positive.

Incidents d'outillage conservés : constante RCC initiale erronée retournait0 ; corrigée par lecture du header à0x00200000, mesure48000000 réelle, aucun changement de clock. Expression GPIOA indisponible après dump Release8192 (read déjà réussie, oracle indépendant confirmé). Logs/octetOEM et erreur de décodage Python après snapshot GDB réussi conservés ; ces erreurs ne sont pas des échecs de transfert supplémentaires.

## Critères, état laissé et prochaine action

**Prouvés :** compilation/flash fidèle, origine images, seconde cible/CID,4bits carte+contrôleur, USB natif, deux jeux128KiB intègres après récupération. **Non prouvés :** fiabilité continue, absence générale de corruption/pertes, cadenceTR2, filesystem/provisioning, checkpoint/power-loss et sept jours. PoC fonctionne par épisodes mais finit à nouveau indisponible ; il ne peut pas être déclaré fiable/adopté.

Dernière observation réelle : image Release31437/sha2a2fe744…, `sd_ready=1`, HAL_STATE_READY mais ErrorCode2005/RESP1B00 (état DATA5 encodé), CLKCR4002, USBconnected/config1, CFSR/HFSR0, GDBdétaché. Windows conserve le périphérique USB mais disque2 **Size0/NoMedia**. [Snapshot final](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/final-original-release-state.log), [état Windows final](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/final-native-enumeration.json). Carte de test et deux câbles connectés selon dernière confirmation ; aucune restaurationGPIO/SDMMC2 ou firmware principal.

Prochaine mission/décision à ChatGPT : isoler le premier CRC sans modifier silencieusement le profil, puis autoriser si pertinent une correction séparée de récupération multi-read/MSC, avec comparaison au firmware strict original. Le risque de stubsCN12 signalé par ST peut être vérifié expérimentalement, mais **aucune nouvelle soudure n'est justifiée automatiquement** ni cause électrique affirmée ici. Ne pas formater pour masquer le problème ; la reconnaissance RAW est distincte du CRC. Pas de correction SD appliquée dans cette mission fidèle.

Journal ETAT_COURANT_TR2 actualisé, M017 archivée byte-identique depuisa517e1c. Dépôt original82169af et deux stashes6251b988…/f3b829e6… préservés ; autres worktrees, production, gels et dettesA04 intacts. Worktree de mission `MSM_vibe-SDMMC1-PoC`, branchelocale `codex/p12h3h-sdmmc1-reference`, ciblemain ; pas de merge/rebase/force/reset destructif. Permissions exercées : développement/outillage, builds, flash/debugseconde, commit/pushmain.

[Index preuves](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/README.md), manifestSHA256, bruts et BIN/ELF/MAP locaux `/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014` et dossiersbuild. Le `result_sha` est **le commit externe tiers réellement évalué**, distinct des commits MSM de lanceur/rapport. Publication normale sur `main` du commit `51da6b09306ad942cb365b8c1a5196297f76fcdb` (`P12 H3h SDMMC1 checkpoint: faithful community build and physical evidence with recurring media failure`) réussie. Référence récupérée de GitHub et **134 fichiers intégralement relus et comparés byte-identiques**, avec **127 entrées du manifest SHA-256 vérifiées** : [preuve de relecture distante](preuves/TR2-20261014-SDMMC1-REFERENCE-POC/remote-readback-51da6b09.json). Checkout tiers toujours propre/épinglé, dépôt original propre à82169af et deux stashes inchangés. Mise à jour finale du rapport FAILED et du journal seule, plus preuve/script de relecture ; aucun nouvel essai, build, flash ou reset. Worktree de mission propre après commit ; publication finale à vérifier de la même manière. Le SHA final distant est communiqué dans la réponse finale, sans auto-référence.
