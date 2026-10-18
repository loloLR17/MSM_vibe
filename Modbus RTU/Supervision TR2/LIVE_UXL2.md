# UX-L2 — tendances persistées et navigation LIVE

Mission `TR2-20261018-UXL2-LIVE-TRENDS-NAVIGATION`, référence initiale `81cbeca3cacb90e2c0bc2633bbcc3d2407b93ddb`. Extension PC/Web readonly de UX-L1 ; aucun firmware, port COM, flash/debug, SD ou commande B5. Le document de conception UX et les contrats V1/S7 restent inchangés.

## Parc et fiche

`/` conserve la Vue générale V1 et son bloc LIVE distinct. `/live-fleet.html` ajoute Attention uniquement, bus, recherche identité/endpoint et tri identité/RMS/crête/sévérité/ancienneté, croissant ou décroissant ; absences classées après les valeurs. L'identité est celle réellement observée, pas un nom d'équipement inventé. L'affectation équipement/local et les noms personnalisés ne sont pas raccordés. La couverture existante reste les1000 dernières observations globales, donc pas un inventaire exhaustif.

Attention signale les erreurs d'archive, continuités CommunicationLost/Rejected, ou une sévérité B3 ≥3 lorsque le B3 est V/F et non HELD/non arrêté. Il s'agit d'un filtre de consultation des faits reçus, pas d'une nouvelle règle d'alarme ou de communication actuelle. L'âge d'une archive et un arrêt déclaré/HELD ne déclenchent pas Attention par eux-mêmes. Les valeurs retenues restent datées et attribuées à leur propre propriétaire ; une identité perdue reste inconnue. Une nouvelle identité n'hérite pas de l'ancienne mesure.

`/live-device.html?deviceId=1` conserve les six onglets S7. Synthèse contient la tendance RMS/crête globales ; Vibrations ajoute les X/Y/Z superposés, visibilité indépendante et choix RMS/crête. SVG natif, sans framework. Tous les chiffres sont en mg ; axes et CSV indiquent la réception PC, jamais une UTC de fenêtre reconstituée. Le diagnostic `/live.html` reste séparé et NON-PRODUCTION pour LiveDiagnostic. Aucun script de commande dans la fiche LIVE.

Les périodes1h/24h/7j se terminent à la dernière réception PC affichée, explicitement datée : cela permet de consulter une archive ancienne sans la présenter comme actuelle. La plage personnalisée est saisie en heure locale du PC, convertie en UTC et limitée à7 jours. La précision sous-milliseconde de la borne archivée est conservée (sinon7j pouvait dépasser la limite de quelques ticks). Les lots sont remplacés à la navigation ; pas d'accumulation d'historique ni de stockage navigateur permanent.

Chaque point SVG vient d'une observation SQLite. Les chemins ne relient que des observations V/F de même identité, endpoint et segment, avec temps PC non décroissant et progression de séquence cohérente : Sequential/+1 ou RepeatedSequence/0. Aucun rééchantillonnage. Les séquences répétées sont conservées, avec cercle creux. HELD, snapshot nul, invalidité, péremption, changement/perte d'identité et lacunes interrompent le tracé ; des traits orange et une liste détaillée conservent ces observations. Une observation V/F archivée est valide/fraîche **à sa réception**, jamais courante aujourd'hui. Les valeurs HELD restent consultables dans la liste/CSV, pas reliées comme nouvelles mesures.

Le navigateur relit seulement la projection PC existante chaque seconde. L'historique est demandé pour la fiche active au changement de sélection/période ou par les boutons de lot, et au plus toutes les5s si une nouvelle observation est reçue. Pas de requête d'historique pour chaque capteur du parc, second poller Modbus ou cache métier autonome. Les requêtes devenues obsolètes sont annulées et leurs réponses ignorées ; les graphiques sont retirés sur onglet non pertinent, identité absente ou erreur.

## API historique bornée

`GET /api/v2/live/history?bus=live-direct&unit=1&from=2026-10-18T16:00:00Z&to=2026-10-18T18:00:00Z&limit=500`

- DTO additifs `LiveHistoryQuery/Entry/Page` et interface `ILiveHistoryReadSource`. Le lecteur SQLite existant implémente les deux interfaces ; aucune nouvelle composition matérielle. `/api/v1/fleet`, `/api/v2/live` et les commandes existantes inchangés.
- Bus1..128 caractères, adresse1..247, période positive ≤7j, limite1..500 ; `before` est un curseur row_id exclusif positif.400 pour paramètres refusés,404 si source historique non configurée,503 pour archive inaccessible/invalide. L'annulation n'est pas masquée.
- Seek sur clé primaire : au plus10000 lignes candidates globales, plus un identifiant de lookahead pour savoir s'il existe un lot antérieur. Le filtre endpoint/date est paramétré et limité à cette plage de row_id ; au plus501 payloads sont désérialisés,500 rendus. Pas de COUNT ou scan de toute la base, migration, nouvel index, changement de rétention ou writer.
- Réponse `entries` en ordre réel d'insertion, `nextBefore`, `scannedRows` (candidats hors identifiant de lookahead), `coverage`, `historical:true`, `isCurrent:false`. Les inversions d'horloge ne sont pas triées silencieusement. Pages successives sans doublon ; aucune ligne raccordée au-delà d'un bord de page.
- Le périmètre de lecture est l'endpoint réellement observé. Les lignes d'identité différente/inconnue restent visibles comme ruptures et portent leur identité propre dans liste/CSV ; leurs mesures ne sont pas tracées pour un autre device_id. Déplacements entre endpoints non agrégés implicitement.
- Un lot peut être vide et avoir un curseur : poursuivre plus ancien si souhaité. Début/fin de lot, troncature et fin de couverture sont explicites, sans promesse de rétention. La page de500 n'est pas une vue exhaustive des7 jours. Une période vide ne démontre pas une absence globale de données.

## CSV et reproduction

Le bouton CSV exporte **uniquement le lot affiché**, même endpoint/période/curseur/ordre, au plus500 observations ; pas de téléchargement exhaustif ni d'export implicite de tout le parc. Colonnes : périmètre, row_id, observation_id, réception PC intégrale, identité réellement observée, bus/adresse/segment/continuité, historique non courant, V/F/HELD/arrêt, séquence, sévérité et flags B3, UTC fenêtre, run/epoch/config, source et huit valeurs mg. Les absences restent vides, y compris UTC/run/config inconnus ; les zéros effectivement reçus dans un snapshot invalide restent bruts avec leur qualité false. Aucune reconstruction de mesure perdue. Métadonnées textuelles pouvant être interprétées comme formules de tableur préfixées d'une apostrophe ; valeurs numériques inchangées. BOM UTF-8, virgules et guillemets échappés, CRLF. Les filtres de parc ne s'appliquent pas implicitement à cette sélection de fiche explicitement étiquetée.

Depuis `Modbus RTU/Supervision TR2`, après build, sur copie **figée, checkpointée, sans writer actif** de l'archive R5-B2 :

```bash
dotnet run --project src/TR2.Supervision.Service --no-build -- \
  --live-archive /chemin/externe/archive-figee.db --listen http://127.0.0.1:5094
```

Ouvrir `/live-fleet.html`, puis `/live-device.html?deviceId=1&view=vibration`. Le mode archive existant est readonly immutable et refuse un WAL non vide ; ne pas l'appliquer à une base vivante. Les compositions physiques existantes gardent leur ouverture ReadOnly ordinaire compatible WAL ; elles ne sont pas lancées durant UX-L2.

## Preuves et limites

Suite .NET :350 tests hors Service et228 Service, tous PASS,0 ignoré ;11 nouveaux scénarios de paramètres/pagination/archives/HTTP/charge logicielle. Ensuite corrections uniquement JS/HTML et builds des assets ; pas de répétition firmware ou suite hôte C. Huit tests JS ciblés et régression des helpers UX-L1. Vrai service HTTP/Kestrel et vrai Edge headless piloté par CDP sur copie R5-B2 :500+28 observations paginées,176 points globaux (88×2),264 XYZ (88×3),412 marqueurs hors tracé dans le lot récent, quatre chemins globaux séparés. Périodes, axes, crêtes, onglets, identité absente, erreurs et CSV vérifiés. L'erreur navigateur503 est simulée et identifiée ; le refus WAL503 est réellement éprouvé en test .NET.

Le CSV du navigateur est comparé intégralement aux500 observations API : identités, réception, sources, continuités, huit mesures,54 HELD et294 snapshots nuls sans zéro substitué. Base source528 observations byte-identique, SHA-256 `6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1`, aucun WAL créé.

Fixture synthétique **logicielle uniquement** :50000 lignes réparties sur50 endpoints ; trois lectures bornées en178,7ms dans cet environnement de test,200 lignes par endpoint dans un lot de10000 candidats, pagination et lot vide tronqué vérifiés. Pas de SLA, d'endurance ou d'inventaire physique de50 capteurs revendiqué. Pas de revue visuelle humaine. Seuils/événements d'alarme avancés UX-L3, profil complet, UTC de fenêtre, run/config autoritatifs et R6 restent hors qualification. Dernier état matériel hérité R5-B2, non relu et non modifié.
