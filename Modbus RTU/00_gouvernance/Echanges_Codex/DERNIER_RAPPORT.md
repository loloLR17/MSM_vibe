---
mission_id: TR2-20261016-R4R-PHYSICAL-TIME-B3
status: IN_PROGRESS
base_ref: main
base_sha: 650aac856b3f912dfbe63ee4163bfe02489a8853
initial_head: 9dc28dbb4ed4a0d8b914b667886eca2de8cd388d
result_sha: 0e808f3
created_at_utc: 2026-10-16T20:53:12.775496+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-R — checkpoint capteur/temps acquis, B3 physique à poursuivre

Mission en cours, pas de clôture ni blocage extérieur. Worktree isolé
MSM_vibe-R4R-physical-time-b3 ; ancienmain82169af/deuxstashes/worktrees préservés.
Archive R4-Q exacte issue650aac8 avant remplacement. Contrat9dc28db inchangé.

- **LOGICIEL** : commits8af2451/0e808f3, provider ancré sur FIFO TS réels avec
  tampon8XYZ, reset/rollover/continuité/défauts/UTCabsente ; binding R3-B optionnel.
 141tests PASS62,28s/ARM ; test final ciblé etASan/UBSan PASS, guardsprofilR4R PASS.
 V1 BIN canonique byte-identique85b1be60…c0f5, jamais flashé.
- **PHYSIQUE OBSERVÉ** : préflight UID004A003D/38425018/20333157 et SN003500463235510B37333439,
 Flash2Mio initialec5bc344d…f5b26 et FRAM262144a13b3401…a2ef9 conformes.
 Diagnostic complet2,000064s,52894XYZ/6612TS/0anomalie, delta24ticks etTS avantXYZ
 du mêmeTAG_CNT ; niveau13/poll401us/gap425us/premiermot10875us.
197brackets ;1896543us/151233ticks (12,54054us/tick DWT), résidumax22us,
bracketmax33us. AucunR2admis/0writeFRAM ; FRAM complète inchangée après essai.
- **ÉCHEC CONSERVÉ** : premier diagnostic interrompu par relectureGDB prématurée,
 52097XYZ puiserreurSPI/FIFO1 etSTOP235us ; jamais déclaré PASS. La procédure
 corrigée attend désormais le breakpoint après arrêt, sans toucher au SPI actif.
- **ÉTAT ACTUEL** : image dédiée0e808f3 chargée, stage11/WAIT après diagnostic,
 capteur arrêté, GDB détaché. Flash initiale récupérable localement, restauration
 finale non encore exécutée. FRAM epoch4/run4/config3 inchangée.

**Suite autonome autorisée** : trigger11, vrai moteurR2 START(epoch4,tx5,run5),
FIFO→R3-B/B3 N4096→RTUloopbackRAM avec chargeFRAM32octets etSTOP(epoch4,tx6).
C'est le seul nouveau run nécessaire à la causalité de cette preuve ; aucun
renouvellement ou nettoyage. Vérifier budgets1000/2500/5000us, horloge±100us
expérimentale toutes100ms, oracles RMS/peak des4096XYZ bruts, V/F/HELD/âge/cohérence.
Si anomalie : STOP sûr, conserver toute identité consommée, pas de rollbackFRAM.
Puis restaurer M016 avec relectureFlash2Mio et vérifierFRAM/partitions, publier bilan.

Limites : pas de temps SI/UTC/latence analogique absolue, pasUART/RS485/IRQélectrique,
pas PC/SQLite/Web/SD. Le provider B3 n'est pas encore qualifié physiquement.
R4global/R6 et composition produit restent ouverts. Détails : [LIVE_R4R](../../05_Firmware/LIVE_R4R.md).
Preuves brutes : [dossierR4R](preuves/TR2-20261016-R4R/README.md), snapshots sans perte,
logs init/essai interrompu/diagnostic complet/build/tests. Backups initiaux horsGit
/home/lolo/dev/msm/reports/TR2-r4r/hardware/flash-before.bin etfram-before.bin.

Publication/relecture distantes de ce checkpoint à vérifier après son push ;
statutIN_PROGRESS reflète également la poursuite physique, aucune validation humaine
intermédiaire demandée. Journal technique actualisé au checkpoint.
