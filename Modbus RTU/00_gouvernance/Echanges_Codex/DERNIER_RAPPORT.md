---
mission_id: TR2-20261013-M016-V1-VIBRATION
status: BLOCKED
base_ref: main
base_sha: 25406aff35a769fc2bcf2dd77b33bf0571b832a9
initial_head: 871d1186b7fea7ab5ce09a44a8e03ad79fc90618
result_sha: 871d1186b7fea7ab5ce09a44a8e03ad79fc90618
created_at_utc: 2026-10-13T17:13:53.189101+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — précondition ActiveConfiguration incompatible avec la voie sans stockage

**M016 non terminée et non qualifiée.** Diagnostic de précondition et cartographie publiés et relus intégralement sur GitHub. Un obstacle réel impose une décision structurante avant le raccordement métier : la voie M016 sans SD ne fournit aucune capacité utilisable, alors que la V1 exige du stockage pour valider une configuration et que le firmware interdit une acquisition métier sans configuration active autoritative. Ce n'est pas un échec FIFO ni une difficulté ordinaire de compilation.

## Faits et reproduction

1. [B4 gelé, §8.1.1](../../01_Specification_source/bloc4.md) : `storage_mode=0` et `storage_limit_mb=0` sont invalides ; seul mode1 store-and-forward, limite positive compatible avec la capacité utilisable. [Architecture boot, §4.1/4.4](../../04_Architecture/ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md) : recovery UNAVAILABLE ne publie aucune ActiveConfiguration ; pas de démarrage sans snapshot autoritatif.
2. `platform/stm32/stm32_production_board.c` fournit `{false,0}`, `storage_available=false` et un CampaignDataStore indisponible. `{true,UINT32_MAX}` reste uniquement le contrôle structurel hors boot. [Politique commissioning](../COMMISSIONING_PRODUCTION_M014_H.md) et manifeste M014-H confirment aucune allocation, aucune capacité autoritative, accèsSD NONE. La partition existante reste à préserver ; sa capacité physique historique ne constitue pas une allocation utilisable.
3. Code inspecté : `configuration_validate` retourne ENVIRONMENT_NOT_CHARACTERIZED ; `configuration_store_recover` le convertit en UNAVAILABLE ; `configuration_service_recover` retire le snapshot actif ; `acquisition_service_begin_window` refuse NOT_AVAILABLE. `campaign_service_start_reserved` exige aussi une active puis `begin_campaign` du DataStore : remplacer seulement l'environnement ne débloquerait pas la chaîne et inventerait une autorité.
4. **Nouveau probe C hors ligne réussi**, sur le record réel conservé A04-02 : décodeur/CRC OK ; generation1/config1/revision1,26667Hz,axes7,full_scale_code0(±2g),window4096,storage_mode1,limit1MB. Environnement production inconnu → refus2, capacité connue0 → refus1. Les témoins hypothétiques connus1MB/plafondUINT32_MAX passent uniquement le validateur ; ils ne créent aucune ressource. Mode0/limite0 restent invalides même avec le témoin1MB. Les sorties validées restent intactes lors des refus.
5. FIFO M015/A04-02 fixe **±8g**, flux diagnostic en RAM, pas de configuration active ni fenêtre métier. La convertir comme ±2g à partir du record persistant inactif serait une fausse mesure. Déclencher le chemin métier direct OUTX en parallèle du propriétaire FIFO SPI3 violerait également l'unicité.

Preuves nouvelles : [résultat effectif](preuves/TR2-20261013-M016/authority-probe-result.json), [probe C](preuves/TR2-20261013-M016/authority_probe.c), [reproduction](preuves/TR2-20261013-M016/README.md), [provenance](preuves/TR2-20261013-M016/provenance.json). Il s'agit de l'analyse d'une candidate RAM sauvegardée, pas d'une nouvelle observation du STM32/FRAM. Les sources C évaluées correspondent au HEAD initial ; firmware byte-source identique au code A04-02 `8bcb8436822ce7444074865617cb3696f877a6ad` pour les chemins firmware/experiments/validation examinés.

## Tranches et critères

| Tranche | Résultat réel |
|---|---|
| Git/gouvernance/préconditions | Vérifiées ; dépôt historique et deux stashes préservés ; worktree isolé basé sur main courant ; base contrat ancêtre |
| Autorité ActiveConfiguration | Refus reproduit hors ligne avec record historique réel ; décision nécessaire |
| Cartographie B3 |26champs/48registres inventoriés, source actuelle et lacunes identifiées |
| Chaîne FIFO→fenêtre→B3, invalidations et budgets nouveaux | NON IMPLÉMENTÉE ; pas de contournement de la précondition |
| Régression hôte/ARM et qualification physique M016 | NON EXÉCUTÉES ; aucun nouveau firmware à valider/flasher |
| Publication | Rapport, preuves et journal publiés au checkpoint et relus intégralement ; décision structurante en attente |

[Cartographie champ par champ](preuves/TR2-20261013-M016/b3-field-map.csv). Le calculateur existant fournit les8RMS/crêtes en mg via sommes carrées, sqrt entier et maxima ; le service d'acquisition accumule une fenêtre sans tampon supplémentaire. Le codec respecte MSW/LSW, projection par candidate entière. Le propriétaire FIFO coopératif est distinct et non raccordé. La projection B3 n'est pas complète : statut0, alarmes/sévérité/axe dominant/dépassements/latch/compteurs initialisés à zéro ; validité limitée à trois bits. Ces zéros ne sont pas des résultats métier qualifiés. L'âge est calculé au snapshot mais le getter runtime rend l'image B3 mise en cache : actualisation de l'âge/fraîcheur encore à implémenter. Ces lacunes sont du travail M016 réalisable après définition de la voie autoritative, pas des validations déjà acquises.

Budgets **antérieurs réutilisés**, aucun WCET nouveau : ELF A04-02 inspecté par `arm-none-eabi-size` → text169468/data480/bss602216 (bss inclut réservation heap/stack), SRAM786432, réserve statique183736octets (~179,43Kio). Pas de mesure dynamique de pile. A04-02 observait FIFOmax293/512, gap5,855ms et marge comparative nominale1,445ms ; ajouter traitements ne bénéficie d'aucune garantie temporelle implicite. A04-05 garde une monotonie TS sans borne supérieure de cadence, A04-07 le boot FRAM bloquant. A04-04/05/06/07/08 et dette A04-03 demeurent ouvertes ; registre conservé, aucune clôture autoattribuée.

## Décision indispensable et suite bornée

**Décision attendue de ChatGPT/utilisateur : définir contractuellement une voie cohérente permettant une configuration métier autoritative.** Les règles actuelles ne permettent ni d'annoncer une capacité non attribuée, ni de traiter le record inactif comme une active, ni d'activer le stockage SD implicitement.

Deux voies à arbitrer, sans les appliquer silencieusement :

- Maintenir intégralement V1 et ses politiques actuelles : mission préalable de stockage réellement attribué/qualifié, avec limites de partition, budget/recovery et autorisation explicite. La FRAM actuelle n'offre pas l'autorité de campagne requise, et aucune zone SD n'est attribuée. La portée M016 sans SD devra être révisée si cette voie est retenue.
- Maintenir M016 sans stockage : décision d'architecture et de politique normative explicite définissant une configuration de supervision sans campagne durable, sa validation/recovery et son exposition. Le mode0 est aujourd'hui interdit par la V1 ; une simple modification de ce validateur ou un fallback permissif n'est pas une solution conforme. Documenter l'évolution/la dérogation avant implementation et ne pas la présenter comme V1 inchangée.

Après décision publiée, reprendre sous le même identifiant/révision explicite : (1) adapter la source FIFO à l'autorité choisie, échelle/axes et quatre taillesV1, propriétaire SPI3 unique ; (2) accumulations existantes et publication cohérente, lifecycle/invalidation/âge, politique d'alarmes selonV1 ; (3) tests A05 FIFO-01..04, DATA-01/02, CALC-01, TIME-01, budgetsMEM/IO ; (4) validation complète puis flash/debug qualifiés et campagne bornée B3/B0/B1 avec protectionB5/FRAM, si préconditions satisfaites. Aucune metrologie ni qualification produit complète présumée.

## Validations, matériel et préservation

Compilation C ciblée `cc -std=c11 -Wall -Wextra -Werror` et probe réussi, sorties/empreintes consignées ; cartographie vérifiée couvrant exactement0..47sans doublons. Erreur de contrôle documentaire initial : nombre de lignes supposé27, réalité26 ; corrigé par contrôle des48offsets couverts, sans toucher au mapping. Pas de suite126 ni cross-build A04-02 répétés sur code inchangé ; pas de modification fonctionnelle, donc ces répétitions n'apporteraient pas de preuve utile. Les126tests/cross-build et observations physiques A04-02 restent **preuves antérieures**, jamais résultats M016.

**Aucune opération matérielle M016** : ni inventaire ST-LINK, GDB, halt/reset/flash, ni COM7/RS485, ni FRAM/SD. Aucun changement firmware ou média ; état courant physique non observé ici. Dernier état documenté A04-02 : cumulatif intégré sur TR2-001, code `8bcb843...`, BIN169948 SHA256 `b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b`, cleanup/READY, debugger détaché, FRAM préservée. Le BIN local conservé a été hashé de nouveau hors cible ; cela ne prouve pas le contenu Flash actuel. Aucune restauration nécessaire puisque aucune intervention.

## Git et livraison

HEAD initial/distant871d1186b7fea7ab5ce09a44a8e03ad79fc90618 ; base contrat25406aff...ancêtre. Dépôt original `/home/lolo/dev/msm/projects/MSM_vibe` reste propre à82169af7e76797731358852bec83bfb0ceb54ccf. Worktree `/home/lolo/dev/msm/projects/MSM_vibe-M016`, branche locale `codex/m016-v1-vibration`, publication autorisée uniquement vers `refs/heads/main`. Stashes6251b988...etf3b829e6...inchangés. Seuls rapport/preuves/journal M016 modifiés, aucune règle de gouvernance, contrat, archive A04-02 ou registre des dettes modifié. Permissions exercées : lecture, compilation/probe hors ligne, documentation, commit/push normaux ; flash/debug non exercés.

Journal actualisé factuellement : refus autoritatif, portée des preuves, absence d'intervention, décision et prochaine étape. Checkpoint `b1bf54ed56d5cb165f5b7e3f485655d15dc7291a`, message `M016 checkpoint: document authoritative configuration blocker with saved-record proof`, poussé normalement sur main puis récupéré par `git fetch origin main` : les10fichiers livrés relus intégralement via objetsGit distants et comparés byte-à-byte. [Reçu de relecture](preuves/TR2-20261013-M016/checkpoint-readback.json). La présente mise à jour BLOCKED est ensuite publiée et contrôlée ; son SHA est fourni dans la réponse finale. Worktree propre au checkpoint ; seulement cette clôture documentaire de diagnostic à publier. Le commit contenant ce rapport ne peut référencer son propre SHA. Aucun statutDONE, tag ou qualification M016 revendiqué.
