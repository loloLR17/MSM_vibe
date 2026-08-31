# FT-LIM-08 — Vue d’ensemble

## Couverture
11 cas génériques et 12 instances couvrent les domaines et invariants normatifs utiles des Blocs 0 et 1.

## Oracles directs
- bits réservés `device_capabilities` à zéro ;
- stabilité des informations d’identification en fonctionnement normal ;
- `last_reset_cause` 0..6 ;
- `storage_status` 0..3 ;
- `acquisition_state` 0..3 ;
- uptime monotone hors reset.

## Contrôles conditionnels
- persistance de `device_id` après reset ;
- unicité entre plusieurs équipements ;
- comportement uptime lors d’un reset réel ;
- persistance des indications de défaut/avertissement lorsqu’une condition est réellement identifiable.

## Non défini / traçabilité
Aucun domaine n’est inventé pour `system_status`, les tables de flags/codes absentes, la température ou les champs `_percent`.

La correction normative du Bloc 1 affectant la table 0..6 à `last_reset_cause` est un prérequis de cette sous-famille.
