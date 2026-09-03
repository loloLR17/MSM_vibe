# V1.1-TRANSACTION-01 — Gel fonctionnel du cycle de vie transactionnel B5

## 1. Statut

Ce document enregistre le gel fonctionnel de l'arbitrage **V1.1-TRANSACTION-01 — Cycle de vie normatif du `transaction_id`**.

Statut : **FUNCTIONALLY FROZEN**.

Ce gel :

- ne modifie pas la baseline normative Modbus RTU V1 ;
- ne remplace pas la politique firmware V1 `lifetime strict` ;
- fixe le modèle fonctionnel à promouvoir dans une future spécification V1.1 ;
- reste en attente de l'arbitrage du mapping B5, des offsets, des codes numériques et de la compatibilité V1/V1.1.

Références :

- `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md` ;
- `RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md` ;
- `02_Validation/FT_CMD/V1_1_TRANSACTION_01/`.

## 2. Décision consolidée

L'identité transactionnelle V1.1 est :

```text
TransactionIdentity = (transaction_epoch, transaction_id)
```

avec :

```text
transaction_epoch : uint32 MSW/LSW
0                  : invalide / réservé
1..0xFFFFFFFF      : domaine valide

transaction_id 1..65534 : transactions ordinaires
transaction_id 65535    : RENEW_TRANSACTION_EPOCH uniquement
```

L'epoch est une autorité persistante du capteur. Elle ne change jamais implicitement sur reboot, perte de liaison, expiration temporelle, changement de centrale ou wrap du `transaction_id`.

Le renouvellement est explicitement demandé par la centrale et activé par le capteur. La valeur de la nouvelle epoch est attribuée par l'autorité capteur.

Une requête d'une ancienne epoch ne peut jamais produire de nouvel effet métier.

## 3. Frontières non encore gelées

Restent à arbitrer avant promotion normative complète V1.1 :

- offsets/adresses B5 des champs epoch ;
- compatibilité de lecture/écriture entre centrale V1 et capteur V1.1 ;
- valeurs numériques des résultats conceptuels `TRANSACTION_EPOCH_STALE`, `TRANSACTION_EPOCH_INVALID`, `TRANSACTION_EPOCH_UNKNOWN`, `TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED` ;
- code numérique de `RENEW_TRANSACTION_EPOCH` ;
- éventuelle exposition d'indicateurs d'approche d'épuisement du namespace.

## 4. Requalification de V1.1-TRANSACTION-02

L'arbitrage TRANSACTION-01 absorbe le problème principal d'épuisement du namespace `transaction_id` : `65535` est réservé au renouvellement et reste donc disponible comme voie de changement d'epoch lorsque `1..65534` ont été consommés.

`V1.1-TRANSACTION-02` n'est toutefois pas supprimé. Il est requalifié pour traiter uniquement les questions résiduelles :

- comportement lorsque le renouvellement est temporairement interdit par ses préconditions ;
- nécessité éventuelle d'un signal d'approche ou d'épuisement ;
- épuisement ultime du domaine `transaction_epoch` uint32 ;
- politique de maintenance/factory reset associée, si elle est retenue ultérieurement.

## 5. Traçabilité des arbitrages

Les arbitrages A à D10 sont gelés dans `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md` et traduits en invariants falsifiables T01-01 à T01-25.

La prochaine étape normative est la passe **mapping B5 + compatibilité V1/V1.1**.
