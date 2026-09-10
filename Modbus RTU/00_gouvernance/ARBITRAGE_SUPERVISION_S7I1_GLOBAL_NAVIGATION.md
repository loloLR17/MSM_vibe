# S7-I1 — Navigation globale et vues parc réelles

## Statut

Implémentation de la première tranche S7-I après gel S7-H3.

## Objet

Rétablir dans l'IHM de production la navigation globale gelée en S6 et raccorder les vues parc manquantes aux autorités de lecture déjà disponibles.

Navigation globale :

```text
Vue générale
Vibrations
Attention & diagnostic
Campagnes
Système
```

## Autorités utilisées

- `/api/v1/fleet` : parc, sessions et snapshots B1/B3/B6/B7 déjà projetés ;
- `/api/v1/devices/{device_id}/commands` : historique transactionnel B5 déjà projeté ;
- aucun nouvel endpoint ;
- aucun accès Modbus navigateur ;
- aucun cache d'autorité parallèle.

## Vue Vibrations

La vue globale affiche uniquement les données B3 déjà reçues : RMS global, crête globale, sévérité B3, axe dominant et fraîcheur PC.

Elle ne calcule ni vitesse mm/s, ni FFT, ni fréquence dominante, ni score santé, ni sévérité transversale.

## Vue Attention & diagnostic

Les situations sont regroupées pour présentation sans fusion de leurs autorités :

- `Communication PC` : session non compatible ;
- `État / défaut TR2` : indicateurs B1 ;
- `Vibration B3` : alarmes/drapeaux B3 ;
- `Opération B5` : dernier état `Prepared`, `Submitted` ou `Ambiguous` du journal transactionnel projeté.

Aucune sévérité universelle n'est créée entre ces familles.

## Vue Campagnes

La vue globale exploite B6 comme inventaire de métadonnées : compte total, compte valide, stockage, validité de la campagne sélectionnée et métadonnées de l'entrée exposée lorsqu'elle est valide.

`selected_campaign_index` reste volontairement absent de cette vue opérateur globale.

Aucun téléchargement Modbus, import SD, parser ou analyse de campagne n'est ajouté.

## Invariants

- identité par `device_id` ;
- dernières valeurs connues conservées et qualifiées par leur fraîcheur ;
- B5 reste distinct de B6 ;
- aucune action d'écriture n'est ajoutée dans les vues globales ;
- la page Système reste séparée des états métier TR2 ;
- la navigation globale est identique sur les pages parc.

## Suite S7-I

S7-I2 traitera la conformité de la navigation contextuelle TR2 et des actions UX restantes, notamment la restitution exacte de `État & configuration` et la suppression de toute action globale d'acquittement non conforme au gel S6.
