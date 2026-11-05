# Sources et niveau de preuve

Les archives/solides fabricants restent dans `/home/lolo/dev/msm/reports/TR2-shield-v26-20261105/`, hors Git et hors ZIP imprimeur. Empreintes complètes : `preuves/sources-fabricants.json`. Les fichiers imprimés et STEP exportés ici sont nos créations ; les images sont des dérivations géométriques explicites.

| Source primaire | Résultat utilisé | Limite |
|---|---|---|
| [ST kit Gerber](https://www.st.com/resource/en/board_manufacturing_specification/steval-mki208v1k_gerber.zip) | GKO/GM4 MKIGIBV2, coordonnées centre-trait : environ18,00×30,93mm ; drills JP1/JP2, pâte GTP et assemblage GM15 | Contour présente une fermeture≲0,03mm ; ce n’est pas une tolérance d’usinage garantie. Révision réelle à recouper à froid. |
| [ST DB4012](https://www.st.com/resource/en/data_brief/steval-mki208v1k.pdf), [schéma](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf), [BOM](https://www.st.com/resource/en/bill_of_materials/steval-mki208v1k_bom.pdf) | Kit capteur/adaptateur et SHF-106-01-L-D-SM /FFSD-06-01-N /nappe3749/12 | Le titre BOM citeMKI209, le corpsMKI208 ; la mention2×5 ne remplace pas la référenceSHF106 à6positions. Aucun schéma électrique utilisé seul comme preuve de zone nue. |
| [Mikroe2D/3D](https://download.mikroe.com/documents/2d-3d-files/RS485_Isolator_2_click-2d-3d-files.zip), [produit3863](https://www.mikroe.com/rs485-isolator-2-click) | Vrai STEP RS485_Isolator_2_click_v101 :163solides, PCB57,15×25,4, composants réels ; colliers mâles2,54mm | Ne pas substituer l’ancien Click. Schéma courantv102 distinct du STEPv101 ; photo/révision et portées à confirmer au premier article. |
| [Adafruit FRAM PCB](https://github.com/adafruit/Adafruit-FRAM-Breakout-PCB), [Adafruit SD PCB](https://github.com/adafruit/Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB), [SD téléchargements](https://learn.adafruit.com/adafruit-microsd-spi-sdio/downloads) | Eagle exacts épinglés ; trous métallisésØ2,5/pastilles3,2 sans net ; empreintes, composant/headers ; STEP SD6solides, PCB1,57mm | Hauteur mâles Adafruit non publiée dans ces fichiers. Enveloppes de corps FRAM : footprints+0,25mm et hauteur4mm conservatrice, non mesure. |
| [Connfly DS1023](https://www.connfly.com/static/upload/file/pdf/DS1023.pdf) | Corps8,5mm ; queue3,0±0,2 ; largeur2,5 ; longueur1×10=25,9±0,3 | Aucun dessin de ressort interne/profondeur minimale de contact. Les enveloppes conservatrices complètes V2.5 restent utilisées ; aucune découpe du corps. |
| [Samtec SHF](https://suddendocs.samtec.com/catalog_english/shf.pdf) | SHF-SM hauteur5,46, largeur5,85, longueur6×1,27+6,35=13,97mm ; FFSD sansSR compatible | Volume connecté de calcul X46,5..58,8 /Y25,8..41 /Z0..8,5 au-dessus du PCB : plus large/haut que le SHF, incluant FFSD et départ de nappe. Enveloppe de conception conservatrice, pas cote mesurée de la nappe réelle. |
| [Accu DIN985 M3](https://www.accu.co.uk/hexagon-nylon-locking-nuts/7957-HNN-M3-A4) | AF5,5, H4, pas0,5 ; pochesAF5,9/H4,4 | Source dimensionnelle, pas stock/prix/livraison vérifiés ni achat. Écrous courant DIN985, pas modèle spécial imposé. |

## Transformations contrôlables

- RepèreCAO : X=XKiCad, Y=−YKiCad, Z0=dessusNucleo ; G12,25, dessusshield13,85.
- ST adapter : raw pouces×25,4, X=45+Xraw, YKiCad=24−Yraw. Pin1(45;24),12(45;51,94),13(60,24;51,94),24(60,24;24), concordance avec les axes commandés. Contour résultantX43,612906..61,611854/Y22,511306..53,438346. Ancienne enveloppe20,32×33,02 provisoire remplacée pour cette étude, **PCB inchangé**.
- Click : rotationCAOZ−90°, translation(87,22;−80;B), B=dessusPCB. SourcePCB Z−1,590022..0, composants au-dessusjusqu’à8,50. Quatre blocs de connecteurs mâles :Z−4,140199..−1,600198, hauteur2,54. Les petits écarts source ne sont pas arrondis en fausse métrologie physique.
- SD : rotationZ+90°, translation(43,04;−108,54;13,85+H). SourcePCB Z0..1,57, coquejusqu’à3,45. Le modèle local versionné déjà obtenu est utilisé ; aucune nouvelle redistribution.
- FRAM : sourceEagleX+42,46,Y+56,46 en KiCad ; SD :X=43,04−YEagle,Y=108,54−XEagle. Les implantations et rotations des composants sont conservées.

## Contacts et réglage

Les quatre appuis de bord par TR2 sont PETG uniquement : MKI0,8×2aux extrémités courtes, Click0,8×2au bout logique. Les calculs natifs et les fenêtres pâte/assemblage sont dans `preuves/`; métal et vis ne touchent aucun module. Au premier article, confirmer absence de composant/soudure sur ces fenêtres et contrôler le jeu. Une révision différente reste une réserve de montage, pas un motif d’attendre pour imprimer le kit réglable.

H≈11 est une déduction nominale8,5+2,5, pas un fait observé sur les modules reçus. Les appuis8/10/12 et cales0,5/1/2 permettent8..15,5 par pas0,5, avec ponçage≤0,5 ; cette **couverture mécanique n’atteste aucune course de contact électrique**. Le réglage des patins couvre les dessusPCBZ24,75..29,15. La fiche de montage limite ces extrapolations explicitement.

## Addendum distant72a28db9, lu avant le premier commit

ChatGPT rapporte sur photo **HW REV.1.02 /ADM2867E** du Click réel ; les images ne sont pas jointes au dépôt et Codex ne les a pas vues directement. L’archive officielle de36,16Mo ne contient aucun membrev102 ; son STEPv101 n’est donc pas présenté comme la preuve intégrale de la révision1.02. Le format et l’extrémité bornier concordent avec le produit3863. Le modèle PCB du STEP simplifie les perçages : une seule boucle du dessus, sans détail des deux ouvertures signalées. Leur fonction réelle n’est pas établie ici ; **aucune fixation ne les utilise**. Les patins restent au bout logique, à distance des rangées mikroBUS et composantsv101 ; le contrôle à froid sur la1.02 reste requis, avec possibilité d’ajuster le PETG seulement.

L’estimation photographique MKI21–22mm n’est pas une cote : le contour officielGKO18×30,93 et l’écartement des rangées15,24 priment. Aucun dessin n’a été recalé sur une règle photographiée. Ces compléments n’introduisent aucune attente de photos ou réception pour imprimer. Voir `preuves/revisions-photos-addendum.json`.
