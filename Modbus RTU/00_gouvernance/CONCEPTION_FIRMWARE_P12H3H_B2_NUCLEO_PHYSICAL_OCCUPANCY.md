# P12-H3h-B2 — Plan d'occupation physique NUCLEO et stratégie de soudure prototype

Statut : **CONCEPTION / PREPARATION MATERIELLE — PAS DE GEL**

## 1. Objet

Préparer une seule passe de soudure des connecteurs ST Morpho CN11/CN12 de la
NUCLEO-U575ZI-Q MB1549-U575ZIQ-C05 en tenant compte des périphériques connus du
prototype TR2, sans figer prématurément les rôles fonctionnels non décidés.

## 2. Référence carte

Carte observée physiquement : NUCLEO-U575ZI-Q, MB1549-U575ZIQ-C05.

Le tableau ST Morpho de l'UM2861 Rev 10 constitue la référence de numérotation
CN11/CN12. Les connecteurs noirs Zio/Arduino internes ne doivent pas être
confondus avec les empreintes Morpho externes CN11/CN12.

## 3. Ressources déjà attribuées

### FRAM MB85RS2 — qualifiée

- SPI1_SCK PA5 = CN12-11
- SPI1_MISO PA6 = CN12-13
- SPI1_MOSI PA7 = CN12-15
- CS PD14 = CN12-46

Le câblage physique actuellement qualifié peut utiliser les connecteurs Zio
existants ; ces fonctions restent néanmoins réservées au niveau MCU.

### IIS3DWB — qualifié

- CS PC9 = CN12-1
- SPI3_SCK PC10 = CN11-1
- SPI3_MISO PC11 = CN11-2
- SPI3_MOSI PC12 = CN11-3

Le câblage physique actuel via D44..D47 reste en place ; les fonctions MCU sont
réservées.

### RS-485 — architecture réservée

- LPUART1_TX PG7 = CN12-67
- LPUART1_RX PG8 = CN12-66

Le transceiver ADM2867E nécessite en plus des signaux de contrôle DE et /RE si
le mode de contrôle indépendant du Click est conservé. Leurs GPIO ne sont pas
encore figés ici. Les fonctions TX/RX restent réservées.

### microSD — choix H3h-B

SDMMC2 4 bits :

- CK PD6 = CN11-43
- CMD PD7 = CN11-45
- D0 PB14 = CN12-28
- D1 PB15 = CN12-26
- D2 PB3 = CN12-31
- D3 PB4 = CN12-27
- 3V3 = CN11-16
- GND = CN11-19 ou CN11-20

## 4. W25Q64 Adafruit 5636

Le produit 5636 est le breakout **SPI simple canal** W25Q64 de 8 MiB, avec
level shifting et régulateur. Ce n'est pas le breakout QSPI DIP.

Aucun bus/CS ne lui est attribué dans cette tranche. Son rôle fonctionnel V1
n'est pas défini par H3h-A et il ne doit pas créer de conflit avant décision.

Conséquence : la stratégie de soudure doit conserver un accès général aux
Morpho plutôt que de ne souder que les huit points microSD.

## 5. Stratégie de soudure recommandée

Pour le banc de développement, équiper **les quatre colonnes complètes** des
empreintes ST Morpho :

- CN11 : deux barrettes mâles 1x35 au pas 2.54 mm ;
- CN12 : deux barrettes mâles 1x35 au pas 2.54 mm.

Des barrettes 1xN sécables conviennent : deux rangées parallèles par connecteur.

Cette stratégie donne accès aux 70 positions de chaque Morpho et évite de
ressouder à chaque nouvelle affectation GPIO. Elle n'impose aucune fonction
électrique aux broches non utilisées.

### Précautions mécaniques

- carte totalement hors tension ;
- vérifier le pas 2.54 mm avant insertion ;
- utiliser une breadboard ou un support droit comme gabarit pendant la soudure
  si disponible ;
- souder d'abord une broche à chaque extrémité, contrôler l'alignement, puis
  terminer la rangée ;
- ne jamais relier électriquement les deux colonnes entre elles ;
- conserver l'accès mécanique aux connecteurs Zio, boutons, USB et jumpers.

## 6. Carte d'occupation consolidée

| Fonction | Bus / GPIO | Morpho | État |
|---|---|---|---|
| FRAM SCK | PA5 | CN12-11 | réservé/qualifié |
| FRAM MISO | PA6 | CN12-13 | réservé/qualifié |
| FRAM MOSI | PA7 | CN12-15 | réservé/qualifié |
| FRAM CS | PD14 | CN12-46 | réservé/qualifié |
| IIS3DWB CS | PC9 | CN12-1 | réservé/qualifié |
| IIS3DWB SCK | PC10 | CN11-1 | réservé/qualifié |
| IIS3DWB MISO | PC11 | CN11-2 | réservé/qualifié |
| IIS3DWB MOSI | PC12 | CN11-3 | réservé/qualifié |
| microSD CK | PD6 | CN11-43 | retenu H3h-B |
| microSD CMD | PD7 | CN11-45 | retenu H3h-B |
| microSD D0 | PB14 | CN12-28 | retenu H3h-B |
| microSD D1 | PB15 | CN12-26 | retenu H3h-B |
| microSD D2 | PB3 | CN12-31 | retenu H3h-B |
| microSD D3 | PB4 | CN12-27 | retenu H3h-B |
| RS-485 TX | PG7 | CN12-67 | réservé |
| RS-485 RX | PG8 | CN12-66 | réservé |
| W25Q64 | à attribuer | à attribuer | non figé |
| RS-485 DE,/RE | à attribuer | à attribuer | non figé |

## 7. Broches spéciales à ne pas détourner sans étude

- PA13/PA14 : SWD/ST-LINK ;
- PC14/PC15 : LSE/RTC dans l'architecture TR2 ;
- PH0/PH1 : fonctions oscillateur selon configuration ;
- NRST et BOOT0 : fonctions système ;
- PG2..PG15 : domaine VDDIO2 configurable sur la Nucleo ; vérifier le réglage
  carte avant tout périphérique utilisant ces broches ;
- alimentations 3V3/5V/VIN/VBAT/VREFP : ne pas traiter comme GPIO.

## 8. Conclusion

Pour minimiser les opérations de soudure, la bonne frontière matérielle n'est
pas « souder seulement les broches microSD », mais **équiper complètement les
empreintes Morpho CN11 et CN12 avec des barrettes mâles 2.54 mm**.

Cela ne modifie aucune baseline firmware et ne nécessite donc aucune compilation
ni exécution de `tr2_validate.sh`.

Après soudure et contrôle visuel/ohmique, H3h-C pourra commencer par le câblage
microSD SDMMC2 non destructif. Le W25Q64 et les GPIO de contrôle RS-485 seront
arbitrés séparément avant câblage.
