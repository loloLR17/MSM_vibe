---
mission_id: TR2-20261031-R5-RAPPORT-VIBRATIONS-ILLUSTRE-V2
status: READY
base_ref: main
base_sha: d89f11ef1afff9331d60278b6000e5505dfbcdbd
created_on: 2026-10-31
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — Rapport illustré V2 : intégrer les figures R5 originales dans le PDF GitHub

## Objet de la demande
L'utilisateur prépare une présentation des capacités du prototype TR2 à son Chef, au Commandant et à l'Armement. Son document de référence est déjà publié sur `main` sous :
`Modbus RTU/02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf`.

La mission est **strictement documentaire/PC**, afin de produire une **version mise à jour du même PDF directement dans le dépôt**, et d'en vérifier la publication. Ne pas toucher aux missions d'acquisition, au STM32, aux algorithmes, au matériel, aux archives de mesure ou aux bases existantes. Pas de Work. Autorisations logicielles permanentes applicables.

## Préalables et méthode
1. Lire `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, la présente mission, le rapport antérieur `archives/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/RAPPORT.md`, et les preuves R5 correspondantes. Vérifier HEAD, état Git, autres processus/worktrees, préserver tout travail local. Aucun achat ni intervention physique.
2. Relire **la version réelle** du PDF actuellement sur `main`. Le PDF existant est une édition de huit pages et contient déjà des tracés reconstruits depuis CSV ; il n'incorpore **pas** les figures fréquentielles originales. Recréer ou éditer le PDF selon l'approche locale la plus sûre, sans reprendre comme source un PDF d'une autre conversation inaccessible.
3. Respecter la démarche A-06 : amélioration éditoriale ciblée, données inchangées, pas de refonte générale ni nouveaux indicateurs inventés. Préférer des outils locaux existants (Python/ReportLab/PyMuPDF, selon disponibilité), sans installer de dépendances globales injustifiées.

## Livrable éditorial demandé (PDF de diffusion, en français)
**Première page :** lisible par non-spécialiste, présenter clairement le **lieu de mesure : plancher du PC machine, local occupé en permanence en navigation**, le contexte expérimental (prototype TR2, six heures ~6h06, 58,55M triplets XYZ), et **les repères BV documentaires 4,0 / 4,5 / 5,0 mm/s** attribués au type `engine control room or switchboard room (continuously manned at sea)` dans NR467 Part F juillet 2026 ch.6 §3 tableau 4, en affichant source et catégorie. Afficher, dans la même unité, **Wm médian 1,224 mm/s, P95 2,082 mm/s, maximum 2,775 mm/s**, calculés comme *RMS pondéré maximal des axes par fenêtre complète de 60 secondes* ; le maximum est une valeur de **fenêtre**, pas le maximum instantané / pic. Éviter la lecture trompeuse « tout va bien 95% du temps ». Première page = présentation de capacité, pas classement BV de la pièce.
**Corps de rapport :** conserver Wm temporel (axes X/Y/Z + max si pertinent), résultats 60/120s, méthode de calcul et une section technique 2–25 Hz (RMS, pics, énergie).
**Analyse fréquentielle : intégrer physiquement les figures originales publiées par Codex**, en conservant couleurs, échelles, unités, légendes et proportions, avec attribution de source. En priorité :
- `Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/02_spectrogram.png` (**obligatoire**, vrai spectrogramme 2–25Hz)
- même dossier : `03_spectra_ridges.png` (**obligatoire**, spectres PSD de périodes faible/médiane/forte et fréquences candidates)
- `04_ridge_power.png` (**fortement recommandé**, puissances des bandes candidates en fonction du temps, si lisible)
- `01_intensity.png` (optionnel, si apporte plus que les graphiques RMS).
La figure `wm_trends.png` et `wm_response.png` du dossier `preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/` peuvent être reprises si la qualité de présentation en bénéficie. Ne **pas** inventer de spectrogramme, reconstituer son intensité depuis trois seules fréquences, masquer la source, interpoler sans provenance ni utiliser une image factice. Les PNG d'origine sont présents sur GitHub dans le même dépôt ; le local Codex dispose normalement du clone et des images.
**Annexes :** préserver origine électronique STM32U575 / IIS3DWB / STEVAL-MKI208V1 / microSD, traitement FIR/decimation/filtrage Wm/RMS, erreurs et limites R3/R5, bases de temps, qualité de l'archive, provenance des tests et références ST/BV/ISO/GOST. Une mention unique et sobre de la portée indicative suffit ; **ne pas insister sur la non-conformité normative** ni prétendre le PC machine conforme/non conforme.
**Mise en forme :** première page synthétique, titres hiérarchisés, figures lisibles sur A4, police/dimensions correctes, pas d'éléments qui débordent ou se chevauchent, pagination vérifiée. Ajouter une ou deux pages plutôt que réduire les graphiques au point de les rendre illisibles. Préserver citations et liens vers les dossiers de preuves. Les événements de navigation et l'identité de machines ne sont pas inventés (temps relatif/UTC inconnu).

## Sources factuelles à préserver
- `preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/` : `README.md`, `SYNTHESE_POC.md`, `summary.json`, `windows_60s.csv`, `01...04_*.png`. Bande filtrée 2–25 Hz : RMS médian 13,361 mg, P95 25,643 mg, max minute 34,405 mg ; trois fréquences candidates 19,57/20,63/24,37 Hz **estimées**, pas identification d'une machine.
- `preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/` : `README.md`, `NOTE_RESULTATS.md`, `metrics.csv`, `summary.json`, `wm_trends.png`, `wm_response.png`, `BV_COMPARABILITY.json`. 365 fenêtres FULL de 60s : Wm mm/s² médiane 43,728, P95 74,344, max 99,102. Conversion liée `v_w=0.028 a_w` : 1,224 / 2,082 / 2,775 mm/s. Pour 120s médiane 1,229 / P95 2,078 / maximum 2,458 mm/s. À titre de cohérence avec le PDF précédent, ne pas modifier les séries sans revalidation.
- BV NR467 Part F juillet 2026 : https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf , chapitre 6, section 3, tableau 4. Vérifier que les trois repères et intitulé exact de local sont fidèles à la source et préciser s'il s'agit de valeurs par grade/documentation (et non de niveaux d'alerte sanitaire).
- Délimitation explicite : la voie RMS Wm n'est pas la voie des composantes PK (tableaux BV 5/6, blocage historique inchangé).

## Critères d'acceptation et publication
1. PDF final placé **au même chemin GitHub** `Modbus RTU/02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf`, fichier binaire PDF réel, non une chaîne texte ou lien HTML ; commit/push normal sur `main`.
2. Vérifier réellement que les **PNG spectrogramme et PSD originaux** sont incorporés comme images lisibles dans les pages du PDF (et non de simples hyperliens), au besoin vérifier par extraction d'images ou inspection des pages rendues.
3. Première page contient PC machine + occupation permanente, 4,0/4,5/5,0 mm/s BV, médiane/P95/**max 2,775 mm/s** et précision fenêtres Wm 60s. Pas de verdict « conforme ».
4. Contrôle PDF indépendant du texte : `pdfinfo` / PyMuPDF, rendu des pages PNG, revue graphique des pages 1, confort, spectro, PSD et annexes, aucune page vide / débordement, références et unités consistantes.
5. Conservation des sources et des métriques. Pas de changement de calcul ni de base de données. PDF optimisé raisonnablement, facile à télécharger (GitHub direct `raw` / visualiseur).
6. `DERNIER_RAPPORT.md` publié par Codex seul, avec présent `mission_id`, SHA de PDF, nombre de pages, nombre d'images originelles effectivement embarquées, liste des figures, tests de rendu et commit confirmé distant. `DONE` seulement si les critères sont effectivement remplis ; `BLOCKED` sinon avec cause concrète. Pas de fausse qualification. ChatGPT effectuera ensuite sa revue indépendante.

Permissions : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Pas d'action STM32, debug, flash, microSD ou Windows.
