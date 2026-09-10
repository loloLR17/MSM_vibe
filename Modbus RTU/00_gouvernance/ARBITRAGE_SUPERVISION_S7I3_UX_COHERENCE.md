# S7-I3 — Cohérence UX des vues réelles

## Statut

Passe de cohérence de l'IHM réelle S7 contre la baseline UX S6-H, après validation locale de S7-I2.

## Constat

La vue globale `Attention & diagnostic` agrège plusieurs sources distinctes : vibration B3, état/défaut TR2 B1, communication PC et opérations B5.

Le premier câblage S7-I1 ignorait silencieusement un échec HTTP lors de la lecture de l'historique B5 d'un TR2. Cette absence silencieuse pouvait conduire à afficher qu'aucune situation ne nécessite d'attention alors que l'état transactionnel B5 était simplement indisponible.

## Décision

Un échec de lecture de `/api/v1/devices/{deviceId}/commands` devient une ligne d'attention explicite de source `Supervision PC` :

- il n'est pas converti en défaut TR2 ;
- il n'est pas converti en état `Ambiguous` ;
- il n'invente aucune sévérité métier ;
- il indique seulement que l'état transactionnel B5 n'a pas pu être lu pour ce `device_id` ;
- les autres sources restent affichées indépendamment.

## Invariants confirmés

- les dernières valeurs B3 connues restent affichables après indisponibilité et sont explicitement qualifiées ;
- les grandeurs globales B3 restent celles fournies par le TR2, sans recalcul PC ;
- aucune vitesse mm/s, FFT, fréquence dominante, ISO severity ou score santé n'est créé ;
- B1, B3, B7, communication PC et opération B5 restent des familles distinctes ;
- B6 reste un inventaire de métadonnées et sa sélection reste distincte de B5 ;
- aucune action Web nouvelle n'est introduite ;
- aucun changement de la spécification Modbus V1 ni du firmware.

## Audit des libellés normatifs

Les mappings opérateur déjà utilisés pour `system_status`, `storage_status`, `acquisition_state`, `last_reset_cause`, `B3_STATUS_GLOBAL`, `B3_SEVERITY_GLOBAL`, `B3_DOMINANT_AXIS`, `system_health_status`, `selftest_status` et `reset_cause` ont été relus contre `bloc1.md`, `bloc3.md` et `bloc7.md`. Aucun écart nécessitant une correction n'a été identifié dans cette tranche.
