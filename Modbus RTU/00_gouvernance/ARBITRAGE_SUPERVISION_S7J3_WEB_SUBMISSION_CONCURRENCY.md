# S7-J3 — Concurrence des soumissions Web par device_id

## Statut

Arbitrage et implémentation de la tranche S7-J3.

## Constat

Après S7-J1 et S7-J2, le scheduler, le registre de coordinators et le lifecycle H3 sont protégés contre les courses identifiées. Le point d'entrée Web `RuntimeCommandSink` conservait toutefois un `SemaphoreSlim` unique pour toutes les soumissions B5 et B6.

Ce verrou global garantissait la sérialisation, mais il sérialisait aussi artificiellement des requêtes visant des TR2 différents. Cette portée est plus large que nécessaire et contredit l'objectif S7-J de conserver l'indépendance des équipements tant que l'autorité bus reste assurée par `BusWorkScheduler`.

## Décision

La sérialisation du sink Web est portée par `device_id` :

- un `SemaphoreSlim` est obtenu atomiquement par `device_id` ;
- B5 et B6 d'un même TR2 partagent ce verrou de soumission ;
- deux TR2 différents n'utilisent pas le même verrou Web ;
- le `BusWorkScheduler` reste l'unique sérialisation physique par bus ;
- le verrou Web ne devient jamais une seconde autorité Modbus.

Les verrous sont conservés pour la durée du runtime. Le parc TR2 étant borné par les équipements effectivement adressés, aucune politique d'éviction concurrente n'est introduite dans S7-J3.

## Invariants conservés

1. Une seule composition de supervision et un seul propriétaire Modbus.
2. Une seule transaction B5 non terminale par `device_id`.
3. `requestIdentity` reste contrôlé par device avant nouvelle soumission B5.
4. B6 reste distinct de B5.
5. Aucune écriture Modbus n'est réalisée par le thread HTTP.
6. Aucun replay automatique B5.
7. Le scheduler reste responsable de la sérialisation physique par bus.
8. Aucun verrou global Web ne bloque artificiellement des TR2 différents.

## Hors périmètre

- paralléliser des transactions sur un même bus physique ;
- modifier les priorités du scheduler ;
- modifier les règles H3 ;
- ajouter une politique d'éviction des verrous par device ;
- modifier le protocole V1.
