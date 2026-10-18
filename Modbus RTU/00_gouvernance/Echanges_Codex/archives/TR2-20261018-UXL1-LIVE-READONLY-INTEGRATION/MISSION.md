---
mission_id: TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION
status: READY
base_ref: main
base_sha: 9c2b65400cbe11564875a94864ab22a896cd3733
created_at_utc: 2026-10-18T17:56:04.317Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# UX-L1 — Première intégration opérationnelle du LIVE dans l'IHM S7

## Décision et objectif

L'utilisateur a approuvé l'évolution fonctionnelle de la supervision TR2 et délégué à ChatGPT les arbitrages d'ergonomie ordinaires. **Référence UX publiée par ChatGPT et à conserver sans réécriture de gouvernance par Codex :** `Modbus RTU/00_gouvernance/CONCEPTION_SUPERVISION_LIVE_UX_20261018.md`. Baselines : maquette `Supervision TR2/ux/s6-h/`, interfaces S7 gelées et vérifiées (`S7-I2/I3`, `S7-K`), démonstration R5-B2 réellement achevée et archivée. R5-B2 `DONE` n'est plus mission active ; ses archives `archives/TR2-20261018-R5B2-PHYSICAL-LIVE-RESUME/` restent immuables.

**But de la présente première tranche :** rendre l'IHM existante utile avec des **observations LIVE réelles et persistées**, en lecture seule, par capteur, sans refaire l'architecture S7 ni multiplier les chemins d'autorité. Montrer les informations effectivement connues dans une Vue générale et une fiche Synthèse/Vibrations LIVE ; commencer petit, reproductible et vérifiable, en respectant la source SQLite/API R5. Ce n'est pas une qualification du profil LIVE complet ni le développement simultané de toutes les fonctionnalités projetées en UX-L2/L3.

## Périmètre et autonomie d'exécution

1. Consulter `AGENTS.md`, gouvernance, protocole échanges, état courant/roadmap, `CONCEPTION_SUPERVISION_LIVE_UX_20261018.md`, S6-H, S7-K, `LIVE_R5A.md`, `LIVE_R5B2.md`, `DERNIER_RAPPORT.md` et la solution .NET réellement courante. Identifier les DTO, `live_observations`, `/api/v2/live`, ressources statiques de `TR2.Supervision.Web` et possibilités de lecture historique SQLite existantes. Vérifier `main`, conserver worktrees et stashes.
2. Réutiliser autant que possible les projections/API/services et design S7 existants. **Concevoir un chemin clair de lecture LIVE dans les vues Web**, sans faire passer `/api/v1/fleet` et `/api/v1/devices` du profil V1 pour des données LIVE lorsqu'ils ne les exposent pas. Une projection/adaptation additive bornée est permise si nécessaire ; elle reste cohérente avec les migrations et la rétention SQLite existantes. Ne pas introduire de cache métier autonome dans le navigateur ni de second moteur de polling/Modbus.
3. **Vue générale LIVE** : une ligne par TR2 identifié, libellé équipement + point de mesure **seulement si un référentiel fiable fournit cette affectation** (sinon afficher l'identifiant/endpoint explicite, sans nom fictif) ; RMS global + crête globale en mg, sévérité B3 brute/interprétée selon spécification, communication, qualité/fraîcheur, ancienneté de la dernière réception PC, dernières mesures connues uniquement avec étiquette périmée si disponibles. Réutiliser navigation et conventions de la Vue générale. Les filtres `Attention`, équipement/local, recherche et tri sont objectifs de l'UX complète : ne les livrer dans cette tranche que si leur autorité/données et le temps de réalisation le permettent sans refonte ; sinon marquer leur statut différé UX-L2 sans bouton factice.
4. **Fiche LIVE Synthèse/Vibrations** : préserver les six onglets contextuels S7. Exposer RMS/crête globaux, X/Y/Z, sévérité, validité et fraîcheur, état communication/acquisition connu et date de réception PC, avec l'identité durable quand disponible. Une petite tendance de synthèse **uniquement si un historique réellement persisté est disponible** ; ne jamais interpoler ou générer une évolution sans source. Si l'historique/graphique est disproportionné pour L1, livrer d'abord la donnée structurée et documenter UX-L2. Ne pas présenter des données LIVE dans les anciennes pages V1 sans indication de profil.
5. Préserver strictement `/live.html` pour diagnostic readonly ou le réintégrer par navigation sans casser R5-B2 ; rester explicite sur `LiveDiagnostic` NON-PRODUCTION, profil partiel, absence B1/B2/B7, temps de fenêtre UTC nul, run/epoch/config inconnus, port COM4 non actuellement ouvert. Les observations R5-B2 capturées peuvent servir au **rejeu hors ligne** des tests seulement, jamais à prétendre une acquisition physique actuelle. Aucune donnée sim/fixture présentée comme réelle. Les cas HELD, repeated sequence, COM loss, `Snapshot:null`, `IsCurrent:false`, dernier connu et `ArchiveState` sont impératifs.
6. **Hors périmètre cette fois :** flash/debug/test/commande STM32 et capteur ; Modbus d'écriture, B4/B5/B6/B7 nouveaux, alarmes/seuils synthétiques, FFT, vitesse mm/s, scores santé, accès réseau distant, nouveaux rôles/authentification, refonte globale de l'IHM, import SD, qualification R6. Les graphiques X/Y/Z sophistiqués, exports CSV et filtres avancés peuvent être reportés en UX-L2 si nécessaires pour garder L1 étroit.

## Acceptation et preuves

- Compile et tests ciblés (et suite touchée lorsque approprié), sans refaire campagnes physiques ou builds firmware acquis.
- Scénarios automatisés incluant mesure LIVE réelle R5-B2 en relecture étiquetée historique ; cas V/F valide et RMS/crête, HELD après STOP, perte communication, aucune mesure, données nulles, UTC absente et provenance/qualité, variations X/Y/Z. Ne pas utiliser de valeurs fictives comme preuve physique.
- HTTP local/DOM headless si faisable et utile : accès page existante + nouvel affichage, sans rediriger la vue V1 vers une API incompatible. Affichage `mg`, ancienneté, sévérité et provenance. Si point difficile faute de données durables ou de source d'équipement, expliquer précisément, privilégier solution minimale.
- Ancien comportement V1/S7 et commandes B5 ne régressent pas ; aucun effet sur `FRAM`, Flash, COM4. Compte rendu des limites, des pages effectivement raccordées et de ce qui reste UX-L2/L3.
- Commit/push normal sur `main`, contrôle distant, documentation d'implémentation sous `Supervision TR2/`, `DERNIER_RAPPORT.md` avec statut exact et SHA; aucun changement aux règles de gouvernance par Codex.

**Autorisations explicites :** commit/push `true`, flash/debug `false`. Intervenir uniquement sur PC/Web et tests ; tout enjeu matériel ou décision structurante non couverte est remonté à ChatGPT. Préserver les acquis, ne pas redemander la validation d'ergonomie courante.
