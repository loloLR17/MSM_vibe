---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: IN_PROGRESS
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
result_sha: a7ca102f857310e139d26f52c6332032ca8e9381
created_at_utc: 2026-10-30T06:34:02.250544+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — checkpoint E0/E1, avant flash et heure physique

Mission distincte de R3-DURABLE 10min, non réexécutée. E0/E1 réalisés ; E2 physique en préparation, E3/E4 non exécutés. Aucun DONE/endurance anticipé.

## Acquis logiciels vérifiés

- Code `a7ca102f857310e139d26f52c6332032ca8e9381` : profil explicite `R3_ENDURANCE_1H=ON`, nouveaux noms H1N0003.TR2 (gate2s des chemins modifiés) puis H1N0005.TR2 (3600s,72MiB). Formatage et anciens paliers bypass/60s/600s refusés dans ce profil. Lecture FatFs+CRC/SHA du gate court avant heure.
- Cause préventive E0 : ancien refus source us>UINT32_MAX et brackets tronqués ; sources/brackets désormaisu64, Resultv3 et fichier version2 avec endpoints sourceu64 footer128/136, lecteur rétrocompatiblev1. FIR/DEC8/coefficients/bus/quanta/file64KiB et références1000/2500/5000us inchangés. [Audit détaillé](../../../../experiments/r3_durable_fat32/ENDURANCE_1H.md).
- Modèle réel FatFs72MiB+lecteurPython PASS2/2 (`host-tests-gate.log`), dont rolloutDWT5400s et temps absolus>u32 ; cas de corruption/troncature/versions/indices, STOP80états et busy150/500ms/groupes saturants conservés. Première assertion de non-écriture du disque plein placée avant les allocations du test : échec conservé puis point de mesure corrigé juste avant le refus, sans affaiblir la garde.
- Cross-build canonique PASS via `bash tr2_validate.sh --cross-build-only` (lanceur sans bit exécutable, premierexit126 conservé). ARM candidat final PASS, text54192/data20/BSS546200B (réserves du linker incluses), warnings newlib stubs non utilisés identiques au contexte de banc. Aucun changement du cœur scientifique, anciens150tests non rejoués. Pile physique à mesurer pendant nouveaux gates.
- BIN54212B SHA256 `e6168e5c999e27d38cc718bdc6cdf98c05b23766da0270f226b7c42b6d9b9239`, ELF `e9991679d9df514477b6a48709a9c4e9dc8f175209fad585e070857f414057e6`. Image non encore flashée à ce checkpoint.

## Préservation et préflight observés

- PC natif : F: FAT32/Realtek/capacité31914983424B,31876808704B libres ; six anciens RUN/R2N re-hachés intégralement et identiques, TR2INFO343B SHA71e626…11553 exact ; deux noms H1N absents. Preuves `pc-before/PRESERVATION.json`.
- Utilisateur confirme ensuite SD réinsérée dans TR2-002 sous ST-LINK en réponse à la demande hors tension/USB natif débranché/câblage inchangé. Pas de mesure électrique nouvelle prétendue.
- GDB qualifié, SN0032001F3235510C37333439/UID001B0046/38425018/20333157 et WAIT2/run0/FIFO arrêté/CS haute observés ; ancienne image6fdae042 intégralement relue sur ses57448octets exacts SHA27f5…f9ef6. WHO7B par lecture ST ciblée, sans commande SD ni reset. Débogueur détaché. Zéro flash à ce point.
- TR2-001/FRAM/COM4/serviceWindows/microSDproduction non accédés. Sauvegardes/images R1/R2/R3 et branche archivea3f5 conservées.
- Dépôt historique main82169af propre et divergent préservé ; deux stashes6251b98/f3b829e intacts. Préparation dans MSM_vibe-R3-1h préservée avec ses modifications ; arrivée distante9e2dec54 ajoutant uniquement roadmap8h, sans modifier cette mission. Intégration sans merge/rebase dans nouveau worktree MSM_vibe-R3-1h-publish basé9e2dec54 ; branche codex/r3-1h-publication destinée par push normal explicite à main. Aucun travail existant supprimé.

## Reprise E2→E4

Répertoire externe `/home/lolo/dev/msm/reports/TR2-r3-1h-fat32-endurance` ; helpers `hardware_second.py`, `run_capture.py`, `analyze_capture.py` préparent uniquement SN002. Conserver image précédente/backups ; flash normal vérifié/readback candidat, cmd1 WHO/CID/mount/cal puis cmd3 gate2s nouveau et analyse. Si conforme, cmd5 après relecture du gate vers3600s, timeout hôte4200s ; aucune interruption GDB pendant FIFO/SDbusy. Un seul essai long, pas de répétition silencieuse ; en cas de faute conserver état/fichier/causalité.
Après terminal, contrôler STOP/FIFOoff/SDquiescente/CS haute/detach, demander retrait physique vers lecteurPC. Copier fichier entier, vérifier chaque record/CRC/index/time/compte/footer/SHA face au digest producteur RAM, 128préfixes et oracle107coeff ; re-hacher six historiques. Rapport final après observation réelle seulement.

Audit8h documenté : élargir durée session/Result/header/footer et scripts ; préallocation~0,5Go framed+marge, DWTmonotone et compteurs, autonomie sans hôte selon roadmap9e2dec54. Aucune8h exécutée. Risques de grappes SD>100ms, file pleine, power-loss/FAT orpheline et métrologie restent distincts. /10candidat~2643Hz, DC préservée, UTC inconnue, pasV1-05modifiée.

Les preuves sont compressées sans modification avec SHA bruts/stored dans [BUNDLE](preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/software/BUNDLE.json). Publication/relecture distante à effectuer ; aucune qualification indépendante ChatGPT annoncée.

## Checkpoint physique — 30/10/2026 06:36 UTC

Rapport logiciel publié01057a5e01da2ad5100cfe81ac9890baa9f09a50, récupéré et69fichiers relus/39artefacts décompressés/hachés à06:34:10UTC. Code évalué a7ca102f inchangé. Un seul flash standard exécuté,54212B relus exactement SHAe616…9239 ; Resultv3/288B, Bracket24B observés. Image précédente et backup2MiB réutilisés/vérifiés, aucun reset logiciel supplémentaire.

WHO7B/CID/CSD/OCRC0FF8000/capacité62333952secteurs exacts, FAT32 montée sansformat. Calibration193brackets/6576ancres/résidu42µs. Gate nouveauH1N0003.TR2 :2,000162s,52584XYZ/5259sorties/31554B,68records/9groupes, FIFO46/file5120B ; zérofaute. Maxwork691,175/service1282,706/gap1538µs ; STOP151/drain9368µs, pile1112B/plancher2984B.103sortiespréfixe indépendantes conformes. Allocation4clusters etfree972803observés. Le lecteur d'analyse a été invoqué une fois trop tôt alors que GDB finissait ses dumps : buffer encorevide refusé, puis analyse effectuée après retour0 ; aucune capture répétée.

**E3 en cours** : commande5 lancée30/10/2026 à06:35:32.530364UTC, après gate réussi. Le firmware vérifie le fichiercourt puis préalloue72MiB/2304clusters avant la capture3600s. HôteGDB encontinue avec unique breakpointterminal, timeout4200s ; aucun sondage/halt durantFIFO/SDbusy. L'horodatagehôte est celui du lancement, pas l'UTC des échantillons. Résultats de l'heure NONENCOREOBSERVÉS. Ne pas flasher/reset/rejouer ou retirer la carte pendant cet état. Processus de pilotage externe `run_hour.py`, sortie `hour-console.log` ; session outil98700 au lancement, preuves terminales prévues dans physical-hour et HOUR-TERMINAL.json.

Après terminaison, analyser distributions/compteurs/pile etquiescence, puis demander retraitversPC. Aucun DONE avant fichier effectivement extrait et validé. Preuves supplémentaires : `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/gate/BUNDLE.json`.
