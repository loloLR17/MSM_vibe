# ARBITRAGE FIRMWARE P12-C0 — Function Codes Modbus exposés

## 1. Objet

Cet arbitrage ferme le verrou documentaire préalable à P12-C (serveur PDU / dispatch vers les adapters B0–B7).

Il ne modifie ni le mapping B0–B7, ni la sémantique transactionnelle B5, ni la persistance/recovery, ni la distinction V1/V1.1.

## 2. Constat issu de `main`

La spécification source B0–B7 décrit le registre logique et les droits RO/RW, mais aucune décision normative explicite n'a été retrouvée dans `main` sous forme de liste de Function Codes serveur autorisés.

En revanche, la supervision PC déjà implémentée et gelée apporte une contrainte d'interopérabilité concrète :

- `NModbusRegisterClient.ReadHoldingRegistersAsync(...)` est le chemin de lecture utilisé par la supervision ;
- `NModbusRegisterClient.WriteMultipleRegistersAsync(...)` est le chemin d'écriture utilisé par la supervision ;
- `ModbusRegisterTransport` n'expose que ces deux opérations au protocole TR2 ;
- le writer B5 utilise `WriteRegistersAsync(...)` aussi bien pour la préparation multi-registres que pour le SUBMIT mono-registre.

Conséquence : le serveur firmware doit au minimum accepter les Function Codes correspondant à ces deux opérations pour être compatible avec la supervision existante.

## 3. Arbitrage P12-C0

Le périmètre serveur Modbus TR2 V1/V1.1 est volontairement minimal :

| Function Code | Nom Modbus | Statut TR2 | Usage |
|---|---|---|---|
| `0x03` | Read Holding Registers | **supporté** | lecture B0–B7 via `modbus_read_adapter_read` |
| `0x10` | Write Multiple Registers | **supporté** | écritures autorisées B2/B4/B5/B6 via les write adapters existants |
| autres FC | — | **non supportés en P12-C** | réponse exception `Illegal Function (0x01)` |

### Décision explicite sur FC06

`0x06` — Write Single Register — n'est **pas** supporté en P12-C.

Raison : la supervision existante utilise `WriteMultipleRegistersAsync` même pour une écriture d'un seul registre (notamment le SUBMIT B5). Ajouter FC06 n'apporte donc aucune capacité nécessaire à l'interopérabilité actuelle et élargirait inutilement la surface protocolaire.

FC06 pourra être réexaminé ultérieurement par arbitrage explicite si un besoin d'interopérabilité externe apparaît.

## 4. Règles PDU à implémenter en P12-C

### FC03 — Read Holding Registers

Requête PDU :

- Function Code : 1 octet (`0x03`)
- Starting Address : 2 octets, big-endian
- Quantity of Registers : 2 octets, big-endian

Le dispatcher valide la forme et les bornes protocole, puis délègue l'autorisation d'adresse au Register Model via `modbus_read_adapter_read`.

Réponse normale :

- Function Code `0x03`
- Byte Count
- valeurs de registres, chaque `uint16` en big-endian.

### FC10 — Write Multiple Registers

Requête PDU :

- Function Code : 1 octet (`0x10`)
- Starting Address : 2 octets, big-endian
- Quantity of Registers : 2 octets, big-endian
- Byte Count : 1 octet
- valeurs, chaque `uint16` en big-endian.

Le dispatcher valide la cohérence `quantity` / `byte_count` / longueur PDU, puis route exclusivement vers l'adapter du bloc concerné.

Réponse normale :

- Function Code `0x10`
- Starting Address
- Quantity of Registers.

## 5. Exceptions Modbus P12-C

Le serveur PDU doit produire au minimum :

- `0x01 Illegal Function` : Function Code non supporté ;
- `0x02 Illegal Data Address` : adresse/plage non valide, registre réservé ou écriture interdite selon le Register Model/adapters ;
- `0x03 Illegal Data Value` : PDU structurellement cohérent au niveau FC mais valeur/quantité/byte-count invalide pour la requête.

Une erreur interne/service qui ne se réduit pas proprement à une erreur d'adresse ou de valeur ne doit pas être inventée silencieusement : son mapping éventuel vers `0x04 Slave Device Failure` devra être explicite dans l'implémentation/tests P12-C.

## 6. Invariants d'architecture

P12-C doit respecter :

- aucun accès direct aux structures métier en contournant `read_adapter` / `write_adapter` ;
- B5 reste transactionnel et passe par `CommandRequestMailbox` ;
- aucune relance automatique d'une commande B5 ;
- aucun UART/HAL/RS-485 dans le serveur PDU portable ;
- aucune décision DE, `/RE`, DMA, IRQ ou T3.5 dans cette tranche ;
- aucune modification du mapping B0–B7 ;
- aucune allocation dynamique requise par le chemin PDU.

## 7. Statut

**ARBITRÉ — prêt pour implémentation P12-C.**

Base factuelle : état courant de `main` et supervision TR2 existante. Le choix minimal FC03 + FC10 est désormais la règle de conception P12-C.