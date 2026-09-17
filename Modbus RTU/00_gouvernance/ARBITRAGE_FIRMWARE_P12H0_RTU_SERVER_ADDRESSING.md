# ARBITRAGE — Firmware P12-H0 — RTU server addressing

## 1. Objet

Cette note arbitre la politique d'adressage nécessaire avant la composition du runtime serveur Modbus RTU P12-H.

Elle ne modifie ni le mapping B0–B7, ni les adapters, ni le transport STM32.

## 2. Référence normative

Référence : MODBUS over Serial Line Specification and Implementation Guide V1.02.

Règles retenues :

- adresse 0 : broadcast ;
- adresses individuelles esclaves : 1 à 247 ;
- adresses 248 à 255 : réservées ;
- une requête broadcast ne produit aucune réponse esclave ;
- une trame non adressée à l'esclave local est ignorée sans réponse.

## 3. Adresse locale du runtime

Le nouveau runtime P12-H reçoit explicitement son adresse locale lors de son initialisation.

L'adresse locale valide est comprise entre 1 et 247 inclus.

L'adresse locale n'est pas :

- codée dans le codec ADU ;
- codée dans le PDU server ;
- codée dans SerialTransport ;
- codée dans le driver STM32 LPUART1.

La provenance production de cette adresse reste hors périmètre de P12-H0 et pourra être arbitrée séparément.

## 4. Unicast

Après délimitation P12-D et validation ADU/CRC P12-B :

- si `unit_id == local_unit_id`, la PDU est transmise au PDU server P12-C ;
- la réponse PDU est réencapsulée avec la même adresse locale ;
- l'ADU réponse est transmis via SerialTransport.

Une erreur de protocole prise en charge par P12-C suit les règles d'exception déjà gelées en P12-C.

## 5. Trame destinée à une autre adresse

Si le CRC et la structure ADU sont valides mais que l'adresse n'est ni l'adresse locale ni 0 :

- aucune PDU métier n'est exécutée ;
- aucune réponse n'est émise ;
- la trame est ignorée.

Les adresses réservées 248 à 255 ne deviennent jamais des adresses locales valides.

## 6. Broadcast adresse 0

Le runtime reconnaît l'adresse 0 comme broadcast.

Politique P12-H :

- aucune réponse ne doit jamais être émise pour une requête broadcast ;
- une lecture FC03 en broadcast n'a pas d'effet utile et n'est pas exécutée ;
- une écriture FC16 broadcast peut être exécutée par le même PDU server/adapters que l'unicast ;
- toute réponse PDU ou exception éventuellement produite en interne pendant le traitement broadcast est supprimée au niveau runtime ;
- aucune stratégie de retry n'est introduite.

Cette politique préserve notamment le chemin transactionnel B5 : une écriture broadcast autorisée vers B5 emprunte le même adapter et la même CommandRequestMailbox qu'en unicast.

## 7. Trame invalide

Une trame dont le codec P12-B refuse la longueur ou le CRC :

- n'atteint pas P12-C ;
- ne provoque aucune réponse ;
- ne doit provoquer aucun effet métier.

Une erreur de transport P12-E invalide la séquence courante ; elle ne doit pas être transformée en requête Modbus.

## 8. Responsabilités

### P12-D
Délimite les octets en trame brute.

### P12-B
Valide longueur/CRC et expose `unit_id + PDU`.

### P12-H
Applique la politique d'adresse et orchestre le traitement.

### P12-C
Traite la PDU FC03/FC16 et les exceptions métier/protocole.

### P12-E
Fournit les événements série et transmet l'ADU réponse.

Cette séparation est normative pour P12-H.

## 9. Hors périmètre

P12-H0 ne fixe pas :

- la valeur de production définitive de l'adresse TR2 ;
- la provenance de cette valeur (constante produit, configuration, persistance, straps, etc.) ;
- DE ou /RE ;
- ADM2587E ;
- turnaround physique RS-485 ;
- retry automatique ;
- validation matérielle.

## 10. Critère pour P12-H1

P12-H1 peut désormais définir un runtime portable testable sur host avec :

```text
SerialTransportEvent
        |
        v
ModbusRtuReceiver
        |
        v
ADU decode / CRC
        |
        v
address policy
        |
        v
ModbusPduServer
        |
        v
ADU encode
        |
        v
SerialTransport TX
```

Le runtime doit être testable avec un faux SerialTransport sans dépendance STM32.
