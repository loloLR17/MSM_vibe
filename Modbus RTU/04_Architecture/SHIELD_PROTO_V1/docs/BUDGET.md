# Budget pour deux équipements — scénario, pas devis

Consulté le02/11/2026, aucun achat, aucun compte/panier créé. Sources et quantités détaillées dans `BOM.csv`. Les modules actifs, cartes SD et nappe ST existants sont réemployés (0€ supplémentaire). L’assemblage manuel utilise l’outillage existant ; travail et futur boîtier exclus de la cible, fixation locale/retention incluses.

Prix affichés : barre double mâle2x40 A-5940 0,90USD ; simple40 A-197 0,15USD ; socket9 Neltron A-7569 0,19USD ; socket12 A-7559 0,29USD **hors stock** ; socket8 A-1305 0,08USD. Les références et variantes sont distinctes : ne pas substituer la page «40 pin female» dont titre et URL mélangent1,27/2,54mm. Les descriptions des séries et les cotes finales d’engagement priment sur le prix.

| Poste pour deux TR2 | Quantité achetée | Provision |
|---|---:|---:|
| PCB candidat90×100mm | lot5, dont2 utilisés | 2,00€ |
| Embases mâles, sockets et points test | 3barres doubles +1simple +12sockets | 5,09€ |
| Passifs | 8R+4C, petite marge | 1,50€ |
| Fixations isolantes | 14entretoises,28vis/rondelles | 8,00€ |
| Sangles et futurs étriers locaux | 2ensembles | 4,00€ |
| Transport PCB et composants | 2envois | 17,00€ |
| Sous-total de calcul | | **37,59€** |
| Réserve TVA20% sur ce sous-total | hypothèse, pas calcul fiscal d’import | 7,52€ |
| Total scénario | | **45,11€** |

Le PCB à2USD/5 pièces est le **prix d’appel fabricant** pour ≤100×100mm/2couches, pas une offre ferme appliquée à ce dessin. Pour ce scénario seulement,1USD est provisionné à1EUR : il ne s’agit pas d’un cours de change observé. Passifs, fixations, retenues, transports et fiscalité sont des **hypothèses de budget**, pas des prix fournisseurs prétendument vérifiés. Le complément de fixation détrompée peut dépasser cette réserve. Sans destination et devis DFM/transport/taxes, **la cible≤50€ n’est pas démontrée** ; marge de scénario4,89€, insuffisante pour garantir les aléas. Le délai/stock des sockets12 et les références exactes de passifs/fixations sont aussi des réserves à figer avant commande. Le candidat Nichicon UVR1C100MDD a été écarté après lecture de la fiche fabricant (fin de vie, pas2mm) ; aucune compatibilitéP2,5 n’est revendiquée pour cette référence.

Alternative minimale si dépassement : conserver le format90×100/FR4 vert/HASL, réemploi intégral des modules, couper les barres mâles au lieu de connecteurs sur mesure, grouper l’achat de passifs/fixations chez un fournisseur local disposant du stock. Utiliser le stock existant seulement après inventaire, sans le supposer disponible. Ne pas économiser en supprimant retenue, isolement ou test de continuité. Aucune commande avant levée des réserves mécaniques et revue du coût livré.

Références prix : [JLCPCB](https://jlcpcb.com/help/article/how-do-i-place-an-order), [embase double Tayda](https://www.taydaelectronics.com/2-x-40-pin-male-2-54-mm-single-row-pin-header-break-away-round-pin-gold-plated.html), [catalogue sockets](https://www.taydaelectronics.com/connectors-sockets/pin-headers.html), [socket12](https://www.taydaelectronics.com/12-pin-2-54-mm-single-row-female-pin-header.html), [socket8](https://www.taydaelectronics.com/8-pin-2-54-mm-single-row-female-pin-header.html). Les caractéristiques constructeur du [Neltron H8,5](https://www.neltron.com.tw/page/product/show.aspx?kind=207&lang=US&num=346&page=1) ne qualifient pas automatiquement un substitut générique Tayda.
