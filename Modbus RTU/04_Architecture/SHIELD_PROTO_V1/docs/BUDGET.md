# Budget V1.1 pour deux shields réemployant les modules

Aucun devis livré ni achat. Les prix observés et sources de connecteurs repris du02/11/2026 sont conservés dans `BOM.csv` ; valeurs de V1 au commit1d2aa668. Conversion comptable1USD=1EUR = provision, pas cours de change. Les autres postes sont des hypothèses.

| Poste deux exemplaires | ProvisionEUR |
|---|---:|
| Lot5PCB90×100 standard + connecteurs et points test | 7,09 |
| Passifs | 1,50 |
| Fixations Nucleo/FRAM/SD | 8,00 |
| Pièces imprimées/selles/poussoirs, fabrication locale à confirmer | 5,00 |
| Vis/écrous/rondelles additionnels étriers et selles | 5,00 |
| Fenêtre transparente simple | 0,80 |
| Deux transports PCB/composants | 17,00 |
| Sous-total scénario | **44,39** |
| RéserveTVA20% | 8,88 |
| Total scénario | **53,27** |

Écart V1 :45,11→53,27€, soit **+8,16€**. Remplacement de la provision4€ de simples sangles par les retenues/selles et leur visserie, plus fenêtre. La cible≤50€ **n’est donc pas tenue dans ce scénario**, et demeure non démontrée sans devis. Variante minimale : ramener les deux ports à14€ par achat groupé/local réellement possible ; le calcul deviendrait49,67€, sans garantie. Ne pas supprimer la retenue ou l’isolation pour atteindre artificiellement50€.

**Avenant V1.1-A** : le bloc secteur externe5V et son câble existants sont réemployés pour CN15. Aucun nouvel interrupteur USB ni powerbank à acheter dans ce budget. Seul le5V entre dans TR2 ; rallonge230V et adaptateur restent hors boîtier, installation selon exigences du bord. L’alimentation du produit final est différée, sans coût batterie ajouté ni choix présumé. Le budget53,27€ est celui des deux shields/connectiques/retentions en réemploi ; disponibilité de deux blocs externes pour une utilisation simultanée à confirmer au besoin, sans achat dans cette mission.

Quantités de pièces : par appareil1étrierMKI,1Click,1selle plate,2selles rondes (RS485/CN15),1poussoir ; selle ST-LINK de maintenance optionnelle réutilisable. Les volumes exacts des modèles pleins sont dans `docs/checks/mechanical-verification.json` (~7,6cm³ par jeu, avant tolérances/supports/infill). Le calcul ne qualifie pas matériau, taux de remplissage ou résistance. PA66/PA12/PETG ne sont pas interchangeables sans revue thermique/mécanique ; aucun PLA déclaré apte à56,5°C.

Stocks, hauteur des sockets, choix passifs/fixations et coûts de fabrication des pièces doivent être figés. Les sockets12 Neltron restaient hors stock au contrôleV1 ; un substitut au même pas ne prouve pas même hauteur. Prix d’appel PCB2USD/lot non devis DFM. Budget pour connectique et mécanique ; futur boîtier complet et étude fixation tête restent distincts.

Sources : [PCB JLCPCB](https://jlcpcb.com/help/article/how-do-i-place-an-order), [Tayda doubleA-5940](https://www.taydaelectronics.com/2-x-40-pin-male-2-54-mm-single-row-pin-header-break-away-round-pin-gold-plated.html), [catalogue connecteurs](https://www.taydaelectronics.com/connectors-sockets/pin-headers.html), [socket12A-7559](https://www.taydaelectronics.com/12-pin-2-54-mm-single-row-female-pin-header.html), [socket8A-1305](https://www.taydaelectronics.com/8-pin-2-54-mm-single-row-female-pin-header.html). Prix de base conservés ; port, imprimés et fiscalité restent provisionnels.
