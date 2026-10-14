---
mission_id: TR2-20261014-P12H3H-SDMMC1-READONLY-FIRSTFAULT
status: FAILED
base_ref: main
base_sha: 94fffb0177fbfb6b02731a31a6e2f2ad284bf50b
initial_head: be0864ae11447c544dfb6ee460c8e983aaca0e15
result_sha: 667154ee041e3b52c11fd54661cd58add57f94ee
created_at_utc: 2026-10-14T15:53:01.900926+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# SDMMC1 : A conforme en 1 bit ; premier CRC B isolé sur SSR en 4 bits

**Diagnostic borné exécuté : A PASS ; B FAILED dès ACMD13/SD Status (64 octets), avant CMD17/CMD18. Première erreur B capturée en RAM avant effacement : STA=0x2/DCRCFAIL.** Un seul départ et un seul trigger par profil, aucun retry. FAILED désigne la qualification physique 4 bits, pas un échec de collecte : l'objectif de localisation de la première erreur est acquis dans cet essai. Ni stockage V1, ni fiabilité TR2, ni qualification industrielle démontrés.

## Résultats réellement observés

| Contrôle | A — 1 bit | B — 4 bits demandé |
|---|---|---|
| Identité | UID/CID exacts ; 62333952 secteurs ×512 | Mêmes UID/CID/capacité exacts |
| Départ utilisateur | Coupure/insertion ; VDD=0,003 V mesuré | Coupure/remise ST-LINK confirmée ; nouvelle mesure VDD non rapportée |
| Init HAL / sélection largeur | HAL_OK/ErrorCode0, CLKCR2 | HAL_OK/ErrorCode0, CLKCR4002 |
| SSR explicite | HAL_OK, DataBusWidth0 conforme | HAL_ERROR/ErrorCode2 ; SSR non valide |
| CMD13 initial, après lectures | TRANSFER4/R1=900 | Non exécutés par le harness après échec SSR |
| CMD17 512 au LBA506304 | HAL_OK, 0 différence intégrale, SHA exact | Non exécuté |
| CMD18 8192 au LBA506304 | HAL_OK, 0 différence intégrale, SHA exact | Non exécuté |
| Première erreur | valid0 | valid1, CRC données sur SSR64 |
| Terminal | stage100/raison0/run_count1 | stage99/raison2/run_count1 |

[Décodage A et contrôles intégraux](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/RUN-A-decoded.json), [décodage B](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/RUN-B-decoded.json), [comparaison et durées](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/comparison-summary.json). Buffers RAM bruts et scripts/GDB/logs sont publiés séparément A/B. Les buffers B sont restés inutilisés ; leurs hashes et mismatch0 initialisés **ne sont pas une intégrité de données validée**. SSRwidth0 B provient de la structure initialisée à zéro après HAL_ERROR, **pas la preuve d'une carte restée en 1 bit**. La sélection HAL4bits réussit et CLKCR4002 confirme le contrôleur ; la largeur effective annoncée dans SSR ne peut pas être confirmée à cause du CRC.

A : SHA512 `a9f4d9b4604080bea7999abf2d2874653e71c43a7805bc1735068b1f004f3daf`, SHA8192 `2ac5183f9757f9b08e94e4cf2f4b1ee03a3a66171e8b366989e5b36b93ab563b` concordent avec les oracles. Comparaison complète embarquée et Python indépendante, 512+8192octets ; pas de nouvelle écriture de référence. Durées DWT/160MHz des appels HAL instrumentés : initA166,678119ms, read5120,965331ms, read819211,575350ms. CMD12 original observé après DATAEND, CMD13 postreadTRANSFER4. HALContext historique1/2 ne suffit pas à conclure carte DATA : statut carte lu séparément. Ce ne sont pas des benchmarks industriels.

## Première erreur B avant cleanup

Phase4 = lecture SSR par `HAL_SD_GetCardStatus` → `SD_SendSDStatus`. Hook à **overlay stm32u5xx_hal_sd.c:3401**, branche DCRCFAIL avant retour2, record figé :

- valid1, kind4 (retour d'erreur HAL instrumenté), error2, tick60654, cycles1114083489, IPSR0 ; HAL_ErrorCode encore0 au moment de la capture.
- **STA00000002**, RESPCMD13, RESP1 **00000920**, ARG0, CMD10D ; ACMD13 identifiée par le chemin CMD55/ACMD13 original, réponse APP_CMD bit5, pas CMD13 de readiness.
- DCTRL62, DLEN64, DCOUNT0, CLKCR4002, MASK0 ; HALStateREADY1/Context0. DCOUNT0 ne prouve pas intégrité des64octets.
- Aucun RXOVERR0x20, DTIMEOUT0x8 ou CCRCFAIL0x1 présent dans cet instantané. CRC données concret avant effacement, pas simple OR tardif des erreurs ni bitCRC attribué par défaut à un problème électrique de commande.

InitB166,541750ms ; sélectionlargeur0,097906ms ; phaseSSR0,102475ms. Retour HAL_ERROR1/ErrorCode2 puis terminal99, sans initialstatus, CMD17, CMD18 ou CMD13 ajoutés par le harness. [Extraits source précis et empreintes](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/SSR-firstfault-source-excerpts.json).

**Limite de l'arrêt :** le HAL original `HAL_SD_GetCardStatus` efface les flags, reporte ErrorCode2 puis envoie **CMD16/512** avant de retourner, même après erreur SSR. Cela explique STA0/RESPCMD16/ARG512 à la sortie de phase, alors que le premier record conserve STA2/RESPCMD13/ARG0. Ce CMD16 est une transaction interne originale, non ajoutée par Codex. Le probe ne détourne pas le HAL au premier hook ; il arrête les phases suivantes au retour de l'appel. « Aucune commande après la première erreur » au sens littéral n'est donc pas démontré : aucune commande de récupération/retry/CMD13 **ajoutée**, mais ce cleanup original subsiste, conformément à la conservation de la séquence HAL demandée. La première preuve n'a pas été perdue. Aucune nouvelle session active n'a tenté de vérifier la disponibilité de la carte après échec.

## Diagnostic et limites causales

**Fait :** le défaut est reproduit hors USB/TinyUSB/MSC/FatFs, dès un transfertSSR64, avant tout lecture multibloc. USB n'est donc pas nécessaire à ce défaut observé ; le recovery de CMD18 ne peut pas expliquer sa première apparition dans B. Ceci n'exclut pas d'autres effets USB/recovery dans les essais historiques.

**Déduction bornée :** sensibilité au profil4bits renforcée par A entièrement conforme/BCRC, avec mêmes sources compilées, broches et fréquence calculée12MHz. Les lignesD1–D3, le débitFIFO quadruplé, l'état électrique initial et le média ne sont pas isolés entre eux. A/B sont deux départs uniques, B sans mesureVDD0 dédiée, pas une statistique de fiabilité. DCRCFAIL sans RXOVR dans la capture ne qualifie ni fronts/overshoot/tensions transitoires ni une ligne particulière. Aucune cause électrique unique prouvée, aucun correctif spéculatif appliqué.

**Prochaines actions proposées à ChatGPT, non exécutées :** revue indépendante du premier record et du cleanupCMD16 ; mission distincte pour une vérification dynamique ciblée alimentation/CLK/CMD/D0–D3 ou un comparatif explicite à fréquence réduite, avec une seule variable et budget d'essais. Ne pas ajouter des pull-ups à l'aveugle (ils existent dans conception), changer OSPEEDR, câblage/straps, passerSD4bits en production, réparer recovery ou refaireA/B sans nouveau contrat. Une éventuelle évolution de la contrainte « aucune commande après erreur » doit définir le traitement du cleanup original avant prochain probe.

## Logiciel, validation et traçabilité

Probe isolé `experiments/sdmmc1_firstfault/`, source tierce inchangée et propre `VILLO88/NUCLEOU575@1e3c9183bad023ebf7d234134823d9270a0e8b64`, HAL1.6.2/TinyUSB0.20.1 originaux, aucune modification upstream. Générateur overlay main/HAL/LL : mêmes séquences originales, hooks frontières/erreurs avant effacement, pas hooks des bouclesFIFO. Boot WAIT sans accèsSD, UIDguard/trigger unique, pas de printf, breakpoint ou halt pendantpolling. Un GDBattach avanttrigger, detach ; attente10s sansdebug ; attach collecte terminale puisdetach. Horloges/GPIO observés à la frontièrepostinit : CPU160MHz, HSI48actif/kernel48MHz, ClockDiv2⇒SD12MHz calculés ; AF12/PULLUP/HIGH conformes, IRQUSBdésactivée, IPSR0. Pas de mesure à l'oscilloscope.

Codeprobe au commit initial `42e8dd236d883366cf37167f9f96293fe95166f4`, réappliqué identique `8d3db40267d4a1dc9093c9b0e3d9b31819768d69`. Outils B validés au commit `667154ee041e3b52c11fd54661cd58add57f94ee`, result_sha ; probe.c/générateur/trace/description de build inchangés, seule définitionFF_PROFILE1→4 entrebuilds. Overlays A/B identiques octet à octet. Générateur vérifie source épinglée/propre et restauration des instructions originales après retrait des seules lignes injectées ; ce n'est pas une identité binaire de HAL instrumenté et original.

| Artefact | A | B (construit seulement après A PASS) |
|---|---|---|
| BINoctets |21852|21860|
| SHA256BIN |`fb2843742056093610f979268c004bb9c66455cf540069ea5d8f55531a252fb1`|`fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829`|
| SHA256ELF |`1d88d7054a82ab75edb3bd7b6f6aa321e4095a3535dbba0107d1cd6cac285b01`|`c9973b5b4713e8215fc24ce1e38ffdd4b50cec466fa890306586903347e1b1f9`|

GCC ARM14.2.1/Os/g3, builds logs effectifs publiés. Écheccompilation A, tentative 1 conservé : hooks appliqués à des utilitairesDMA sans SDMMCx ; générateur limité à la sectionSD, attempt2réussi. Build B réussi première tentative. Profil implicite rejeté, py_compile outils réussi, ABIARM2260/200/84octets vérifiée hors cible, décodeur contrôleframevalide/phase_countinvalide. nm A/B : absence des symbolesTinyUSB/MSC/FatFs/write/erase listés dansartifacts. Aucun affaiblissement desgardes, aucun codeproduction/validationantérieure rejoué. Validation proportionnée au probe autonome : son cross-build et outils hors ligne, puis les deux exécutions physiques contractuelles, pas les testsglobauxTR2 inchangés. Compilation/flash et observationdebug sont distingués des comparaisons physiques acquises A ; BCRC est une observation physique d'échec.

## Sécurité, autorisations et état laissé

Main concurrente02f7c4 a suspendu lesopérations surCN11-18erronémentGND. RevueSTUM2861table19 : CN11-16=3V3/CN11-18=5V/CN11-20=GND ; utilisateur confirme hors tension bleuCN11-20/aubergineCN11-16, sans changement de câblage. Retours exacts [pinout](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/pinout-confirmation.json), [départA](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/physical-start-A.json), [départB](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/physical-start-B.json). Conditions factuelles de l'arrêt de sécurité satisfaites ; aucun changement du contrat ChatGPT par Codex, statusBLOCKED historique du fichier conservé et signalé. A VDD0,003V mesuréparutilisateur ; B seulementcycleconfirmé, nontransposition de mesureA. Aucune observationvisuelle/continuitéohmique parCodex prétendue.

Cible exclusive SN0032001F3235510C37333439/UID001B0046/38425018/20333157. PréflashA image originale31437octets sauvegardée et SHA2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a conforme (ancienne image enError_Handler, SDabsente). PréflashB UID/imageA exacts et terminalPASS confirmés. Deuxflashstandards0x08000000/verify, secteursnécessaires0–2, resetflash avantdépart électrique dechaqueprofil, readbackscomplets exacts. Aucunflash supplémentaire/reset pendantRUN, pasmasserase/OptionBytes/RDP/writeSD/MSCWindows. LeshooksnefontpasdewriteSD ; nos outilsGDB n'appellentpasd'APIcarte. Interventions physiques effectuées exclusivement par l’utilisateur.

**État final observé seconde Nucleo :** imageBconservée, stage99/trigger0/run_count1, HAL_READY/ErrorCode2/Context0, dernierRESPCMD16/R1=900 aprèscleanuporiginal. MicroSDtestinsérée/STLINKseul/CN15déconnecté selon confirmations ; debuggerdétaché, aucune transaction automatique après terminal. Cet état HAL ne qualifie pas la disponibilité de la carte ; pas de CMD13 ajouté. Ne pas restaurer automatiquement l'imageUSBoriginale : cela ajouterait du trafic SD/MSC non requis. Backup original, readbacks et images A/B conservés localement, empreintes publiées.

## Git, archives et publication

Dépôt originalhistorique propreHEAD82169af7e76797731358852bec83bfb0ceb54ccf et stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2/f3b829e6188f491920edf74a5f9ed0f559d08b87 préservés. Worktreeinitial42e8dd2 conservé, intégrationisoléeautoriséeexplicitementparutilisateur depuismainconcurrente02f7c4, sansmerge/rebase/reset/forcepush. Worktreeactif `MSM_vibe-SDMMC1-firstfault-integrated`. Principal / FRAM/média512Go/Modbus/gels/registre A04 nonmodifiés. ROOTCAUSE-REVIEW archivée byte-identique depuis94fffb0. Journalactualisé avec A PASS, premierCRCSSR B, limites et prochainesactions.

Checkpoints normauxmain publiés et relus :12ca349 (25fichiers/13empreintes),559ae68 (38/26),667154e (57/45),9926693 (69/57). [Dernièrepreuvecheckpoint](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/remote-readback-9926693.json). LeslogsbrutsCRLF etdiffstiers sontconservésidentiques octet à octet malgréwhitespace ; documents/JSON/codepropres contrôlés. Les permissions du contrat commit/push/flash/debug ont été exercées dans le périmètre ; aucune opération matérielle hors contrat.

Rapport et preuves B publiés normalement sur `main` au commit **`fc737cd02a5b3702c685c5922ddd34a40f7c301a`**, message `P12 H3h: capture first 4-bit CRC on SSR, preserve A PASS and stop without retries`. Référence récupérée de GitHub ; **86 fichiers relus intégralement, identiques octet à octet, 74 empreintes vérifiées**. [Preuve de relecture distante](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/remote-readback-fc737cd.json). UID/CID et exécutions A/B, archives et documents protégés, source tierce propre, dépôt historique et stashes préservés vérifiés. Worktree actif propre après publication.

Cette finalisation documentaire conserve le statut **FAILED** : qualification B non acquise, premier défaut isolé et protocole arrêté conformément au contrat, sans retry ou essai annexe. Son push normal sera également vérifié ; SHA distant final fourni dans la réponse sans auto-référence. Contrôle indépendant ChatGPT et cadrage d'une future mission restent distincts. Aucun besoin de manipulation physique supplémentaire pour clôturer ce protocole.
