# FT-SEQ — Scénarios séquentiels complets

## Statut

Famille en cours de reconstruction pour la V1 sur la branche d'audit dédiée.

Le contenu courant de `main` reste le référentiel gelé. FT-SEQ ne sera mergée qu'après validation explicite de chaque sous-famille, consolidation, audit final et GO de gel.

## Objet

FT-SEQ valide les séquences fonctionnelles complètes nécessaires à l'exploitation du capteur TR2 via Modbus RTU.

Elle compose les oracles déjà gelés sans en reprendre la propriété.

## Sous-familles retenues

1. `FT-SEQ-01_Qualification_initiale_contexte`
2. `FT-SEQ-02_Preparation_activation_configuration`
3. `FT-SEQ-03_Preparation_synchronisation_temps`
4. `FT-SEQ-04_Demarrage_acquisition_ouverture_campagne`
5. `FT-SEQ-05_Arret_cloture_consultation_campagne`
6. `FT-SEQ-06_Cycle_nominal_complet_campagne`
7. `FT-SEQ-07_Refus_reprise_sequence`

## Frontière de propriété

FT-SEQ possède uniquement l'oracle portant sur l'enchaînement complet.

Les propriétés élémentaires restent déléguées à FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT et FT-CMD. Les défauts de communication restent à FT-RBT et les comportements après reboot à FT-PER.

## Doctrine V1

- aucune exigence inventée ;
- aucune recommandation métier promue en exigence ;
- aucune séquence intermédiaire imposée sans source V1 ;
- aucune tolérance temporelle créée ;
- toute ambiguïté reste visible via `NOT_DEFINED`, `TRACE_ONLY` ou `CONDITIONAL` ;
- les évolutions souhaitables sont réservées à un fichier V1.1 séparé lors de la clôture.

Voir `Specifications.md` pour la doctrine détaillée.
