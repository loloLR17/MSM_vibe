---
mission_id: TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY
status: IN_PROGRESS
base_ref: main
base_sha: 3538017a361d7178a1ac19a9563aeaf2eae1af37
initial_head: 5efffbd84950cbe96d114b0619fd8846b07d63f0
result_sha: c6c56b63e02cb78f176f278195dc114a6b7cbe76
created_at_utc: 2026-10-13T22:02:17.131009+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — LOAD-ONLY, checkpoint avant unique essai

Préparation terminée et préflight réussi. **Campagne non déclenchée à ce checkpoint.** Interruption utilisateur survenue avant INIT/RUN ; processus et fichiers revérifiés, aucunrunmarker et aucune session active. Confirmation banc/carterenouvelée, pas de répétition d'essai/flash. Deuxconfirmations physiques respectées : card retirée avantflash, puis réinsérée horsalimentation après vérification de la libération GPIO.

ProjetST indépendant réutilisé, nouveauprofil fixe1bit10MHz/600s et arrivée26667records16octets/s, vraiefilebornée512KiB, chunks64KiB, métriques puis relectureintégrale. [Scénario reproductible](../../../experiments/sdmmc_st_reference/LOAD_ONLY.md). HAL/driver/références gelées inchangés, pasde4bit/benchmarkannexe/oscilloscope/format. Cross-buildspécialisé réussi puisrebuild instrumentationpremieréchec, pasdesuitehost/cumulativeancienne rejouée. BSS609152octets dansprojetisolé ; paspréjugédeRAMruntime.

SecondSN0032001F3235510C37333439/UID001B00463842501820333157 vérifiés ; firmwarec6c56b6 BIN23760 SHA2639dee58fd27da7471c513336a29bcc7493ff92a51c65b5fb9a02ac087791f1 vérifiéCLI etreadbackGDB intégral. FirmwareGPIO remplacé sanscarte, PB15/3/4INPUT, POWER0 au repos. User autoriséàréinsérer horsalimentation, retourconfirmé. Pasdenouveaureflash aprèsinterruption.

INITunique strictementnécessaire : stage2/error0, CID03534453/44333247/858BF092/3C01927A,62333952secteurs512, kernel160MHz/div8/CLKCR8→10MHz ; largeurSSRcarte0 etWIDBUS0=1bit ; R1=0900 ready, HALerror0. RégiontestLBA2048,500007secteurs pour256003584octetsphysiques, médiumsecondentièrementsacrifiable. Aucunbytegénéré/écrit jusquecepoint.

Prochaineaction : markeruniqueRUN, GDBhardwarebreakpointterminal résoludepuisELF08001666, continue sanshalt intermédiaire pendant600s etreadback, puiscapturecompteurs/latences/intégrité. Un seulrun, aucunretry ; premieréchec prouvéconsigné si présent. Sourcecode évaluée c6c56b63e02cb78f176f278195dc114a6b7cbe76, paramètresFIXES. Fréquence déduitedesregistres, pasmesureélectrique.

[Preuves](preuves/TR2-20261013-M016-1BIT-LOAD/artifacts.json), commandes/logs/scripts/binhex ; local `/home/lolo/dev/msm/reports/TR2-M016-1BIT-LOAD-20261013T215507Z`. Source de véritémaininitial5efffbd, missionbase3538017 différente duHEAD mais référencehistorique ; worktreeisoléMSM_vibe-M016-1bit-load-only. Stashes/worktrees/archivespréservés, ancienrapportGPIO50c78fa archivébyte-identique. Principal/FRAM/Modbus/médiasprincipauxnonaccédés ; probeprincipalhistorique nonrestauré, conformémentaupérimètre. Pas de qualificationruntime/V1/7jours. Journalactualisé pourtransition sûre/préparation. Commit/pushnormalmain et relectureintégrale avantcampagne, SHAfinal aprèsrapport.
