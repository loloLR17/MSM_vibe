# S7-J1 — Sûreté de concurrence à l’entrée Web

## Statut

Tranche d’intégration multi-client de la supervision Web S7.

Baseline de départ :

`ff977fae3d7993847a979879e5c12407b655eaed`

`Supervision: freeze S7-I web UX wiring`

## Objet

S7-J1 traite uniquement les courses pouvant être introduites par plusieurs requêtes HTTP simultanées vers le runtime de supervision partagé.

Elle ne modifie ni le protocole Modbus V1, ni le firmware, ni les règles transactionnelles B5 gelées en S7-H3.

## Risques constatés

Avant S7-J1 :

- `CommandCoordinatorRegistry` utilisait un `Dictionary` mutable sans synchronisation ;
- `BusWorkScheduler` utilisait des collections et compteurs mutables sans synchronisation ;
- deux demandes B5 simultanées pour le même `device_id` pouvaient entrer concurremment dans la préparation ;
- un work prioritaire pouvait être rendu visible au runner avant que son contexte applicatif associé soit publié.

Ces hypothèses étaient acceptables dans les phases mono-flux précédentes mais ne sont plus suffisantes avec un serveur HTTP multi-client.

## Décisions S7-J1

### Registre de coordinators

`CommandCoordinatorRegistry` devient synchronisé et expose `GetOrAdd` comme opération atomique.

Invariant : un seul `CommandCoordinator` est publié par `device_id`.

### Scheduler

`BusWorkScheduler` sérialise ses mutations internes : allocation des `work_id`, file pending, work actif par bus et complétion.

L’invariant historique reste inchangé : au plus un work actif par bus.

Un overload de `QueuePriority` permet de publier le contexte associé via `initializeBeforePublish` avant l’ajout du work à la file visible. Ce callback n’est pas une nouvelle couche métier ; il garantit uniquement l’atomicité de publication work/contexte.

### Commandes B5 Web

`SupervisionOperationalFacade` sérialise la préparation des commandes par `device_id` avec un gate dédié.

Conséquences :

- deux clients peuvent appeler l’API simultanément ;
- pour un même TR2, une seule préparation B5 peut progresser à la fois ;
- après la première transaction non terminale, les suivantes restent rejetées par l’invariant B5 existant ;
- des `device_id` différents ne sont pas artificiellement bloqués par un verrou global ;
- aucun replay n’est ajouté.

Le work `CommandTransaction` et son `B5CommandRequest` sont publiés atomiquement vis-à-vis du runner.

La sélection B6 utilise la même règle de publication work/contexte mais reste strictement distincte de B5.

## Invariants préservés

- runtime de supervision unique ;
- propriétaire Modbus unique ;
- un seul work actif par bus ;
- une seule transaction B5 non terminale par `device_id` ;
- `Prepared`, `Submitted`, `Ambiguous` inchangés ;
- aucune relance automatique ;
- B6 distinct de B5 ;
- aucun comportement V1.1 importé ;
- aucune nouvelle autorité de données.

## Validation ajoutée

S7-J1 ajoute :

1. un test de charge concurrente du scheduler vérifiant unicité des work IDs, contexte publié avant consommation et unicité du work actif par bus ;
2. un test de demandes B5 concurrentes sur le même `device_id` vérifiant qu’une seule commande non terminale est préparée et qu’un seul coordinator existe.

## Hors périmètre immédiat

La concurrence interne du lifecycle physique H3 (`monitoring`, `reconciliation`, anti-duplication) est auditée séparément en S7-J2 afin de conserver une tranche J1 cohérente et limitée à l’entrée Web et aux primitives partagées.
