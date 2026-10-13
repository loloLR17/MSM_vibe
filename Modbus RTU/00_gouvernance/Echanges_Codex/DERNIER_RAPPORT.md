---
mission_id: TR2-20261013-M016-SDMMC-REFERENCE-PHYSICAL
status: IN_PROGRESS
base_ref: main
base_sha: 167b8cbcf1ba93209789ea6890799c77bbc7fc96
initial_head: 087dd9e3f1947a5b8b2906fba0ca862b968185bd
result_sha: d9a223e4de996e01ffc701eecc17c5038398e8c5
created_at_utc: 2026-10-13T20:42:05.770795+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Qualification du second banc SDMMC2

**Le 4 bits n'est pas qualifié.** Initialisation correcte en 4 bits, mais les deux campagnes bornées échouent pendant la première relecture. À4MHz réellement appliqués, le CRC échoue encore en4bits. À la même horloge, le diagnostic readonly1bit lit les4096octets sans erreur et correspond exactement au motif écrit avant réinitialisation. Intervention physique précise demandée pour DAT1–DAT3/contacts/connexions partagées ; retour utilisateur : « le câblage semble correct », sans correction ou mesure précisée ; aucune nouvelle écriture après ces deux campagnes. Statut provisoire IN_PROGRESS en attente de publication/relecture, Investigation en cours après retour utilisateur « le câblage semble correct » (contrôle rapporté, continuité non mesurée). Ni origine électrique exacte ni qualificationV1/endurance démontrée.

## Identités, autorisations et reprise

Utilisateur et contrat autorisent tout contenu de la microSD du **second banc** sacrifiable. Une seule sonde détectée : NUCLEO-U575ZI-Q, ST-LINK0032001F3235510C37333439/V3J15M7 ; GDB UID001B0046/38425018/20333157 confirmé avant chaque programmation. Principal003500463235510B37333439/UID004A003D explicitement non sélectionné. Câblage H3h-B confirmé par l'utilisateur, pas inspection indépendante. GPIO effectivement lus : PB14/PB15/PB3/PB4 AF12, PD6/PD7 AF11 ; pas une vérification de continuité externe.

Carte de test réellement identifiée : CID `03534453 44333247 858BF092 3C01927A`, différent du CID principal historique,62333952secteurs de512octets =31914983424octets. Région de test attribuée **LBA2048..2055**,4096octets, dans capacité réelle. Pas de format/repartition/erase nécessaire. Aucun accès FRAM ni capteur/Modbus dans le projet. SecondeFlash jetable selon décision permanente M015-M§4 : pas de sauvegarde2MiB ni restauration historique automatique ; identité/BIN/readback conservés, pas de masserase/OptionBytes/RDP.

Worktree isolé `MSM_vibe-M016-sdmmc-physical`, branchecodex/m016-sdmmc-physical basée sur087dd9e. Projet repris sans réécriture ; artéfacts précédents rehashés (BIN/ELF/MAP) exacts, provenance/licences exactes. Ancienne missionREFERENCE-BENCH archivée byte-identique à167b8cb. GelsH3h, detteA04, mission et gouvernance ChatGPT inchangés. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2/f3b829e6188f491920edf74a5f9ed0f559d08b87 et worktrees historiques préservés.

## Essais et résultats, séparés

|Observation|Profil réel|Résultat|
|---|---|---|
|Préflight readonly|4bits,24MHz, un node non utilisé|HALinitOK, stage2, CID/capacité conformes, R1=0900, CMD13error0, CLKCR14001, CFSR/HFSR0|
|Écriture/relecture essai1|Un node,4bits/24MHz, code0778351|Stage99/phase5/read,0round complet ; HAL/DMAerror1002=DATA_CRC_FAIL+RX_OVERRUN ; écriture991ms, contenu non validé dans cet essai|
|Écriture/relecture essai2|Deux nodes contigus,4bits/24MHz, coded9a223e|Stage99/phase5/read,0round complet ; HAL/DMAerror1002 ; écriture135ms, relecture invalide|
|Tentative de changement ClockDiv par HAL_ConfigWide|Demande4MHz mais CLKCRreste14001|HAL_ERROR ; **pas un essai réellement à4MHz**. Relecture diagnostique toujours24MHz échouée1002, conservée séparément|
|Diagnostic readonly4bits|LL SDMMC_Init appliqueCLKCR14006,4MHz|DMAread refuse, DATA_CRC_FAIL02 ;4096dumpés mais seulement50octets égaux, aucune intégrité validée|
|Diagnostic readonly1bit|ST CMD55/ACMD6arg0 puis LL SDMMC_Init, CLKCR10006,4MHz|CMD55/ACMD6returns0, ready1, DMAread1, HAL/DMA/CMD13errors0, R1=0900 ; **4096/4096octets exacts**, comparaison hôte complète|

Les deux triggers d'écriture sont uniques et identifiés ; programme prévu pour3rounds mais **aucun round complet n'a réussi**. Les diagnostics supplémentaires sont des lectures seulement, justifiées par les hypothèses ; aucun retry d'écriture automatique. Pauses GDB absentes pendant les deux campagnes. Les appels readonly sousGDB sont un diagnostic, pas des métriques de débit de campagne.

Réinitialisations : software reset après chaque flash (voie qualifiée et étatIRQ restauré) et après essai2 pour établir un état de lecture propre. Le motif de l'essai2, round0, reste exact après cette réinitialisation puis relecture1bit. Pas de coupure d'alimentation physique observée ni preuve powerloss/E4. Les quatre derniers octets CRC ne sont pas le seul oracle : comparaison des4096octets bruts effectuée ; SHA256 du RXexact `9ec45ab0c8505527af5931e27722ed5908748e5073169b4571c84e006eb90140`. Le succès1bit n'est pas substitué au critère4bits.

## Correction et hypothèses

Lecture du HAL réel confirme : HAL_SDEx_DMALinkedList_ReadBlocks/WriteBlocks initialise IDMALAR avec ULA et sizeof(node), donc adresse initiale head+12. Le premierport à unnode ne matérialisait pas ce nœud suivant. GDB confirme head200020EC, IDMALAR E000000C, nodeunique12octets. Correction minimale vers les **deux nodes contigus ST**, chacun2048octets, liste finie ; pas de driver tiers, HAL inchangé, horloge/câblage/bus inchangés. Le nouvel essai échoue pareil : ce changement est une correction de fidélité au modèle ST, **pas la cause racine prouvée**.

Référence et écartcallbacks précédemment établis conservés : STU575I-EV SDMMC1, CubeU5v1.9.0/d88042df ; notreport SDMMC2, HAL_RxCplt/TxCplt réellement appelés, pas les anciens noms de l'exemple. Provenance/licences dans [README](../../../experiments/sdmmc_st_reference/README.md) et SOURCES.json. Clock160MHz/HSI48/48MHzkernel, edgefalling, div1 ; aucune recherche exhaustive de clocks. Passage temporairediv6 enreadonly pour tester fréquence ; passage1bit enreadonly pour isoler DAT1–DAT3. STprimitives publiques utilisées, retours vérifiés, erreurs conservées dans leurs logs, pas de nettoyage manuel d'erreur pour faire réussir un test.

**Déduction limitée :** interfacecommande/DAT0, DMA de lecture et persistance fonctionnent dans le contrôle1bit4MHz ; la défaillance est associée au mode4bit dans ces essais. Cette comparaison oriente vers DAT1–DAT3 ou leurs conditions de réception, sans preuve de contact inversé, de rail ou de composant défectueux. La baisse à4MHz supprime l'indicationRX_OVR mais pas leCRC ; elle ne suffit pas. Aucune mesure oscilloscope disponible :24/4MHz **calculés depuis kernel réel/CLKCR**, pas fréquence électrique mesurée. Aucune performance4bit validée : tempswrite991/135ms sont observations de tentatives échouées, pas débit soutenu ni borne industrielle ; tempsread1bit non instrumenté, aucune vitesse inventée à partir de duréeGDB.

## Validations et erreurs d'outillage

Builds readonly/armed précédemment acquis non rejoués surcodeinchangé ; artefacts rehashés. Après correction2nodes : **un cross-buildARM isolé réussi**, GCC14.2.1/O2/g3, logs complets. Pas de suite127/128ni buildproduction répété, aucune source/interface partagée modifiée. Les trois flashesnormaux (readonly, essai1, essai2) ont été vérifiés CLI puis intégralement comparés au BIN parGDB ; commandes/BIN/codeSHA consignés.

Erreurs conservées sans réessai physique superflu : premièrecommandeGDBdump avec guillemetsUNC traitéscommefilename invalide →commande corrigée sans reflash ; diagnostic1bit préparé avec HAL_SD_DeInit absent (garbage-collected) →arrêtavant action, puis primitivesST CMD55/ACMD6/SDMMC_Init vérifiées dans SDK et guardsret. HelperGDB écrit stdout encontinu pour conserver les erreurs/timeout, aucuneexpiration dans cettecampagne. Diagnostic HAL_ConfigWide retourneERROR et n'applique pasdiv6 ; le registre fait foi, pas la demandeRAM.

BINs exécutés : readonly b1091e8908614fb99576134b8cc18a334449073c3c9851fcc13e460e38e752f9 (20108octets), essai1 c183e3296715af7aebee83c316532a237b7ab0535943087ef92177314f8011b9 (21328octets), essai2 092a1db046c5821e3401418f92a0a3c83f9b46ae7414e2af4ed75741fb2c166d (21372octets). [Artéfacts/empreintes](preuves/TR2-20261013-M016-SDMMC-PHYSICAL/artifacts.json), scripts et logs CLI/GDB adjacents, images/dumps .hex, analyses indépendantes et manifeste. Preuveslocales `/home/lolo/dev/msm/reports/TR2-M016-SDMMC-PHYSICAL-20261013T202934Z`.

## État final et actions restantes

Dernièreobservation secondeNUCLEO : firmware2nodes coded9a223e, stage2/trigger0 aprèsreset préalable, **configurationSD effective temporaire1bit4MHz (CLKCR10006)** àla fin du diagnostic, errors0/R1TRANSFER+READY ; debuggerdétaché. Variablesbench_clkcr/clock/bus initialescachées restent4bit24MHz : pour ce diagnostic, les registresréels et logs font foi. Un reset/powercycle rétablit les paramètres4bit24MHz du firmware mais **ne déclenche aucune écriture** (trigger/CIDRAM par défautnuls). Aucun nouveau flash/reset depuis la demande de manipulation physique ; user peut couper/rétablir le secondbanc. Dernièreétat avant intervention, pas télémétrie courante.

TR2principal totalementhorspérimètre : aucuneconnexion/flash/reset/Modbus/FRAM/SD surcettecible. Dernierétat historiqueprobeSTORAGE2/code38c0273 conservé, cumulatifA04-02 nonrestauré ; aucuneprétention de readback actuel surcarte principale. Volumes historiquesM016 préservés par absence d'accès, fichiers/proofs inchangés. La restauration et l'intégrationproduction nécessitent missiondistincte.

**Interventionphysique indispensable, AGENTS§9** : alimentationsecondbanc coupée, vérifier continuité/contacts DAT1→PB15/CN12-26, DAT2→PB3/CN12-31, DAT3→PB4/CN12-27, aucuneinversion/connexionpartagée ; vérifier3,3V/GNDbreakout. Remettre uniquementsecondbanc sous tension et décrirevérifications/corrections. Puis reprendre un contrôle4bit justifiépar changementphysique, sans refaireles succèsinchangés ; si nécessaire instrumentationélectrique. Aucunformat ne réparera une erreurCRC de liaison, doncaucunformat lancé.

ComparaisonTR2 : référence DMAinterne/HALlié/IRQ, HSI48/24MHz/falling/4bits ; portTR2principal précédentpollingHAL/PLL160MHz/div16/5MHz/rising/1bit et I/OpartiellesRMW. **Pas de correctionproduction recommandée comme qualifiée** tantquebanc4bit nonfonctionnel. Ne pas attribuer ses DATA_TIMEOUT antérieurs à l'écartnode du projet isolé. V1, admission/provisioning, coexistenceFIFO/Modbus, endurance et H3h-E4 restenthorspreuve.

JournalETAT_COURANT_TR2 actualisé : bancdisponible/identités, deuxéchecs4bit, lecture1bitexacte, configurationtemporairefinale et vérificationphysique demandée. Publicationnormalemain, pasdeforce/merge/rebase. Commitlogiciel d9a223e4de996e01ffc701eecc17c5038398e8c5 « M016 physical checkpoint: restore official two-node SDMMC DMA layout ». Rapport/proofs puisfetch/relecture intégrale ; statutfinal consignéaprèspublication et SHAfinal dansréponseCodex. Aucunequalificationindépendante ni tag.

Poursuite au checkpoint : contrôler multiplexage/trace des broches et comparer readonly4bit4MHz surfrontmontant (hypothèsephase, standardST supporté) au frontdescendant original. Pas de nouvelessaiwrite autoriséimplicitement par cecheckpoint ; carte de test reste sacrifiable dans contrat, prochainecampagne seulementaprès hypothèse étayée.
