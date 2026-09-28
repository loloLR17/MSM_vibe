# P12-H3h-A — Arbitrage architecture du stockage bulk de campagne

## 1. Objet

Résoudre le blocage identifié après H3g-D avant la composition complète de
`SystemRuntime` : le champ `ConfigurationValidationEnvironment.usable_storage_capacity_mb`
doit représenter une capacité réelle de stockage de campagne et ne peut pas être
déduit artificiellement de la FRAM transactionnelle.

## 2. Contrats existants à préserver

Le gel P8-B impose :

- un record bulk fixe de 16 octets par `VibrationSample` lu avec `TR2_OK` ;
- un append par échantillon ;
- un checkpoint à la fin de chaque fenêtre d'acquisition ;
- seul le préfixe couvert par le dernier checkpoint/finish durable est récupérable ;
- le `CampaignDataStore` reste un stockage bulk opaque au protocole Modbus.

La V1 impose une fréquence d'acquisition de 26667 Hz et autorise une durée de
campagne de 60 s à 604800 s.

## 3. Débit logique minimal résultant

Sans compression, le flux bulk logique vaut :

```text
26667 samples/s * 16 bytes/sample = 426672 bytes/s
```

Soit environ :

- 25.6 MB par minute (base décimale) ;
- 1.536 GB par heure ;
- environ 10.3 GB pour 6 h ;
- environ 258 GB pour 7 jours.

Ces valeurs décrivent le contrat logique P8-B ; elles ne constituent pas une
mesure de débit physique du futur média.

## 4. Médias matériels disponibles

Matériel du prototype :

- FRAM MB85RS2MTA, 2 Mbit / 256 KiB ;
- NOR Flash W25Q64JV, 64 Mbit / 8 MiB ;
- breakout microSD Adafruit 4682, compatible SPI ou SDIO.

Le STM32U575 dispose de contrôleurs SDMMC matériels. Le choix de broches et le
raccordement NUCLEO feront l'objet d'une tranche dédiée avant câblage.

## 5. Arbitrage

### 5.1 FRAM

La FRAM reste l'autorité transactionnelle H3d2/H3d3 existante.

Elle ne devient pas le stockage bulk de campagne et sa baseline physique gelée
ne doit pas être modifiée par H3h.

### 5.2 W25Q64JV

Le W25Q64JV de 8 MiB ne peut pas être l'autorité bulk V1 principale.

À 426672 bytes/s, 8 MiB correspondent théoriquement à environ 19.7 s de records,
avant tout overhead. C'est inférieur à la durée minimale V1 de 60 s.

Aucun rôle de cache, journal ou staging ne lui est attribué dans cette tranche :
un tel rôle introduirait de nouvelles frontières de durabilité et de recovery
qui devraient être spécifiées et qualifiées séparément.

### 5.3 microSD

La microSD est retenue comme **candidat d'autorité physique du bulk de campagne V1**.

Raisons :

- capacité naturellement exprimable en centaines de MB ou en GB ;
- compatibilité du breakout Adafruit 4682 avec SPI et SDIO ;
- STM32U575 doté de périphériques SDMMC ;
- le débit logique requis (426672 bytes/s) justifie d'étudier en priorité une
  interface SDMMC/SDIO plutôt qu'un raccordement SPI choisi uniquement par
  simplicité.

Ce choix d'architecture ne constitue pas encore une qualification physique du
débit, de la carte, du filesystem ou de la résistance au power-loss.

## 6. Conséquence pour ConfigurationValidationEnvironment

Tant que la microSD réelle n'a pas été :

1. détectée/initialisée ;
2. caractérisée en capacité utilisable ;
3. raccordée à une implémentation `CampaignDataStore` conforme au contrat P8 ;
4. qualifiée au minimum sur append/checkpoint/recovery et débit soutenu ;

la composition finale ne doit pas publier
`storage_capacity_known = true`.

La valeur `usable_storage_capacity_mb` devra provenir du média réellement
disponible pour les campagnes, après réserves/layout éventuels, et non de la
capacité nominale supposée.

## 7. Tranches suivantes

- **H3h-B** : étude officielle STM32U575/NUCLEO + Adafruit 4682, choix SDMMC,
  bus et pinout sans conflit avec les ressources déjà gelées ;
- **H3h-C** : bring-up physique minimal microSD (présence + initialisation +
  identification/capacité), sans filesystem imposé prématurément ;
- **H3h-D** : conception du backend bulk et de ses frontières de durabilité ;
- **H3h-E** : implémentation/qualification `CampaignDataStore` physique ;
- puis seulement caractérisation honnête de
  `ConfigurationValidationEnvironment` et composition `SystemRuntime`.

## 8. Invariants

- aucune modification des sémantiques Modbus B0..B7 ;
- aucun formatage automatique/destructif de média sans garde explicite ;
- aucune réutilisation de la FRAM comme bulk de campagne ;
- aucun rôle implicite du W25Q64 ;
- aucune affirmation de débit microSD avant mesure physique ;
- le contrat P8-B reste l'autorité logique du payload et des checkpoints.
