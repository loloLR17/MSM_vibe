# Arbitrage supervision S7-G1 — Fondation commandes B5 Web

## Statut

Décision d’implémentation supervision PC, tranche S7-G1.

## Objet

Ouvrir un premier write path Web B5 sans contourner l’autorité transactionnelle existante et sans exposer directement les registres Modbus ni les paramètres bruts du Bloc 5.

## Décisions

- Le navigateur ne parle jamais Modbus directement.
- Toute commande Web passe par `SupervisionOperationalFacade.QueueCommandAsync(...)` dans le runtime existant.
- `TR2.Supervision.Web` définit uniquement un contrat de présentation et une interface `ISupervisionCommandSink` ; l’adaptateur concret reste dans `TR2.Supervision.Service` afin d’éviter toute dépendance Web vers Service.
- G1 n’autorise que les commandes V1 à paramètres déterministes et non protégées : `APPLY_CONFIG`, `SYNC_TIME`, `START_ACQUISITION`, `STOP_ACQUISITION`, `SELFTEST` standard, `REFRESH_INDICATORS`, `ENTER_MAINTENANCE`, `EXIT_MAINTENANCE`.
- Le Web n’accepte pas de `code`, `param1`, `param2`, `param3` ou `confirm_key` libres. Le mapping vers `B5CommandIntent` est centralisé côté Service.
- `ACKNOWLEDGE_FAULT` reste hors G1 car son contrat nécessite le code défaut et le mode unitaire/global.
- `SOFTWARE_RESET` reste hors G1 car il s’agit d’une commande protégée nécessitant `confirm_key = 0xA55A` et une interaction dédiée.
- `RESET_STATISTICS` reste absente : le scope firmware resettable est `EMPTY`.
- Chaque requête HTTP doit porter un `requestIdentity` opaque non vide, limité à 128 caractères.
- Pour un même `device_id`, un `requestIdentity` déjà présent dans le journal B5 est rejeté avant toute nouvelle allocation de transaction. Cette règle empêche un retry HTTP de créer silencieusement une seconde commande.
- Les requêtes de queue issues du Web sont sérialisées dans l’adaptateur par un verrou asynchrone ; ce verrou n’est pas une nouvelle autorité métier ni un cache d’état.
- Une acceptation HTTP signifie uniquement que le travail B5 a été préparé et mis en file. Elle ne signifie pas que le TR2 a exécuté ou réussi la commande.
- Les états `Prepared`, `Submitted`, `Ambiguous` et terminaux restent gérés par l’infrastructure transactionnelle existante.

## API G1

`POST /api/v1/devices/{deviceId}/commands`

Corps JSON :

- `requestIdentity` ;
- `command` parmi les huit commandes autorisées par G1.

Réponses :

- `202 Accepted` : travail mis en file, avec `workId`, `deviceId`, `transactionId`, `command` et `requestIdentity` ;
- `400 Bad Request` : contrat HTTP invalide ;
- `404 Not Found` : `device_id` inconnu ;
- `409 Conflict` : identité déjà vue ou état runtime/session/transaction incompatible ;
- `503 Service Unavailable` : runtime non prêt.

## Hors périmètre

- boutons de commande dans l’IHM ;
- ACK défaut ;
- reset logiciel ;
- RESET_STATISTICS ;
- écriture B6 ;
- replay automatique ;
- résolution d’un état `Ambiguous` depuis l’HTTP ;
- authentification/ACL.

## Sécurité d’exposition

La politique S7-A reste applicable : loopback par défaut, exposition LAN uniquement explicite. À partir de G1, une exposition LAN non authentifiée permet effectivement à un client joignable de demander les commandes B5 autorisées ; elle ne doit donc jamais devenir implicite.
