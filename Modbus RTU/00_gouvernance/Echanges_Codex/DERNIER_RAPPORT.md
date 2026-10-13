---
mission_id: TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION
status: IN_PROGRESS
base_ref: main
base_sha: 9dd90081ec6e51ec5c6fa84bd5b66f8369717ceb
initial_head: df5050f
result_sha: null
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M017 — tranche préparatoire publiée, exécution à poursuivre immédiatement

Inspection du dépôt distant et préservation du dépôt principal/worktrees/stashes. Comparaison C2c/M016 : polling vs DMA, pas de producteur C2c, métriques ms vs DWT, READY plus strict dans M016 ; identité du média C2c non établie par son rapport de débit. Aucune conclusion de défaut matériel ou d'insuffisance intrinsèque du bus.

Diagnostic physique ciblé seconde SN0032001F3235510C37333439 / UID001B0046/38425018/20333157 : carte CID03534453/44333247/858BF092/3C01927A, 62333952 secteurs, 1bit/10MHz ; seize writes/relectures exactes (huit DMA puis huit polling), sans producteur, HALerror0. Les deux transports restent proches de190ms/chunk ; délai final READY1–3ms, majorité dans phase transfert. Pause maximale955ms conservée. Détail brut dans `preuves/TR2-20261014-M017-SDMMC-1BIT/diagnostic-final.log`, image originale et diff de diagnostic conservés.

Erreur d'outillage initiale conservée : GDB Windows ne dispose pas de shell sleep et sa commande interrupt asynchrone n'a pas arrêté comme attendu ; aucune charge déclenchée. Préflight repris par la méthode qualifiée détachée. Breakpoint inline de diagnostic revenu avant production, retiré lors du detach ; seize transferts mesurés ensuite en fonctionnement libre, capture après fin. Le RUN de qualification utilisera un symbole terminal noinline et aucun halt intermédiaire.

Harness instrumenté (durée données/READY/coût producteur) cross-build réussi. Ring128KiB, plus plausible que512KiB face à _ebss A04-02=0x2009283c/768KiB ; buffers et métriques de démonstrateur ne sont pas une intégration de production. HAL/CMSIS et production inchangés, pas de test ancien rejoué. [Protocole](../../../experiments/sdmmc_st_reference/M017.md), [runner](../../../experiments/sdmmc_st_reference/run_m017.py).

**Ce checkpoint n'est ni un rapport final ni une qualification.** Code/protocole publiés avant RUN, puis flash standard seconde cible, vérification intégrale, INIT et RUN600s à effectuer immédiatement. Trois runs complets identiques seulement si le premier passe ; sinon arrêt des répétitions, analyse et au plus un correctif prouvé. Principal/FRAM/Modbus/média principal non accédés. Carte de test laissée en firmware SD sûr, aucune restauration GPIO.
