# Modbus RTU TR2 V1 — Baseline de gel

## 1. Objet

Ce document formalise la baseline candidate au gel final du référentiel Modbus RTU TR2 V1 après audit transversal, corrections normatives ciblées et propagation minimale vers la validation.

## 2. Référence de départ auditée

Baseline initiale gelée avant audit transversal :

`83cf401a6c31a3aa383266439623cbcc5e90f19b`

Commit : `FT-OBS — Audit industriel et gel V1`.

Cette référence a servi de point de comparaison unique pour la passe finale.

## 3. Branche de préparation du gel

Branche :

`audit/modbus-v1-final-baseline`

Aucun merge vers `main` n'est autorisé sans GO explicite.

## 4. Corrections normatives intégrées

### Bloc 1

- définition normative de `system_status` ;
- définition complète des bits `system_flags`, `fault_flags`, `warning_flags` ;
- réservés explicitement à 0 ;
- absence volontaire de dérivation exhaustive entre statut, flags et B7.

### Bloc 2

- clarification : aucune machine d'état exhaustive implicite de `time_status` ; seules les transitions imposées explicitement par les règles V1 et les commandes associées sont normatives.

### Bloc 5

- déclenchement de `submit` sur front montant 0→1 ;
- auto-clear obligatoire de `submit` après prise en compte ;
- `transaction_id = 0` invalide à la soumission ; valeurs 1..65535 valides ;
- refus fonctionnel code 14 pour une soumission avec ID 0 ;
- fermeture des valeurs réservées de `cmd_status` ;
- héritage explicite de la table de résultat pour `cmd_last_result_code`.

### Bloc 6

- fermeture des valeurs réservées de `campaign_state` ;
- `campaign_id != 0` pour toute campagne valide ;
- unicité de `campaign_id` entre campagnes distinctes présentes dans l'inventaire d'un même TR2 ;
- identification globale centrale par `(device_id, campaign_id)` ;
- `duration_s = end_timestamp - start_timestamp` pour une campagne terminée sans discontinuité temporelle affectant l'intervalle ;
- autres cas de relation exacte de durée laissés explicitement non définis.

### Bloc 7

- fermeture des domaines réservés de `system_health_status` et `selftest_status`.

## 5. Propagation de validation réalisée

Les corrections ont été propagées uniquement là où l'oracle, la classification ou le test était devenu obsolète :

- FT-CMD-01 : test déterministe de `transaction_id = 0` et code résultat 14 ;
- FT-BLK-05 : unicité locale de `campaign_id` et règle nominale de `duration_s` ;
- FT-OBS-01 : `system_status` et `system_flags` désormais interprétables selon V1 ;
- FT-OBS-03 : `fault_flags` et `warning_flags` désormais interprétables selon V1 ;
- matrices/synthèses FT-CMD et FT-BLK réalignées ;
- backlog FT-OBS et FT-CMD nettoyé des dettes résolues.

Les familles non impactées n'ont pas été réécrites.

## 6. Résultat de l'audit transversal

Le rapport `AUDIT_FINAL_MODBUS_V1.md` conclut :

- A — spécification suffisamment définie pour implémentation : **GO** ;
- B — firmware implémentable sans décision protocolaire privée indispensable : **GO** ;
- C — référentiel de validation capable de prononcer des verdicts objectifs : **GO**, sous réserve des préconditions d'essai explicitement classées `CONDITIONAL` ;
- D — exploitation centrale possible selon V1 sans heuristique privée indispensable : **GO**.

Verdict global :

> **GO — baseline Modbus RTU V1 prête pour implémentation et validation sur cible réelle.**

## 7. Limites conservées

Les limites restantes sont consolidées dans :

`Modbus RTU/00_gouvernance/REGISTRE_LIMITES_MODBUS_V1.md`

Elles restent explicitement `NOT_DEFINED`, `CONDITIONAL`, `TRACE_ONLY` ou `DELEGATED` selon le cas et ne doivent pas être transformées en exigences implicites.

## 8. Évolutions futures

Le backlog candidat V1.1 est consolidé dans :

`Modbus RTU/00_gouvernance/BACKLOG_MODBUS_V1_1_CONSOLIDE.md`

Ce backlog est informatif et non normatif pour la V1.

## 9. Critères de gel final

La baseline peut être gelée lorsque les conditions suivantes sont simultanément satisfaites :

1. branche de préparation 0 commit derrière la référence `main` retenue pour le merge ;
2. aucune contradiction normative connue non arbitrée ;
3. aucun ancien `NOT_DEFINED` résolu encore présenté comme dette active dans une synthèse de référence ;
4. toutes les corrections normatives propagées vers leurs validations propriétaires ;
5. registre des limites et backlog V1.1 présents ;
6. rapport d'audit transversal présent ;
7. contrôle final du diff effectué ;
8. GO explicite donné avant merge vers `main`.

## 10. Règle post-gel

Après merge et gel, toute modification de la V1 devra être réalisée par évolution formelle du référentiel avec analyse d'impact, mise à jour des owners de validation et contrôle de non-régression.

Le gel V1 ne signifie pas que toutes les préférences industrielles possibles sont définies ; il signifie que le contrat effectivement revendiqué par V1 est explicite, traçable et testable sans hypothèse cachée indispensable.