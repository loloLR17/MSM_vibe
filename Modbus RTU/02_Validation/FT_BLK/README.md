# FT-BLK — Validation fonctionnelle intra-bloc

## Statut

Famille reconstruite et auditée sur la spécification Modbus RTU V1 gelée. Le présent état est candidat au gel après validation explicite de la passe finale.

## Objet

FT-BLK valide les relations fonctionnelles, invariants, états et comportements observables établissables à l'intérieur d'un seul bloc, sans retester la structure, les permissions, les domaines purs de valeurs ni les relations inter-blocs.

## Sous-familles

- `FT-BLK-01_Etats_invariants` — B1/B2/B3/B7 ;
- `FT-BLK-02_Temps_monotonie_derivations` — B1/B2/B3/B7 ;
- `FT-BLK-03_Supervision_vibratoire` — B3 ;
- `FT-BLK-04_Cycle_configuration` — B4 ;
- `FT-BLK-05_Inventaire_campagnes` — B6 ;
- `FT-BLK-06_Stabilite_delegations` — B0 et classification B5.

## Frontières

- FT-STR : adresses, tailles, types, MSW/LSW, ASCII, réservés, cohérence de snapshot ;
- FT-ACC : RO/RW, accès interdits, exceptions Modbus, absence d'effet de bord d'un accès rejeté ;
- FT-LIM : bornes, valeurs hors domaine, enums invalides, bits réservés, index invalides en tant que domaine ;
- FT-INT : relations entre blocs ;
- FT-CMD : moteur transactionnel B5 et effets commandés ;
- FT-SEQ : séquences métier complètes ;
- FT-RBT : répétitions, pertes, concurrence et dégradations protocole ;
- FT-PER : reboot, coupure, persistance et reprise.

## Règle d'oracle

Aucun comportement n'est inventé. Toute règle insuffisamment définie est classée `NOT_DEFINED`, `CONDITIONAL`, `TRACE_ONLY` ou `DELEGATED` avec sa limite ou sa destination.

## Documents de famille

- `Specifications.md` — cadrage et doctrine de la famille ;
- `MATRICE_COUVERTURE_FT_BLK_V1.md` — couverture consolidée et dettes normatives.
