# V1.2 — assemblage futur et réserves PCB

Lire d'abord PRECOMMANDE_V1_2.md et J5_ET_EMPILEMENT_V1_2.md. **NE PAS COMMANDER**, aucune manipulation autorisée/exécutée dans cette mission.

1. Sous mission d'assemblage dédiée, bancs hors tension : confirmer C05/gabarit/hauteurs et accessibilité Morpho. Ajouter une femelle deux points sur CN12-69/70 de chaque Nucleo. Repérer broche69/PG4, ne pas ponter PG6.
2. Souder J1–J5 sous le shield, tous les corps dans le même plan. Vérifier au gabarit les numéros1 et la vue miroir. Engager à blanc l'ensemble sans force ; ajuster les entretoises à la hauteur réelle. Poser l'ancrage caché sous SD avant les modules. Les vis et entretoises portent les efforts.
3. Souder J6–J9 dessus, orienter les sockets, nettoyer et examiner toutes les queues/bridges hors tension. FRAM JP1-9VIN→3V3 ; JP1-8sortieLDO NC. SD J7-1=3V3,2GND, fente-X. Aucun5V injecté.
4. FRAM et SD : deux entretoises isolantes et vis chacune, dans trous existants. Pas de vis métallique contre cuivre, pas de serrage sur composants ni sur fente SD ; headsØ≤5mm dans les dégagements réservés.
5. MKIGIBV2 : JP1 à gauche, broches globales1/2(VDDIO/VDD) en haut à gauche, JP2 à droite24(MISO) en haut,13(GND) en bas. Pas2,54/entraxe15,24 nominal dérivé du15,2 mesuré. Capteur et nappe vers-Y, boucle sans traction. Fixation positive par étrier simple aux deux ancrages, patins de rive ajustés aux composants réels, aucun appui sur contacts.
6. Click : extrémité isolée et bornier+X, J9-1 marqué ; VCCSEL3V3 et liaison DE-/RE du module conformément au schéma retenu. Étrier simple et reprise de câble sur structure. Ne pas relier GND2/A/B au GND logique et ne pas traverser la barrière par une vis ou pièce conductrice.
7. Le détrompage automatique est abandonné pour ces deux prototypes : remplacer sa promesse par repères lisibles et contrôle croisé du pinout hors tension avant insertion. Ne jamais utiliser le vissage pour rattraper inversion/décalage ou contacts non engagés. Modules démontables après dépose de la retenue.
8. Après réception PCB : adapter boîtier, selles, fenêtres LED et poussoir USER captif, RESET protégé ; accès CN15/ST-LINK. Ces cotes finales sont différées, leurs volumes réservés restent libres. Aucune pièce REVIEW_ONLY n'est qualifiée à imprimer pour campagne sans revue de contact/matériau/thermique.

Les seuls relevés qui conditionnent encore le PCB sont listés dans PRECOMMANDE_V1_2.md : C05+J5 et comparaison enveloppes MKI/Click aux ancrages. Aucun nouveau relevé entraxe/GND MKI demandé. Le plan de qualification électrique/physique reste ultérieur ; ne pas déduire une tenue vibratoire d'un assemblage nominal ni du DRC.
