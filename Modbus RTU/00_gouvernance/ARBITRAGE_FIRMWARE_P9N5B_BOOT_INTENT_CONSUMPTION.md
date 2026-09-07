# Projet MSM — Capteur de vibration TR2

## P9-N5b1 — Arbitrage de consommation du BootIntent

## 1. Objet

Ce document fige la politique runtime de consommation du `BootIntent` après P9-N5a.

Il ne modifie pas la spécification Modbus RTU V1. Il précise un choix d'implémentation/recovery nécessaire pour respecter l'architecture gelée : un `BootIntent` ne vaut que pour le prochain boot attendu et ne doit jamais pouvoir être réutilisé lors d'un boot ultérieur.

Baseline d'entrée :

- `f55f2c8ac8b50d24dba6a93be203a42f89db0a1a` — `Firmware: wire P9-N5a software reset runtime` ;
- `db16fa6046b6ad5a4b98b189420ab00ab10a63cc` — `Firmware: fix P9-N5a runtime dependency test setup` ;
- validation locale utilisateur : `66/66` tests réussis.

## 2. Invariants déjà gelés

Pour `SOFTWARE_RESET` :

```text
RESERVED durable
→ CommandRecoveryContext BOOT_INTENT durable
→ BootIntent SOFTWARE_RESET(txid) durable
→ STARTED durable
→ PlatformResetTrigger.software_reset()
```

Au boot :

- le `ResetCauseProvider` reste l'autorité de la cause matérielle ;
- le `BootIntent` n'est qu'une preuve corrélée ;
- un `BootIntent` ne peut jamais écraser une cause matérielle contradictoire ;
- `STARTED + contexte correspondant + BootIntent correspondant + reset cause SOFTWARE` peut prouver l'effet terminal ;
- toute preuve absente, contradictoire ou insuffisante produit un résultat conservateur, au minimum `INDETERMINATE` ;
- aucun replay automatique de la commande n'est autorisé.

## 3. Problème à résoudre

Après P9-N5a, un `BootIntent` valide reste physiquement présent dans le store après le premier boot qui l'a exploité.

Sans consommation explicite, un deuxième boot pourrait relire cette intention périmée. Même si les autres barrières de reconciliation empêchent aujourd'hui une conclusion positive abusive, conserver cette preuve au-delà du boot auquel elle appartient viole son contrat de durée de vie et augmente inutilement l'ambiguïté de recovery.

## 4. Politique gelée P9-N5b1

### 4.1 Ordre de traitement

Le boot doit suivre cet ordre logique :

```text
capture ResetCause matériel
→ recover BootIntent dans une copie runtime immutable pour ce boot
→ recover CommandJournal
→ calculer le verdict de reconciliation avec cette copie BootIntent
→ restaurer l'état transactionnel incomplet si nécessaire
→ consommer durablement le BootIntent récupéré VALID
→ seulement ensuite rendre le runtime Modbus-ready
```

Le clear durable intervient donc **après exploitation de la preuve par la reconciliation**, mais **avant `system_ready_for_modbus = true`**.

### 4.2 Portée de la consommation

Seul un `BootIntentRecoveryStatus == VALID` est consommé par cette règle.

- `EMPTY` : aucune écriture inutile ;
- `VALID` : clear durable obligatoire après exploitation ;
- `CORRUPTED`, `UNSUPPORTED`, `UNAVAILABLE` : aucune interprétation positive et aucun clear silencieux dans cette tranche ; ces états restent des anomalies distinctes qui ne doivent pas être transformées implicitement en EMPTY.

Une extension ultérieure peut définir une politique d'assainissement explicite des records corrompus/unsupported, mais elle ne fait pas partie de P9-N5b.

### 4.3 Échec du clear

Si `boot_intent_store_clear()` échoue :

```text
verdict de recovery calculé
→ clear BootIntent échoue
→ boot échoue conservativement
→ runtime NON Modbus-ready
```

Il est interdit de continuer normalement avec une intention qui aurait dû être consommée mais dont l'invalidation durable n'est pas prouvée.

Le store passe alors dans son état de recovery requis selon son contrat existant.

### 4.4 Copie runtime après consommation

Après clear réussi, la copie `runtime->boot_intent_recovery` doit être normalisée à :

```text
status = BOOT_INTENT_RECOVERY_EMPTY
intent = BOOT_INTENT_NONE
```

Le verdict déjà calculé dans `CommandBootRecoveryResult` reste inchangé : la consommation de la preuve ne réécrit pas l'histoire du boot courant.

## 5. Propriété one-shot

Après un boot ayant exploité un `BootIntent VALID` :

```text
Boot N
  BootIntent VALID(txid)
  + journal/cause
  → verdict recovery
  → BootIntent clear durable

Boot N+1
  → BootIntent EMPTY
  → l'ancien intent ne peut plus contribuer à la reconciliation
```

Même si le journal conserve une transaction `STARTED` lifetime-strict, un boot ultérieur sans nouvelle preuve correspondante ne peut pas re-prouver l'effet du reset à partir de l'ancien intent.

## 6. Causes contradictoires

La consommation est indépendante du verdict positif ou négatif :

```text
BootIntent SOFTWARE_RESET(txid) + ResetCause SOFTWARE
→ reconciliation éventuellement EFFECT_PROVEN
→ clear intent
```

```text
BootIntent SOFTWARE_RESET(txid) + ResetCause POWER_ON/BROWNOUT/WATCHDOG/EXTERNAL
→ reconciliation INDETERMINATE
→ clear intent
```

Ainsi une intention contradictoire est elle aussi one-shot et ne peut contaminer un boot suivant.

Les matrices de faute P9-K restent l'autorité de non-surclassement de la cause matérielle.

## 7. Classification

| Sujet | Classification |
|---|---|
| BootIntent associé à SOFTWARE_RESET | architecture gelée |
| ResetCause matériel autoritaire | architecture gelée |
| BootIntent ne surclasse jamais une cause contradictoire | architecture gelée |
| durée de vie "prochain boot uniquement" | architecture/FW_POLICY gelée |
| clear après reconciliation et avant Modbus-ready | FW_POLICY P9-N5b1 |
| échec clear => boot non ready | FW_POLICY P9-N5b1 |
| assainissement automatique CORRUPTED/UNSUPPORTED | hors périmètre / NOT_DEFINED ici |

## 8. Tranche suivante

P9-N5b2 doit implémenter exactement cette politique dans `SystemRuntime` et ajouter les tests runtime associés :

1. matching SOFTWARE => verdict positif conservé puis store EMPTY ;
2. deuxième boot => aucune réutilisation de l'intention ;
3. cause contradictoire => INDETERMINATE puis store EMPTY ;
4. échec du clear => boot non Modbus-ready.
