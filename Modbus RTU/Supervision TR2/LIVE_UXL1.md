# UX-L1 — parc et fiche LIVE en lecture seule

Mission `TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION`, code `8722c02e805e785dacacf0f2602f94a102b94119`, HEAD initial `cfabfbf291c83e20878f90181b7d18925f079ddb`. Aucun accès COM4, commande Modbus, flash ou debug. La conception UX publiée et les interfaces V1/S7 restent inchangées.

## Pages et source d'autorité

- `/` conserve sa Vue générale V1 et ajoute une section distincte Parc LIVE.
- `/live-fleet.html` affiche une ligne par identité TR2 connue dans la couverture, avec endpoint explicite en l'absence d'affectation équipement/local raccordée.
- `/live-device.html?deviceId=1` conserve six onglets : Synthèse, Vibrations, État & configuration, Commandes, Campagnes, Diagnostic. Les deux premiers affichent global et axes ; les autres décrivent les informations disponibles et les limites. Aucun bouton de commande ni script B5 n'est chargé dans cette fiche readonly.
- `/live.html` et `/api/v2/live` restent le diagnostic R5 existant ; `/api/v1/fleet`, les fiches V1 et commandes S7 conservent leurs contrats.

La nouvelle API additive `GET /api/v2/live/overview` combine les vues du service LIVE existant et les dernières1000 observations SQLite `live_observation`. Pas de migration, modification de rétention ou second moteur Modbus. Les données ne passent pas par les DTO V1. L'API déclare sa couverture bornée : elle ne prétend ni inventaire exhaustif ni historique illimité. Une longue perte peut sortir une ancienne mesure de cette couverture ; elle ne sera pas inventée.

La projection est sans cache mutable. Une identité durable connue déduplique les endpoints ; un endpoint sans identité reste explicitement non identifié. Une identité différente ne reprend pas la mesure de l'ancien capteur. Sur perte d'identité, `lastKnown` conserve son propre propriétaire et sa date ; il ne devient jamais l'identité courante. Une erreur d'archive est exposée séparément, et `ArchiveState` du service est conservé.

L'UI montre RMS/crête en mg, sévérité transmise brute et interprétée, communication, V/F/HELD, réception PC et ancienneté. Les valeurs retenues sont marquées périmées, avec propriétaire et date. Aucune absence remplacée par zéro. L'état d'acquisition est limité à ce que déclare la dernière observation B3, pas extrapolé à l'instant présent. UTC fenêtre, run/epoch/config non exposés restent inconnus. `LiveDiagnostic` est marqué NON-PRODUCTION. HTML rendu par textContent, aucune écriture HTTP, aucun cache métier navigateur.

Les archives et le replay ont toujours `historical=true` et `isCurrent=false` dans la projection opérationnelle, même si le B3 archivé déclarait FRESH. Le chemin direct existant peut utiliser sa fraîcheur actuelle sans nouveau polling Modbus ; il n'a pas été lancé durant cette mission. Le diagnostic R5 garde son contrat historique distinct.

## Consultation reproductible sans matériel

Depuis `Modbus RTU/Supervision TR2`, sur une **copie d'archive fermée, checkpointée et figée**, sans writer actif :

```bash
dotnet run --project src/TR2.Supervision.Service --no-build -- \
  --live-archive /chemin/externe/archive-figee.db --listen http://127.0.0.1:5093
```

Ouvrir `/live-fleet.html` ou `/live-device.html?deviceId=1&view=vibration`. Le même service S7 est utilisé, sans usine série, polling de replay ou command sink. Le mode figé refuse un WAL non vide. Le lecteur est en ReadOnly et utilise `immutable=1` exclusivement pour une archive déclarée figée ; ne pas utiliser ce mode sur une base active. La lecture d'une base vivante dans les compositions direct/replay reste en ReadOnly normal, compatible avec leur WAL. Aucun changement de politique SQLite existante. Référence : [URI SQLite et immutable](https://www.sqlite.org/uri.html).

Le smoke a décompressé une copie de l'archive R5-B2 publiée.528 observations réelles historiques, SHA `6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1`. La base reste byte-identique, aucun WAL créé. La preuve DOM affiche notamment HELD1073/1157mg global, axes RMS77/1068/68mg, crêtes134/1151/126mg, sévérité0, UTC indisponible et historique explicite. Ce n'est pas une acquisition actuelle ni un nouveau test physique.

## Validation et limites

Build .NET sans avertissement ni erreur. Première suite complète :350 tests hors Service réussis ;214 Service réussis et un échec nouveau sur la création WAL en lecture seule ordinaire. Cette assertion a été conservée : ajout du chemin SQLite figé adapté, refus explicite d'un WAL non vide, test de conservation du fichier et des valeurs. Ensuite215 Service passent, puis217 après ajout du contre-test WAL et de l'intégration HTTP. Les six nouveaux tests couvrent capture/archive R5-B2, identité et perte, replay non courant, déduplication, erreurs, readonly et pages/six onglets/POST404. Les tests S7/V1 préexistants passent. Tests JS ciblés sur V/F historique, XYZ, HELD, perte et retenue, absence, UTC et absence d'écriture/cache. Build final des assets et smoke HTTP200, API V1 distincte, POST commandes404. Deux DOM Edge headless réels (parc et Vibrations), pas de navigateur humain ni screenshot.

L'historique persisté existe, mais tendance, multi-axes graphiques, sélection temporelle, filtres/tri avancés et CSV sont différés à UX-L2 pour garder L1 centré sur les données structurées et leurs qualités. Aucun contrôle factice. Les affectations équipement/local ne sont pas inventées. Seuils, overlays d'événements et finitions relèvent d'UX-L3, seulement avec autorités documentées. Aucun score santé, FFT, mm/s, norme ou profil produit nouveau. Les six onglets sont contextualisés readonly ; les interfaces V1 de commandes ne sont pas copiées vers le diagnostic.

Preuves : [UX-L1](../00_gouvernance/Echanges_Codex/preuves/TR2-20261018-UXL1/README.md). Matériel non touché et non relu ; dernier état matériel connu reste celui de R5-B2, M016 restauré/FRAM2b501a…7dd84. R6 et qualification indépendante ChatGPT restent distincts.

Dernière correction de sélection : une fiche ouverte par identifiant peut suivre sa dernière mesure lors d'une perte d'identité, sans promouvoir son ancien propriétaire en identité actuelle. Si une observation identifiée existe ailleurs pour ce device_id, elle reste prioritaire. Tests JS ciblés, build des assets et nouveaux DOM headless après correction réussis ; aucun changement du backend couvert par les217 tests Service.
