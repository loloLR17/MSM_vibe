# Arbitrage supervision S7-F2 — Vue Web campagnes B6

## Statut

Décision d’implémentation supervision PC, tranche S7-F2.

## Objet

Rendre visible dans le détail TR2 l’inventaire B6 projeté par S7-F1, sans ajouter de lecture Modbus, sans écrire `selected_campaign_index` et sans inventer une liste de campagnes que la V1 n’expose pas simultanément.

## Décisions

- La vue Web consomme exclusivement `GET /api/v1/devices/{deviceId}` et son `CampaignInventoryState`.
- Aucune API supplémentaire ni cache Web parallèle n’est créé.
- La vue `Campagnes` est ajoutée à la navigation contextuelle du TR2.
- Le Bloc 6 V1 n’exposant qu’une seule entrée à la fois, la vue distingue :
  - les métadonnées globales d’inventaire et de stockage ;
  - l’index actuellement sélectionné ;
  - l’entrée actuellement exposée par le TR2.
- S7-F2 ne présente jamais cette entrée comme une campagne valide si `selected_campaign_valid = 0`.
- `duration_s` est affichée telle que fournie par le TR2. Elle n’est jamais recalculée à partir des timestamps.
- `end_timestamp = 0` est affiché comme absence de timestamp de fin, conformément au cas V1 d’une campagne en cours ; aucune durée n’en est déduite.
- Les identifiants sont affichés avec le `device_id` afin de rappeler que l’identité globale côté centrale est `(device_id, campaign_id)`.
- Les enums V1 explicitement définis dans le Bloc 6 peuvent recevoir un libellé ; toute autre valeur reste affichée comme réservée avec sa valeur numérique.
- Les labels campagne/mission issus du TR2 sont rendus comme texte, jamais injectés comme HTML.
- La fraîcheur PC et la disponibilité du snapshot B6 restent visibles séparément de l’intégrité des données de campagne.
- En cas de perte HTTP, les dernières valeurs affichées restent conservées par le comportement S7-E2 existant.

## Hors périmètre

- écriture de `selected_campaign_index` ;
- navigation automatique de l’inventaire ;
- téléchargement ou transfert de données brutes ;
- import, FFT ou analyse de campagne ;
- commande B5 ;
- inventaire historique construit artificiellement côté Web.

## Suite

Après validation locale de S7-F2, S7-F pourra poursuivre avec la vue Système / communications PC à partir des autorités runtime et journaux existants.
