---
mission_id: TR2-20261013-A04-AUDIT-FIRMWARE
status: READY
base_ref: main
base_sha: 82a151273d214b07bee033082824d4c0dc0abba6
created_at_utc: 2026-10-13T00:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# A-04 — Audit technique indépendant du firmware STM32 avant M016

## Objectif et autorisation
Inspecter le code et les preuves **réels et actuels** sur `main`, identifier les défauts démontrés, les risques significatifs et les lacunes de reproductibilité avant M016. **Mission d'audit et de rapport uniquement** : ne pas modifier le firmware, les tests, la configuration, les règles de gouvernance ni les documents opérationnels autres que `DERNIER_RAPPORT.md` et les éventuelles preuves strictement nécessaires. Aucune correction technique dans cette tranche. Les permissions commit/push sont accordées pour le rapport et ses preuves, sur `main`, selon `AGENTS.md`. Les permissions flash/debug STM32 sont explicitement accordées avec les garde-fous ci-dessous, mais **aucune intervention physique n'est requise par défaut** ; privilégier l'analyse statique et les builds/tests non intrusifs.

## Audit attendu
1. Établir le HEAD distant réel et le SHA exact du firmware M014+M015 rapporté testé (`ef468539b9943cc9b9b4d832e513a0f8ab05c7ae`), puis inventorier les profils CMake, options, macros, fichiers liés et commandes de build ARM. Identifier le **build cumulatif canonique reproductible** et les divergences entre configuration par défaut, essais M015 et configuration réellement flashée. Ne pas supposer qu'un build hôte prouve un comportement matériel.
2. Examiner l'acquisition IIS3DWB : driver officiel ST et son encapsulation, SPI3, FIFO, timestamps, synchronisation, nettoyage, limites de débit et risques de perte d'échantillons ; distinguer ce qui est observé de ce qui est inféré.
3. Cartographier les chemins synchrones et potentiellement bloquants : Modbus RTU B0/B1/B5, persistance FRAM/recovery, microSD/SDMMC, acquisition et supervision. Relever les durées mesurées, estimées ou inconnues, les budgets FIFO et latences critiques, et les interactions de concurrence ; étudier notamment la pause B5/FRAM d'environ 141 ms rapportée dans M015-V.
4. Vérifier les dépendances et invariants V1 gelés, la couverture des tests hôte et ARM, les preuves physiques existantes et les trous de coexistence. Aucun essai non exécuté ne doit être présenté comme réalisé.
5. Classer les constats : **bloquant avant M016**, **important non bloquant**, **facultatif**. Pour chaque constat : chemin/fonction/ligne ou configuration précise, preuve vérifiable, scénario de risque, proposition minimale, test de non-régression, impact sur les acquis et incertitudes.

## Sécurité et méthode
Préserver worktrees, stashes, modifications locales et état du STM32. Ne pas effacer, restaurer, reflasher ou déboguer par automatisme. Si un essai physique devient réellement utile, les autorisations `allow_flash: true` et `allow_debug: true` s'appliquent uniquement au TR2-001 identifié, avec vérification de l'image et de son empreinte, de la méthode de programmation qualifiée, des sauvegardes nécessaires et de la récupérabilité ; préserver FRAM, microSD, identité, Option Bytes et firmware cumulatif. Suspendre en cas de risque matériel non maîtrisé ou de geste physique nécessaire. Aucune opération destructive inhabituelle. Ne pas confondre statut Codex `DONE` avec qualification indépendante A-01.

## Livrable et clôture
Publier un **rapport A-04 détaillé mais ciblé** dans `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`, avec mission_id identique, statut, HEAD/commits, inventaire des vérifications, preuves exactes, constats priorisés, limites et recommandations de tranches de correction **non exécutées**. Préserver le rapport M015-V existant dans une archive immuable avant de le remplacer, sans écraser une archive déjà présente ; archiver sous `Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261013-M015-V-COOPERATIVE-CLOTURE/RAPPORT.md` si absent, ou référencer une archive existante équivalente. Codex publie et relit les livrables distants, puis fournit les SHA. ChatGPT contrôle ensuite indépendamment le rapport avant toute décision de correction. En cas d'interruption, checkpoint `IN_PROGRESS` et reprise sous le même identifiant selon A-03.
