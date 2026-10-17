---
mission_id: TR2-20261017-R5AFIX-B3-WIRE-ORDER
status: IN_PROGRESS
base_ref: main
base_sha: e100770474cf4d43a73f2af2e6e02be91e2cd75c
initial_head: 5103ea4e87c169c1b154d0df8cdcc50511619a24
result_sha: 29a6f4ecf83c97499055b58fc73d631763cf5813
created_at_utc: 2026-10-17T11:39:19.501716+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R5-AFIX — correctif testé, qualification physique préparée

Correction minimale des deux destinations :3002alarmes/3003sévérité ; expressions etmasques inchangés. Oracle indépendant neufétats/mots/ADU, échec suranciencode puisPASS aprèscorrection. Uneassertionhistorique qui visaitelleaussi lemauvaisoffset estcorrigée sans supprimer soncontrôle. Revuedetable ciblée sansautreinversionadjacente démontrée, aucune norme/moteurR2 modifiés.

146tests firmware PASS96,02s/cross-build canoniquePASS/BIN85b1…c0f5 inchangé ; probeNON-PRODUCTION ARMpass.4suitesASan/UBSanO2PASS8,80s ;202testsServicePASS, dontanciennequarantaineR5-A etnouvellefixtureC→PC/SQLitepositive. Erreurinitiale decompilation du nouveloracle (indentation) corrigée ; première invocationdirectetr2_validate nonexécutable, reprisevia bash réussie, tracesconservées. [Plan](../../05_Firmware/LIVE_R5AFIX.md).

Préflight11:36:20UTC UID/SN/M016/Flash2Mio c5bc…f5b26/FRAM70e…73587 entières exactes, capteurWHO7B/ODRoff/FIFObypass/TSoff ; aucuneécriturepersistante nireset. COM4FTDI0403:6001/BG04KSSW retrouvéT1000, aucunmaître concurrentrépertorié. ArchivesR5-A BLOCKED préservées, aucunchangementPCcontournant ledéfaut.

Suiteautorisée : unseulSTART5/10/run10 puisSTOP5/11, aucunretry/collision/RENEW, surguardesexactesjournal62/root75/epoch5/run9/config3. La lecture0/0seule ne différenciepaslesoffsets ; une vraieV/F autoritative nécessite admissionR2. Modèlemétadonnéesvraistoreprévoitjournal70/root83/run10, pasunepreuvephysique. Probe147640octets SHA d50fe6bbdde1b5127ee9cd2c5f3ffcbaae5045453153df71eb6e793a2b13f99b ; flashàfaireaprèspublication/relecture. Respectbudgets1000/2500/5000µs/STOPsûr, captureCRC/offsets etPC, auditFRAM, restaurationM016/relecturecomplète puisrapportfinal. AucunessaiR4-U/V rejoué, SD/HP/OB/RDP/masserase interdits. Publicationdececheckpointn'estpasvalidationphysique.
