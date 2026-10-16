---
mission_id: TR2-20261016-R4S-SERVICE-BUDGET-SAFE-RECOVERY
status: IN_PROGRESS
base_ref: main
base_sha: a57cc5bcc84e593a4a99f4100600346b6d990eb4
initial_head: 20c8089514b40a75ed59d7c6616b75838482e0c8
result_sha: 452ab6cc3a5b871b19648448740a9fb08ce12f9b
created_at_utc: 2026-10-16T21:32:09.743860+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-S — checkpoint phase 1, poursuite autonome en cours

La première tentative volatile f52a5a8 a réellement exécuté FIFO + métrologie +
RTU loopback + FRAM readonly, sans admission R2 ni écriture FRAM. Préflight UID/SN,
Flash M016 complète et FRAM14fb conformes. Archive R4-R déjà présente vérifiée exacte,
aucune duplication/réécriture ; worktree isolé, ancien main/stashes conservés.

Résultats : 31787XYZ/3974TS en1,206563s, work335us/service2322us/gap2457us,
FIFOmax73/0erreur. Une fenêtre numérique diagnostique N4096 contrôlée par Python :
RMS global/X/Y/Z960/953/102/40mg, crêtes997/989/135/79mg. Aucune mesure autoritative
R2 prétendue : autoritéfalse et0V/F RTU,96réponses cohérentes/CRC contrôlés.
Maximum apparié2318us : poll1803 dont consommation663, RTU513, publication0.

Arrêt sûr sur autre défaut : conversion temporelle affine fixe dérive jusqu'à
109us >100us après12contrôles. Seuil inchangé, échec conservé. La FRAM entière
est restée exacte14fb, étatR2 indéterminé epoch4/tx5/run5 inchangé. Carte actuellement
enprobe phase1 terminal arrêté, GDBdétaché, M016 sauvegardée récupérable ; restauration
finale pas encore faite car qualification volatile en cours.

Correction préparée : suivi des deux horloges depuis brackets réellement observées,
pivot continu au dernierTSFIFO et aucune redatation de valeur publiée. Le même
contrôle100us est appliqué avant toute correction ; aucune grille nominale ou UTC.
141tests complets60,50s/ARM, modèle10s+80ppm/refus de l'ancien affine et garde100us,
ASan/UBSan-O2 fournisseur et448oracles métrologie PASS. Nouvelle instrumentation
sépare datation/métrologie et garde des traces appariées. Code du checkpoint logiciel
ci-dessus, nouvelle image pas encore essayée ; V1 et B5/R2 inchangés.

Prochaine action autonome : nouveau probe volatil instrumenté, dix secondes sous
charge et vérification intégrale FRAM ; seulement après PASS, étude d'applicabilité
du retrait contrôlé R4-M/R4-P au snapshot authentique14fb. Pas encore phase2/3,
aucune isolation/RENEW/START R2. Conserver le run5 consommé et toutes les preuves.

Détails : [LIVE_R4S](../../05_Firmware/LIVE_R4S.md), [preuves](preuves/TR2-20261016-R4S/README.md).
Journal mis à jour. Publication/relecture à vérifier après push ; statutIN_PROGRESS
signifie poursuite physique, pas arrêt sur checkpoint ni demande de validation.
