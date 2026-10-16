---
mission_id: TR2-20261016-R4Q-LIVE-B3-RTU-COMPOSITION
status: IN_PROGRESS
base_ref: main
base_sha: 48805fe638151a33f2e44f17eb6da01a00e91efa
initial_head: cf9d1b5321923a0abb2494b46b2c7860e5dcecc6
result_sha: 6e813501a973fb25bd41bf3b0713435763414779
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-Q — checkpoint logiciel avant probe readonly

Composition [LIVE_R4Q](../../05_Firmware/LIVE_R4Q.md) implémentée : snapshots B3 RAM
cohérents, âge/fraîcheur à chaque lecture depuis copie sans callbacks, contexte
run/config/génération, staging B4 et capture APPLY immuable, ingress B5 différé
vers le vrai moteur R2, trois captures normales/réserveSTOP et garde de non-réentrée.
Handler RTU optionnel ; production V1 conservée, effets/B1/B2/B6/B7 successeur
non composés et aucune capacité opérationnelle LIVE annoncée.

140tests complets PASS61,23s + ARM canonique ; nouveau test enrichi quatre tailles
PASS2,86s, ASan/UBSan ciblés PASS (logs complets). Modèle FIFO+R3B+FRAM/RTU :
1554683XYZ/7019RTU, work484/service498/gap613us, FIFOmax26/RXmax6. Ce sont des
horloges et allowances SIMULÉES, jamais des temps cible. Échecs de construction
des nouvelles fixtures conservés : état initial inactif après boot, nouvelle
source RTU à redémarrer au début d'une trame, génération effective2 après APPLY.
Aucun oracle gelé modifié ni test affaibli ; test R4 historique simplement rendu
réutilisable par guard de main, sa régression toujours exécutée.

BIN canonique V1 différent85b1be60…c0f5 (runtime enrichi/disposition mémoire),
non-régression de comportement par140tests ; pas de byte-identité annoncée.
R2/R3-B/média/driver ST inchangés, R0/V1/dettes/archives préservés.

Préflight réel14:54:46UTC : uniqueSN003500463235510B37333439,
UID004A003D/38425018/20333157, WHO7B et capteur arrêté ; Flash2Mio fraîche
c5bc344d…f5b26/FRAM262144a13b3401…a2ef9 identiques aux backups vérifiés R4-P.
Pas encore de flash/reset/writeFRAM en R4-Q. Prochaine tranche : image explicite
tr2-r4q-readonly-rtu-NON-PRODUCTION, seultrigger9, récupération readonly5 puis
10s de lectures FRAM avec B0/B3/B4/B5 en RTU loopback RAM. Pas UART/RS485,
capteur reste arrêté : fournisseur temps événement STM32 non qualifié, aucune
valeur vibratoire B3 valide inventée. Ensuite restauration Flash intégrale,
FRAM exacte conservée sans rollback et publication/relecture finale.

Worktree isolé depuis maincf9d1b5, ancienmain82169af et stashes conservés ;
R4-P mission/rapport archivés byte-identiques depuis48805fe. Toutes autorisations
logicielles exercées ; debug de préflight seul à ce checkpoint. Aucun SD/HP,
OptionBytes ou intervention physique. Preuves dans preuves/TR2-20261016-R4Q.
