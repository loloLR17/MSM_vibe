# MSM TR2 — Revue indépendante A-01 — R5 confort non pondéré, P0 ISO indisponible

**Date :** 2026-10-31. **Auteur :** ChatGPT. **Verdict : ACCEPTATION PARTIELLE des résultats scientifiques préliminaires / BLOCKED confirmé sur l'indicateur BV/ISO pondéré demandé.** Cette revue ne change pas le statut `BLOCKED` du rapport final Codex, ne fournit aucun GO industriel et ne qualifie ni les instruments, ni le confort humain, ni un grade BV.

## Référentiel vérifié en direct

- Dépôt `loloLR17/MSM_vibe` branche `main`, gouvernance `AGENTS.md` et `GOUVERNANCE_CHATGPT_CODEX.md`, mission `TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF`, `DERNIER_RAPPORT.md` final `BLOCKED`, et fiche `experiments/r5_comfort_analysis/P0_METHOD.md`.
- Commits Codex : `45e70a789feb70ed3ed753c13f542252af3bf26e` (code préliminaire), `c2c1437b080a44589836fff37b30c706adc4c1cc` (résultats réels et SQLite), `e8789f21901825dbe8ae31ce820be52d09e8c021` (rapport final BLOCKED). Comparaison GitHub depuis `7575a051` : **3 commits**, nouveaux code/tests/analyse/artefacts, rapport et état ; pas de modification du firmware ou des modules .NET de supervision.
- [Résultats](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/README.md), [synthèse POC](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/SYNTHESE_POC.md), [CSV de 2204 fenêtres](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/metrics.csv), [preuves SQLite](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/sqlite-proof.json), [tests](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/tests-final.log), [contrôles croisés](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/real_crosschecks.json), [méthode](../../../experiments/r5_comfort_analysis/README.md).

## Contrôle indépendant des chiffres publiés

- Entrée déclarée inchangée : `E6N0005.TR2`, SHA256 entier `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`, format v3/58 552 136 triplets XYZ validés selon lecteur strict déjà examiné en R3/R5. **ChatGPT n'a pas directement relu les 640 MiB bruts**, ni la base du T1000 ; cette assurance reste fondée sur les preuves et traces Codex.
- Depuis `metrics.csv` publié, **recalcul séparé par ChatGPT** des 365 fenêtres MCU de 60 secondes `FULL` pour la grandeur `mm/s`, maximum des RMS directionnels XYZ capteur : **médiane 1,4244190403211543 mm/s ; P95 2,1965901527880893 mm/s ; maximum 2,990163279255228 mm/s** ; concordance avec `summary.json` publiée. Cela ne valide pas indépendamment la chaîne brute→métrique mais prouve la cohérence statistique des résultats publiés.
- `tests-final.log` : **11 tests Python déclarés PASS** ; `real_crosschecks.json` : contre-calculs réels FFT/filtre temporel, deux scénarios d'horloge et unités mg/mm/s, avec critères explicites, `passed=true` ; pas d'oracle physique calibré ni de contrôle Wm.
- SQLite secondaire `comfort-secondary-v2.sqlite` déclaré sain `PRAGMA integrity_check=ok` et sans violation FK, 1 campagne, 4 définitions, 4 analyses, **2204 métriques** (= 4 analyses × (367 fenêtres 60s + 184 fenêtres 120s)), 24 références ; identité/comptes identiques lors du second import idempotent. Catalogue R5 v1 précédent SHA `e436de69e55e2126c029acc5afea9cb058b0a6fc6ce857e41bc70a26693646c5` inchangé selon preuve. La base réelle locale n'a **pas** été ouverte directement par ChatGPT.
- La mesure publiée est une **vitesse physique approximativement intégrée après filtre Butterworth 1–80 Hz, sans pondération Wm**, avec amplitude nominale, orientation axes navire inconnue, fixation double-face provisoire, chronologie relative MCU `UTC_UNKNOWN` et écart de cadence estimée MCU/QPC ~0,68 %. Les agrégats 1–80Hz non pondérés ne peuvent pas être comparés directement aux valeurs RMS pondérées ISO/BV, ni aux critères **crêtes spectrales** BV-PK.

## Décision, réserves et suite

**A-01 partielle :** résultats non pondérés, tests, méthode et persistance R5 confort considérés aptes à alimenter la prochaine tranche PC **sans réexécuter l'ancienne mission**. **P0 normatif reste `BLOCKED`** : ISO20283-5 §7/8.1, équivalence normative, Wm ISO2631-2/ISO8041 non établies dans les aperçus légaux. Les publications restent `EXPLORATOIRE_NON_NORMATIF`. Le SHA producteur RAM perdu en R3 n'est pas réhabilité.

**Décision utilisateur 31/10 :** prochaine tranche dédiée à la **voie alternative BV 1PK/2PK/3PK**, réellement décrite dans la NR467 Partie F chapitre6 §2.1.3 et section3 tableaux5/6, avec CF=1/critères de **crête d'une composante individuelle** et non RMS global ; ISO6954:1984 sert de référence historique. L'utilisateur examine de son côté l'achat licite des normes ISO de confort pondéré, qui demeure un chantier séparé.

**Vérification importante (source BV juillet2026) :** pour `Engine control room or switchboard room (if continuously manned at sea)`, section3 tableau5 **5–100Hz crête vitesse : 4,0 / 4,5 / 5,0 mm/s** ; tableau6 **1–5Hz crête accélération : 125 / 141 / 157 mm/s²** (grades 1PK/2PK/3PK). Des références antérieures de 125 / 157 / 188 mm/s² étaient **inexactes pour ce local de section3** et correspondent à d'autres catégories/sections ; ne pas les reprendre.

## Limites de la revue

Relecture indépendante des textes/CSV/JSON/code/diffs et vérification distante GitHub seulement ; pas de second retraitement intégral depuis la source brute, pas de vérification matérielle, pas de validation visuelle exhaustive de la figure PNG, pas de revendication ISO/BV. Le statut partiel ne doit pas être résumé « R5 confort ISO DONE ».
