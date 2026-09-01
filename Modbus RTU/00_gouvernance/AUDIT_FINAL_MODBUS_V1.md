# Audit final Modbus RTU TR2 — Baseline V1

## 1. Objet

Ce document clôt l'audit transversal du référentiel de validation Modbus RTU V1 du capteur TR2.

Référence de départ de l'audit : `main` au commit `83cf401a6c31a3aa383266439623cbcc5e90f19b` (`FT-OBS — Audit industriel et gel V1`).

Branche de correction et de consolidation : `audit/modbus-v1-final-baseline`.

Principe d'audit : aucune hypothèse implicite, aucune invention de comportement, la spécification normative V1 reste l'oracle. Toute propriété non définie conserve explicitement un statut `NOT_DEFINED`, `CONDITIONAL`, `TRACE_ONLY` ou `DELEGATED` selon sa nature.

## 2. Périmètre audité

Le contrôle transversal couvre :
- spécifications sources V1 B0 à B7 ;
- charte de typage ;
- mapping Modbus unifié ;
- gouvernance et plan directeur de validation ;
- FT-STR ;
- FT-ACC ;
- FT-LIM ;
- FT-BLK ;
- FT-INT ;
- FT-CMD ;
- FT-SEQ ;
- FT-RBT ;
- FT-PER ;
- FT-OBS.

## 3. Méthode de revue finale

L'unité d'analyse retenue est la propriété normative V1, avec la chaîne de traçabilité suivante :

`source normative → propriété → bloc/mécanisme → famille propriétaire → classification → justification → test → condition d'exécution → verdict`.

Les passes suivantes ont été menées :
1. intégrité du snapshot de référence ;
2. couverture normative V1 → ownership ;
3. traçabilité bidirectionnelle ;
4. consolidation des `NOT_DEFINED`, `CONDITIONAL` et `TRACE_ONLY` ;
5. frontières inter-familles ;
6. exécutabilité sur cible réelle ;
7. readiness implémentation / firmware / validation / centrale ;
8. correction normative minimale des anomalies démontrées ;
9. propagation minimale des corrections dans les familles de validation impactées ;
10. passe consolidée finale.

## 4. Anomalies normatives corrigées avant gel

### 4.1 Bloc 1 — état système et bitfields

La V1 exposait `system_status`, `system_flags`, `fault_flags` et `warning_flags` sans définition suffisante au regard de la charte de typage.

Correction intégrée :
- `system_status` : `UNKNOWN`, `NOMINAL`, `DEGRADED`, `FAULT`, puis réservés ;
- table complète de `system_flags` ;
- table complète de `fault_flags` ;
- table complète de `warning_flags` ;
- bits réservés imposés à 0 ;
- persistance des bits tant que la condition associée est présente ;
- absence volontaire de fonction exhaustive de dérivation entre état global, flags, codes B1 et diagnostic B7.

Les algorithmes internes produisant les conditions de défaut ou d'avertissement ne sont pas normalisés par le protocole V1 et ne sont pas inventés par le référentiel de validation.

### 4.2 Bloc 2 — transitions temporelles

La formulation générique sur les transitions d'état a été clarifiée : la V1 ne définit pas de machine d'état exhaustive de `time_status`. Seules les transitions résultant explicitement des règles normatives du Bloc 2 et des commandes associées sont imposées.

Aucune séquence d'état implicite ne doit être déduite.

### 4.3 Bloc 5 — soumission et transaction

Les points suivants ont été corrigés :
- `submit` est déclenché sur front montant `0→1` ;
- après prise en compte, TR2 remet automatiquement `submit` à `0` ;
- l'ancienne alternative autorisant le maintien à 1 a été supprimée ;
- `transaction_id = 0` est invalide à la soumission ;
- `transaction_id = 1..65535` sont des identifiants valides ;
- une soumission avec ID 0 n'exécute pas l'action et expose `cmd_result_code = 14` ;
- l'écriture de 0 dans le registre RW reste un accès Modbus valide ;
- le domaine réservé de `cmd_status` est fermé ;
- `cmd_last_result_code` réutilise explicitement la table de `cmd_result_code`.

Le bouclage/réemploi d'identifiants au regard de l'historique d'idempotence reste hors V1.

### 4.4 Bloc 6 — campagnes

Les clarifications suivantes sont intégrées :
- `campaign_state` ferme explicitement son domaine réservé ;
- toute campagne valide a un `campaign_id != 0` ;
- deux campagnes distinctes simultanément présentes dans l'inventaire d'un même TR2 ont des `campaign_id` distincts ;
- aucune unicité inter-capteurs n'est imposée ;
- l'identification centrale est portée par `(device_id, campaign_id)` ;
- pour une campagne terminée sans discontinuité de base de temps, `duration_s = end_timestamp - start_timestamp` ;
- pour une campagne en cours ou traversant une discontinuité temporelle, la relation exacte de `duration_s` aux timestamps reste `NOT_DEFINED` en V1.

### 4.5 Bloc 7 — domaines enum16

Les domaines réservés manquants ont été explicitement fermés pour `system_health_status` et `selftest_status`.

## 5. Propagation dans la validation

Les corrections normatives ont été propagées uniquement là où elles modifiaient effectivement un oracle ou une classification :
- FT-CMD-01 : création d'un test déterministe pour `transaction_id = 0` et code résultat 14 ;
- matrice et audit consolidés FT-CMD : passage du point correspondant de `NOT_DEFINED` à `COVERED` ;
- FT-BLK-05 : unicité locale des campagnes et équation nominale de durée ;
- matrice consolidée FT-BLK : retrait de l'ancienne dette générale sur `duration_s` et conservation explicite des cas encore non définis ;
- FT-OBS-01 : `system_status` et `system_flags` désormais interprétables selon la table normative ;
- FT-OBS-03 : `fault_flags` et `warning_flags` désormais décodables ;
- FT-INT-05 : aucune nouvelle égalité B1↔B7 n'a été ajoutée, les relations exhaustives restant non définies.

Aucune famille gelée non impactée n'a été remaniée pour simple homogénéisation documentaire.

## 6. Résultat des passes transversales

### Pass 0 — intégrité du snapshot

**PASS avec dette documentaire mineure.**

Les écarts identifiés concernent essentiellement des statuts historiques ou formulations de README/VERSION et ne modifient pas l'oracle normatif.

### Pass 1 — propriété normative → propriétaire

**PASS après correction des anomalies B1/B2/B5/B6 et fermeture des domaines enum16.**

Aucune propriété normative V1 indispensable ne reste sans propriétaire clair.

### Pass 2 — traçabilité bidirectionnelle

**PASS.**

Aucun test métier orphelin ou oracle privé bloquant n'a été démontré. Quelques tests de non-surinterprétation restent volontairement utilisés pour empêcher l'invention d'une propriété absente.

### Pass 3 — `NOT_DEFINED` / `CONDITIONAL` / `TRACE_ONLY`

**PASS avec dette normative contrôlée.**

Les `NOT_DEFINED` encore présents correspondent à des comportements non indispensables au contrat V1 ou à des raffinements explicitement reportés à une évolution ultérieure.

### Pass 4 — frontières inter-familles

**PASS.**

Les frontières FT-STR / FT-ACC / FT-LIM / FT-BLK / FT-INT / FT-CMD / FT-SEQ / FT-RBT / FT-PER / FT-OBS restent cohérentes et sans double ownership contradictoire démontré.

### Pass 5 — exécutabilité

**PASS méthodologique.**

Trois classes d'exécution restent utilisées :
- test standard ;
- test instrumenté ;
- fault injection.

Le banc réel devra permettre de construire les stimuli correspondants. Cette exigence porte sur le moyen d'essai, pas sur une lacune du protocole V1.

## 7. Readiness finale A/B/C/D

### A — Spécification suffisamment définie pour implémenter le protocole

**GO.**

Les ambiguïtés indispensables qui empêchaient un gel strict ont été arbitrées et intégrées. Les comportements restant non définis ne sont pas nécessaires pour construire une implémentation V1 conforme dès lors que l'implémentation n'invente pas de garantie supplémentaire côté protocole.

### B — Firmware implémentable sans décision privée indispensable

**GO.**

Le firmware dispose désormais des domaines, états, règles transactionnelles et invariants nécessaires au contrat Modbus V1. Les critères internes produisant certains défauts, avertissements ou diagnostics restent des choix de conception du système TR2 et ne constituent pas une décision protocolaire cachée.

### C — Référentiel de validation permettant un verdict objectif

**GO, avec préconditions de banc explicites.**

Les propriétés possédant un oracle V1 déterministe sont couvertes ou déléguées à leur propriétaire. Les tests dépendant d'un stimulus physique/instrumenté restent `CONDITIONAL` au niveau d'exécution sans dégrader la définition normative.

### D — Centrale capable d'exploiter TR2 selon V1 sans heuristique privée indispensable

**GO pour le contrat V1.**

La centrale peut décoder et utiliser les états, flags, validités, temps, résultats de commande, campagnes et diagnostics exposés. Elle ne doit pas déduire de règles non normées telles qu'une priorité exhaustive entre états, une équivalence B1↔B7 ou une politique globale binaire `exploitable/inexploitable`.

## 8. Verdict global

**GO — baseline Modbus RTU V1 prête pour implémentation et validation sur cible réelle.**

Aucun bloqueur normatif V1 démontré ne subsiste après la passe de correction et de propagation.

Les limites encore ouvertes sont conservées explicitement comme dette V1.1, `NOT_DEFINED`, `CONDITIONAL`, `TRACE_ONLY` ou `DELEGATED` selon leur nature. Elles ne doivent pas être converties en exigences V1 par convenance d'implémentation ou de supervision.

## 9. Points restant hors gel V1

Restent notamment candidats V1.1 :
- relations exhaustives `system_status` / flags / diagnostics B7 ;
- catalogues détaillés `error_code`, `warning_code`, `last_fault_code`, `cmd_result_detail`, `selftest_result_code/detail` ;
- profondeur/durée/persistance de l'idempotence ;
- même `transaction_id` avec payload différent ;
- règle de bouclage/réemploi après `65535` ;
- cycle complet d'annulation ;
- effet détaillé de `clear_request_fields` ;
- priorité entre causes simultanées de refus ;
- critères exhaustifs de santé/intégrité stockage ;
- comportement exact de `duration_s` pendant une campagne en cours ou une discontinuité temporelle ;
- historique diagnostic enrichi ;
- politique d'unicité industrielle de `device_id` ;
- spécification détaillée du banc de validation et des mécanismes de fault injection.

## 10. Condition de gel définitif

Le présent verdict autorise le gel technique de la baseline sur la branche d'audit.

Le merge vers `main` reste soumis au GO explicite du responsable du projet. Aucun merge automatique n'est effectué par ce document.
