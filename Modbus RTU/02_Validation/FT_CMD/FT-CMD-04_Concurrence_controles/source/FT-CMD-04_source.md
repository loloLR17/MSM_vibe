# FT-CMD-04 — Source normative consolidée

## 1. Sources

Source principale : `Modbus RTU/01_Specification_source/bloc5.md` V1.

Délégations antérieures : FT-BLK réserve au moteur FT-CMD la concurrence, l'annulation et le nettoyage ; FT-INT conserve les effets inter-blocs des commandes abouties.

## 2. Concurrence

Le Bloc 5 impose :
- une seule commande active à la fois ;
- si une nouvelle commande arrive alors qu'une commande est déjà en cours, elle est refusée ;
- `cmd_result_code = 13` signifie `commande déjà en cours` ;
- le périmètre ne prévoit pas de file de commandes.

### Limite normative

Le Bloc 5 expose un seul couple `cmd_active_code` / `cmd_active_transaction_id` et impose par ailleurs une corrélation stricte de la réponse avec l'identifiant envoyé.

La V1 ne précise pas, lors du refus de B pendant A :
- si les champs `cmd_active_*` restent attachés à A ;
- s'ils exposent B pour corréler son refus ;
- ni comment les deux informations seraient simultanément représentées.

Le résultat `13` est normatif ; cette représentation exacte est `NOT_DEFINED`.

## 3. Annulation

`cmd_request_control.bit1 = cancel_request` est défini comme une demande d'annulation si la commande courante est annulable.

`cmd_engine_flags.bit9` indique : `annulation supportée pour la commande courante`.

`cmd_result_code = 15` signifie : `commande non annulable`.

### Limites normatives

La V1 ne définit pas :
- la liste des commandes annulables ;
- une commande donnée comme obligatoirement annulable ;
- le moment précis où une annulation reste recevable ;
- le statut final d'une annulation réussie ;
- un code résultat dédié au succès d'annulation ;
- les effets partiels éventuels d'une commande interrompue.

En conséquence :
- l'existence du mécanisme est traçable ;
- un test de refus code `15` n'est exécutable que si le DUT expose une commande courante avec bit9=0 dans une situation contrôlable ;
- aucun scénario de succès d'annulation n'est normé en V1.

## 4. Nettoyage des champs de requête

`cmd_request_control.bit2 = clear_request_fields` est défini comme une demande de remise à zéro des champs de requête.

La V1 ne définit cependant pas explicitement :
- la liste exacte des champs remis à zéro ;
- si le registre de contrôle lui-même est concerné ;
- le timing du nettoyage ;
- son interaction avec une commande active ;
- son interaction avec les champs `cmd_active_*` et l'historique.

Il est donc interdit de fabriquer un oracle du type `5000..5007 = 0` après activation.

## 5. Frontière avec FT-RBT

FT-CMD-04 vérifie l'oracle fonctionnel transactionnel directement défini par le Bloc 5. Les répétitions agressives, pertes de réponse, temporisations hostiles et stress de concurrence relèvent de FT-RBT.

## 6. Dettes candidates V1.1

À reporter au rapport d'audit final :
1. définir la représentation/corrélation du refus concurrent ;
2. définir la liste ou la règle de détermination des commandes annulables ;
3. définir le cycle de vie et le résultat d'une annulation réussie ;
4. définir précisément la sémantique de `clear_request_fields`.
