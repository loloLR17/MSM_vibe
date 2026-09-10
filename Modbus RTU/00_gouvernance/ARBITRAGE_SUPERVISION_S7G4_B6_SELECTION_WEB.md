# Arbitrage supervision S7-G4 — Sélection Web de campagne B6

## Statut

Décision d’implémentation supervision PC, tranche S7-G4.

## Objet

Rendre l’écriture `selected_campaign_index` du Bloc 6 accessible depuis l’IHM Web sans la transformer en commande B5 et sans créer une seconde autorité d’exécution.

## Références

- Bloc 6 V1 : `selected_campaign_index` est le seul registre RW du bloc.
- Une valeur hors plage d’inventaire reste une écriture Modbus valide ; le TR2 expose alors `selected_campaign_valid = 0`.
- `SupervisionOperationalFacade.QueueCampaignSelection(...)` est l’autorité applicative existante pour cette opération.
- Le runner physique écrit exactement une fois le registre 6003, sans retry automatique.

## Décisions

- La sélection B6 reste strictement distincte de B5.
- Nouveau contrat Web sémantique : `QueueCampaignSelectionRequest(campaignIndex)` ; aucun registre Modbus brut n’est exposé au navigateur.
- Nouveau endpoint : `POST /api/v1/devices/{deviceId}/campaign-selection`.
- Les valeurs `0..65535` sont acceptées. L’API Web n’invente pas une règle interdisant les indices supérieurs à `total_campaign_count`.
- Le même `RuntimeCommandSink` implémente aussi l’interface de sélection B6 afin de conserver une seule façade opérationnelle et une seule composition runtime.
- La résolution du `deviceId` exige exactement une session compatible identifiée, comme pour les commandes B5.
- L’état `NotReady` retourne HTTP 503 ; une session incompatible retourne 409 ; un device inconnu retourne 404.
- HTTP 202 signifie uniquement « travail B6 mis en file ». Il ne prouve ni l’écriture physique ni la validité de la campagne sélectionnée.
- L’IHM demande confirmation avant émission et n’effectue aucun retry automatique.
- Après émission, la validation fonctionnelle reste l’observation ultérieure de B6 (`selected_campaign_index` / `selected_campaign_valid`) par le polling normal.

## Hors périmètre

- énumération artificielle de toutes les campagnes ;
- lecture séquentielle automatique de tous les indices ;
- transfert de données campagne ;
- B5 ;
- résolution transactionnelle `Ambiguous` ;
- retry automatique ;
- authentification / ACL.

## Validation attendue

- endpoint Web distinct du endpoint B5 ;
- transmission exacte du `ushort campaignIndex` vers `QueueCampaignSelection(...)` ;
- absence de validation métier artificielle contre `total_campaign_count` ;
- UI avec confirmation et retour « mise en file » uniquement ;
- aucun registre 6003 ou paramètre Modbus brut exposé à l’utilisateur.
