# Brochage proposé — seconde NUCLEO-U575ZI-Q

**Proposition documentaire, aucun câblage réel confirmé.** La référence historique
du projet est STEVAL-MKI208V1K + adaptateur STEVAL-MKIGIBV2 ; la présence et
la révision de ce couple sur le second banc restent à confirmer. Si le module
est différent, cette table n'autorise aucun branchement.

Sources constructeur consultées le 12 octobre 2026 :

- [MB1549-U575ZIQ C-02, feuille 5 connecteurs et feuille 3 MCU](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c02_schematic.pdf).
- [STM32U575 DS13737 Rev 10, tableau AF0..7 p.134](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf).
- [STEVAL-MKI208V1K DB4012 Rev 4, figures 1/2 p.2](https://www.st.com/resource/en/data_brief/steval-mki208v1k.pdf)
  et [schéma ST version 1, feuille 1](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf).
- [IIS3DWB DS12569, alimentation et interface SPI](https://www.st.com/resource/en/datasheet/iis3dwb.pdf).

| NUCLEO proposée | STM32 / fonction | Adaptateur proposé | Signal |
|---|---|---|---|
| CN8-4 / D44 | PC9, GPIO output | MKIGIBV2 JP2-21 | CS actif bas |
| CN8-6 / D45 | PC10, AF6 SPI3_SCK | MKIGIBV2 JP2-22 | SCL/SPC, horloge |
| CN8-8 / D46 | PC11, AF6 SPI3_MISO | MKIGIBV2 JP2-24 | SDO-SA0, retour capteur |
| CN8-10 / D47 | PC12, AF6 SPI3_MOSI | MKIGIBV2 JP2-23 | SDA/SDI, entrée capteur |
| CN8-7 / 3V3 | alimentation | MKIGIBV2 JP1-2 | VDD |
| CN8-7 / 3V3 | alimentation | MKIGIBV2 JP1-1 | VDDIO |
| CN8-11 ou CN8-13 / GND | masse commune | point **GND à identifier** sur module/adaptateur | GND |

Les nets du schéma MB1549 rattachent explicitement PC9..12 à CN8-4/6/8/10
(`PICN804/806/808/8010`), 3V3 à CN8-7 et la masse à CN8-11/13.
Les deux fonctions d'alimentation du kit sont distinctes. **Le numéro d'un
contact GND du DIL24 n'est pas établi par l'extraction textuelle consultée** :
ne pas inventer un JP/pin. L'utilisateur devra identifier le point GND avec
le dessin du schéma et la continuité hors tension avant toute alimentation.
La table signaux/VDD/VDDIO est la reprise sourcée du pinout H3g-B, croisée avec
les documents constructeur ; l'orientation physique de JP1/JP2 reste non vérifiée.
Les téléchargements binaires locaux ST ont expiré ; les documents ont été
consultés via la lecture web, sans revendiquer un SHA de PDF non récupéré.

VDD : 2,1..3,6 V ; VDDIO : 1,62 V..VDD+0,1 V. Proposition : les deux à
3,3 V, masse commune. Aucun 5 V sur ces nets. CS haut au repos et bas pendant
commande et données ; SPI 4 fils, MSB-first, 8 bits, mode 0 (CPOL=0/CPHA=0,
mode 3 également supporté). Le portage prévoit noyau MSI 16 MHz et SPI3/PCLK3
divisé par 8, soit 2 MHz calculés, non mesurés. Limite capteur 10 MHz.
INT1/INT2 restent non connectés, polling sans IRQ capteur.

PC9..12 n'utilisent pas les lignes SWD PA13/14, VCP PG7/8, LEDs ou UCPD du
schéma de référence. Le firmware autonome configure seulement PC9..12 ; aucune
SDMMC/UART/FRAM n'est initialisée. SPI3/CN8 sont retenus pour un raccordement
documenté, mais mode, horloge et séquences diffèrent du harness I/J. Ce choix
ne démontre pas l'absence de conflit sur un montage réel ou une autre révision.

À confirmer **après montage**, sans essai maintenant : référence/révision
des deux cartes, orientation et chaque net de la table, point GND, VDD/VDDIO
réels, absence d'autres modules/maîtres, puis SN ST-LINK et UID distincts du
TR2. Conserver la première NUCLEO avec M014-H et ses médias intacte.
