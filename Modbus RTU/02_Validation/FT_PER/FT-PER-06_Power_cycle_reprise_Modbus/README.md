# FT-PER-06 — Power cycle et reprise générale Modbus

## 1. Objet

FT-PER-06 ferme la famille FT-PER en auditant ce que la V1 permet réellement d'affirmer après une mise sous tension / power cycle et lors du retour de l'interface Modbus.

## 2. Oracle V1 disponible

La V1 distingue explicitement les causes du dernier reset :

- `1` = power-on ;
- `2` = reset logiciel ;
- `3` = watchdog ;
- `4` = brown-out ;
- `5` = reset externe ;
- `6` = mise à jour firmware.

Ainsi, lorsqu'un moyen d'essai permet de provoquer sans ambiguïté une nouvelle mise sous tension, la cause post-boot doit identifier un `power-on` dans les champs normatifs de cause de reset.

## 3. Ce que la V1 ne définit pas

Aucune exigence V1 n'impose :

- une durée maximale de boot ;
- une durée maximale avant première réponse Modbus valide ;
- une séquence observable de démarrage ;
- une réponse Modbus particulière pendant l'initialisation ;
- un état global initial déterminé ;
- une configuration par défaut générale ;
- une équivalence de persistance entre RESET SOFTWARE et power cycle ;
- une politique générale de reprise par cause de reset.

Ces points restent `NOT_DEFINED` ou `TRACE_ONLY`.

## 4. Test actif

- `TT-PER-B01B07-002` — power cycle contrôlé puis cause `power-on` observable (`CONDITIONAL`).

La disponibilité Modbus et le temps de reprise sont relevés comme traces uniquement.

## 5. Frontières

- RESET SOFTWARE : FT-PER-01 ;
- persistance `device_id` : FT-PER-02 ;
- configuration : FT-PER-03 ;
- moteur transactionnel : FT-PER-04 ;
- acquisition/campagnes/diagnostic : FT-PER-05 ;
- robustesse des échanges Modbus hors reboot : FT-RBT.

## 6. Doctrine

Le retour d'une réponse Modbus valide démontre seulement que l'interface est redevenue exploitable. En l'absence de délai normatif, la durée de reprise ne peut pas constituer un verdict V1.

De même, une simple perte de communication ne suffit pas à qualifier la cause du reboot : le verdict utilise les champs normatifs `last_reset_cause` et `reset_cause`.

## 7. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
