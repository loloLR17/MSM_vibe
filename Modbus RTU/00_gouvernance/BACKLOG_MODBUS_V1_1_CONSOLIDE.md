# Modbus RTU TR2 — Backlog consolidé V1.1

## Statut

Ce document regroupe les évolutions candidates identifiées pendant l'audit transversal V1. Il est informatif et non normatif pour la V1.

Aucun point de cette liste ne doit être utilisé comme oracle V1 tant qu'il n'a pas été arbitré, intégré dans une spécification future puis propagé dans les validations concernées.

## B0 — Identification

- politique industrielle de génération et de garantie d'unicité de `device_id` ;
- compatibilité entre versions matérielles, firmware et protocole ;
- immutabilité du numéro de série ;
- information de localisation/installation ;
- terminologie explicite des frontières de persistance.

## B1 — État système

- relation exhaustive `system_status` ↔ `system_flags` / `fault_flags` / `warning_flags` ;
- domaines explicites des pourcentages si nécessaire ;
- catalogues `error_code` et `warning_code` ;
- priorité du code erreur/avertissement principal ;
- relation `acquisition_state` ↔ `active_campaign_id` ;
- plages physiques de température.

## B2 — Temps

- machine d'état exhaustive de `time_status` ;
- critères d'entrée/sortie de DEGRADED ;
- règle de récupération et priorités éventuelles entre causes temporelles.

## B5 — Commandes

- profondeur et durée de mémoire d'idempotence ;
- même `transaction_id` avec payload différent ;
- politique de réutilisation/bouclage des IDs ;
- représentation des refus concurrents ;
- liste des commandes annulables et cycle de succès d'annulation ;
- effet précis de `clear_request_fields` ;
- catalogue `cmd_result_detail` ;
- priorité des causes simultanées de refus ;
- masque SELFTEST étendu si conservé ;
- portée exacte de REFRESH ;
- règles de mode maintenance et emploi des codes associés ;
- définition des opérations critiques pour RESET SOFTWARE ;
- portée et masque éventuel de RESET STATISTICS.

## B6 — Inventaire campagnes

- réutilisation historique des `campaign_id` après suppression ;
- règle exacte de `duration_s` pour campagne en cours ;
- règle exacte de durée lorsqu'une discontinuité temporelle intervient ;
- invariant `valid_campaign_count <= total_campaign_count` si souhaité ;
- relation stockage utilisé/libre/capacité ;
- critères de `storage_health_status` ;
- définition détaillée de `data_integrity_status` ;
- politique explicite des valeurs exposées quand `selected_campaign_valid = 0` si nécessaire ;
- copie normative de configuration/contexte par campagne si besoin d'exploitation ;
- CRC/hash de campagne si une vérification d'intégrité forte est requise ;
- ordre stable de la liste lorsque l'inventaire évolue.

## B7 — Diagnostic

- catalogue non nul de `last_fault_code` ;
- catalogues `selftest_result_code` et `selftest_detail` ;
- plages fonctionnelles température/tension ;
- invariants essentiels `system_fault_flags` ↔ `system_health_status` si réellement requis ;
- priorité entre défauts multiples ;
- relation explicite entre données dupliquées B1/B7 ou déclaration formelle d'absence de garantie d'égalité ;
- effets observables détaillés du SELFTEST ;
- convention de `last_fault_timestamp` lorsqu'aucun défaut n'est connu.

## Transversal

- politique de versionnement/compatibilité centrale-capteur ;
- harmonisation documentaire des classifications propriété vs test lorsque nécessaire ;
- fermeture des dettes documentaires historiques dans les README/version files ;
- spécification du banc matériel, préconditions et moyens d'essai ;
- observables discriminants nécessaires à la preuve d'idempotence ;
- éventuelle politique synthétique d'exploitabilité globale uniquement si un besoin système concret le justifie.

## Processus de promotion

Pour promouvoir un candidat en V1.1 :
1. arbitrer le comportement sans hypothèse implicite ;
2. modifier la spécification source propriétaire ;
3. vérifier charte de typage et mapping ;
4. propager vers la famille de validation propriétaire ;
5. mettre à jour les dépendances transversales ;
6. ajouter ou modifier l'oracle de test ;
7. effectuer une passe de non-régression avant gel.