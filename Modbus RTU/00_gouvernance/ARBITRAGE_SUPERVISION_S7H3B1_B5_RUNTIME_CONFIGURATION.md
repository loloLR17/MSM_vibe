# Projet MSM — Capteur de vibration TR2

## S7-H3B1 — Configuration runtime du cycle B5

Date : 2026-09-10

Cette tranche implémente uniquement la partie configuration décidée par S7-H3A. Elle ne raccorde encore ni le monitoring post-submit ni la réconciliation physique.

## 1. Objet

Le runtime accepte désormais un bloc JSON optionnel `b5` :

```json
"b5": {
  "postSubmitPollIntervalMilliseconds": 500,
  "postSubmitTimeoutMilliseconds": 30000,
  "reconciliationIntervalMilliseconds": 2000
}
```

Les valeurs du fichier d'exemple sont illustratives et relèvent de `SUPERVISION_POLICY`.

## 2. Modèle runtime

Le bloc est projeté en `RuntimeB5LifecyclePolicy` avec trois `TimeSpan` :

- `PostSubmitPollInterval` ;
- `PostSubmitTimeout` ;
- `ReconciliationInterval`.

`RuntimeConfiguration.B5` reste nullable afin de préserver la compatibilité avec les configurations antérieures.

Absence du bloc :

```text
RuntimeConfiguration.B5 == null
```

Aucune valeur implicite n'est alors injectée.

## 3. Validation fail-fast

Lorsque le bloc `b5` est présent, ses trois membres sont obligatoires.

Règles :

- `postSubmitPollIntervalMilliseconds > 0` ;
- `postSubmitTimeoutMilliseconds > 0` ;
- `reconciliationIntervalMilliseconds > 0` ;
- `postSubmitTimeoutMilliseconds >= postSubmitPollIntervalMilliseconds`.

Un bloc incomplet ou incohérent provoque `InvalidDataException` au chargement de la configuration.

## 4. Invariants préservés

Cette tranche :

- ne déduit aucune valeur du timeout série ;
- ne modifie pas la spécification Modbus V1 ;
- ne change aucune `FW_POLICY` ;
- n'introduit aucune règle V1.1 ;
- ne modifie pas encore `PhysicalPriorityWorkRunner` ;
- ne programme aucun nouveau travail scheduler ;
- ne change pas les sémantiques B5/B6 existantes.

## 5. Validation attendue

Les tests vérifient :

- absence de `b5` => cycle H3 désactivé explicitement ;
- chargement exact des trois durées ;
- rejet des valeurs nulles/non positives ;
- rejet d'un timeout inférieur à l'intervalle de monitoring ;
- rejet d'un bloc incomplet.

La tranche suivante, S7-H3B2, raccordera ces paramètres aux travaux `CommandPostSubmitMonitoring` et `TransactionReconciliation` existants.
