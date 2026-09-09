# Projet MSM — Capteur de vibration TR2

## S7-D — Vue générale réelle du parc

Date : 2026-09-09

Baseline de départ :

`eeb5aed04866831f8075862cc94468201846a3f9`

`Supervision: integrate S7-C2 web runtime`

Cette tranche réalise la première page Web réellement alimentée par l'état autoritatif de la supervision : la **Vue générale**.

Toute décision de présentation ci-dessous est une `UX_POLICY` / `SUPERVISION_POLICY`. La spécification Modbus RTU V1 et les sémantiques B1/B2/B3/B5 restent inchangées.

## 1. Périmètre

S7-D ajoute une page statique servie par `TR2.Supervision.Web` et alimentée uniquement par :

`GET /api/v1/fleet`

La page affiche :

- nombre d'endpoints surveillés ;
- nombre de sessions compatibles ;
- nombre de points nécessitant l'attention opérateur ;
- nombre de points dont la télémétrie PC est fraîche ;
- une ligne par endpoint configuré avec bus, adresse Modbus, `device_id`, communication/session, fraîcheur PC, RMS global B3, état B1 brut et attention.

## 2. Source de vérité

Le navigateur ne possède aucun modèle métier autoritatif.

Il interroge l'API read-only S7-C1/C2. Cette API projette directement les autorités existantes :

- `FleetRegistry` ;
- `DeviceTelemetrySnapshotRegistry`.

Aucun accès Modbus direct, aucun polling Modbus Web, aucun cache métier Web et aucun second runtime ne sont ajoutés.

## 3. Données absentes et dernière valeur connue

La page respecte les invariants S6/S7-B :

- un endpoint non identifié n'invente aucun `device_id` ;
- une valeur jamais reçue est affichée comme absente ;
- une valeur `Unavailable` conserve et affiche la dernière valeur connue lorsqu'elle existe ;
- la fraîcheur PC reste distincte des champs de validité B3 ;
- aucune valeur vibration n'est recalculée dans le navigateur.

## 4. Attention opérateur

Dans S7-D, la colonne `Attention` est volontairement limitée aux informations réellement disponibles dans le read model courant.

Un point est signalé si au moins une des conditions suivantes est vraie :

- session non `Compatible` ;
- télémétrie B1 ou B3 absente, indisponible, vieillissante ou périmée ;
- `FaultFlags`, `WarningFlags`, `ErrorCode` ou `WarningCode` B1 est non nul ;
- `AlarmFlags` ou `AlarmLatched` B3 est non nul.

Ce regroupement est une aide de tri IHM, pas un nouveau niveau de sévérité TR2 et pas un score santé.

Les états B5 `Ambiguous` ne sont pas encore projetés dans cette tranche ; ils restent réservés à S7-G/H.

## 5. Libellés

Tant que le modèle d'équipement durable n'est pas projeté vers l'IHM, la première colonne identifie le point par son endpoint :

`<bus> / adresse <n>`

Aucun nom d'équipement fictif issu de la maquette S6-H n'est conservé.

Le `device_id` est affiché comme identité durable lorsqu'il est connu.

## 6. États bruts

S7-D ne transforme pas silencieusement les codes numériques B1/B3 en libellés métier non encore arbitrés.

- `SystemStatus` B1 est affiché comme valeur brute ;
- RMS global B3 est affiché en `mg`, unité normative du mapping ;
- la fraîcheur affichée est la fraîcheur PC S7-B.

Les libellés détaillés de statut/sévérité seront traités dans S7-E avec les contrats de détail.

## 7. Rafraîchissement HTTP

La page effectue un premier chargement puis rafraîchit `GET /api/v1/fleet` périodiquement toutes les 2 secondes.

Ce délai est une `UX_POLICY` de présentation HTTP. Il ne change pas les cadences Modbus, ne déclenche aucune lecture bus supplémentaire et ne modifie pas la politique de fraîcheur PC.

Une seule requête est en vol à la fois ; le prochain rafraîchissement est planifié après la fin du précédent.

## 8. Défaillance HTTP

Si l'API Web devient momentanément indisponible :

- les dernières lignes affichées restent visibles ;
- la page indique explicitement que la mise à jour Web a échoué ;
- les valeurs ne sont pas remplacées par zéro.

Cela ne doit pas être confondu avec une perte de communication Modbus d'un TR2 individuel.

## 9. Fichiers statiques

`TR2.Supervision.Web` embarque ses propres fichiers `wwwroot` dans le projet :

- `index.html` ;
- `styles.css` ;
- `app.js`.

Le host active `UseDefaultFiles()` puis `UseStaticFiles()`.

La maquette `ux/s6-h` reste une référence UX historique et n'est pas modifiée.

## 10. Hors périmètre

S7-D ne réalise pas :

- détail TR2 / navigation contextuelle S7-E ;
- campagnes, système et communications S7-F ;
- commandes B5 S7-G ;
- résolution `Ambiguous` S7-H ;
- reprise complète de toutes les vues S6-H S7-I ;
- noms d'équipements durables si leur projection n'existe pas encore ;
- authentification, CORS ou HTTPS.

## 11. Suite

Après validation locale de S7-D, la tranche suivante est :

**S7-E — Détail TR2, vibrations et diagnostic read-only**, avec cadrage précis des projections complémentaires nécessaires.