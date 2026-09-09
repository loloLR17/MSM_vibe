# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-C — Startup, ouverture SQLite et recovery B5

Date : 2026-09-09

Cette tranche complète S3-A et S3-B. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Objectif

S3-C introduit la barrière de démarrage du host de supervision :

1. la composition runtime existe mais n'est pas exploitable ;
2. SQLite est ouvert, configuré et migré selon S2 ;
3. les `device_id` présents dans le journal B5 durable sont énumérés ;
4. un `CommandCoordinator` est reconstruit pour chacun ;
5. tout état B5 non terminal est restauré `Ambiguous` par `CommandCoordinatorRecoveryService` ;
6. seulement après succès complet, la barrière runtime passe à `Ready`.

## 2. Classification

- schéma SQLite, WAL, `synchronous=FULL`, migrations et stores : **S2 gelé** ;
- sémantique B5 durable et recovery non terminal -> `Ambiguous` : **S1/S2 gelé** ;
- ordre de startup et barrière `Ready` : **SUPERVISION_POLICY S3-C** ;
- startup one-shot sans réparation silencieuse : **SUPERVISION_POLICY S3-C** ;
- énumération SQLite des `device_id` réellement présents dans le journal B5 : mécanisme host nécessaire à S3-C ;
- aucune identité dérivée de l'adresse Modbus : invariant S0/S1 conservé.

## 3. Gap résolu

La configuration S3-A connaît des bus et adresses Modbus, mais ne connaît pas les `device_id` avant découverte B0.

Le recovery B5 durable est au contraire indexé par `device_id`.

S3-C interdit donc de fabriquer un `device_id` depuis `(bus, adresse)`. Le journal SQLite fournit uniquement la liste distincte des identités qu'il contient déjà réellement.

`SqliteCommandTransactionJournal.ReadDeviceIdsAsync()` est ajouté pour cette lecture.

Cette méthode ne modifie pas `ICommandTransactionJournal` : l'énumération est un besoin concret du host SQLite, pas un nouveau contrat métier général de l'Application.

## 4. Barrière de readiness

`RuntimeReadinessGate` est fermée à la composition.

`EnsureReady()` échoue tant que le startup n'est pas terminé.

La barrière n'est ouverte qu'après :

- ouverture/migration SQLite réussie ;
- lecture des identités B5 réussie ;
- création des coordinateurs réussie ;
- recovery de chaque historique réussi.

Une exception laisse la barrière fermée.

## 5. Politique d'échec

Le startup est one-shot.

En cas d'erreur SQLite, migration, données B5 invalides ou recovery incohérent :

- pas de correction automatique ;
- pas de suppression de journal ;
- pas de passage en `Ready` ;
- pas de poursuite opérationnelle sur un runtime partiellement reconstruit.

Une reprise dans le même objet startup n'est pas autorisée. Le process devra être arrêté/recréé après traitement de la cause.

## 6. États B5 au redémarrage

Pour chaque `device_id` connu du journal :

- historique terminal : coordinateur restauré sans transaction active ;
- historique non terminal : transaction active restaurée `Ambiguous` ;
- historique incohérent : startup en échec.

Aucun replay automatique de commande n'est ajouté.

Les réservations sans événement de journal restent conformes à S2 : elles n'inventent pas de transaction active. Elles seront naturellement prises en compte par le store lors d'une future allocation pour ce `device_id`.

## 7. Hors périmètre

S3-C n'introduit pas :

- transport série/RS-485 ;
- découverte B0 active ;
- polling ;
- exécution de commande ;
- retry automatique ;
- lifecycle long-running ou shutdown ;
- Web/API ;
- Windows Service ;
- V1.1.

Ces sujets restent aux tranches suivantes.

## 8. Validation attendue

Les tests S3-C doivent prouver au minimum :

- barrière fermée avant startup ;
- création/migration de la base au startup ;
- ouverture de la barrière après succès ;
- restauration d'une transaction non terminale en `Ambiguous` après fermeture/réouverture logique ;
- historique terminal sans transaction active restaurée ;
- caractère one-shot du startup.
