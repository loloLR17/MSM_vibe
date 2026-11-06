# Sources, identité des enveloppes et limites

Consultations le6 novembre2026 ; aucune commande ni disponibilité commerciale promise. Les fiches dimensionnelles ne prouvent ni la fabrication ABS ni le fonctionnement réel du bouton. Les sources historiques restent aux chemins existants, sans réécriture ni redistribution de leurs gros modèles.

| Source | Utilisation et preuve |
|---|---|
| ST MB1549 C05, archive CAO officielle | SHA STEP `55f465f8de566561905f97e4b10533a4de33dd13842a5bd9f25bc9d7f06a9900`,1060solides, transformation dans `../V2_0_EMBOITEMENT/preuves/C05-collisions-G12p25.json` ; enveloppes dérivées `sources/C05-enveloppes-derivees.json`. X/Y non modifiés. |
| [ST UM2861](https://www.st.com/resource/en/user_manual/dm00789949.pdf) | Boutons à capuchons ; aucune force/surcourse maximale de B1 établie. Empreinte Altium SWI_WEALTHMETAL_TD-0341_4P_BLUE/HEIGHT7,55 : nom d'empreinte, pas MPN BOM qualifié. Dessus10,2mm rapporté par utilisateur prioritaire pour le cap réel ; premier clic≈0,4, pas une spec max. |
| ST MKIGIBV2, Gerber STEVAL-MKI208V1K | SHA archive `265d576a827daf8b19fa7dea2c1308d388568538d920775fe7708d2b3510e4a7`, contour GKO / drills / pâte / assemblage. Corps18,00×30,93, rangées15,24, pas d'invention à partir d'une photo. Références complètes dans `../V2_6_IMPRESSION_COMPLETE/SOURCES.md`. |
| Mikroe3863, RS485 Isolator2Click | Archive officielle STEPv101 SHA `ff4eed56c5ac02656b4a2f14e99e96460be65ddf18da9a824a52051984cf608e`,163solides. Carte utilisateur HWREV1.02 rapportée par ChatGPT ; photos non reçues directement et aucune équivalence1.01/1.02 supposée. |
| Adafruit4718/4682 | EagleFRAM + SD6solides, pinning et trous réels ; `../V2_6_IMPRESSION_COMPLETE/preuves/sources-fabricants.json` et `sources/adafruit-derives.json`. Les nouvelles vues n'incluent que contours/enveloppes dérivés. |
| Connfly et Samtec | Sources épinglées V2.6 : corps femelle8,5 ; collier mâle2,5 hypothèse courante, pas hauteur effective mesurée. H≈11 n'est pas une fenêtre électrique garantie. |
| [Gutekunst D-064](https://www.federnshop.com/en/products/compression_springs/d-064.html) | Exemple standard DIN2098 : fil0,5, diamètre moyen6,3, extérieur6,8±0,25, libre13,5±0,75, raideur0,728N/mm, minstatique3,34/dynamique3,64. Achat à ce fournisseur non imposé ; équivalent dimensionnel contrôlé ou assortiment courant. Ressort représenté par une enveloppe annulaire, pas par des spires exactes. |
| [Vital Parts W433-M2-NY-N](https://www.vital-parts.co.uk/nylon-washers/33267-w433-m2-ny-n), [Bülte rondelles](https://www.bulte.com/en/our-products/washers/standard-washers) | Nylon DIN433 M2, ID2,2/OD4,5/épaisseur0,3.24distinctes pour les deux TR2, maintien explicite de l'isolation des vis. Prix/stock non qualifiés. |
| [Prusa : ABS](https://help.prusa3d.com/article/abs_2058) | Retrait/gauchissement, enceinte/profil/adhésion à adapter. Aucune mise à l'échelle globale imposée ni précision machine garantie. |
| [Vishay MRS16](https://www.vishay.com/doc?28724=) | Corps3,6×1,6 ; enveloppe calcul4,5×2,5×3,5 avec marge de montage. Centres obtenus par moyenne des vrais pads, pas par origine arbitraire d'empreinte. |
| [KYOCERA AVX SR205C104KAR](https://www.tme.eu/Document/8bd74184c9c9f1c71ca9a7c545331f20/skycap-sr.pdf) | Corps5,08×3,175×5,08max ; enveloppe C1 6×4×7 avec surélévation. Référence V2.2 conservée, pas nouvelle décision électronique. |
| Panasonic EEUFR1H100, source V2.2 archivée | CorpsD5/H11 ; enveloppe C2 D6/H14 avec surélévation, marge conservatrice à prise SD0,22. La nomination est celle du dossier existant, pas une substitution électronique nouvelle. |

## Nature de l'assemblage livré

`cad/assemblage-complet-enveloppes.step` et `cad/eclate-un-prototype.step` regroupent141objets logiques en12groupes pour UN prototype, dont23pièces imprimées utiles. Les composants fabricants sont des enveloppes fidèles référencées, non des solides exacts redistribués ; les vis/écrous/câbles sont des enveloppes dimensionnelles de conception identifiées. Pas de pas de vis/spires/détails cosmétiques inventés. Les alternatives/réserves du lot complet ne sont pas assemblées simultanément (12variantes+2réserves+1coupon pourdeux TR2 ;15non montées au total).

Les têtes Ajile restent des enveloppes de calcul non mesurées ; vérifier à froid. Les câbles sont des scénarios admissibles (RS485Ø6, protection≤8 ; nappe8×1), pas des références commerciales ou mesures réelles. Les ressorts/boutons ne constituent pas un modèle de force physique.

Toutes les fonctions physiques mentionnées dans `MONTAGE_A_FROID.md` restent non qualifiées. Les preuves numériques et leurs paramètres permettent d'examiner la proposition avant la première impression, sans attendre la livraison des PCB.
