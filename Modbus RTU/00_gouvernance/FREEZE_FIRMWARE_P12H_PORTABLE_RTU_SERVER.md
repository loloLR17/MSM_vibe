# Freeze — P12-H — serveur Modbus RTU portable

## Statut

P12-H0/H1/H2 est gelé au niveau portable après validation Host et STM32 Cortex-M33 cross-build.

- Host : VALIDATED.
- Cross-build STM32 : VALIDATED.
- Hardware : PENDING.

## Chaîne gelée

```text
SerialTransportEvent
 -> ModbusRtuReceiver
 -> validation ADU / CRC
 -> politique d'adressage
 -> ModbusPduServer
 -> adapters B0..B7
 -> encodage ADU
 -> SerialTransport transmit
```

## Adressage

- adresse locale injectée à l'initialisation ;
- unicast valide : 1..247 ;
- adresse 0 : broadcast ;
- 248..255 : non admises comme adresse locale ;
- unicast étranger : aucune exécution PDU, aucune réponse ;
- CRC/ADU invalide : aucune exécution métier, aucune réponse.

## Broadcast

- aucune réponse RTU ;
- FC03 broadcast : ignoré ;
- FC16 broadcast : peut être exécuté via le même PDU server et les mêmes adapters ;
- B5 broadcast reste soumis au `CommandRequestMailbox` et aux invariants transactionnels existants.

## Fonctions

- FC03 : lecture ;
- FC16 : écriture autorisée selon les adapters ;
- FC06 : non supporté et produit l'exception Modbus Illegal Function en unicast ;
- exceptions Modbus produites par le PDU server sont encapsulées dans une réponse RTU unicast.

## Robustesse verrouillée par H2

Les tests couvrent notamment :
- réponse FC03 unicast complète avec CRC ;
- exception FC06 ;
- FC16/B5 unicast et broadcast ;
- silence pour unicast étranger, broadcast FC03 et CRC invalide ;
- absence d'effet métier dans les cas qui doivent être ignorés ;
- `T1.5 -> BYTE -> T3.5` rejeté ;
- erreur transport : reset du receiver ;
- erreur de transmission propagée sans retry automatique.

## Hors scope

- composition `SystemRuntime` STM32 ;
- source de production de l'adresse locale ;
- DE et /RE ;
- ADM2587E et câblage physique ;
- terminaison/bias ;
- validation instrumentée du timing ;
- intégration supervision PC S7 bout en bout.

Les mappings B0..B7 et les sémantiques transactionnelles B5 restent inchangés.
