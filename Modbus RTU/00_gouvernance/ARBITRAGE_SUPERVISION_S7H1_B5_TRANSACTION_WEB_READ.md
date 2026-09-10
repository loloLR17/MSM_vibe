# Arbitrage supervision S7-H1 — Projection Web des transactions B5

## Statut

Décision d’implémentation supervision PC, tranche S7-H1.

## Objet

Exposer à l’IHM l’état durable des transactions B5 sans créer de seconde autorité transactionnelle et sans transformer une preuve terminale en succès métier supposé.

## Source d’autorité

La source est exclusivement `ICommandTransactionJournal`, déjà persistée en SQLite. Les événements V1 existants sont :

- `Prepared` ;
- `Submitted` ;
- `Ambiguous` ;
- `TerminalEvidenceObserved`.

Aucun état supplémentaire n’est inventé côté Web.

## Projection

Pour chaque couple `(transaction_id, request_identity)`, la projection retient le dernier événement persistant observé. Les transactions sont retournées de la plus récente à la plus ancienne, avec une fenêtre Web bornée à 20 transactions.

Le contrat de présentation contient uniquement : `deviceId`, `transactionId`, `requestIdentity`, `state`, `observedAt`.

`TerminalEvidenceObserved` signifie uniquement qu’une preuve terminale B5 a été observée. Cette tranche ne l’interprète pas comme un succès de la commande.

## API

`GET /api/v1/devices/{deviceId}/commands`

retourne les états transactionnels issus du journal durable. Le même chemin reste utilisé en `POST` pour soumettre une commande B5 ; la distinction repose sur la méthode HTTP.

Le Web n’accède jamais directement à SQLite. `RuntimeCommandSink` implémente également la source de lecture afin que le `SupervisionWebHost` réutilise exactement la même composition runtime déjà injectée.

## Invariants

- aucun nouveau cache transactionnel Web ;
- aucune nouvelle machine d’état ;
- `Ambiguous` reste un état de premier rang ;
- aucune résolution `Retry`, `Ignore` ou `Force` ;
- aucune relance automatique ;
- une reconnexion seule ne résout pas `Ambiguous` ;
- la persistance existante reste l’autorité après redémarrage ;
- aucune conclusion « succès TR2 » n’est dérivée d’un HTTP 202 ni de `TerminalEvidenceObserved`.

## Hors périmètre H1

- blocage visuel des boutons selon l’état transactionnel ;
- workflow opérateur de récupération ;
- détail du résultat terminal B5 ;
- résolution `Ambiguous` ;
- modifications firmware ou protocole.

## Validation attendue

- compilation inchangée des write paths G1 à G4 ;
- API de lecture disponible avec une source d’autorité ;
- 404 si aucune source de lecture transactionnelle n’est fournie ;
- sérialisation des états sous leur nom explicite ;
- aucune représentation artificielle de succès.
