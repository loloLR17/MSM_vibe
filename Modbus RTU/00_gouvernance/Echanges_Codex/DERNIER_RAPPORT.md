---
mission_id: TR2-20261031-R5-BV-PK-ALTERNATIF-6H
status: IN_PROGRESS
base_ref: main
base_sha: 95ef8c8bc03b35e0ad9d7ad889ca370070292de3
initial_head: e0887972f12fd12e520e2947d3f7e85716be0265
result_sha: null
created_at_utc: 2026-10-31T10:40:52.188407+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5-PK — référence vérifiée et extraction testée, calcul réel suivant

BVNR467F juillet2026 téléchargé etvisualisé localement : CF1, composantecrête,
sec3table5 vitesse5–100Hz4/4.5/5mm/s, table6accélération1–5Hz125/141/157mm/s².
PDFSHA5ae75d…050ceef, index0based238/246/247 (imprimés237/245/246).
PasdePDFcomplet/pagesprotégées publiés. ABS/LR consultés : peak-hold≠MRA des
signauxmodulés ; leursfacteurs ne remplacentpas leCF1deBV. Montage/axes/local
nonqualifiés ; aucuneattributiondegrade. Le blocageWm précédent reste distinct.

Algorithme candidat : Hanncorrigé/DFTinterpolée, vraies composantes paraxe/bande,
fenêtres60set6×10s, contrastes/formes/raiesproches/modulations/bords explicites.
RecoupementtemporelLS indépendant préparé ; aucuneconversion duRMSbroadband.
**6tests ciblésPASS**, oracles analytiques/tolérancespréfixées, sinus1/2/4.9/5/
5.1/20/80/99/100/101Hz/on-offbins, AM/FM/proches/multiton/impulsion/bruit/DC/
qualité/fenêtres/CF/ unités. MoteurPC préparé ; sixheures, recoupementsréels,
SQLite/pageàexécuter. Pasde résultatphysique oudecomparaisonBV proclamé ici.

Reprise depuisorigin/maine0887972 dansMSM_vibe-R5-BV-PK isolé ; anciennesmissions,
worktrees/catalogues/stashes6251b988/f3b829e6 conservés. AucunSTM32/SD/COM/.NET/
Windowssecurity. Journalactualisé. Prochaines tranches : calculintégral, contrôle
indépendant3plages, cohorteSQLversionnéepréservantv2, affichageconditionnel puis
rapportetpublicationvérifiée. Pasdevalidationintermédiaire demandéepourlogiciel.
