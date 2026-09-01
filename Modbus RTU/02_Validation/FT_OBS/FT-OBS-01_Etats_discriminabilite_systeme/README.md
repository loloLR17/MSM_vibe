# FT-OBS-01 — États et discriminabilité système

## 1. Objet

FT-OBS-01 vérifie qu'une centrale peut interpréter sans heuristique les **états système explicitement définis par la V1** dans les Blocs 1, 4 et 6.

La sous-famille ne revalide ni les domaines simples, ni les transitions métier, ni les relations inter-blocs. Elle possède uniquement la propriété suivante : lorsqu'un état est normativement codifié, chaque situation définie doit être distinguable côté centrale sans convention privée du firmware.

## 2. Périmètre

Observables principaux :
- B1 `system_status` ;
- B1 `system_flags` ;
- B1 `storage_status` ;
- B1 `acquisition_state` ;
- B4 `config_state` ;
- B6 `selected_campaign_valid` ;
- B6 `campaign_state` ;
- B6 `storage_health_status`.

Les défauts, alarmes, warnings et historiques sont réservés à FT-OBS-03. La validité/fraîcheur des données est réservée à FT-OBS-02. Les détails d'exploitabilité des campagnes sont approfondis en FT-OBS-05.

## 3. Oracle V1 retenu

La V1 fournit des tables normatives discriminantes pour :
- `B1.storage_status` : non disponible / disponible / plein / erreur ;
- `B1.acquisition_state` : arrêtée / en cours / pause / erreur ;
- `B4.config_state` : vide / brouillon / valide / actif / erreur de validation / erreur d'application ;
- `B6.selected_campaign_valid` : index invalide / campagne valide ;
- `B6.campaign_state` : vide / préparation / en cours / terminée / erreur / partiellement corrompue ;
- `B6.storage_health_status` : OK / warning / dégradé / critique.

En revanche, B1 déclare explicitement que le domaine détaillé de `system_status` n'est pas défini en V1. `system_flags` est exposé comme bitfield d'état mais aucune table normative détaillée n'est fournie dans B1.

## 4. Classifications principales

- états explicitement codifiés ci-dessus : `COVERED` ;
- `B1.system_status` : `NOT_DEFINED` ;
- sémantique détaillée de `B1.system_flags` : `NOT_DEFINED` ;
- domaines et valeurs réservées : `DELEGATED` vers FT-LIM ;
- transitions B4 et mécanismes de changement d'état : `DELEGATED` vers FT-BLK / FT-CMD / FT-INT selon la propriété ;
- cohérence structurelle des lectures : `DELEGATED` vers FT-STR ;
- persistance après reboot : `DELEGATED` vers FT-PER.

## 5. Tests retenus

- `TT-OBS-B01-001` — discriminabilité des états B1 normativement définis ;
- `TT-OBS-B04-001` — discriminabilité de `config_state` B4 ;
- `TT-OBS-B06-001` — discriminabilité des états campagne et stockage B6.

Ces tests peuvent être exécutés sur simulateur, centrale ou banc disposant de scénarios déjà validés permettant d'obtenir les états concernés. Ils ne doivent pas recréer les scénarios propriétaires des autres familles.

## 6. Dettes V1 mises en évidence

- table normative de `B1.system_status` absente ;
- table normative détaillée de `B1.system_flags` absente ;
- aucune règle FT-OBS ne doit déduire un état global synthétique à partir d'une combinaison privée de plusieurs champs.

Ces lacunes ne sont pas comblées dans V1.

## 7. Artefacts

- `source/FT-OBS-01_source.md` ;
- `detaille/FT-OBS-01_detaille.md` ;
- `detaille/FT-OBS-01_matrice_couverture.csv`.

## 8. Statut

Sous-famille reconstruite pour revue sur la branche `audit/ft-obs-v1`. Aucun gel ni merge sans validation explicite.