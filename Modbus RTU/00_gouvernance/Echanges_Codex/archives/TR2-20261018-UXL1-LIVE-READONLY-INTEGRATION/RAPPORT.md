---
mission_id: TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION
status: DONE
base_ref: main
base_sha: 9c2b65400cbe11564875a94864ab22a896cd3733
initial_head: cfabfbf291c83e20878f90181b7d18925f079ddb
result_sha: 8722c02e805e785dacacf0f2602f94a102b94119
created_at_utc: 2026-10-18T18:12:01.613300+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# UX-L1 — parc et fiche LIVE raccordés aux observations persistées

**DONE : tranche logicielle réalisée, bilan et preuves publiés puis relus intégralement depuis GitHub.** Aucun blocage technique. Le parc LIVE et une fiche Synthèse/Vibrations readonly sont intégrés au host S7, avec distinction explicite entre historique et réception physique actuelle. Pas d'acquisition nouvelle, COM4 non ouvert, aucun flash/debug/commande STM32.

Code évalué **`8722c02e805e785dacacf0f2602f94a102b94119`**, `fix(uxl1): retain open detail on identity loss without promoting its previous owner` ; socle initial `3b4b103d6febe51b14ad6f989d4b9d81ee9bfc74`, `feat(uxl1): integrate readonly LIVE fleet and sensor views with persisted provenance`.Changements limités aux sources PC/Web et tests ; aucune modification du firmware, gouvernance, norme ou archives R5-B2.

## Livré

- Section Parc LIVE dans `/`, page `/live-fleet.html`, fiche `/live-device.html?deviceId=1` à six onglets, données structurées Synthèse/Vibrations et limites honnêtes dans les autres vues.
- API additive `/api/v2/live/overview`, projection sans cache autonome et lecteur SQLite borné aux1000 observations récentes ; ni migration ni rétention changées. Identité durable connue dédupliquée, endpoints inconnus explicites, aucune affectation équipement/local inventée.
- RMS/crête globales et X/Y/Z en mg, sévérité B3 brute/interprétée, communication et qualité distinctes, réception PC et ancienneté, état acquisition déclaré dans l'observation. HELD, repeated sequence, perte/snapshotnull, `IsCurrent:false`, `ArchiveState` et erreur d'archive conservés. Dernière mesure retenue étiquetée périmée, avec son propriétaire et sa propre date ; jamais promue en identité ou mesure actuelle.
- Nouveau lancement du **même service** `--live-archive <copie-figee.db> --listen http://127.0.0.1:5093`, sans transport/polling Modbus, writer ou replay. Historique/replay toujours non courant dans la nouvelle projection. Compositions LIVE existantes raccordées additivement, sans nouveau maître ; elles n'ont pas été exécutées sur matériel ici.
- `/live.html` et `/api/v2/live` conservés, pages/API V1 et commandes S7 préservées. Les nouvelles fiches n'exposent aucune action B5/POST. `LiveDiagnostic` marqué NON-PRODUCTION ; B1/B2/B7, UTC fenêtre et identités run/epoch/config non exposés restent inconnus.

[Note d'implémentation et lancement](../../Supervision%20TR2/LIVE_UXL1.md), [preuves](preuves/TR2-20261018-UXL1/README.md). Les graphiques/tendances et filtres avancés restent UX-L2 : l'historique existe mais L1 privilégie les données structurées et la qualité, sans courbe interpolée ou bouton factice. Aucun accès distant, auth, export, seuil, score, FFT ou mm/s ajouté ; UX-L3/R6 distincts.

## Validations réellement exécutées

Build .NET réussi sans avertissement/erreur. Première suite complète : **350 tests hors Service PASS**, **214 Service PASS et1 échec nouveau**. SQLite ReadOnly ordinaire créait du WAL/SHM sur la copie : assertion conservée, chemin d'archive figée corrigé avec URIimmutable, garde refusant un WAL non vide ; aucune relaxation du test ni masquage. Ensuite215 Service PASS ; après ajout de deux scénarios de garde/HTTP, **217 Service PASS/0skip**. Les anciens tests V1/S7/B5 sont inclus. Ne pas présenter cette séquence comme une nouvelle suite complète de567 tests exécutée après la dernière modification.

Les six nouveaux tests couvrent R5-B2 réel relu et étiqueté historique, V/F, HELD, perte et identité inconnue, changement d'identité sans fuite d'ancienne mesure, replay non courant, déduplication, erreurs, conservation des octets, refus WAL et API/pages/six onglets/absence de commandes. Tests JS ciblés sur captures R5-B2 réelles, XYZ, sévérité, UTC absente, perte/retenue/absence sans zéro, absence d'écriture/cache/innerHTML. Build final des assets et smoke HTTP avec le service Linux réel :200 sur/, parc, fiche, diagnostic, nouvelle API etAPI V1 ; POST commandes404.

**Deux DOM Edge headless réellement rendus**, parc et onglet Vibrations, depuis une copie de l'archive physique R5-B2. Historique/LiveDiagnostic/NON-PRODUCTION/mg visibles ; HELD et dernière valeur périmée affichés, UTC absente.528 observations historiques dans la copie, base byte-identique SHA6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1, aucun WAL créé. Aucun navigateur humain, screenshot ou mesure physique nouvelle revendiqué.

Le mode d'archive figée nécessite une copie stable, checkpointée et sans writer ; il refuse un WAL non vide. Les lectures des bases vivantes utilisent le ReadOnly normal, sans immutable. La limite1000 est explicitement affichée : pas d'inventaire exhaustif, une valeur hors couverture n'est pas inventée. Voir la note pour les limites de rafraîchissement et les URI SQLite documentées.

## Préservation, état et publication

Autorisations exercées : logiciel, compilation/tests, commit/push ; flash/debug interdits et non utilisés. Aucun accès au STM32, FRAM, Flash ou COM4, pas de campagne/build firmware répété. État matériel R5-B2 hérité, **non relu** dans cette mission. Journal et roadmap actualisés uniquement pour cette capacité logicielle et ses limites ; gouvernance UX/S7 et archives immuables préservées.

Dépôt historique main82169af propre, autres worktrees et deux stashes préservés. Worktree `MSM_vibe-UXL1-live-readonly`, branche `codex/uxl1-live-readonly`. Les sources courantes PC explicitement modifiées dans le périmètre sont distinguées des archives/proofs protégés. Aucun reset/clean/merge/rebase/force push.

**Publication vérifiée :** bilan `50d1ca30049d5fcc3a6d9e3dcda53f58ccbd1858` récupéré de GitHub à2026-10-18T18:18:09.573733+00:00,96 fichiers comparés intégralement,78 empreintes/76 artefacts vérifiés. TRX final217 Service PASS,350 autres tests déjà réussis, deux DOM de la version finale et copie SQLite inchangée contrôlés. [Preuve de relecture](preuves/TR2-20261018-UXL1/readback-50d1ca3.json). Livraison DONE, contrôle indépendant ChatGPT distinct. La mise à jour DONE et ses archives sont ensuite poussées normalement et relues ; le SHA final sera fourni dans la réponse Codex.

Préservation vérifiée :8295 fichiers protégés,67 autres worktrees, deux stashes et dépôt historique main82169af inchangés. Le serveur de test a été arrêté et le port HTTP libéré après chaque smoke. Aucun port COM ou accès matériel exécuté. Aucun nouvel essai physique ou test firmware pendant clôture.

Dernière correction de sélection : une fiche ouverte par identifiant peut suivre sa dernière mesure lors d'une perte d'identité, sans promouvoir son ancien propriétaire en identité actuelle. Si une observation identifiée existe ailleurs pour ce device_id, elle reste prioritaire. Tests JS ciblés, build des assets et nouveaux DOM headless après correction réussis ; aucun changement du backend couvert par les217 tests Service.
