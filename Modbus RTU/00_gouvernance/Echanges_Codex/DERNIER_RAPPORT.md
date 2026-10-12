---
mission_id: TR2-20261013-M015-T-CPU-SPI4
status: BLOCKED
base_ref: main
base_sha: 79c52f2882a63f4a12a2cf713c978c28253baf24
initial_head: b28b15a0a9d5e2cbc77c285653edb28f1b56f7af
result_sha: null
created_at_utc: 2026-10-12T23:39:12Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-T — checkpoint horloges préparées

CPU160MHz parPLL/MSI4, VOS1/Flash4WS, APB160 ;SPI3HSI16/div4 indépendant maintient SCK4MHz calculés. SourceSPI MSI→HSI changée, fréquencephysique nonmesurée ;nepasprétendreisolementparfait du CPU. [Protocole T](../../../experiments/iis3dwb_st_reference/QUALIFICATION_M015_T.md) fixe contrôlesRCC/PWR/latence/clockavanttrigger et critèresQ/S inchangés.

DeuxrefusCMake ciblés et deuxARM réussis ;BINréférenceCPU16byte-identiqueS4, aucunevalidationprécédentephysique/parserrejouée. Pasencore d'accèscibleT ;uneseuleacquisition4096 aprèspréflightvalidé. ArchivesSbyte-exactes,worktreeTisolé/primaire/stashes préservés ;FRAM/SD/Modbus/HP exclus. BLOCKEDprovisoire pourphysique etpublication. Preuves : /home/lolo/dev/msm/reports/TR2-mission015-T-20261012T233912Z.
