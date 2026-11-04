# Provenance et limites des empreintes

Consultations du02/11/2026, fichiers et hashes ci-dessous. Documentation fabricant privilégiée. Les PDF et archives complets sont conservés localement dans `/home/lolo/dev/msm/reports/TR2-shield-proto-v1-20261102/sources`, non recopiés en masse dans Git. Le manifeste distingue téléchargements réussis et timeouts ; une URL citée n’implique pas qu’un fichier local a pu être obtenu.

| Interface | Source primaire / référence figée | Ce qu’elle établit |
|---|---|---|
| SD Adafruit4682 | [PCB officiel](https://github.com/adafruit/Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB/tree/959792478854acec21fb862650427ffaa32b07f1) | contour25,4×22,86, JP1-1..9, pitch2,54, trous2,5 à(2,54;20,32)/(22,86;20,32) |
| FRAM Adafruit4718 | [PCB officiel](https://github.com/adafruit/Adafruit-FRAM-Breakout-PCB/tree/f7d4afa22ddd562f52c557e2e8f003611515b4f4) / `SPI FRAM 2 and 4 Mbit.brd` | contour25,4×17,78, JP1 tourné180°, numérotation et trous(2,54;15,24)/(22,86;15,24) |
| FRAM alimentation | [Guide fabricant](https://learn.adafruit.com/spi-fram-2mbit-4mbit?view=all) | entrée VIN, sortie régulateur3V3 à ne pas réinjecter |
| Click | [Produit](https://www.mikroe.com/rs485-isolator-2-click), [CAO v101](https://download.mikroe.com/documents/2d-3d-files/RS485_Isolator_2_click-2d-3d-files.zip), [schéma v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf) | connecteur16 broches, pitch2,54/rangées22,86, contour57,15×25,4 ; GND1/GND2 distincts, ponts et transceiver |
| Nucleo MB1549 | [UM2861 Rev4 ST Japon](https://www.stmcu.jp/wp/wp-content/uploads/2021/10/UM2861_Rev4.pdf), [manuel ST actuel](https://www.st.com/resource/en/user_manual/dm00789949.pdf) Rev10 consulté via index web | Fig6 encombrement70×133,34 et tables Zio/Morpho ; **pas de CAO C05 obtenue** |
| Kit ST | [Schéma](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf), [BOM](https://www.st.com/resource/en/bill_of_materials/steval-mki208v1k_bom.pdf), [fiche](https://www.st.com/resource/en/data_brief/steval-mki208v1k.pdf) | pinout, raccord1,27mm, nappe30cm ; téléchargement direct ST en échec, textes constructeur consultés en ligne |
| Sockets candidats | [Neltron2212S-XXG-85](https://www.neltron.com.tw/page/product/show.aspx?kind=207&lang=US&num=346&page=1) | pitch2,54, hauteur8,5 ; engagement et variantes à contrôler au choix définitif |
| Nappe tête | [SamtecFFSD](https://www.samtec.com/products/ffsd-06-d-01.00-01-n) | famille12 contacts, pitch1,27 ; référence/longueur distinctes du kit ST30cm |

Les empreintes sont des extractions/géométries de connexion, pas des copies de circuits fabricants complets. Attribution Adafruit Industries, licences originales **CC-BY-SA3.0** jointes ; adaptation : sockets support orientés/placés et trous mécaniques correspondants. Les empreintes dérivées J6/J7, leurs extractions et adaptations sont diffusées sous cette même licence. La citation des autres fabricants ne confère pas de certification ni de droit d’usage de marque.

Attention : l’empreinte KiCad générique `ST_Morpho_Connector_144_STLink_MountingHoles` examinée contient un nombre de contacts différent ; elle n’a pas été utilisée comme gabarit C05. Les Zio sont reconstruits à partir de la documentation identifiée ; leur ajustement réel reste un gate. L’enveloppe MKIGIBV2 dans F.Fab est **provisoire**, non une cote constructeur prétendument obtenue.

Observations utilisateur : MB1549-U575ZIQ-C05, FRAM MB85RS2, MIKROE3863, kit MKI208V1+MKIGIBV2 ; entraxe15,2mm mesuré ; pas breadboard2,54mm rapporté ; GND bas de JP2 à droite vue dessus, sans numérotation absolue lue. La correspondance globale13 est issue du schéma et distinguée du comptage local12.

## Complément V1.1 — références nouvellement obtenues

- [Socket Adafruit1660](https://www.adafruit.com/product/1660) : le fabricant l’identifie comme le composant `MICROSD` de sa bibliothèque, push-push ; [dessin 4UCON15882](https://cdn-shop.adafruit.com/datasheets/15882.pdf), feuille2/3 : positions de dépassement0,60/1,60/5,30mm. Orientation confirmée par vue d’éjection et transformation Eagle, non par photo seule.
- [Modèles3D Adafruit4682](https://github.com/adafruit/Adafruit_CAD_Parts/tree/6c5cd9e3e65252bb5d5339608cf8d3cdabcef251/4682%20Micro%20SD%20Breakout), commit6c5cd9e3e65252bb5d5339608cf8d3cdabcef251. STEP/STL conservés dans le rapport local ; STL borné25,4×22,86×3,45mm. Ces modèles nominalement fabricants ne sont pas une mesure de la carte détenue.
- [OpenSCAD2021.01 officiel](https://github.com/openscad/openscad/releases/tag/openscad-2021.01), AppImage Linux en cache régénérable uniquement ; dépendances Debian libharfbuzz0b/libgpg-error0/libgraphite2-3 extraites localement sans installation système, version CLI observée et hashes conservés. Aucun réglage Windows, firmware, sandbox ou système modifié. Deux chargements de dépendances manquantes résolus avant compilation des gabarits.

Vues et cotes constructeur sont distinguées de nos réservations : bornes de bouche SD et zones opérateur sont conservatrices/provisoires ; centres LED/USER/CN15 exacts C05 non obtenus. Les modèles d’étriers sont nos géométries paramétriques de revue, sans certification fabricant.

## Complément V1.2 — 03/11/2026

`sources-v1-2.json` donne les vrais hashes des dessins Harwin reçus et les échecs des ZIP ST : aucun fichier C05 inspecté ni hash d'archive connu. Aucun contenu C02/C03 n'est assimilé à C05. Choix M20-7821242 disponible, femelle2 M20-7820242 à ajouter sur Nucleo et mâles famille998/999 documentés dans J5_ET_EMPILEMENT_V1_2.md ; fiches passifs primaires dans la BOM. Le clic vers une archive binaire non prise en charge par l'outil documentaire ne vaut pas extraction. Les anciens hashes Adafruit et Click restent ceux des sources réellement utilisées ; révision mécaniqueClickv101 et schémav102 ne sont pas assimilées.

## Complément V1.3

`sources-v1-3.json` identifie les dessins ConnflyDS1021/DS1023 legacy et Neltron2212S réellement reçus, les variantes rejetées et le stock des références exactes. Les dessins DS1021-01H4,3 et DS1021H2,5 ne sont pas assimilés. Les mâles69/70 existants sont une observationutilisateur, pas une mesureCodex. Les essais STbornés n'ont reçu aucunearchive ; aucunhashC05 prétendu. Les originaux fabricants sont conservés localement, leurslicences ne sont pas supposées autoriser redistribution.

## Complément V1.4

`sources-v1-4.json` et `checks/v14/independent-findings.json` distinguent le manuelSTRev8 réellement reçu (table19p37/vuefigure4p10), les observations utilisateur du quinconce et ses hauteurs, la décision autorisant2×3/ancrage déplacé, les dessinsSamtec et les prixaffichés. Les profondeursSamtec ne sont pas attribuées àConnfly. Aucune CAOC05Zip reçue ni cote photogrammétrique inventée.
