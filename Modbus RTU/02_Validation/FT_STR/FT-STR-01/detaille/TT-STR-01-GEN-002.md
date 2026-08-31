# TT-STR-01-GEN-002 — Continuité interne et absence de chevauchement

## Objectif

Vérifier génériquement que les champs constituant un bloc couvrent sa portée conformément à V1, sans trou interne non défini et sans chevauchement.

## Entrées

- liste ordonnée des plages de champs du bloc ;
- bornes normatives du bloc.

## Contrôles

1. Ordonner les champs par première adresse.
2. Vérifier qu'aucune plage ne recouvre une plage précédente.
3. Vérifier que l'union des plages couvre toute la portée normative du bloc.
4. Tout trou interne éventuel doit être explicitement prévu par V1 ; à défaut il est non conforme.

## Résultat attendu

La couverture interne est déterministe, complète selon V1 et sans chevauchement.

## Hors périmètre

Ce test ne vérifie ni les valeurs lues ni la capacité du serveur Modbus à accepter une forme particulière de requête.

## Instanciation

Ce test est instancié par `TT-STR-01-B0-001` à `TT-STR-01-B7-001`.
