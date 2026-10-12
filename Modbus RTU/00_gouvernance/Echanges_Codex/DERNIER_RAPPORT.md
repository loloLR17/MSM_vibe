---
mission_id: TR2-20261012-M015-M-SPI-LONG-TRANSFER
status: BLOCKED
base_ref: main
base_sha: f8e782a41e2cca94ab12ff3f180a2dc6461611aa
initial_head: 3e147109cb21c30a4b4525a664504bdee9eeefa7
result_sha: null
created_at_utc: 2026-10-12T20:51:11.479862+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-M — checkpoint logiciel avant campagne physique

Tranche logicielle validée : observation passive avant nettoyage HAL SPI,
acceptation complète fondée sur offsets/compteurs conservés, copie de HAL
instrumentée strictement pinnée, optimisation du seul module SPI (`O0`/`O2`).
Pilote/exemples ST, paramètres capteur, horloges et timeouts inchangés.
14 scénarios C + 2 tests Python réussis (2 CTest) ; deux builds ARM FIFO STATUS10
réussis. Validation proportionnée au projet autonome ; aucun build TR2 ni test
historique inchangé rejoué. Aucune opération matérielle M015-M à ce stade.

Le budget théorique RX est 4µs/octet, soit64cycles CPU16MHz. Saturation du
service HAL Debug est une hypothèse à confronter aux snapshots O0 puis O2 ;
OVR SPI STM32 ne prouve pas OVR FIFO capteur. L'ancien HAL_CloseTransfer remet
les compteurs à zéro même en échec : ceux de M015-L ne prouvent pas896octets reçus.

[Preuves logiciel](preuves/TR2-20261012-M015-M/), originaux `/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z`.
Archives M015-L/essai02 exactes conservées distinctes de l'échecWHO initial.
WorktreeM isolé, dépôt principal/worktrees historiques/deux stashes préservés.

Prochaine tranche autorisée : identifier seconde cible, vérifier image/hash,
flash normal/verify/readback, un diagnostic FIFO O0 puis candidat O2 si refus
connu seulement ; préserver tous les bruts et analyser intégrité hors ligne.
Nouvelle règle : **aucun backup/restauration Flash2MiB du banc secondaire**.
Le firmware final restera diagnostique et inactif après la séquence bornée.
TR2 principal/M014-H/FRAM/microSD/Modbus strictement exclus.

BLOCKED provisoire porte sur preuves physiques et relecture distante encore
à obtenir, pas sur une décision utilisateur attendue. Journal actualisé.
