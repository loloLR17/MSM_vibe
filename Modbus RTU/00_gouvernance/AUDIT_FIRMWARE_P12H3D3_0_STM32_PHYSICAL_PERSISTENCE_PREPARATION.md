# Audit P12-H3d3-0 — Préparation de la persistance physique STM32

## Statut

**CADRAGE PRÉ-MATÉRIEL — non gelé**

Baseline d'entrée : P12-H3d2 gelé par `9e08ae03d086adc8fa8336e8827ac2150bff1644`.

Cette tranche prépare H3d3 sans choisir prématurément un composant, un SPI, des pins ou une fréquence.

## 1. Contrat déjà imposé par H3d2

Le moteur portable consomme uniquement :

```c
typedef struct {
    void *context;
    Tr2Result (*read)(void *context, uint32_t offset, void *buffer, size_t size);
    Tr2Result (*write)(void *context, uint32_t offset, const void *buffer, size_t size);
} TransactionalImagePhysicalStorage;
```

Le driver H3d3 doit donc fournir des lectures/écritures physiques bornées et synchrones du point de vue de l'appelant. Il ne porte aucune sémantique `commit()` : la publication transactionnelle reste exclusivement dans H3d2.

Invariant d'architecture :

```text
TR2 portable / transactional_image_media
              |
              v
TransactionalImagePhysicalStorage
              |
              v
platform/stm32 physical-storage adapter
              |
              v
HAL/LL SPI + composant NVM
```

Aucun header STM32/HAL ne doit remonter dans `include/tr2` ou `src`.

## 2. Ce qui doit rester non décidé avant inspection du matériel

Ne pas geler avant identification physique et documentation constructeur :
- référence exacte de la mémoire non volatile ;
- tension et niveaux logiques ;
- instance SPI ;
- SCK/MISO/MOSI/CS ;
- mode SPI ;
- fréquence ;
- taille/adressage de la mémoire ;
- commandes READ/WRITE/WREN éventuelles ;
- status register et protections ;
- contraintes CS et timing ;
- politique timeout HAL ;
- présence éventuelle de HOLD/WP ;
- géométrie de production.

Le profil 256 KiB H3d2 reste un profil de qualification, pas une preuve de la capacité du module réellement reçu.

## 3. Budget mémoire préliminaire

Le linker STM32 courant déclare :

```text
FLASH = 2048 KiB
RAM   = 768 KiB
heap minimum  = 0x200  = 512 B
stack minimum = 0x800  = 2048 B
```

H3d2 exige un buffer candidat contigu de :

```text
51 818 B = environ 50.6 KiB
```

Soit environ 6.6 % des 768 KiB déclarés par le linker.

Conclusion préliminaire : le buffer est plausible en capacité brute, mais **pas encore qualifié en production**. H3d3 doit l'allouer statiquement, produire le `.map` réel et vérifier au minimum :
- `.data` ;
- `.bss` ;
- réserve stack ;
- réserve heap ;
- marge SRAM restante ;
- buffers UART/SPI et futurs buffers vibration ;
- absence d'allocation dynamique nécessaire au runtime embarqué.

Aucune conclusion de capacité finale n'est autorisée avant cette mesure.

## 4. Politique proposée pour le driver physique

H3d3 doit commencer par un driver minimal et bloquant. Pas de DMA, cache, queue, retry automatique ou optimisation tant que le chemin simple n'est pas qualifié.

Propriétés attendues :
- vérification `context != NULL` ;
- `buffer == NULL` accepté uniquement pour `size == 0` ;
- zéro longueur = succès sans transaction physique ;
- vérification anti-overflow de `offset + size` ;
- rejet hors capacité ;
- erreurs HAL/composant converties en `Tr2Result` explicite ;
- timeout borné ;
- aucune répétition automatique après résultat ambigu ;
- aucune mutation de H3d2 en cas d'erreur ;
- CS ramené dans un état sûr sur tous les chemins de sortie.

Les écritures physiques peuvent être fragmentées uniquement si le composant l'exige. Cette fragmentation ne doit pas être confondue avec une transaction H3d2.

## 5. Séquence de qualification matérielle prévue

### H3d3-A — Inventaire et identification
- photos recto/verso ;
- références exactes ;
- documentation officielle ;
- tension/pinout/interface ;
- confirmation de la capacité minimale requise par la géométrie choisie.

### H3d3-B — NUCLEO seule
- ST-LINK détecté ;
- MCU identifié ;
- flash/debug/reset minimal ;
- lecture du `.map` et budget mémoire réel.

### H3d3-C — SPI électrique minimal
- câblage d'un seul périphérique NVM ;
- CS maîtrisé ;
- transaction SPI connue et observable ;
- lecture d'identification/status si le composant le permet.

### H3d3-D — PhysicalStorage brut
- lecture bornée ;
- écriture bornée ;
- read-after-write ;
- limites 0 / fin mémoire ;
- zéro longueur ;
- timeout/erreur ;
- reboot/power-cycle.

### H3d3-E — H3d2 sur stockage réel
- `EMPTY` initial ;
- format explicite ;
- génération 1 ;
- write + commit ;
- reboot ;
- recovery `VALID` ;
- données engagées conservées ;
- candidate non engagée perdue ;
- alternance images/publications.

### H3d3-F — qualification de coupure réelle
- coupure contrôlée pendant écriture image ;
- coupure pendant publication ;
- recovery ancienne ou nouvelle autorité complète ;
- jamais d'image mixte ;
- répétitions suffisantes pour couvrir les fenêtres critiques.

## 6. Mesures à relever

Sur matériel réel, relever :
- fréquence SPI réellement utilisée ;
- durée lecture 64 B ;
- durée écriture 64 B ;
- durée lecture 51 818 B ;
- durée écriture 51 818 B ;
- durée d'un `commit()` H3d2 complet ;
- consommation SRAM statique via map ;
- stack high-water si instrumentation disponible ;
- comportement et durée des timeouts.

Ces mesures serviront à décider ensuite si le chemin bloquant reste acceptable ou si une optimisation est nécessaire. Ne pas optimiser avant mesure.

## 7. Critères d'arrêt

Arrêter H3d3 et ne pas contourner si :
- référence du composant incertaine ;
- documentation officielle contradictoire avec le module ;
- niveaux électriques incompatibles ;
- capacité insuffisante pour la géométrie retenue ;
- écriture/readback non déterministe ;
- erreur physique masquée ;
- buffer candidat ou stack ne tient pas avec marge démontrable ;
- coupure réelle permet une autorité incohérente.

## 8. Sortie de H3d3-0

H3d3-0 ne modifie aucun code de production.

À l'arrivée du matériel, l'ordre est donc fixé :

```text
inventaire officiel
-> NUCLEO seule
-> budget mémoire réel
-> NVM seule sur SPI
-> PhysicalStorage brut
-> moteur H3d2 gelé
-> power-loss réel
```

Cette préparation permet de commencer le monde physique sans réouvrir H3d2 et sans laisser le matériel acheté dicter rétroactivement l'architecture.
