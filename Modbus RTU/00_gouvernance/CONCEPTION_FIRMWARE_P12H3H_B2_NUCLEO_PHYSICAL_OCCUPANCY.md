# P12-H3h-B2 — Plan d'occupation physique NUCLEO et stratégie de soudure prototype

Statut : **PROTOTYPE A SOUDE ET CONTROLE — PROTOTYPE B A REPRODUIRE**

## 1. Objet et contrainte réelle

Préparer une seule passe de soudure des ST Morpho CN11/CN12 de la
NUCLEO-U575ZI-Q MB1549-U575ZIQ-C05 pour **deux prototypes identiques**.

Stock disponible : 4 barrettes mâles 1x40 au pas 2.54 mm, soit 160 contacts.

## 2. Référence

La numérotation CN11/CN12 est celle du tableau Morpho de l'UM2861 Rev 10.
Les connecteurs noirs Zio/Arduino internes ne sont pas les empreintes Morpho.

## 3. Ressources existantes à préserver

FRAM qualifiée :
- SPI1 SCK/MISO/MOSI : PA5/PA6/PA7 = CN12-11/13/15 ;
- CS : PD14 = CN12-46.
Le câblage Zio qualifié reste en place.

IIS3DWB qualifié :
- CS PC9 = CN12-1 ;
- SPI3 SCK/MISO/MOSI : PC10/PC11/PC12 = CN11-1/2/3.
Le câblage Zio qualifié reste en place.

## 4. microSD SDMMC2 retenue H3h-B

- CK PD6 = CN11-43
- CMD PD7 = CN11-45
- 3V3 = CN11-16
- GND = CN11-20
- D1 PB15 = CN12-26
- D3 PB4 = CN12-27
- D0 PB14 = CN12-28
- D2 PB3 = CN12-31

CN11-18 est 5V : il peut être physiquement présent dans un segment mais ne doit
pas alimenter la microSD.

## 5. RS-485

LPUART1 est réservé :
- RX PG8 = CN12-66
- TX PG7 = CN12-67

Le Click ADM2867E expose aussi DE et /RE. Pour former un bloc physique compact,
CN12-68 (PG5) et CN12-69 (PG4) sont **réservés comme candidats** de contrôle,
sans gel fonctionnel à ce stade. PG4..PG8 appartiennent au domaine VDDIO2 :
sa configuration devra être vérifiée avant connexion.

## 6. W25Q64 Adafruit 5636

Aucun GPIO Morpho n'est consommé maintenant. Le partage du SPI1 déjà utilisé
par la FRAM, avec CS distinct, sera évalué séparément. Aucun rôle V1 ni aucun
câblage n'est gelé ici.

## 7. Plan de soudure optimisé — identique sur A et B

Broches mâles vers le dessus de la carte, plastique sous la carte, soudures
dessous.

| Segment | Empreinte | Positions | Longueur |
|---|---|---|---:|
| A | CN11 impairs | 43,45 | 2 |
| B | CN11 pairs | 16,18,20 | 3 |
| C | CN12 impairs | 27,29,31 | 3 |
| D | CN12 pairs | 26,28 | 2 |
| E | CN12 impairs | 67,69 | 2 |
| F | CN12 pairs | 66,68 | 2 |

Total : **14 contacts par prototype**, **28 contacts pour deux prototypes**.
Stock restant théorique : 132 contacts.

Le segment B contient physiquement CN11-18 = 5V, volontairement inutilisé pour
la microSD. CN12-29 est une position de réserve non attribuée.

## 8. Procédure

1. Carte hors tension et débranchée.
2. Présenter à blanc les six segments du prototype A avant soudure.
3. Confirmer le sens de numérotation et les colonnes paire/impaire sur la C05.
4. Souder A seulement après cette vérification.
5. Contrôle visuel et ohmique d'absence de pont.
6. Reproduire exactement sur le prototype B.

### Validation physique prototype A — 2026-09-28

Les six segments A à F ont été présentés puis soudés sur le prototype A.
Contrôle visuel des deux faces : implantation conforme au plan, pas de pont de
soudure visible. Les contrôles de continuité entre contacts adjacents demandés
ont été réalisés et déclarés conformes. Le prototype B reste à réaliser à
l'identique ; cette validation ne vaut donc pas encore qualification de la
paire de prototypes.

## 9. Garde-fous

- PA13/PA14 : SWD/ST-LINK ;
- PC14/PC15 : LSE/RTC ;
- NRST/BOOT0 : système ;
- PG2..PG15 : domaine VDDIO2 à vérifier ;
- alimentations : ne jamais traiter comme GPIO.

## 10. Conclusion

L'équipement intégral des Morpho était surdimensionné avec le stock réel.
Le plan optimisé couvre microSD et réserve un bloc RS-485 avec seulement
14 contacts par carte, tout en conservant les deux prototypes identiques.

Aucune modification firmware : aucune compilation ni `tr2_validate.sh`
n'est requise pour cette tranche mécanique.
