# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-D — Lifecycle runtime, cancellation et shutdown

Date : 2026-09-09

Cette tranche complète S3-C. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Objectif

S3-D introduit un lifecycle host minimal et explicite :

1. `Created` ;
2. `Starting` pendant le startup/recovery S3-C ;
3. `Running` uniquement après ouverture de la readiness ;
4. `Stopping` sur demande d'arrêt ;
5. fermeture de la readiness avant fin d'arrêt ;
6. `Stopped` après arrêt propre ;
7. `Faulted` si le startup ou le runtime échoue hors annulation demandée.

## 2. Classification

- startup/recovery SQLite + B5 : **S3-C gelé par validation locale** ;
- lifecycle host et états : **SUPERVISION_POLICY S3-D** ;
- `CancellationToken` comme autorité d'arrêt du host : **SUPERVISION_POLICY S3-D** ;
- fermeture de la readiness au shutdown/fault : **SUPERVISION_POLICY S3-D** ;
- aucun comportement firmware, Modbus V1.1 ou transport physique n'est ajouté.

## 3. Autorité de lifecycle

`SupervisionRuntimeHost` est l'unique objet S3-D qui porte l'état long-running du host.

Il est one-shot : une même instance ne peut pas être relancée après `Stopped` ou `Faulted`.

Cette règle conserve la politique one-shot de S3-C et évite de réutiliser un graphe runtime potentiellement partiellement consommé.

## 4. Cancellation

L'arrêt demandé passe par le `CancellationToken` fourni à `RunAsync`.

Une annulation demandée :

- n'est pas traitée comme un fault ;
- ferme la readiness ;
- conduit à `Stopped` ;
- ne déclenche aucun retry ou restart automatique.

Une annulation déjà demandée avant le startup conduit également à `Stopped` sans ouverture opérationnelle.

## 5. Readiness

La readiness représente désormais l'aptitude opérationnelle du host, pas seulement la fin du startup.

Elle est :

- fermée avant S3-C ;
- ouverte uniquement après startup/recovery réussi ;
- fermée dès le shutdown ;
- fermée sur fault.

`EnsureReady()` reste la barrière à utiliser par les futures façades opérationnelles.

## 6. Shutdown S3-D

À ce stade, aucun transport physique, aucune boucle de polling et aucune tâche longue métier ne sont encore actifs.

S3-D n'invente donc pas de délai de drain, de timeout d'arrêt ou de séquence de fermeture matériel.

La tranche S3-E devra raccorder ses futures tâches à ce lifecycle et respecter le token d'arrêt.

## 7. Hors périmètre

S3-D n'introduit pas :

- polling ;
- cadence ;
- transport série/RS-485 ;
- découverte B0 active ;
- exécution B5 ;
- retry ;
- Windows Service ;
- Web/API ;
- signal OS spécifique ;
- timeout de shutdown arbitraire ;
- V1.1.

## 8. Validation attendue

Les tests doivent prouver au minimum :

- passage à `Running` après startup réussi ;
- readiness ouverte en `Running` ;
- cancellation -> `Stopped` ;
- readiness fermée après cancellation ;
- token déjà annulé -> arrêt sans readiness ;
- host one-shot.
