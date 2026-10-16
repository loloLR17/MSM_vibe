---
mission_id: TR2-20261016-R4P-PHYSICAL-COEXISTENCE
status: IN_PROGRESS
base_ref: main
base_sha: f7366e535b90a4ef79642a3debfa601f35c40b9c
initial_head: 03309c076da7322fb774d41c032112cd663390fd
result_sha: 369887b9583ca05d0208c3d18bdd485311bcb151
created_at_utc: 2026-10-16T12:55:58Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-P — checkpoint avant unique maintenance physique

Préflight réel : ST-LINK003500463235510B37333439/UID004A003D/38425018/20333157,
Flash2Mio et FRAM262144 doublement relues, récupérables dans reports/TR2-r4p/hardware,
identiques R4-D ; SHA Flashc5bc344d…f5b26, FRAMf2bd7a99…8a0c1. Capteur relu arrêté,
WHO7B/ODRoff/FIFObypass/TSoff. Aucun flash/reset/writeNVM encore exécuté en R4-P.
Mission et rapport R4-D archivés byte-identiques ; stashes/worktrees protégés.

[Plan](../../05_Firmware/LIVE_R4P.md) : une seule isolation logique, état intermédiaire
quarantiné par frontend CLOSED/RETIRING ; vraiRENEW2->3 avant admission, INTENT128
etfloor run2 conservés, ancienneclé refus24, prochainsruns3/4. Aucun reçu pour
l'ancien STARTED. Snapshot intégral initial archivé sans perte ; historique/tail
jamais écrits. Aucun trigger6 de reprise/renettoyage dans ce profil.

Test ciblé139 PASS1,93s,142coupures+4phasesRENEW sur vrais stores/snapshotf2bd,
anti-rejeu/floors/no replay et contrôle négatif du pont. ASan/UBSan ciblé PASS1,12s,
cross-build canonique/probe PASS. Cœur R4-D inchangé,138anciens tests nonrejoués,
BINcanonique V1 inchangé. Oracle finalMODEL-ONLY a13b3401…a2ef9 n'est pas une preuve cible.

Tranche suivante : flash standard de l'image dédiée issue du code369887b9583ca05d0208c3d18bdd485311bcb151, WAIT/UID/readback,
unique trigger4 ; vrai START3/Renew4 sousFIFO/60s/STOP/APPLY/START4 puis faute service
après COMPLETED. Arrêt immédiat si nouveau STARTED indéterminé, dépassement ou défaut.
Si succès, recovery readonly5 après reset, puis Flash initiale restaurée2Mio.
FRAM finale conservée sans rembobinage. Aucun SD/HP/RTUélectrique/R6/B3.
