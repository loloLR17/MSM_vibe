# Freeze P12-H3d3-C — SPI électrique minimal / FRAM RDID

## Statut

**GELÉ — validation matérielle réelle**

Baseline d'entrée :
- P12-H3d3-B gelé : `0f345c9ddfbd7bb4875e9b59573a56a8819f39f7`
- correctif HAL SPI : `b89d582abc2a68e87c8023ffb85d02105102fbf9`
- ajout du driver HAL SPI au link : `51dd364edef5fa070121a9c157f309872bdedc2b`
- transaction RDID non destructive : `fc7fdcdc22eeca9616518f2f4f0244cbe7353f2f`
- correction finale CS Arduino D10 / PD14 : `f5c8cacbf4ace39e624576a2b2f87d4d6c219dac`

Cette tranche qualifie uniquement le chemin SPI électrique minimal entre la NUCLEO-U575ZI-Q et la FRAM MB85RS2MTA par lecture RDID. Elle ne qualifie ni l'écriture FRAM, ni le PhysicalStorage H3d3-D, ni H3d2 sur média réel, ni les coupures d'alimentation.

## 1. Matériel et câblage qualifiés

Carte cible :
- ST NUCLEO-U575ZI-Q ;
- révision physique MB1549-U575ZIQ-C05 ;
- MCU STM32U575ZIT6Q.

Mémoire :
- Adafruit 4718 SPI FRAM ;
- composant MB85RS2 / MB85RS2MTA ;
- alimentation directe 3,3 V.

Câblage physiquement validé :
- FRAM 3V3 -> NUCLEO 3V3 ;
- FRAM GND -> NUCLEO GND ;
- FRAM SCK -> Arduino D13 / PA5 / SPI1_SCK ;
- FRAM MISO -> Arduino D12 / PA6 / SPI1_MISO ;
- FRAM MOSI -> Arduino D11 / PA7 / SPI1_MOSI ;
- FRAM CS -> Arduino D10 / PD14 ;
- !WP non raccordé ;
- !HOLD non raccordé.

Mesure au repos après correction :
- alimentation FRAM : environ 3,3 V ;
- CS physique : environ 3,3 V.

## 2. Configuration SPI qualifiée pour le bring-up

Configuration firmware :
- SPI1 maître ;
- 2 lignes ;
- données 8 bits ;
- MSB first ;
- CPOL LOW ;
- CPHA 1EDGE, mode SPI 0 ;
- NSS logiciel ;
- SPI1 kernel clock = SYSCLK ;
- SYSCLK = 160 MHz ;
- prescaler SPI = 64 ;
- fréquence SPI initiale = 2,5 MHz ;
- CS logiciel actif bas sur PD14 ;
- timeout HAL de la transaction RDID = 10 ms ;
- aucun DMA ;
- aucun retry automatique.

Cette configuration est qualifiée uniquement pour le test RDID minimal de cette tranche.

## 3. Transaction physique qualifiée

Transaction utilisée :
```text
CS LOW
TX  : 9F
RX  : 04 7F 48 03
CS HIGH
```

La commande RDID est non destructive : aucun WREN et aucune écriture de contenu FRAM ne sont effectués.

Variables de preuve exposées par le firmware :
- `tr2_fram_rdid_status` ;
- `tr2_fram_device_id[4]` ;
- `tr2_fram_device_id_matches`.

## 4. Défaut de câblage logique découvert pendant la qualification

La première implémentation associait à tort le CS Arduino D10 à PA4.

Premier essai physique :
- firmware démarré normalement ;
- HAL SPI : `HAL_OK` ;
- RDID lu : `FF FF FF FF` ;
- correspondance : 0 ;
- alimentation FRAM mesurée : 3,3 V ;
- CS physique mesuré : 0 V au repos.

Ce résultat a invalidé l'hypothèse PA4 pour le CS physique.

Après relecture du schéma officiel de la révision MB1549-U575ZIQ-C05, le CS Arduino D10 a été corrigé vers PD14. Le câblage physique n'a pas été modifié.

Correctif :
- `TR2_FRAM_CS_PORT = GPIOD` ;
- `TR2_FRAM_CS_PIN = GPIO_PIN_14` ;
- activation de l'horloge GPIOD.

Commit du correctif : `f5c8cacbf4ace39e624576a2b2f87d4d6c219dac`.

## 5. Validation matérielle finale

Après recompilation, programmation Flash et reset :
- LD1 / PC7 continue de clignoter normalement à 250 ms ;
- CS est mesuré à environ 3,3 V au repos ;
- le firmware atteint sa boucle principale.

Lecture GDB après exécution réelle de la transaction :
```text
(gdb) p (int)tr2_fram_rdid_status
$1 = 0

(gdb) p/x *(unsigned char (*)[4])&tr2_fram_device_id
$2 = {0x4, 0x7f, 0x48, 0x3}

(gdb) p (unsigned char)tr2_fram_device_id_matches
$3 = 1 '\001'
```

Résultat qualifié :
- HAL SPI retourne `HAL_OK` ;
- le MB85RS2MTA retourne exactement `04 7F 48 03` ;
- l'oracle firmware confirme la correspondance ;
- le chemin aller SCK/MOSI/CS et le chemin retour MISO sont donc démontrés sur matériel réel pour cette transaction.

## 6. Build qualifié

Après ajout du support SPI HAL, le cross-build STM32 est passé à 86/86 étapes.

Build C1 observé :
```text
text     data    bss     dec     hex
37124      20   3428   40572    9e7c
```

La tranche C2 et la correction PD14 ont ensuite été recompilées avec succès avant programmation et validation matérielle.

## 7. Limites explicites du gel

Ce gel valide :
- initialisation SPI1 sur STM32U575 ;
- brochage électrique SPI utilisé ;
- pilotage CS sur Arduino D10 / PD14 ;
- alimentation et sélection de la FRAM ;
- transaction RDID bloquante avec timeout borné ;
- réponse physique exacte du MB85RS2MTA ;
- absence de régression visible du bring-up existant.

Ce gel ne valide pas :
- WREN ;
- WRITE ;
- READ de données utilisateur ;
- endurance ou rétention ;
- comportement après écriture ;
- limites de taille/adressage du driver ;
- TransactionalImagePhysicalStorage réel ;
- sémantique transactionnelle H3d2 sur FRAM ;
- recovery réel ;
- coupure d'alimentation ;
- fréquence SPI supérieure à 2,5 MHz ;
- DMA, cache ou optimisation de transport.

## 8. Conclusion du gel

P12-H3d3-C est validée et gelée comme **SPI électrique minimal**.

La preuve matérielle centrale est la lecture non destructive du RDID `04 7F 48 03` avec `HAL_OK` et correspondance firmware positive sur la NUCLEO-U575ZI-Q reliée à la FRAM MB85RS2MTA.

La tranche suivante est **P12-H3d3-D — implémentation et qualification du TransactionalImagePhysicalStorage réel sur FRAM**, sans réimplémenter la sémantique transactionnelle déjà gelée en H3d2.
