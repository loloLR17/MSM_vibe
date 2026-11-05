# Configuration proposée — cinq PCB nus TR2 V2.3

**Fiche de préparation, sans téléversement ni commande.** Le candidat est prêt du point de vue des contrôles CAO/exports exécutés. L'utilisateur décide de sa libération pour JLCPCB, du coût livré, du transport et du paiement. Le risque mécanique prototype accepté dans la mission n'est pas une qualification industrielle.

| Champ de commande | Valeur proposée |
|---|---|
| Archive | `exports/TR2_SHIELD_V23_5PCB_CANDIDAT_NON_LIBERE.zip` ; vérifier son empreinte dans `preuves/CAM-manifeste.json` |
| Contenu | 7 Gerber +2 Excellon PTH/NPTH, un seul PCB ; aucun stencil ni assemblage |
| Dimensions du contour | **90×100 mm**, axes de `Edge_Cuts.gm1`, sans miroir ni redimensionnement |
| Quantité / livraison du PCB | **5**, PCB individuels, pas de panneau ajouté |
| Substrat | FR4, TG135 standard envisagé |
| Couches / épaisseur | **2 couches, 1,6 mm** ; nominal CAD et job vérifiés |
| Cuivre extérieur | **1 oz**, nominal35µm dans le stack généré |
| Finition à choisir | **HASL sans plomb** ; ne pas garder une sélection sans finition ou au plomb |
| Masque / sérigraphie | Vert / blanc sur faces présentes dans les Gerber |
| Perçages | Excellon métrique absolu, PTH et NPTH distincts ; ne pas fusionner leurs attributs |
| Vias | Tenting correspondant aux masques fournis ; pas de remplissage cuivre/résine ajouté |
| Impédance / fraisage / stencil / SMT | Aucun service supplémentaire demandé ; aucun nouveau découpage |
| Identifiant fabricant | Demander l'absence d'ajout de numéro sur les zones fonctionnelles, option à vérifier au devis ; ne pas écraser ARNACO/repères |
| Revue de production | Contrôler contour, couches, perçages et aperçu fabricant avant tout paiement/libération ; ne pas accepter une modification électrique automatique |
| Prix et livraison France | **Non actualisés dans V2.3**, aucun devis accepté ni date ferme ; les estimations V1.8 ne sont pas une facture |

Les instructions fabricant décrivent les fichiers [Gerber/Excellon nécessaires](https://jlcpcb.com/help/article/gerber-files-preparation) et les [options KiCad](https://jlcpcb.com/help/article/how-to-generate-gerber-and-drill-files-in-kicad-6), notamment les extensions Protel et le format X2. Il n'y a pas de pâte à souder nécessaire pour ces PCB nus à assemblage traversant. Les deux plans de perçage PDF sont disponibles séparément si la revue humaine/fabricant les demande.

Le `.gbrjob` automatique est **hors ZIP candidat**, conservé pour traçabilité : `Size=90,05×100,05` correspond à l'enveloppe du trait de contour de0,05, et `Finish=None` n'est pas une prescription de commande. La géométrie des axes du contour90×100 est vérifiée dans les vrais Gerber ; la finition sans plomb doit être choisie dans la commande future. Aucune de ces métadonnées n'a été corrigée en modifiant la géométrie native.

## Réserves à conserver sur le premier article

Premier montage à froid avec pièces exactes : emboîtement J5 Samtec et Zio, G12,25±0,10 sous les hypothèses V2.0, contact PG4/DE et cinqNC, absence de contrainte sur les quatre broches66/67/68/69. Corps Connfly1×10 conservés, patte supplémentaire neutralisée sans connexion parasite : dégagement et état des pièces réelles non vérifiés ici.

D126 : dimensions, planéité, orientation et maintien sans précharge excessive ; visserie isolante conforme à l'enveloppe ; fluage/vibrations non qualifiés. Confronter modules, étriers, isolation RS485, soudures/queues et accès CN15/ST-LINK/microSD/RS485/nappe, USER simple et LED au vrai assemblage. Pas d'alimentation avant contrôle de polarité et absence de court-circuit. Aucun geste physique demandé ou exécuté pendant cette mission.

La personnalisation ne nécessite pas une nouvelle impression papier et ne requalifie pas les campagnes STM32. Elle ne modifie aucun signal, routage, masque, perçage ou fixation. Le fichier natif et le ZIP de **V2.3** doivent être utilisés ensemble ; les anciens exports V1.4/V1.8 restent historiques.
