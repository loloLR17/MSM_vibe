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

## Reprise et checkpoint avant qualification en écriture

main distant et worktree confirmés à214e1055b01a669f89fed57ea9ab41698410bbc4 ; travaux non commitées préservés. Contrat actuel inchangé. Identité et Flash du probe readonly reconfirmées sans reset, read_done256, trigger0.

**Acquis physiques nouveaux, lecture seulement** : CID03534453/44353132/8562E43C/F80195EA, capacité511868665856octets, MBR inchangé (partition type07 LBA65536/count999677952). Les16777216octets de [LBA16384,LBA49152) lus en256chunks64K sont entièrement nuls ; backup complet local SHA256080acf35a507ac9849cfcba47dc2ad83e01b75663a516279c8b9d243b719643e. FRAM262144octets SHA256d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce, identique A04-02. Aucun writeSD/FRAM.

[Attribution explicite](../ALLOCATION_STORAGE_M016.json) établie dans le périmètre de la mission : zone hors partition, intégralement lue nulle, sauvegardée, liée UID/CID/MBR ; pas de repartition/formatage ni de réutilisation de données préexistantes. La qualification future écrit seulement des nouvelles campagnes bornées dans cette région. Une commande RAM unique, profil isolé, nouvel examen intégral de vacuité et refus sur mismatch précèdent toute écriture. Les extents de qualification seront conservés.

**Code préparé/validé logiciel** : adaptateur CampaignBulkMediaRegion réutilisant PersistentMediaRegion existant, confinement32bits sur média64bits, limites alignées secteurs, pas d'allocation implicite ; nouveau test réel de backend/durabilité, overflows, sentinelles hors région et erreurs. Profil STORAGE_QUALIFICATION exclusif, sans UART/FIFO/legacyharness. Programme autonome prévu :4Mio d'append/64checkpoints,4096octets de queue sans checkpoint, autre campagne65536octets avecfinish, recovery, débit requis426672octets/s puis recoveryConfigurationStore V2 réel sur FRAM en lecture seule avec callbackwrite refusant systématiquement. **Ces essais d'écriture ne sont pas encore réalisés.**

Validation conservée :127/127tests hôte + cross-build, nouveau testregions inclus ; quatrecas de profils puis leur test permanent ciblé réussis. Après revue du confinementFRAM et des conditions de préflight, cross-builds spécialisés et cumulatif réussis. Un module expérimental lié inutilement modifiait l'ordre de résolution des archives/sections malgré GC : liaison désormais conditionnelle au seul profil spécialisé, BIN cumulatif169948octets à nouveau exactA04-02 (b4301d6f...). Pas de répétition des127tests sur des changements CMake/harness qui n'altèrent pas le cœur ; preuves de cross-build et refus CMake proportionnées. Profil de débit :Debug, core et modulequalification -O2, HAL inchangé ; pas une garantie de performance cumulative.

BIN qualification62312octets SHA256d1b736a8027fbd373c4ce36c15a0a9f4d7a2d882a92e876e1ea95a24fb09b463 ; empreinteELF/profil dans `qualification-artifacts.json`. Capacité logique géométrique envisagée14MB décimaux, après8192metadata et huitréserves4608, payload4096/extent4608 ; pas encore une readinessproduction ni valeur normale du runtime. Qualification physique, reprise après reset, limites et productionreadiness restent à établir. Firmware actuel :probe readonly ; retour au cumulatif vérifié prévu après campagne.

Cinq tests backend/configuration ciblés sous -O2 réussis après ajout de cette optimisation au seul profil isolé ; ils complètent les127tests déjà acquis sans rejouer les cas inchangés. Le test de profils permanent a été ajouté et exécuté sur ses quatre configurations, anciennes méthodes inchangées. Revue et cross-build finaux confirment les BIN publiés ; aucun nouveau flash ni writeSD depuis le checkpoint214e105.
