# Audit binaire des anciens ZIP

V2.6 :32 entrées réellement décompressées ; SHA `2fee5474fc251d96a5d2b01eb2df2c50561965d72085f32319f07780f9a80230`.
V2.7 technique :**38 entrées /25 STL**, réellement décompressées ; SHA `7f21033ffeb50732b3223037159ed1aecf0052235d98e4e7c62ca02c599bbb8d`.

**Le ZIP V2.7 ne contient ni `trappe_USER`, ni `cale_*`, ni cassette sud/SD obsolète.** Les noms signalés existent réellement dans le ZIP V2.6 ci-dessous. Nous ne disposons pas du téléchargement consulté par l’utilisateur ; son origine exacte n’est donc pas démontrée. Ce constat ne présume aucune erreur d’utilisateur.

## Noms obsolètes réellement trouvés dans V2.6

- `STL/04_cassette_sud.stl`
- `STL/05_cassette_SD.stl`
- `STL/08_trappe_USER.stl`
- `STL/09_trappe_SD.stl`
- `STL/12_trappe_outil_RS485.stl`
- `STL/23_cale_0p5.stl`
- `STL/24_cale_1.stl`
- `STL/25_cale_2.stl`

## Liste blanche V2.7 vérifiée sur les octets

25 modèles différents,61 exemplaires **pour les deux prototypes ensemble**. Un STL représente une forme, pas la quantité à imprimer ; ne pas multiplier le lot par deux.

| STL | Rôle | Copies pour deux TR2 | Source gelée | Conforme |
|---|---|---:|---|---|
| `01_base_ABS.stl` | Caisse |2| `V2_7_KISS_ABS/stl/01_base_ABS.stl` | SHA/taille identiques au ZIP technique |
| `02_couvercle_ARNACO_USER.stl` | Couvercle |2| `V2_7_KISS_ABS/stl/02_couvercle_ARNACO_USER.stl` | SHA/taille identiques au ZIP technique |
| `03_facade_STLINK_LED_nappe.stl` | Façade avec fenêtre |2| `V2_7_KISS_ABS/stl/03_facade_STLINK_LED_nappe.stl` | SHA/taille identiques au ZIP technique |
| `06_passe_cable_RS485.stl` | Passage de câble |2| `V2_7_KISS_ABS/stl/06_passe_cable_RS485.stl` | SHA/taille identiques au ZIP technique |
| `07_cadre_vitre.stl` | Cadre de fenêtre |2| `V2_7_KISS_ABS/stl/07_cadre_vitre.stl` | SHA/taille identiques au ZIP technique |
| `10_serre_nappe.stl` | Bride de nappe plate |2| `V2_7_KISS_ABS/stl/10_serre_nappe.stl` | SHA/taille identiques au ZIP technique |
| `11_serre_cable_RS485.stl` | Bride de câble rond |2| `V2_7_KISS_ABS/stl/11_serre_cable_RS485.stl` | SHA/taille identiques au ZIP technique |
| `13_D126.stl` | 6 utiles + 2 en réserve |8| `V2_7_KISS_ABS/stl/13_D126.stl` | SHA/taille identiques au ZIP technique |
| `15_retenue_Nucleo.stl` | Retenue de carte |6| `V2_7_KISS_ABS/stl/15_retenue_Nucleo.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_FRAM_H10p6.stl` | Support mémoire 10,6 |2| `V2_7_KISS_ABS/stl/20_selle_FRAM_H10p6.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_FRAM_H11p2.stl` | Support mémoire 11,2 |2| `V2_7_KISS_ABS/stl/20_selle_FRAM_H11p2.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_FRAM_H11p8.stl` | Support mémoire 11,8 |2| `V2_7_KISS_ABS/stl/20_selle_FRAM_H11p8.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_SD_H10p6.stl` | Support lecteur 10,6 |2| `V2_7_KISS_ABS/stl/20_selle_SD_H10p6.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_SD_H11p2.stl` | Support lecteur 11,2 |2| `V2_7_KISS_ABS/stl/20_selle_SD_H11p2.stl` | SHA/taille identiques au ZIP technique |
| `20_selle_SD_H11p8.stl` | Support lecteur 11,8 |2| `V2_7_KISS_ABS/stl/20_selle_SD_H11p8.stl` | SHA/taille identiques au ZIP technique |
| `30_etrier_MKI_ABS.stl` | Retenue du capteur |2| `V2_7_KISS_ABS/stl/30_etrier_MKI_ABS.stl` | SHA/taille identiques au ZIP technique |
| `31_etrier_Click_ABS.stl` | Retenue du module |2| `V2_7_KISS_ABS/stl/31_etrier_Click_ABS.stl` | SHA/taille identiques au ZIP technique |
| `32_patin_MKI_avant_ABS.stl` | Petit patin avant |2| `V2_7_KISS_ABS/stl/32_patin_MKI_avant_ABS.stl` | SHA/taille identiques au ZIP technique |
| `33_patin_MKI_arriere_ABS.stl` | Petit patin arrière |2| `V2_7_KISS_ABS/stl/33_patin_MKI_arriere_ABS.stl` | SHA/taille identiques au ZIP technique |
| `34_patin_Click_ABS.stl` | Petit patin de retenue |4| `V2_7_KISS_ABS/stl/34_patin_Click_ABS.stl` | SHA/taille identiques au ZIP technique |
| `40_cage_USER_captive.stl` | Guide du poussoir |2| `V2_7_KISS_ABS/stl/40_cage_USER_captive.stl` | SHA/taille identiques au ZIP technique |
| `41_USER_pointe_Z8p8.stl` | Poussoir, version 1 |2| `V2_7_KISS_ABS/stl/41_USER_pointe_Z8p8.stl` | SHA/taille identiques au ZIP technique |
| `41_USER_pointe_Z9p6.stl` | Poussoir, version 2 |2| `V2_7_KISS_ABS/stl/41_USER_pointe_Z9p6.stl` | SHA/taille identiques au ZIP technique |
| `41_USER_pointe_Z10p4.stl` | Poussoir, version 3 |2| `V2_7_KISS_ABS/stl/41_USER_pointe_Z10p4.stl` | SHA/taille identiques au ZIP technique |
| `50_coupon_ABS.stl` | Petit échantillon ABS |1| `V2_7_KISS_ABS/stl/50_coupon_ABS.stl` | SHA/taille identiques au ZIP technique |

Total61 =46 utiles +12 alternatives +2 D126 de réserve +1 coupon. Les61 exemplaires ne deviennent pas61 fichiers ;25 fichiers ne signifient pas2×25 impressions.

Le nouvel envoi ne réemploie aucune archive ancienne :seuls les25 fichiers de cette liste blanche seront copiés, avec deux nouveaux PDF. Les dossiers historiques restent présents dans Git pour traçabilité, hors de toute diffusion à Morvan.
