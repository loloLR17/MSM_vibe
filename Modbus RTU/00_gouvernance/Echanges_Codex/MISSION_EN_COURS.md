---
mission_id: TR2-20261013-A05-AUDIT-TESTS
status: READY
base_ref: main
base_sha: d0238953ee0b0245f8540041fe6593939615c703
created_at_utc: 2026-10-13T00:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A-05 — Audit ciblé de la pertinence des tests et des preuves TR2

## Contexte et objectif
A-04 (`TR2-20261013-A04-AUDIT-FIRMWARE`) est `DONE` selon le rapport Codex et ses preuves. La revue ChatGPT a accepté l'audit **sans qualifier de nouveau le matériel**. Les neuf dettes A04-01 à A04-09, leurs priorités et l'ordre de traitement sont **déjà publiés** dans `Modbus RTU/00_gouvernance/DETTE_TECHNIQUE_A04.md`, l'état courant et la roadmap. **Ne pas supprimer, réinterpréter ni clôturer ces dettes** sans preuves et revue.

Mission **exclusivement d'audit et de rapport** : identifier les lacunes significatives de couverture, définir la matrice de tests minimale, reproductible et proportionnée avant les corrections A04-01, A04-02 et l'intégration A04-03/M016. Aucun changement de firmware, CMake, tests, règles de gouvernance, état courant, roadmap ou registre de dette dans cette tranche. Préserver M014/M015 et le mapping V1 gelé.

## Investigations demandées
1. **Inventaire réel** des tests hôte, ARM, intégration, hardware, manifests et preuves existantes, avec fichiers/fonctions/commandes/SHAs précis ; distinguer tests exécutés pendant A-05, résultats historiques vérifiés et assertions non démontrées.
2. **A04-01 / build sûr :** matrice CMake des profils cumulatif, défaut, qualification et combinaisons invalides ; preuve ELF/symboles, absence de chemins d'écriture SD dans les profils sûrs, empreinte du BIN de référence, refus explicite d'une sélection dangereuse. Définir critères de non-régression et limites des tests statiques.
3. **A04-02 / FRAM-Modbus :** tests de budget et latence des scans, commits, snapshots et requêtes B0/B1/B3/B5 sous acquisition ; fiabilité des projections, durabilité transactionnelle, corruption/recovery, retries, erreurs de média et état d'indisponibilité explicite. Distinguer tests mock et mesures physiques indispensables ; proposer instrumentation minimale.
4. **A04-03 / M016 :** tests du glue STM32, propriétaire unique SPI3/FIFO, quatre tailles, quatre échelles, axes, configuration/génération, timestamps (gaps, wrap, tolérances), saturation, CRC, invalidation, absence de publication partielle/périmée, budget RAM/stack/heap.
5. **Autres dettes A04-04 à A04-08 :** identifier les vérifications pertinentes sur queue UART, démarrage FRAM, SD off / SD futur, délais d'erreur HAL, stabilité et perte d'échantillons ; garder A04-09 facultatif. Séparer ce qui doit précéder M016 de ce qui relève d'une qualification ultérieure.
6. Proposer une **matrice de tests priorisée** (identifiant A04, scénario, niveau hôte/ARM/physique, méthode, critères d'acceptation mesurables, preuves attendues, priorité, coût/risque), sans transformer les tests existants en obligations redondantes.

## Autorisations, limites et livrable
Commit et push autorisés **uniquement** pour `DERNIER_RAPPORT.md`, l'archivage immuable du rapport A-04 et les preuves documentaires strictement nécessaires. `allow_flash: false`, `allow_debug: false` : **aucune intervention STM32 ni média physique en A-05** ; analyses statiques, builds et tests hôte/ARM non intrusifs autorisés. Préserver les worktrees/stashes et les preuves M015-V/A-04. Ne pas modifier les tests ou corriger des défauts pendant l'audit. Ne pas introduire CI lourde, refonte ou RTOS par défaut.

Archiver le rapport A-04 **byte-identique** dans `Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261013-A04-AUDIT-FIRMWARE/RAPPORT.md` s'il n'existe pas, avant remplacement. Publier le rapport A-05 avec le même `mission_id`, statuts V2, SHA, méthodes, matrice et limites dans `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`. Vérifier la publication et relire à distance les livrables. ChatGPT réalisera la revue indépendante avant toute mission de correction. En cas d'interruption, conserver l'identifiant et publier un checkpoint `IN_PROGRESS` selon A-03.
