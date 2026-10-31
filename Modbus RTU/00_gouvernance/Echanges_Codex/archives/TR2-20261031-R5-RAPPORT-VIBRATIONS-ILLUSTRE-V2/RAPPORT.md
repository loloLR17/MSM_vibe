---
mission_id: TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2
status: DONE
base_ref: main
base_sha: d89f11ef1afff9331d60278b6000e5505dfbcdbd
initial_head: a9077dcf41d0653d4664cd2ab1a9d94203726075
result_sha: null
created_at_utc: 2026-10-31T16:29:19.219601+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Rapport vibratoire illustré V2 — publié et relu sur main

Le PDF demandé est remplacé **au même chemin**, avec **10 pages et trois figures R5 originales incorporées**. Les critères éditoriaux, contrôles locaux et publication/relecture distante sont satisfaits.

[Ouvrir le PDF](../../02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf) · [Preuves et reproduction](preuves/TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2/README.md)

- Binaire PDF réel, **1 588 432 octets**, SHA-256 **`4e8756c5dbb4ef0bd3e0caf3d95d47b28ca0b59c1610034a50815dde11bd80c0`**.
- Couverture : plancher du PC machine, occupation permanente en navigation fournie par le contrat ; environ6h06 en temps relatif MCU/58,55MXYZ. Wm médiane1,224/P952,082/maximum2,775mm/s sur365fenêtres complètes60s. Le maximum est un RMS de fenêtre, pas une crête instantanée. Aucun «tout va bien95%du temps».
- Repères BV documentaires4,0/4,5/5,0mm/s explicitement attribués aux grades1/2/3 et à «Engine control room or switchboard room (if continuously manned at sea)», source NR467F juillet2026 ch6§3 tableau4 vérifiée. Pas de verdict BV ni d'alerte sanitaire. Mention de portée unique et sobre en couverture.
- Pages2–3 : courbes temporelles vectorielles de l'édition existante conservées, regroupement énergétique5min identifié ; tableau60/120s ajouté sans nouvelle analyse. Valeurs120s1,229/2,078/2,458mm/s issues de la synthèse publiée. RMS2–25Hz/crêtes/énergie inchangés.
- **Page4 `02_spectrogram.png` ; page5 `03_spectra_ridges.png` ; page6 `04_ridge_power.png`** : images natives1950pixels en largeur, trois pagesA4 paysage, légendes/couleurs/échelles/unités/proportions intactes et sources cliquables. Le schéma de trois fréquences est remplacé par ces figures réelles. Pas d'identification de machine inventée.
- Pages7–10 : électronique STM32U575/IIS3DWB/STEVAL-MKI208V1/microSD, FIR107Q15/décimation, méthodeWm/RMS, qualité/bases de temps et référencesST/BV/ISO/GOST préservées. Correspondance axes capteur/navire explicitement non documentée. Ancien texte sur figures absentes supprimé.

## Validation réellement exécutée

Outillage existant Python3/PyMuPDF1.26.7, aucune dépendance globale installée. Construction réussie :

```bash
python3 'Modbus RTU/02_Validation/Rapports/outils/illustrate_report_v2.py' /home/lolo/dev/msm/reports/TR2-R5-rapport-V2-20261031/build-v4
python3 'Modbus RTU/02_Validation/Rapports/outils/verify_report_v2.py' /home/lolo/dev/msm/reports/TR2-R5-rapport-V2-20261031/verification-v2
```

Les deux commandes ont terminé avec code0. Vérificateur séparé du constructeur : signaturePDF/non chiffré,10pagesA4, texte attendu, pagination, bornes de tous les blocs, aucune page vide, **rendu des dix pages** et **comparaison RGB native de chacune des trois images extraites** : PASS. Revue visuelle effective de toutes les pages, dont couverture/confort/spectrogramme/PSD/annexes : lisibilité et absence de débordement/chevauchement observées. Pixels et proportions des PNG originaux strictement identiques ; détails et SHA dans [verification.json](preuves/TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2/verification.json).

Lors de la préparation, des boîtes de texte trop petites et une ressource de police après redaction ont arrêté la construction avant livraison ; dimensions/police corrigées. Le contrôleA4 strict a ensuite révélé l'arrondi595×842 des anciennes pages : MediaBox normalisée sans changer le contenu, tolérance de contrôle conservée. La version finale passe les contrôles. Aucun nouveau test algorithmique/hôte/ARM ni calcul sur les6h : mission éditoriale, données et code scientifique inchangés, ces tests ne valideraient pas la mise en page.

## Préservation et traçabilité

HEAD initial réel main récupéré **a9077dcf41d0653d4664cd2ab1a9d94203726075**, distinct du base_sha documentaire du contrat. Worktree isolé `MSM_vibe-R5-rapport-V2`, branche locale `codex/r5-rapport-illustre-v2`, destination autorisée explicite main. Ancien dépôt/branches/worktrees et stashes6251b988/f3b829e6 préservés ; aucun reset/clean/restore/rebase/merge/forcepush.

Ancien PDF8pages : SHA`e6912dcda53f8f0a48f89a350ec9ac28e431ad5b24517f07f811da0990fef803`, conservé dans Git et copie locale `reports/TR2-R5-rapport-V2-20261031/original-main.pdf`. Deux dossiers R5/Wm comparés intégralement àa9077dcf, identiques : [source-preservation.json](preuves/TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2/source-preservation.json). Archives précédentes, mesures/CSV/PNG/JSON, bases et algorithmes ne sont pas modifiés. Le contexte d'occupation nouvellement fourni est présenté dans ce PDF sans transformer rétroactivement le manifesteWm antérieur.

Permissions exercées : édition documentaire/outillage local, commit et push normaux seulement. Aucun STM32/flash/debug/reset/SD/FRAM/COM/Windows. Journal technique actualisé avec ce livrable et la prochaine revue indépendante, sans changer les réserves historiques R3/Wm/PK. `result_sha: null` : aucun code de mesure évalué dans cette mission ; la source du document et les versions des outils sont tracées dans les preuves. Pas de qualification métrologique ou normative nouvelle.

## Publication

Premier commit publié et relu : **`de63a838ad95b0c0c90b651181a3c38f300d1ad9`**, `R5 report V2: embed original spectra and verify illustrated PDF`. Push normal `HEAD:refs/heads/main` réussi, puis `git fetch origin main`. **80 fichiers / 11,441,259 octets** relus intégralement avec `git show origin/main:<chemin>` et comparés aux fichiers locaux : tous identiques, mission et70sourcesR5/Wm comprises. BinairePDF récupéré rouvert indépendamment en mémoire :10pages/3images, SHA final exact. [Preuve de relecture](preuves/TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2/remote-readback.json).

État Git propre après ce premier push, HEAD=origin/main. Les deux stashes historiques sont inchangés. Le présent rapportDONE, la preuve de relecture et le journal final sont publiés dans un dernier commit normal, dont le SHA sera fourni après contrôle distant. Aucun SHA auto-référent. La source documentaire reste a9077dcf et les algorithmes n'ont pas été réévalués ; revue indépendante ChatGPT à effectuer.
