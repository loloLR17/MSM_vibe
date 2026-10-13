---
mission_id: TR2-20261013-M016-STORAGE-PREREQUISITE
status: IN_PROGRESS
base_ref: main
base_sha: bb299d711d9bd0a76d52eafb201c2670e6cff161
initial_head: 9647cb0b152dde28b2485de88f551f95883b8e06
result_sha: null
created_at_utc: 2026-10-13T17:25:50.574444+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Stockage préalable M016 — checkpoint d'inspection sûre

Mission commencée, non terminée. Voie A du contrat : V1 inchangée ; FIFO→B3 hors périmètre. Rapport BLOCKED antérieur préservé byte-à-byte dans `archives/TR2-20261013-M016-V1-VIBRATION/`.

Tranche réalisée : inspection du backend bulk, géométrie/durabilité et ressources ; extension minimale du profil **COMMISSIONING_READONLY** avec une commande RAM de lecture SD bornée (1..128secteurs, avantLBA65536, contrôle capacité réelle), sans write/format/allocation ni auto-repeat. Aucun DataStore de production activé. La zone pré-partition n'est pas encore une propriété de TR2.

Validation : cross-build readonly réussi et symboles writeSD/Fram_Write/Sdmmc2_Bringup absents de l'ELF ; cross-build cumulatif canonique `tr2_validate.sh --cross-build-only` réussi, BIN169948octets SHA256 `b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b` **identique A04-02**. Pas de suite hôte répétée pour ce seul harness readonly exclu du cumulatif. Firmware permanent inchangé à ce checkpoint.

Préflight physique : ST-LINK003500463235510B37333439 attendu disponible, UID004A003D/38425018/20333157 vérifié par GDB ; préfixe Flash intégral égal au BIN A04-02 vérifié. Productionstage2/result0, FIFO READY/cleanup0C8000, FRAM recoveryVALID/gen12, TXidle/faultfalse. Debugger détaché. Backup et image de retour A04-02 vérifiés, pas de sauvegarde massive répétée.

[Preuves checkpoint](preuves/TR2-20261013-M016-STORAGE/checkpoint-provenance.json), logs builds/GDB adjacents ; preuves locales dans `/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z`. Prochaine tranche : flash normal du seul probe readonly, lecture de la capacité/CID/MBR et inspection conservatoire de l'espace candidat, puis retour au cumulatif A04-02. **Aucune écriture SD autorisée tant qu'une attribution non destructive n'est pas démontrée.** Toute zone occupée ambiguë exige une décision ; aucune interprétation de zéros comme permission automatique d'écrire.

Dépôt original et deux stashes conservés. Worktree isolé `MSM_vibe-M016-storage` à partir de9647cb0. Seule modification de code : `platform/stm32/main.c` dans branche préprocesseur readonly ; archives et rapport/preuves de checkpoint. Les profils A04-01 et la politique B5 A04-02 sont inchangés. Commit/push normal versmain et relecture distante avant poursuite ; SHA évalué/physique consigné dans les preuves ultérieures.
