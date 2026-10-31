# Revue indépendante A-01 — R5 préqualification de l'énergie et du FS
Date : 2026-10-31. Mission : `TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE`.
**Verdict : ACCEPTATION TECHNIQUE BORNÉE des méthodes exploratoires numériques et de leur publication ; aucun KPI Wm, aucun verdict de confort et aucune qualification métrologique ou matérielle.**

## Références distantes vérifiées par ChatGPT
- Contrat et `DERNIER_RAPPORT.md` de même `mission_id`, ce dernier `DONE` sur `main` au commit final [4192f4cb](https://github.com/loloLR17/MSM_vibe/commit/4192f4cbe0436a30cfe57a0033bbd869612931d5).
- Checkpoint logiciel [729416af](https://github.com/loloLR17/MSM_vibe/commit/729416af1a01772783026c3961e79371b35cb031), publication des résultats [e211909b](https://github.com/loloLR17/MSM_vibe/commit/e211909bb552ce10c358bf7049e5fed292980cd9), clôture 4192f4cb. Comparaison GitHub des commits contrôlée.
- Preuves publiées : `preuves/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/README.md`, `NOTE_DECISION.md`, `ASSESSMENT.json`, `tests-final-ten.log`, `remote-readback.json`, `summary.json` et `experiments/r5_prequal_offline/METHOD.md`. Le journal de tests montre 10 cas OK. Le manifeste de relecture distant **déclare** 51 fichiers et 4 292 315 octets comparés ; ChatGPT a vérifié la présence distante de ces preuves, sans reproduire localement cette comparaison exhaustive.
- Six heures et 58 552 136 triplets XYZ de `E6N0005.TR2`, source SHA256 annoncé `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5` ; **ChatGPT n'a pas accès à l'archive locale pour refaire lui-même le SHA/CRC**. Les 144 contre-calculs temporels/spectraux réels PASS et la v4 SQLite préservatrice sont attestés par les artefacts Codex, non rejoués directement par ChatGPT.

## Conclusion et limites maintenues
Les tests, la reconstruction énergétique 60/120 s, les deux hypothèses de cadence et la conservation de l'historique justifient l'acceptation du **socle numérique expérimental** comme base de développement. Le filtre LP10 est un test non perceptif ; aucune fonction Wm ni seuil d'acceptabilité n'a été implémenté par cette mission. R3 demeure `CLOSED_WITH_RESERVATIONS` (digest RAM perdu) ; horloge SI, pose, axes, réponse physique 1 Hz, sensibilité et bruit non qualifiés. La sortie filtrée Z atteint +7,676728 g nominal sur une courte période ; `±8 g` demeure le choix prudent et la cause de cet épisode est indéterminée. Le statut PK-BV antérieur `BLOCKED` est inchangé.

La nouvelle décision utilisateur autorise **un indicateur TR2-Wm indicatif** inspiré d'une pondération Wm publiée, pour comparer des campagnes et des seuils de référence, **sans revendiquer la conformité ISO/BV**. Les définitions normatives publiquement accessibles sont une référence de développement, pas une certification de la chaîne. Prochaine mission PC indépendante, sans STM32 ; aucune qualification de nouvelle capture ni de seuil de confort par la présente revue.

**Tag matériel Mnnn non créé** : il n'existe dans cette revue ni nouvelle image STM32 qualifiée ni preuve métrologique complète justifiant un jalon matériel. Aucun changement des rapports Codex historiques.
