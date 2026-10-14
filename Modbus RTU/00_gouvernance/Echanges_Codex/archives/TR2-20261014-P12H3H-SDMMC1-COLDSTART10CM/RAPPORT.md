---
mission_id: TR2-20261014-P12H3H-SDMMC1-COLDSTART10CM
status: FAILED
base_ref: main
base_sha: d4f451c1ced5c39f2df9abf690a71be4bb8ce53e
initial_head: ac39881d54792156fa78524351960507a2fd1991
result_repository: https://github.com/VILLO88/NUCLEOU575
result_sha: 1e3c9183bad023ebf7d234134823d9270a0e8b64
created_at_utc: 2026-10-14T11:06:19.164737+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# SDMMC1 — contrôle après coupure électrique complète, fils 10 cm

**Résultat technique : FAILED.** Après le power-cycle rapporté par l'utilisateur dans le contrat, **No Media est déjà présent avant tout GDB/reset**. Un seul diagnostic minimal confirme l'image inchangée et des erreurs SDMMC. **Zéro reset Codex, zéro flash, zéro écriture, zéro cycle de données**. Arrêt selon §2 de la séquence contractuelle ; aucun retry ni récupération supplémentaire. Statut final FAILED : média indisponible après le redémarrage électrique rapporté, comparatif non réalisable dans ce périmètre.

## Tranche 1 — état initial sans perturbation GDB

Mission récupérée sur main `ac39881d54792156fa78524351960507a2fd1991` ; gouvernance inchangée depuis `d4f451c1ced5c39f2df9abf690a71be4bb8ce53e`, journal et dernier rapport relus. Coupure complète ST-LINK + USB natif puis reconnexion, microSD test32Go insérée, fils10cm et SB19/SB22 inchangés : **confirmation utilisateur publiée par ChatGPT dans le contrat**, pas une mesure électrique par Codex. Aucune manipulation physique supplémentaire demandée.

**Windows à11:04:23,309UTC**, avant toute connexion GDB ou reset : USB **0483:572B**, serial123456, présent/OK. Disque2 `STM32 SD Card`, BusUSB, **Size0 / OperationalStatus No Media**, PartitionStyleRAW. Cette énumération ne constitue pas un accès réussi aux données.

## Tranche 2 — diagnostic minimal et arrêt

Seconde cible vérifiée exclusivement : ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**. GDB : CID du handle **03534453/44333247/858BF092/3C01927A**, **62333952 secteurs512** enregistrés. **Un CID mémorisé dans le handle ne prouve pas la disponibilité physique actuelle**.

Readback complet de l'image **31437octets**, byte-identique au BIN tiers Release précédemment qualifié pour sa provenance : **SHA-256 2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a**. ELF **511322f3bcfc2a19b0572d5db34d86dfc53ce6eb055abd502ae8b07bbc077494** ; checkoutVILLO88 propre au commit `1e3c9183bad023ebf7d234134823d9270a0e8b64`. Pas de reflash/rebuild ni modification firmware. `result_sha` est ce commit externe, distinct du commit MSM de publication.

| Lecture réelle | Valeur |
|---|---|
| SDMMC1_CLKCR |0x4002 : contrôleur4bits/div2|
| RCC CR / CCIPR1 / CCIPR2 |0x03003005 /0 /0 : HSI48 ON/RDY et kernel48MHz, donc SD12MHz calculés|
| CPU |160MHz|
| sd_ready / HALstate |1 /READY ; ne démontrent pas la disponibilité|
| HAL.ErrorCode |**0x1007**|
| RESP1 / STA |0xB00 /0x81000|
| USB connected/config / ejected |1/1 /false|
| CFSR / HFSR |0/0|

Interprétation d'après header HAL tiers conservé : **0x1007 = CMD_CRC_FAIL0x1 + DATA_CRC_FAIL0x2 + CMD_RSP_TIMEOUT0x4 + COM_CRC_FAILED0x1000**. Pas de RX_OVERRUN0x20 ni ILLEGAL_CMD0x2000 dans ce relevé. RESP1B00 encode DATA5 et READY_FOR_DATA. Ces erreurs sont accumulées : le relevé n'isole ni la première commande fautive ni la cause électrique. Aucune nouvelle commande SSR ou transfert SD exécuté par GDB ; la largeur4bits côté carte reste une preuve historique du PoC, non rejouée ici.

Débogueur détaché. **Windows final à11:04:31,307UTC : USB toujours reconnu, disque Size0 / No Media**. Aucun test16×8192 lancé puisque le support est indisponible ; aucun cycle2/3, benchmark, changement de configuration ou recovery. Windows/USB peuvent traiter automatiquement des commandes dans le firmware tiers inchangé ; zéro écriture opérateur ne signifie pas que ces échanges automatiques ont été instrumentés.

## Comparaison et limites

| État | Résultat |
|---|---|
| PoC20cm original |Succès ponctuel jeuB128KiB exact ; write+flush3178,7446ms (~41234B/s), read187,4372ms (~699285B/s), puis CRC/NoMedia|
| Comparatif10cm précédent |NoMedia initial persistant après un reset logiciel, zéro cycle|
| **10cm après coupure complète rapportée** |**NoMedia avant GDB et après diagnostic, zéro cycle, aucun reset supplémentaire**|

**Aucun gain chiffré calculable** : pas de cycle mesuré dans cette mission. Les temps20cm sont applicatifs hôte/USB, pas du débit SD pur. L'accès n'est pas restauré dans cette observation ; **cela ne conclut pas à l'inefficacité des fils10cm**, ni à une panne intrinsèque de SDMMC1 ou de la microSD. Origine CRC, séquence d'initialisation/USB et causes électriques restent non isolées ; influence du halt/detach non exclue, mais NoMedia préexistait à cette session. Coupure électrique déclarée, durée/rail non mesurés par Codex.

Ni cadenceTR2 426672B/s, endurance, filesystem, intégrité générale ou recovery ne sont qualifiées. Aucune correction HAL/multi-read/MSC, variation de fréquence, formatage ou changement de câblage : hors périmètre. Si une investigation de la première erreur devient nécessaire, elle doit faire l'objet d'une **mission distincte**, pas de resets répétés dans celle-ci.

## Tranche 3 — préservation, preuves et publication

[Preuves minimales](preuves/TR2-20261014-SDMMC1-COLDSTART10CM/README.md), [résumé](preuves/TR2-20261014-SDMMC1-COLDSTART10CM/campaign-summary.json), [Windows avant debug](preuves/TR2-20261014-SDMMC1-COLDSTART10CM/host-before-debug.json), [image](preuves/TR2-20261014-SDMMC1-COLDSTART10CM/image-verification.json). Bruts/readback local : `/home/lolo/dev/msm/reports/TR2-SDMMC1-COLDSTART10CM-20261014`. Image/sources tierces non redistribuées, méthode/hashes/petits logs avec manifestSHA256 publiables.

Dépôt d'origine propre, HEAD82169af7e76797731358852bec83bfb0ceb54ccf historiquement divergent conservé. Nouveau worktree `MSM_vibe-SDMMC1-coldstart10cm` / branche `codex/p12h3h-sdmmc1-coldstart10cm`, base main courant ; aucun merge/rebase/reset ou écrasement. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 /f3b829e6188f491920edf74a5f9ed0f559d08b87 et worktrees/précédentes preuves préservés. Mission/rapport précédent archivés byte-identiques depuis `d4f451c1ced5c39f2df9abf690a71be4bb8ce53e` sous `archives/TR2-20261014-P12H3H-SDMMC1-CABLE10CM-COMPARISON/`.

**État laissé :** seconde Nucleo imageoriginaleRelease inchangée, debuggerdétaché ; dernier MCUerror1007/R1B00 puis WindowsNoMedia. Carte/câbles connectés selon contrat, pas de télémétrie continue après detach. **Principal003500463235510B37333439, FRAM, média512Go, Modbus, protections/OptionBytes non accédés**. Aucune élévation, politiqueWindows ou ACL modifiée. `allow_debug` exercée pour observation ponctuelle ; `allow_flash` non exercée. Commit/pushmain de documentation/preuves autorisés.

Validation proportionnée : identité/image/diff et observations réelles. Firmware et interfaces inchangés : aucun build/test ancien rejoué, aucune validation globale nécessaire pour les documents. JournalETAT_COURANT_TR2 actualisé avec fait nouveau utilisateur, NoMedia avantGDB, erreur1007, zéro reset/cycle et arrêt. Gouvernance/gels/roadmap/detteA04 inchangés. Push normal sur `main` réussi : `8253835ef0adbe990db7842e847ae6ea12e7629d` (`P12 H3h SDMMC1: preserve cold-start No Media observation without reset or write`). Référence récupérée de GitHub et **20 fichiers relus intégralement, byte-identiques**, **15 empreintes du manifest vérifiées** : [preuve de relecture distante](preuves/TR2-20261014-SDMMC1-COLDSTART10CM/remote-readback-8253835e.json). Archives/fichiers protégés contrôlés, source tiers propre, dépôt original et stashes préservés. Mise à jour finale FAILED purement documentaire, aucun nouvel accès matériel ; worktree de mission propre après commit. La dernière publication sera vérifiée à son tour et son SHA distant communiqué dans la réponse finale sans auto-référence.
