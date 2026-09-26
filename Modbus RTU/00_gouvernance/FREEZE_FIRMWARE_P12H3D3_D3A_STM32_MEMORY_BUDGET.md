# Gel firmware — P12-H3d3-D3-A — Budget mémoire STM32 réel

## 1. Objet

Cette tranche clôt la mesure du budget mémoire du binaire STM32 courant après le gel D2-D, sans modification fonctionnelle du firmware ni écriture supplémentaire sur la FRAM.

Baseline d'entrée :

`752b7925aa430844a6809ddb9ef414970e9f4d9f` — `Governance: define P12-H3d3-D3 persistence closure`

Le binaire a été produit par la validation complète habituelle `tr2_validate.sh`, qui inclut le cross-build STM32.

## 2. Mesures ELF

`arm-none-eabi-size` :

- text : 46 644 octets ;
- data : 52 octets ;
- bss : 55 264 octets selon la vue synthétique GNU size, qui inclut les réserves allocables non initialisées ;
- total : 101 960 octets.

Vue par sections `arm-none-eabi-size -A` :

- `.isr_vector` : 568 octets ;
- `.text` : 46 068 octets ;
- `.ARM.exidx` : 8 octets ;
- `.init_array` : 4 octets ;
- `.fini_array` : 4 octets ;
- `.data` : 44 octets ;
- `.bss` : 52 704 octets ;
- `._user_heap_stack` : 2 560 octets.

Les différences entre les deux vues proviennent de leur classification des sections ; les valeurs de section et symboles du map/ELF font foi pour le budget SRAM détaillé.

## 3. SRAM

Le linker STM32 déclare :

- origine RAM : `0x20000000` ;
- fin RAM / `_estack` : `0x200C0000` ;
- capacité : 768 KiB = 786 432 octets.

Symboles observés :

- `_sdata = 0x20000000` ;
- `_edata = 0x2000002C` ;
- `_sbss = 0x20000030` ;
- `_ebss = 0x2000CE10` ;
- réserve heap minimale : `0x200 = 512` octets ;
- réserve stack minimale : `0x800 = 2 048` octets ;
- fin de `._user_heap_stack` : `0x2000D810`.

Budget occupé/réservé jusqu'à cette limite :

- `.data` : 44 octets ;
- alignement entre data et bss : 4 octets ;
- `.bss` : 52 704 octets ;
- heap + stack réservés : 2 560 octets ;
- total depuis l'origine RAM jusqu'à `0x2000D810` : 55 312 octets ;
- marge d'adressage SRAM restante jusqu'à `0x200C0000` : 731 120 octets.

Cette marge est une marge de capacité/adressage du binaire actuel ; elle ne constitue pas une mesure de stack high-water du futur runtime complet.

## 4. Buffer candidat H3d2

Le symbole a été observé directement dans l'ELF avec `arm-none-eabi-nm -S --size-sort` :

`200000e4 0000ca6a b tr2_fram_d2_candidate`

Donc :

- adresse : `0x200000E4` ;
- taille : `0xCA6A = 51 818` octets ;
- stockage : `.bss`.

Le buffer candidat représente environ 98,3 % de la `.bss` courante, mais seulement environ 6,6 % des 768 KiB de SRAM physique déclarée.

La capacité brute SRAM n'est donc pas bloquante pour ce buffer dans le binaire actuellement qualifié.

## 5. FLASH

Les sections chargées observées représentent environ 46,7 KiB de contenu code/données face aux 2 MiB de FLASH déclarés par le linker.

Aucune contrainte de capacité FLASH n'est identifiée sur le binaire courant.

## 6. Limites

D3-A ne qualifie pas :

- stack high-water réel ;
- consommation RAM du futur `SystemRuntime` STM32 complet ;
- buffers futurs d'acquisition vibration ;
- consommation de pile sous charge Modbus/RS-485 ;
- consommation dynamique éventuelle future ;
- performances temporelles FRAM/H3d2.

Ces points ne doivent pas être déduits de la seule marge SRAM actuelle.

## 7. Conclusion

P12-H3d3-D3-A est gelée pour le périmètre suivant :

- artefacts ELF/map STM32 mesurés ;
- buffer H3d2 de 51 818 octets confirmé directement dans l'ELF ;
- budget SRAM actuel compatible avec ce buffer avec une large marge de capacité ;
- capacité FLASH actuelle non bloquante ;
- aucune modification firmware ou FRAM nécessaire pour obtenir ces résultats.

La tranche suivante est D3-B : caractérisation temporelle physique bornée de la FRAM et du commit H3d2.
