# Arbitrage supervision S7-F1 — Inventaire campagnes B6 read-only

## Statut

Décision de pré-implémentation supervision PC, tranche S7-F1.

## Objet

Rendre disponible dans la projection IHM l'état courant du Bloc 6 déjà lu par le polling Slow, sans ajouter de lecture Modbus côté Web et sans ouvrir le write path `selected_campaign_index`.

## Décisions

- `B6CampaignInventoryState` reste l'unique modèle protocolaire du Bloc 6.
- Le polling Slow existant continue de lire B4/B5/B6 ; S7-F1 publie désormais B6 dans `DeviceTelemetrySnapshotRegistry` lorsqu'une session est compatible.
- `DeviceTelemetrySnapshots` est étendu avec `ObservedSnapshot<B6CampaignInventoryState> CampaignInventoryState`.
- Les mêmes règles `NeverReceived` / `Receive` / `MarkUnavailable` que pour B1/B2/B3/B4/B7 s'appliquent.
- Une perte de communication conserve la dernière valeur B6 mais la marque indisponible ; la fraîcheur PC devient `Unavailable`.
- La projection IHM expose exactement les champs V1 du Bloc 6, sans déduire de métadonnée supplémentaire.
- `campaign_id` n'est globalement identifié côté centrale qu'avec `device_id` : couple `(device_id, campaign_id)`.
- Si `selected_campaign_valid = 0`, les métadonnées de l'entrée sélectionnée restent présentes comme valeurs brutes mais ne doivent pas être présentées comme campagne valide.
- `duration_s` est affichée telle que fournie par le TR2 ; aucune durée n'est recalculée à partir des timestamps.
- Aucun téléchargement de données brutes, import, FFT, transfert campagne ou analyse de campagne n'est créé en S7-F1.
- Aucune écriture B6 n'est exposée en S7-F1. La sélection `selected_campaign_index`, bien que RW dans la V1, reste hors périmètre de cette tranche.
- B6 n'est pas traité comme une transaction B5.

## Projection IHM

`IhmCampaignInventoryReadModel` contient :

- version structure ;
- compteurs total / valides ;
- index sélectionné et validité ;
- stockage utilisé / libre / santé ;
- identifiants campagne / mission ;
- timestamps début / fin ;
- état campagne ;
- durée ;
- taille ;
- labels campagne / mission ;
- intégrité des données.

## Périmètre suivant

Après validation locale de S7-F1 : page Campagnes réelle, puis exposition système/communications dans les sous-tranches S7-F suivantes.
