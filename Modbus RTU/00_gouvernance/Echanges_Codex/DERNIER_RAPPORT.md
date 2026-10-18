---
mission_id: TR2-20261018-UXL2-LIVE-TRENDS-NAVIGATION
status: IN_PROGRESS
base_ref: main
base_sha: 8ee37a7ae1d4da30e922ca6f723aa11206ca9488
initial_head: 81cbeca3cacb90e2c0bc2633bbcc3d2407b93ddb
result_sha: e5537ebe2b1898da9d461339a425826754590154
created_at_utc: 2026-10-18T19:09:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# UX-L2 — tendances archivées, navigation du parc et CSV

**Tranche logicielle réalisée et vérifiée ; IN_PROGRESS uniquement pour publication/relecture distante du présent bilan. Aucun blocage technique.** Le code a été publié par push normal sur main. Les courbes utilisent les observations réellement persistées R5-B2 et ne présentent jamais une archive comme une acquisition actuelle. Aucun accès COM4/STM32, flash/debug, B5, FRAM ou SD pendant cette mission.

Code évalué **`e5537ebe2b1898da9d461339a425826754590154`**, `feat(uxl2): add bounded LIVE archive trends, fleet navigation and CSV`. Référence de contrat8ee37a7 ; mission publiée81cbeca, ancêtre du code. Travail dans le nouveau worktree isolé `MSM_vibe-UXL2-trends-navigation`, branche auxiliaire `codex/uxl2-trends-navigation`, publication explicite `HEAD:refs/heads/main`. Dépôt initial main82169af propre préservé ; aucune fusion, réécriture, restauration Git ou suppression de travaux.

## Livré et critères techniques

- **Synthèse** : tendance RMS globale et crête globale issue exclusivement des points SQLite, axes mg/réception PC explicites. **Vibrations** : globales et X/Y/Z superposés, axes visibles indépendamment, choix RMS/crête, légendes et données détaillées. Cercles creux pour séquences répétées ; chemins séparés sur pertes, invalidité/péremption, HELD, segments, séquence incohérente, changement/perte d'identité ou recul de l'horloge. Aucun rééchantillonnage ou comblement de lacunes. Les HELD/pertes restent dans liste/CSV avec leur qualité et propriétaire brut ; pas de nouvelle mesure inventée.
- Périodes1h/24h/7j et personnalisée≤7j. Fin prédéfinie = dernière réception PC observée, datée, pour permettre la consultation d'une archive sans la qualifier de courante. Dates personnalisées locales du PC, axes/export UTC de réception. Début/fin de lot, troncature et lots plus anciens explicites. Historique/replay non courants, V/F du B3 décrit uniquement au moment reçu. UTC fenêtre, run/epoch/config absents restent inconnus.
- **Parc** intégré à `/` et `/live-fleet.html` : Attention uniquement, filtre bus honnête, recherche identité/endpoint, tris identité/RMS/crête/sévérité/ancienneté et direction. Absences après valeurs. Attention = erreurs archive ou CommunicationLost/Rejected, ou sévérité B3≥3 si V/F/non HELD/non arrêté ; ni ancienneté d'archive ni STOP/HELD seuls ne créent une alarme. Affectation équipement/local et noms personnalisés absents, explicitement indiqués. Dernières valeurs UX-L1 datées/qualifiées, propriétaire conservé sans promotion d'identité.
- **CSV livré** : uniquement le lot effectivement affiché de la fiche, endpoint/période/curseur/ordre explicites,≤500 observations. Identité brute, provenance, continuité/segment, horodatage PC intégral, historique, V/F/HELD/arrêt/séquence/sévérité/flags B3, huit valeurs mg, UTC/run/epoch/config inconnus laissés vides. Snapshots nuls sans zéro ; zéros réellement reçus dans snapshot invalide conservés avec leur qualité. Métadonnées textuelles protégées contre formules de tableur, guillemets échappés. Pas d'extraction illimitée ou d'export implicite du parc.
- **API additive** `GET /api/v2/live/history` et DTO/interfaces de lecture. SQLite ReadOnly paramétré : seek PK sur≤10000 lignes candidates globales (+1 identifiant de lookahead),≤501 payloads lus pour≤500 rendus, curseur row_id exclusif et période≤7j. Pagination réelle sans doublon, même si un lot est vide. Pas de scan de toute la base, COUNT global, migration/index, rétention modifiée, writer ou cache métier. Une requête de fiche active, pas une requête historique par capteur du parc. Réponses obsolètes ignorées/annulées ; graphiques retirés sur onglet non pertinent, erreur ou capteur absent.
- V1/S7, `/api/v1/fleet`, diagnostic `/live.html`/`/api/v2/live`, commandes et profil partiel NON-PRODUCTION conservés. Pas de nouveau polling Modbus, commande Web, auth/LAN/déploiement ou modification normative/gouvernance.

[Note de lancement, API et limites](../../Supervision%20TR2/LIVE_UXL2.md), [preuves et manifeste](preuves/TR2-20261018-UXL2/README.md). Journal `ETAT_COURANT_TR2.md` actualisé avec les acquis logiciels, preuves, limites et suite ; constat factuel ajouté à la roadmap, aucun jalon physique qualifié unilatéralement.

## Validations réellement exécutées

1. `dotnet build 'Modbus RTU/Supervision TR2/TR2.Supervision.sln' -m:1` : réussi,0 avertissement/erreur. Le premier essai `--no-restore` dans le nouveau worktree avait échoué car les assets obj n'existaient pas ; build avec restauration normale réussi, échec conservé. Aucun changement de configuration de machine.
2. `dotnet test 'Modbus RTU/Supervision TR2/TR2.Supervision.sln' -m:1 --no-restore --logger 'trx;LogFilePrefix=uxl2-full' --results-directory /home/lolo/dev/msm/reports/TR2-uxl2/full-tests` : **578 PASS,0 échec,0 ignoré**, dont350 hors Service et228 Service. **11 nouveaux tests** (cas Theory inclus) pour pagination528 réelles, qualités/identités/nulls, refus paramètres et injection, limites, base absente/WAL, HTTP400/503, lecture readonly et charge bornée. Les régressions V1/S7/B5 existantes sont incluses. Validation .NET complète justifiée par les interfaces/backend partagés touchés ; aucun test firmware/cross-build matériel pertinent ici.
3. **8 tests JS ciblés PASS**, commande `node 'Modbus RTU/Supervision TR2/tests/live-trends.test.cjs' 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2'`, puis régression `live-overview.test.cjs` réussie. R5-B2 réel + cas logiciels identifiés : mg/XYZ, V/F historique, HELD/perte, identités/segments, séquences répétées/incohérentes, recul temps, filtres/tris/nulls, Attention, CSV, sécurité DOM et précision7j. Aucun test supprimé ou attente affaiblie.
4. Vrai host Kestrel avec `--live-archive <copie-figee.db> --listen http://127.0.0.1:5094`, API historique et V1 disponibles. **Vrai Edge headless piloté par CDP**, profil dédié, sans utilisateur/série : parc, fiche, axes, crête,1h/24h/7j/personnalisée, vide, pagination, onglets, identité absente et erreurs. DOM et journaux conservés. L'erreur503 navigateur est simulée et explicitement étiquetée ; le refus503 de WAL est réellement testé en .NET. Pas de revue visuelle humaine ni de qualification physique.
5. Graphe réel du lot récent500 : **176 points globaux (88×2),264 XYZ (88×3),4 chemins globaux séparés et412 marqueurs hors tracé**. Pagination500+28 =528 observations de l'archive. Les graphiques ne génèrent pas500 mesures :88 observations vibratoires V/F exploitées, reste qualifié HELD/invalidité/null/identité inconnue.
6. CSV réellement produit dans le navigateur, **500 observations comparées intégralement** à l'API : row_id/observation_id, date PC, identité/source/continuité et huit valeurs exactes ;54 HELD,294 snapshots nuls exportés sans zéro substitué. UTC fenêtre/run/epoch/config vides. Copie SQLite byte-identique, SHA-256 **`6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1`**, aucun WAL créé. Source archivée R5-B2 immuable, pas de nouvelle acquisition.
7. Fixture **synthétique logicielle**50000 lignes/50 endpoints :10000 candidats maximum/lot,200 observations de l'endpoint dans le lot, pagination sans doublon et lot vide tronqué ; trois lectures bornées **178,7ms** dans cet environnement de test. Aucun SLA ou inventaire de50 capteurs réels revendiqué.

Les corrections finales après la suite .NET portent uniquement sur JS/HTML ; builds des assets, tests JS ciblés et scénario navigateur final exécutés ensuite. Ne pas présenter la suite .NET comme rejouée après ces changements UI, ni répéter firmware/essais physiques sans motif. Sorties effectives dans les sept TRX, `validation-summary.json`, `browser-summary.json`, `csv-verification.json` et logs référencés par BUNDLE/SHA256SUMS.

## Erreurs analysées et corrigées

Premier test JS de rupture : le raccord acceptait trop largement une séquence inchangée en Sequential. Correction : Sequential exige+1, RepeatedSequence exige0 ; assertions conservées, tests finaux PASS. Premier smoke navigateur : une borne reçue à7 décimales convertie en millisecondes faisait dépasser7j de0,6849ms ; précision du timestamp conservée et test dédié ajouté. Deuxième smoke : ancien SVG laissé dans DOM masqué lors de sortie d'onglet ; retrait explicite des tracés et export désactivé, puis campagne finale navigateur réussie. Sorties des deux échecs conservées séparément, aucun échec dissimulé.

## Préservation, limites et suite

**8318 fichiers protégés,68 autres worktrees, deux stashes et dépôt d'origine propres/inchangés** au contrôle de préservation. Les stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87` sont conservés. Archives et preuves historiques UX-L1/R5-B2, normes, dettes et gouvernance immuables. Seuls sources/tests/doc PC, nouveau dossier de preuves et faits autorisés journal/roadmap modifiés.

Couverture bornée :1000 observations pour le parc,500 par lot historique et≤10000 candidats globaux ; une longue absence peut sortir une identité de la vue parc. Pas d'inventaire exhaustif, de rétention infinie, d'agrégation implicite des déplacements d'endpoint ni de données inventées pour une période vide. Les identités différentes/inconnues à un endpoint restent des ruptures, et leurs valeurs ne sont pas tracées sous un autre device_id. Pas de noms/affectations inventés. Drapeaux B3 reçus ne sont pas des événements d'alarme datés indépendants.

Aucun nouveau relevé matériel : dernier état hérité R5-B2 = M016 Flash2Mio `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`, FRAM262144o `2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84`, journal78/root91/epoch5/run11/config3gen3/STOP5/13, acquisition arrêtée/COM fermé à la clôture physique précédente. **Non relu ici** ; aucune restauration nécessaire ou réalisée. Ne pas réutiliser les clés consommées5/12–5/13.

Critères logiciels UX-L2 satisfaits par les preuves disponibles. Restent hors mission : UX-L3 seuils/overlays/événements avancés, profil complet B1/B2/B7, UTC fenêtre et identités run/config sur le fil, endurance/R6/métrologie et contrôle indépendant ChatGPT. Aucune décision ou intervention utilisateur nécessaire pour cette livraison logicielle.

## Publication

Code `e5537ebe2b1898da9d461339a425826754590154` publié par push normal explicite sur `main` après contrôle de portée/diff indexé et référence distante. Le présent bilan et les62 artefacts de preuve sont prêts ; relecture intégrale depuis la référence récupérée de GitHub à effectuer, puis mise à jour DONE et archivage neuf de cette mission. `allow_commit/allow_push` exercés, `allow_flash/allow_debug:false` respectés. Pas de force push/merge/rebase, commandes matérielles ou changement de configuration Codex. État attendu après publication : worktree de mission propre et HEAD égal à main distant ; état effectif et SHA du commit de rapport fournis après contrôle distant.
