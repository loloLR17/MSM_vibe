# S7-H2 — Exploitation IHM des états transactionnels B5

## Objet

Cette tranche raccorde la vue **Commandes** à la lecture transactionnelle introduite en S7-H1 afin que l'opérateur voie l'état durable de la transaction B5 courante et que l'IHM n'offre pas une nouvelle commande lorsqu'une transaction non terminale est connue.

## Autorité

La source reste exclusivement le journal B5 persistant déjà utilisé par `ISupervisionCommandHistoryReadSource`.

L'IHM Web ne crée aucun état transactionnel parallèle et ne déduit aucun succès métier à partir d'une réponse HTTP 202.

## États présentés

Les états exposés restent exactement ceux de S7-H1 :

- `Prepared` ;
- `Submitted` ;
- `Ambiguous` ;
- `TerminalEvidenceObserved`.

Aucun autre état n'est inventé.

## Politique de blocage IHM

La transaction la plus récente, telle que retournée par `GET /api/v1/devices/{deviceId}/commands`, gouverne l'interverrouillage visuel :

- `Prepared` : nouvelles commandes B5 désactivées ;
- `Submitted` : nouvelles commandes B5 désactivées ;
- `Ambiguous` : nouvelles commandes B5 désactivées ;
- `TerminalEvidenceObserved` : aucun blocage Web issu de cette transaction.

L'autorité réelle de rejet reste le moteur transactionnel serveur. Le blocage navigateur n'est qu'une protection opérateur supplémentaire.

## Échec de lecture de l'historique

Si l'état transactionnel ne peut pas être relu, les commandes B5 restent désactivées par prudence. L'IHM indique que l'état transactionnel est indisponible et ne transforme pas cet échec Web en état TR2.

## Ambiguous

`Ambiguous` reste un état de premier rang :

- aucune nouvelle commande n'est proposée ;
- aucune action Retry / Ignore / Force n'est ajoutée ;
- une reconnexion seule n'est pas considérée comme une résolution ;
- la résolution reste du ressort des mécanismes de réconciliation B5 existants et sera traitée séparément dans la suite S7-H.

## Historique affiché

La vue Commandes affiche les transactions retournées par S7-H1, bornées côté serveur, avec :

- transaction_id ;
- request_identity ;
- état ;
- date d'observation du dernier événement persistant.

`TerminalEvidenceObserved` est présenté comme « preuve terminale observée », jamais comme « succès ».

## Rafraîchissement

Le navigateur relit l'état transactionnel périodiquement. Aucun polling Web ne provoque de lecture Modbus supplémentaire : il lit uniquement la projection issue du journal persistant.

Après un POST accepté en 202, une relecture de l'historique est déclenchée ; le 202 reste une mise en file et non une preuve d'exécution.

## Hors périmètre H2

- mécanisme de résolution manuelle d'`Ambiguous` ;
- Retry / Ignore / Force ;
- nouvelle logique de réconciliation ;
- modification du journal ou du moteur B5 ;
- interprétation de `TerminalEvidenceObserved` comme succès métier ;
- modification B6.
