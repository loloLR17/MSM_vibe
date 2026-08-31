# FT-CMD-04 — Concurrence et contrôles

## Objet

Cette sous-famille valide les règles normatives du moteur de commandes relatives à la concurrence, à la demande d'annulation et au nettoyage des champs de requête.

Elle ne complète pas les zones que la V1 laisse ouvertes.

## Périmètre

Inclus :
- unicité de la commande active ;
- refus d'une nouvelle commande lorsqu'une commande est déjà en cours ;
- code résultat `13` pour ce refus ;
- existence de `cancel_request` ;
- indicateur `cmd_engine_flags.bit9` d'annulation supportée pour la commande courante ;
- code résultat `15` pour une commande non annulable ;
- existence de `clear_request_fields`.

Exclus ou différés :
- robustesse temporelle hostile et pertes de trames : FT-RBT ;
- règles propres aux commandes : FT-CMD-05 à FT-CMD-07 ;
- structure et accès des registres : FT-STR / FT-ACC ;
- effets inter-blocs : FT-INT.

## Classification stricte

- concurrence et refus code `13` : `COVERED` ;
- représentation exacte de la commande refusée dans `cmd_active_*` : `NOT_DEFINED` ;
- existence du mécanisme d'annulation : `TRACE_ONLY` ;
- refus d'annulation lorsqu'une commande identifiée comme non annulable est observable : `CONDITIONAL` ;
- liste des commandes annulables : `NOT_DEFINED` ;
- état final d'une annulation réussie : `NOT_DEFINED` ;
- existence de `clear_request_fields` : `TRACE_ONLY` ;
- effet exact et timing du nettoyage : `NOT_DEFINED`.

## Tests instanciés

- `TT-CMD-B05-300` — refus d'une nouvelle commande pendant une commande en cours ;
- `TT-CMD-B05-301` — absence de seconde commande active ;
- `TT-CMD-B05-302` — annulation d'une commande explicitement signalée non annulable, uniquement si cette situation est observable sans hypothèse.

Aucun test de succès d'annulation ni de contenu exact après `clear_request_fields` n'est créé en V1.

## Point de vigilance

Le refus de la transaction B pendant l'exécution de A est normé par le résultat `13`, mais la V1 ne précise pas si `cmd_active_transaction_id` reste celui de A ou expose temporairement B. Aucun oracle n'est inventé sur ce point.

## Statut

Reconstruite sur branche d'audit. En attente de validation avant merge.
