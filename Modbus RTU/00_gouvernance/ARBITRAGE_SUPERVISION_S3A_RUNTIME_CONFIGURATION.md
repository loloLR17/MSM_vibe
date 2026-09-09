# Projet MSM — Capteur de vibration TR2

## S3-A — Politique de configuration runtime de la supervision

Date : 2026-09-09

Ce document formalise l'arbitrage **S3-A — configuration runtime host**. Il complète les gels S0, S1 et S2. Il ne modifie ni la spécification Modbus RTU V1 ni les politiques firmware.

Toute décision ci-dessous est une `SUPERVISION_POLICY` sauf mention contraire.

## 1. Objet

S3-A définit uniquement comment le futur runtime host connaît, au démarrage :

- le chemin de la base SQLite locale ;
- les bus logiques ;
- les endpoints TR2 associés à chaque bus.

S3-A ne compose pas encore le moteur complet et ne démarre aucun polling.

## 2. Support de configuration

La configuration runtime est portée par un fichier JSON local unique.

Le chargement repose uniquement sur `System.Text.Json`, fourni par .NET. S3-A n'introduit pas `Microsoft.Extensions.Configuration`, de conteneur DI ni de dépendance NuGet supplémentaire.

La configuration est lue au démarrage et validée avant composition des autorités runtime concernées.

## 3. Schéma S3-A

Structure minimale :

```json
{
  "persistence": {
    "databasePath": "data/tr2-supervision.db"
  },
  "buses": [
    {
      "id": "bus-1",
      "endpoints": [1, 2, 3]
    }
  ]
}
```

Un fichier d'exemple non MSM-spécifique est fourni dans le projet Service.

## 4. Chemin SQLite

`persistence.databasePath` est obligatoire et non vide.

- un chemin absolu est conservé comme tel ;
- un chemin relatif est résolu relativement au répertoire du fichier de configuration lors de `Load` ;
- le chemin final est ensuite confié à `SqlitePersistenceOptions`, autorité S2 existante.

Le chemin de base n'est jamais codé en dur pour MSM.

## 5. Bus logiques

Chaque bus possède un identifiant texte non vide.

Les identifiants de bus doivent être uniques avec comparaison ordinale.

Le runtime instancie le type `SerialBus` existant ; S3-A ne crée pas de second modèle de bus.

Une configuration peut contenir zéro bus afin de permettre un démarrage host sans matériel. Cela ne préjuge pas de la configuration de production finale.

## 6. Endpoints

Les endpoints sont déclarés dans le bus logique auquel ils appartiennent.

Chaque valeur est convertie vers le type `ModbusAddress` existant puis vers `TR2Endpoint`.

Dans l'état courant de `main`, `ModbusAddress` est représenté par un `byte` sans règle métier additionnelle. S3-A valide donc uniquement la représentabilité `0..255` et n'invente pas une plage normative différente.

Deux adresses identiques sur un même bus sont refusées. Une même adresse sur deux bus différents reste autorisée car l'identité d'endpoint est `(bus, address)`.

## 7. Validation fail-fast

Une configuration invalide provoque un refus explicite de chargement.

Sont notamment refusés :

- JSON invalide ;
- membre JSON inconnu ;
- absence de `persistence.databasePath` ;
- identifiant de bus vide ;
- identifiant de bus dupliqué ;
- adresse non représentable par `ModbusAddress` ;
- endpoint dupliqué sur un même bus.

Aucune correction silencieuse, aucune valeur métier inventée et aucun enrichissement V1.1 ne sont autorisés.

## 8. Polling

Les cadences FAST / MEDIUM / SLOW / STATIC ne sont pas figées en S3-A.

Elles restent `SUPERVISION_POLICY` et seront traitées lors de S3-E, lorsque le runtime de polling sera effectivement composé.

S3-A ne réserve pas de valeur chiffrée normative implicite.

## 9. Hors périmètre

Restent hors S3-A :

- port COM ;
- baudrate ;
- parité ;
- stop bits ;
- adaptateur USB/RS-485 ;
- driver Windows ;
- transport Modbus RTU physique ;
- Windows Service ;
- API/Web UI ;
- authentification ;
- configuration B4 ;
- provisioning adresse ;
- synchronisation analytique ;
- mécanismes V1.1.

## 10. Prérequis PC

Le registre `PREREQUIS_PC_SUPERVISION.md` a été vérifié pour S3-A.

S3-A n'introduit aucun nouveau logiciel externe ni package NuGet de production. Les prérequis confirmés à la clôture S2 restent suffisants pour cette tranche.

## 11. Suite

Après validation locale de S3-A, la tranche suivante est :

**S3-B — Composition root host**, avec création d'une seule composition cohérente des autorités S1/S2, sans encore introduire le transport RS-485 physique.
