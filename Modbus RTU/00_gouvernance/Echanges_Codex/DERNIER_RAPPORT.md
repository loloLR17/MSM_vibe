---
mission_id: TR2-20261013-A04-01-SAFE-BUILD
status: IN_PROGRESS
base_ref: main
base_sha: null
initial_head: a639b636e85b3ca2f1015c0f8dfa8b04b940e831
result_sha: c9ee22292ae69cc6f333aaab3527d18556ee8272
created_at_utc: 2026-10-13T14:51:21.502331+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A04-01 — Sélection sûre des builds STM32

**Résultat logiciel acquis :** configuration générique implicite refusée ; profil cumulatif explicite ; artefacts distincts et options consignées ; harness SD historique conservé derrière opt-in. BIN cumulatif **byte-identique** à M015-V : **169380 octets**, SHA-256 **8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480**. Aucun matériel accédé. **IN_PROGRESS uniquement pour la publication et relecture distante restant à effectuer**, aucun blocage technique identifié. Le registre A-04 reste inchangé ; ChatGPT décide de la clôture A04-01 après contrôle indépendant.

## Références et préservation

Mission récupérée sur `main` à `a639b636e85b3ca2f1015c0f8dfa8b04b940e831`, identifiant et permissions vérifiés. `base_sha` et date de création absents du contrat : aucun SHA supposé, référence réelle initiale consignée. Exécution isolée dans `/home/lolo/dev/msm/projects/MSM_vibe-A04-01`, branche locale `codex/a04-01-safe-build`, destination autorisée `main`. Dépôt historique `main` conservé à `82169af7e76797731358852bec83bfb0ceb54ccf`, worktrees préservés. Deux stashes conservés : `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87`.

AGENTS.md, charte ChatGPT, protocole, journal intégral, dette A-04, rapports A-04/A-05 et matrice A-05 consultés. [Rapport A-05 archivé](archives/TR2-20261013-A05-AUDIT-TESTS/RAPPORT.md) **byte-identique** au fichier de la référence initiale, avant remplacement. Aucun contrat/gouvernance, spécification V1, dette, driver ST, SDK, source cœur FRAM/Modbus/FIFO ni test existant modifié. SDK Linux et architecture T1000 présents ; aucune migration ou configuration Codex modifiée.

## Correction minimale et commit évalué

Commit **`c9ee22292ae69cc6f333aaab3527d18556ee8272` — `A04-01: require explicit STM32 profiles and label cumulative artifacts`**, sept fichiers, +266/-112 lignes. [Diff complet](preuves/TR2-20261013-A04-01/source-change.patch) et [empreintes/provenance](preuves/TR2-20261013-A04-01/provenance.json).

- `platform/stm32/CMakeLists.txt` : recette **TR2_STM32_BUILD_PROFILE=CUMULATIVE**, production/TX qualifié/observer/FIFO coopérative/Debug. Refus des cache/options OFF contradictoires ; gardes historiques maintenues. Sélection absente ou plusieurs modes primaires refusés. **TR2_STM32_LEGACY_SD_HARNESS=ON** seul permet le harness générique ; exclusif des modes dédiés, TX qualifié et observer. SPI4/8 et autorité de commissioning restent contrôlés.
- `platform/stm32/main.c` : garde de précompilation refusant aussi un build direct sans mode dédié ni opt-in legacy. Aucun changement de runtime dans les modes existants.
- `platform/stm32/tr2_build_stm32.sh` : **--profile obligatoire**, directories distincts, artefacts et empreintes contrôlés, aucun effacement de build. Réutilisation seulement d'un directory déjà identifié ; sinon refus. Profil cumulatif utilise explicitement les manifest/registre M014-H et SPI4.
- `tr2_validate.sh` : cross-build sélectionne explicitement le cumulatif ; suite hôte existante conservée. `.gitignore` ignore uniquement les deux nouveaux outputs régénérables.
- `platform/stm32/README.md` : instructions actuelles, comparaison golden et contrôle d'artefact avant toute programmation future ; anciennes commandes de flash génériques retirées de ce README. Les procédures qualifiées historiques ne sont pas réécrites.
- `tests/platform/test_stm32_build_profiles.py` : cinq tests de contrats CMake réels, incluant huit refus historiques, défaut, conflit cache, labels et refus du helper sans profil. Aucun mock de ces contrôles.

Noms : `tr2-cumulative-m014-m015-spi4.{elf,bin,map}` ; `tr2-special-<mode>-NON-CUMULATIVE.{elf,bin,map}` ; **`tr2-legacy-SD-WRITES-NON-PRODUCTION.{elf,bin,map}`**. `build-profile.txt` consigne les options effectives et inputs ; `artifact-sha256.txt` est écrit par le helper après succès. Les flags spécialisés individuels restent disponibles et ne sont pas présentés comme le cumulatif. Les modificateurs d'un profil spécialisé doivent être lus dans son sidecar/cache, pas déduits du seul nom.

## Validations réellement exécutées

Validation proportionnée : uniquement sélection/harness/outillage, aucun cœur ni interface fonctionnelle modifié. **Cross-build et tests ciblés**, sans répéter les 125 tests hôte ou essais physiques historiques sur code inchangé. Les six tests d'autorité sont rejoués conformément au contrat. Les builds spécialisés sont nécessaires pour vérifier la sélection et les chemins liés après changement CMake.

Depuis le worktree ci-dessus :

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  python3 -B 'Modbus RTU/05_Firmware/tests/platform/test_stm32_build_profiles.py' \
  --evidence-dir /home/lolo/dev/msm/reports/TR2-A04-01-20261013T144140Z/configurations-final -v
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash ./tr2_validate.sh --cross-build-only
python3 -B 'Modbus RTU/05_Firmware/tests/platform/test_commissioning_generator.py' -v
bash -n tr2_validate.sh 'Modbus RTU/05_Firmware/platform/stm32/tr2_build_stm32.sh'
git diff --check
```

| Acceptation | Résultat réellement observé | Preuves |
|---|---|---|
|BUILD-01|Recette cumulatif acceptée ; défaut implicite refusé ; U et legacy explicitement sélectionnés acceptés et images distinctes ; conflit cache OFF refusé|[17 commandes/retours](preuves/TR2-20261013-A04-01/matrix-commands-results.json), logs matrice et caches/profils|
|BUILD-02|**5/5 tests de contrats** ; 16 configurations :3 acceptations/13 refus attendus, dont les huit gardes A-05 spécifiques ; helper sans profil refusé avec retour2. **6/6 tests d'autorité** réussis|[Tests profils](preuves/TR2-20261013-A04-01/profile-tests-final.log), [autorité](preuves/TR2-20261013-A04-01/authority-tests.log)|
|BUILD-03|Build cumulatif neuf, puis contrôle complet du helper corrigé ; réussite du lanceur. Comparaison SHA **et bytes** du BIN avec le golden publié M015-V ; taille169380 et SHA attendus|[Build initial](preuves/TR2-20261013-A04-01/cumulative-build.log), [lanceur final](preuves/TR2-20261013-A04-01/cumulative-validator-final.log), [inspection](preuves/TR2-20261013-A04-01/artifact-inspection.json)|
|SD-01|Entrée `main` vers serveur production **noreturn** avant le fallthrough SD ; store de campagne indisponible, callback disponibilité=false. U vers routine ST **noreturn**. ELF sans HAL_SD_Init/WriteBlocks et sans Sdmmc2_Bringup dans ces deux images. Legacy conserve call et WriteBlocks|Sources + désassemblages main/serveur/composition/callbacks, listes modules et symboles dans le dossier de preuves|
|Garde compilation directe|Préprocessing ARM avec vrais headers : all-OFF refusé par **#error spécifique** ; legacy explicite accepté|[commandes/retours](preuves/TR2-20261013-A04-01/artifact-inspection.json), direct-implicit.log/direct-legacy.log|

Deux autres builds ARM neufs : U avec `TR2_STM32_ST_FIFO_WINDOW=ON`, legacy avec `TR2_STM32_LEGACY_SD_HARNESS=ON`, via CMake/Ninja, retour0. [Commandes exactes](preuves/TR2-20261013-A04-01/additional-build-commands.json), logs et options archivés. U : BIN66444, SHA `761a413f05be582d4058da3b267fbb07e403bdbdf25a5433275316bd3c1fbfc1`. Legacy : BIN162744, SHA `b70aa493b09e6ac7a1172dd8c974ceefb8498fa37b4b26c2d0ce0e8b55d957e2`. Ce sont des observations de compilation, **aucun de ces deux BIN n'a été flashé**.

Toolchain observée : GCC14.2.1 20241119, CMake3.31.6, SDK v1.9.0 `d88042df24f16799957c24f00c2b234c9e306188`, état propre et sous-modules consignés. Options O2 ciblées ST/FIFO conservées. ELF cumulatif SHA `f16e47ea2f1e88b5ae318695b33355461eef7ffa38d2eaed64a59ac6b0ab4dff`, distinct de l'ancien ELF (métadonnées/chemins/debug notamment) ; **BIN runtime inchangé**, aucune justification de reflash.

### Erreurs d'outillage corrigées

Première exécution : une comparaison de message CMake échoue à cause du retour à la ligne automatique, et le helper utilise par erreur sa variable basename comme variable de boucle d'artefacts, faisant échouer le calcul SHA après un build réussi. Messages comparés avec espaces normalisés **sans changer le refus attendu** ; variable de boucle distincte. Logs initiaux conservés ; cinq tests et lancement final réussis après correction. La répétition de ce lancement vérifie le helper corrigé et sa réutilisation, pas une nouvelle qualification fonctionnelle. Le contrôle de préprocessing a aussi été ajusté pour retirer les options de génération de dépendances du compilateur ; il ne modifie pas les fichiers de build. Lors du packaging, le contrôle whitespace global signale uniquement les fins de lignes/blancs des caches, logs `size` et du diff brut archivés ; ces preuves sont conservées byte-identiques. Le contrôle `git diff --cached --check` sur code et documents hors preuves brutes réussit. Avertissements linker newlib/nosys observés dans les logs, sans erreur finale ; aucune bibliothèque artificiellement substituée.

## Portée, limites et prochaine décision

Absence de symboles SD **recoupée**, et non utilisée seule : contrôle du main lié, routines noreturn, pointeurs des callbacks `unavailable_store`, `storage_available=false`, options et liste des translation units. Les sources HAL/adapter SD restent compilées dans les inputs historiques mais sont éliminées du cumulatif/U par le link ; aucun backend SD utilisable n'est injecté. Le legacy conserve `Sdmmc2_Bringup` et ses writes automatiques, dont LBA2048 : **opt-in ne rend pas ces writes sûres pour une partition existante**. Utilisation uniquement par mission matérielle dédiée et média sacrificiel. Les noms/sidecars ne remplacent ni le hash, ni l'identité de la cible, ni les permissions de flash.

Aucun accès STM32/ST-LINK/GDB, flash, reset, COM7/HP/RS-485, FRAM ou microSD pendant cette mission. Firmware M014/M015 et données persistantes **non touchés par ces opérations** ; état matériel actuel non relu, dernier état connu celui du rapport M015-V. Aucune preuve physique nouvelle, aucune endurance, métrologie, chaîne V1/DSP ou communication bidirectionnelle requalifiée. A04-02, A04-03 et les autres dettes ne sont pas corrigées ; aucune décision d'architecture nouvelle.

`DETTE_TECHNIQUE_A04.md` préservé byte-identique (SHA **804b90967317196374a844a86c955c2e91880045061e0d7241aeac9f3cfa8a8a**), tous statuts conservés. Journal `ETAT_COURANT_TR2.md` actualisé brièvement : correction logicielle A04-01 et BIN identique, revue ChatGPT attendue, matériel non vérifié. Roadmap inchangée. Prochaine décision ChatGPT : contrôler ce livrable, puis mettre à jour la dette si les preuves satisfont ses critères ; A04-02 reste la tranche suivante prévue, hors autorisation de ce contrat.

## Preuves et publication

[Manifest des preuves publiques](preuves/TR2-20261013-A04-01/manifest.json) :65 fichiers et leurs empreintes, ~482Ko ; scripts réellement exécutés, options, sources liées, désassemblages, logs et commandes. Images ELF/BIN/map conservées localement dans `/home/lolo/dev/msm/reports/TR2-A04-01-20261013T144140Z/qualified-cumulative-artifacts/`, autres builds dans ce même dossier de mission. Le golden public M015-V existant est réutilisé, sans dupliquer un BIN identique.

Commit code local `c9ee22292ae69cc6f333aaab3527d18556ee8272` créé après inspection du diff/index et validations ciblées. Publication rapport/preuves et relecture intégrale depuis référence GitHub à effectuer ; push normal explicitement vers `HEAD:refs/heads/main`, aucun force push/merge/rebase. État de reprise : tranche logicielle complète et cohérente ; aucune modification expérimentale ni aucun blocage extérieur. Après publication/relecture, seul le statut et sa preuve distante restent à consigner ; **pas de nouveau build/test/accès matériel nécessaire**.
