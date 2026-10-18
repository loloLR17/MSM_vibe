---
mission_id: TR2-20261018-UXL2-LIVE-TRENDS-NAVIGATION
status: READY
base_ref: main
base_sha: 8ee37a7ae1d4da30e922ca6f723aa11206ca9488
created_at_utc: 2026-10-18T18:40:56.313Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# UX-L2 — Courbes vibratoires LIVE et navigation opérateur

## Point de départ et but

UX-L1 `TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION` est **DONE**, rapport et mission conservés dans `archives/TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION/`, code de référence `8722c02e805e785dacacf0f2602f94a102b94119`. Vues `/live-fleet.html` et `/live-device.html`, API additive `/api/v2/live/overview`, source SQLite LIVE bornée, gestion honnête des observations historiques/HELD/pertes/identité. Preuves : 217 tests Service finaux, 350 autres, JS/HTTP et deux DOM headless. Ne pas retester systématiquement le firmware ou les campagnes matérielles.

**But UX-L2 :** enrichir ce parcours opérateur avec **graphiques temporels issus exclusivement des observations persistées** et **outils de consultation du parc** que l'utilisateur a validés. Conserver l'architecture S7/R5, la distinction LIVE/V1, et les engagements du document `Modbus RTU/00_gouvernance/CONCEPTION_SUPERVISION_LIVE_UX_20261018.md`. Procéder incrémentalement selon A-06, sans refonte ou framework supplémentaire sans bénéfice établi.

## Travaux autonomes autorisés

1. Consulter `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole d'échange, contrat UX, état courant/roadmap, S6-H et S7, UX-L1 (code et rapport) ainsi que les preuves SQLite R5-B2 et le modèle réel `live_observation`. Vérifier `main` et les capacités réelles de lecture historique/filtrage, la rétention et la couverture. Garder worktrees/stashes et archives protégés.
2. **Fiche Synthèse LIVE :** proposer une petite courbe de tendance récente (RMS global et crête globale), à partir des points réels avec horodatage de **réception PC** explicite. Afficher les lacunes sans interpolation implicite, les changements de segment et la différence entre valeurs courantes/HELD/périmées. Si la source ne permet pas une courbe significative, indiquer la limite et conserver l'état structuré UX-L1.
3. **Fiche Vibrations LIVE :** courbes RMS global/crête et X/Y/Z **superposés**, visibilité X/Y/Z indépendante, sélecteur RMS ou crête, légende et unités mg, axes temporels. Proposer périodes 1h/24h/7j/plage personnalisée uniquement avec données effectivement disponibles : couverture tronquée signalée, limite de lecture documentée et pagination/bornage raisonnables ; ne pas annoncer une rétention infinie ni émettre de mesures fictives. Séparer **état valide/frais de l'échantillon B3 au moment observé** et caractère courant aujourd'hui ; historique et replay restent explicitement non courants.
4. **Parc LIVE :** une ligne par capteur, conserver RMS/crête/sévérité/communication/fraîcheur et dernière valeur connue qualifiée. Ajouter les quatre outils approuvés, selon données réellement autoritatives : `Attention uniquement`, filtre équipement/local **uniquement si l'affectation existe ; sinon un filtre de bus/endpoint honnête et un contrôle inactif correctement expliqué ou l'absence du contrôle**, recherche sur identité/endpoint et nom réellement renseigné, tri de colonnes (RMS, crête, sévérité, ancienneté, nom). Les outils ne doivent pas inventer une affectation machine/local ou transformer une archive historique en communication actuelle.
5. **Export CSV** borné et explicite des observations réellement consultées/filtrées, avec valeurs, mg, timestamps et provenance, statut et qualité, absence d'UTC signalée, identification des HELD/pertes sans zéro ni reconstruction ; aucune extraction sans bornes d'une base volumineuse. Garder cohérence de sélection/tri/filtres et export. Si cela élargit disproportionnellement L2, traiter comme option priorisable après graphiques et décrire le reliquat.
6. Utiliser de préférence les DTO, projections et API existants. Si nécessaire, endpoint(s) additif(s) lecture seule avec lecture SQLite paramétrée bornée, sans migration destructive, sans cache métier concurrent, sans second polling ni dépendance Modbus depuis le navigateur. Protéger performances/temps de réponse pour plusieurs dizaines de capteurs et période consultée bornée. Ne pas modifier `/api/v1/fleet` ni les commandes B5, ni cacher la séparation du diagnostic `/live.html` et du profil complet. Respecter la source d'identité durable ; au changement/perte d'identité ne pas attribuer les mesures de l'ancien capteur au nouveau.
7. **États à éprouver :** snapshot null (communication perdue), dernière connue non courante et datée, B3 V/F, HELD, séquences répétées, UTC fenêtre nulle, run/epoch/config absents du fil, archives historiques R5-B2, erreurs de base/API, début/fin couverture, périodes vides et basculement de capteur. Conserver DOM HTML sûr, gestion des échecs HTTP et commandes Web readonly. Séparer authentique événement d'alarme et drapeau B3 à la réception PC.
8. **Hors périmètre :** seuils B4 et marqueurs d'événements avancés UX-L3 (sauf simple visibilité B3 déjà autoritative), FFT, mm/s, score santé, calibration, modification Modbus V1/LIVE, firmware/flash/debug, COM4/STM32, B5 writers, microSD/SD, auth/LAN/Windows service déploiement, qualification R6 ou essai physique.

## Critères de sortie

- Navigation/graphes fonctionnels depuis le **vrai contenu archivé** (copie figée et sûre R5-B2), l'API et DOM headless/navigateur selon accès. Mettre en évidence un graphique réel multi-points; aucun exemple fictif présenté comme preuve d'acquisition.
- Graphiques et listes n'effacent pas les ruptures de mesure; provenance `historical`, UTC absent, qualité, identités et périmé visibles. Un historique n'est jamais qualifié de courant. Filtres/tri robustes, pas de bouton fictif, CSV conforme si livré.
- Tests .NET, JS et HTTP ciblés, régression de la partie S7/V1 touchée ; chiffres exacts des tests réellement exécutés. Pas de nouveau flash ni d'accès matériel.
- Documentation dans `Supervision TR2/` décrivant les pages, mode de lancement, limites de couverture et reste UX-L3, preuves traçables, état factuel/roadmap mis à jour dans le périmètre. Ne pas modifier les règles de gouvernance ni les archives.
- Commit et push normaux sur `main`, rapport `DERNIER_RAPPORT.md` avec statut honnête, SHA et vérification de publication distante. Contrôle indépendant ChatGPT ultérieur distinct.

## Autorisations et conditions d'arrêt

`allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Autonomie de développement PC/Web/tests accordée. Arrêter/escalader seulement risque majeur de performance ou d'intégrité de données, décision structurante, conflit de gouvernance ou dépendance matériel/physique imprévue. Ne pas demander à l'utilisateur des validations répétitives de détail ergonomique.
