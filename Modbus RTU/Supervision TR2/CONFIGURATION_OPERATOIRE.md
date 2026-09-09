# Supervision TR2 — Configuration opératoire

## 1. Objet

Ce document décrit la configuration opératoire du host console de supervision TR2 après S5-B.

Lancement :

```text
TR2.Supervision.Service --config <path>
```

Le fichier `tr2-supervision.example.json` fournit un exemple syntaxiquement complet.

## 2. Statut des valeurs de l'exemple

Les valeurs suivantes du fichier d'exemple sont des **valeurs illustratives de supervision** :

```text
COM7
115200 bauds
8 bits
parité None
1 stop bit
750 ms de timeout
adresse Modbus 1
```

Elles ne constituent pas des paramètres matériels TR2 validés et ne doivent pas être reprises comme tels au banc.

Avant la qualification matérielle, le port série réel, les paramètres série réels, le timeout approprié et l'adresse Modbus réellement configurée doivent être établis à partir de la configuration matérielle/normative applicable.

## 3. Identités à ne pas confondre

```text
bus.id       = identité logique du bus dans la supervision
serial.portName = port série physique utilisé par ce bus
endpoints[]  = adresses Modbus des équipements rattachés au bus
```

Ces trois notions sont distinctes. En particulier, `bus.id` n'est pas un nom de port COM.

## 4. Persistance

`persistence.databasePath` désigne le fichier SQLite de la supervision.

Un chemin relatif est résolu relativement au répertoire contenant le fichier de configuration, et non relativement au répertoire courant du processus.

Exemple :

```text
configuration : C:\TR2\config\tr2-supervision.json
databasePath  : data/tr2-supervision.db
chemin résolu : C:\TR2\config\data\tr2-supervision.db
```

## 5. Bus série

Une section `serial` active le transport série physique pour le bus correspondant.

Champs actuellement requis :

```text
portName
baudRate
dataBits
parity
stopBits
responseTimeoutMilliseconds
```

Les valeurs de `parity` reconnues sont celles exposées par la configuration runtime (`None`, `Odd`, `Even`, `Mark`, `Space`).

Les valeurs de `stopBits` reconnues sont celles exposées par la configuration runtime (`One`, `Two`, `OnePointFive`).

`responseTimeoutMilliseconds` doit être strictement positif.

## 6. Endpoints

`endpoints` contient les adresses Modbus associées au bus logique.

Une même adresse ne peut pas être déclarée deux fois sur le même bus.

Le loader vérifie que chaque valeur peut être représentée par le type `ModbusAddress`. Les contraintes métier/normatives plus strictes, si elles existent, restent de l'autorité du protocole et ne doivent pas être inventées par la configuration opératoire.

## 7. Erreurs de configuration

Le host console classe une erreur de chargement ou de validation de configuration comme erreur d'usage/configuration et retourne le code de sortie `2`.

Exemples :

```text
fichier absent ou inaccessible
JSON invalide
champ obligatoire absent
membre JSON inconnu
bus.id dupliqué
endpoint dupliqué sur un bus
valeur série non reconnue
responseTimeoutMilliseconds <= 0
```

Une erreur survenant après composition/démarrage du runtime est une défaillance runtime et retourne le code `1`.

Un arrêt normal ou demandé par cancellation retourne le code `0`.

## 8. Limite pré-matériel

Ce document décrit uniquement la configuration logicielle du PC.

Il ne valide pas :

```text
port COM réel
adaptateur USB/RS-485
câblage A/B
terminaison/polarisation
baud/parité/stop bits réels du TR2
timeout physique approprié
comportement hot-plug réel
STM32 réel
```

Ces points restent à qualifier au banc matériel.
