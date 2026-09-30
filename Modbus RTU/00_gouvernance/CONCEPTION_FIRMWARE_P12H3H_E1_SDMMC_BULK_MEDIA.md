# P12-H3h-E1 — Conception adaptateur CampaignBulkMedia SDMMC

## Statut

Conception et première implémentation logicielle. Validation locale et physique
requises avant gel.

Baseline de départ :

```text
97055c99cb9cfce6ff07e7cb2f2bbb5f336a590b
Firmware: validate H3h-D5C bulk data store
```

## Objet

Adapter le contrat byte-addressed `CampaignBulkMedia` D1 au périphérique
SDMMC STM32, sans filesystem et sans modifier le format D4/D5.

## Source HAL

La cible projet reste STM32CubeU5 v1.9.0. Le contrat utilisé est celui du
driver HAL SD U5 déjà intégré au build STM32 :

- bloc logique SD : 512 octets ;
- `HAL_SD_GetCardInfo` fournit `LogBlockNbr` et `LogBlockSize` ;
- `HAL_SD_ReadBlocks` / `HAL_SD_WriteBlocks` utilisent un index de bloc et
  un nombre de blocs ;
- `HAL_SD_GetCardState` expose notamment TRANSFER, PROGRAMMING,
  DISCONNECTED et ERROR.

La compilation croisée contre le checkout STM32CubeU5 local reste la preuve
de compatibilité de version utilisée par le projet.

## Traduction byte -> blocs

### Accès alignés complets

Les séquences couvrant un ou plusieurs secteurs complets sont transmises
directement au HAL sous forme multi-blocs.

### Accès partiels

Un accès partiel utilise un scratch de 512 octets fourni par l'appelant.

Lecture :

1. lire le secteur ;
2. copier la sous-plage demandée.

Écriture :

1. lire le secteur ;
2. modifier uniquement la sous-plage ;
3. réécrire le secteur complet ;
4. attendre TRANSFER.

Cette règle est nécessaire notamment pour les blocs D5 dont la taille physique
n'est pas nécessairement multiple de 512 octets.

## Durabilité et état carte

L'adaptateur n'introduit aucun cache différé.

Avant une opération physique, la carte doit revenir à TRANSFER.

Après une écriture HAL réussie, l'adaptateur attend TRANSFER avant de rendre
la main. Cela est nécessaire pour permettre les écritures séquentielles D5
sans dépendre d'un sync entre chaque bloc.

`sync()` attend également TRANSFER. Il constitue la barrière média exposée à
D3/D4.

Un retour HAL réussi ne suffit donc pas, à lui seul, à terminer une écriture
du point de vue de cet adaptateur.

## Capacité

La capacité exposée est :

```text
LogBlockNbr * LogBlockSize
```

calculée en uint64_t, uniquement après validation d'un bloc logique de 512
octets.

## Timeouts

Deux timeouts sont fournis explicitement à l'initialisation :

- timeout des appels HAL read/write ;
- timeout d'attente du retour TRANSFER.

Aucune valeur de production n'est figée par E1.

## Non-objectifs

E1 ne :

- choisit pas la taille du buffer D5-C ;
- ne branche pas encore D5-C dans SystemRuntime ;
- ne remplace pas le bench destructif H3h-C2c ;
- ne prétend pas qualifier les accès partiels physiquement ;
- ne résout pas le découplage acquisition / stalls SD ;
- ne publie pas encore la capacité dans ConfigurationValidationEnvironment.

## Validation requise

Avant gel :

1. compilation host/core inchangée ;
2. compilation STM32 contre le checkout STM32CubeU5 v1.9.0 ;
3. revue des warnings avec -Werror ;
4. tranche physique ultérieure dédiée aux accès E1, sans confondre ce test
   avec le bench C2c existant.
