# Assemblage préparé et réserves avant fabrication

**NON LIBÉRÉ. Aucun assemblage ni changement de banc exécuté.** Ce document prépare une opération ultérieure autorisée, il ne demande aucune manipulation immédiate.

## Empilage candidat

Carte support FR4 1,6 mm, 90 × 100 mm ; origine CAO (20,17). J1–J5 mâles soudés côté B se branchent dans les connecteurs femelles Nucleo. J6–J9 sockets soudés côté F reçoivent les broches existantes des modules. Les trois ancrages Nucleo non alignés se situent en CAO (32,62;21,19), (33,89;97,39), (79,61;97,39), perçages 2,8 mm pour M2,5. Le quatrième trou voisin de CN7 est **omis volontairement** : sa tête de fixation entrerait dans le volume du connecteur. Aucun contact n’est utilisé comme fixation principale.

Cible de distance carte/module ≈11 mm avec socket H8,5 et embase mâle ≈2,5 mm ; **hauteur totale réelle à vérifier**, y compris longueurs d’engagement, corps et queue soudée. J6/J7 : deux entretoises M2,5 sous chaque module, centres extraits des PCB Adafruit, perçage support 2,7 mm. Fixer le support Nucleo avant de poser la SD qui masque un ancrage. Prévoir fixations isolantes PA66 et rondelles adaptées : pas de tête métallique en appui sur une piste ou de face soudée touchant une carte voisine. Leur tenue vibratoire/thermique n’est pas démontrée.

J8 et J9 : deux trous de sangle de 3 mm chacun pour reprise d’effort, hors des broches ; ne pas faire passer une sangle sur composants, connecteur de nappe ou bord coupant. Le Click dépasse le côté droit : son bornier isolé doit rester hors du cuivre et du Nucleo, sans vis conductrice entre domaines. Dans le CAD v101, le centre ADM2867E se projette vers X118,46 mm, contre bord support X110 mm ; c’est un **contrôle de disposition**, pas une validation d’isolement du module réel ni de la révision v102.

Les courtyards KiCad décrivent les **sockets au niveau du support** ; les corps des modules élevés sont sur F.Fab et dans le gabarit. Cette distinction corrige les faux conflits de queues traversantes sous un module élevé. Elle n’autorise aucun chevauchement matériel : queues des connecteurs Nucleo coupées proprement, dégagement vertical et tête des vis à contrôler au montage à blanc. La 3D fournie représente seulement le support nu ; elle ne prouve pas ces dégagements.

## Détrompage et retenue — réserve essentielle

La nappe usine ST 12 voies utilise le connecteur Samtec FFSD au pas 1,27 mm et une longueur annoncée de 30 cm. Réemploi de la tête et de son interface existante ; pas de Dupont individuel ni d’IDC 2,54 mm substitué au 1,27 mm. La simple sérigraphie PIN1 et une sangle **ne constituent pas un détrompage mécanique**. Les sockets de modules actuels sont ouverts, réversibles lors de l’insertion ; les broches seules ne suffisent pas à la retenue.

Avant libération, il faut définir un étrier isolant asymétrique pour le MKIGIBV2/Click, solidaire des trous de sangle, et le dispositif de maintien du connecteur de tête à partir de leurs enveloppes et accès réels. Son volume et ses points de contact ne sont **pas inventés** ici. Une variante soudée permanente peut être étudiée mais n’est pas considérée équivalente sans revue de réparabilité/hauteur. Les fichiers actuels ne satisfont donc pas encore le critère complet « détrompé et retenu ».

## Réserves minimales à lever

1. **Nucleo C05 :** confronter le gabarit à l’échelle à la carte réelle ou obtenir sa CAO fabricant C05. Vérifier positions CN7/8/9/10 et dernière paire CN12, les trois ancrages, et accès USER/RESET/LED/CN15/ST-LINK/jumpers avec module élevé. Le dessin UM2861 Rev4 et la table Rev10 ne constituent pas une parité CAO C05 indépendante. Un relevé/photo dessus coté, échelle visible, suffit à préparer cette revue ; ne pas déplacer le banc pour la présente mission.
2. **MKIGIBV2 :** relever longueur/largeur hors tout, débord du connecteur de nappe, sens/pin1 et hauteur au-dessus des broches. L’entraxe **15,2 mm et le GND bas droit sont acquis comme observations utilisateur** ; ne pas les redemander. Ces cotes manquantes servent à l’enveloppe et à l’étrier détrompé. Une photo dessus et de profil avec règle, lors d’une prochaine intervention hors tension autorisée, est le minimum utile.
3. **Empilage/Click et nomenclature :** contrôler la hauteur engagée des embases et le dégagement des queues/vis, le sens réel des modules réemployés, la révision du Click et la position de la barrière/borniers au-delà du support. Figer les références de passifs/fixations et la hauteur des sockets disponibles sans substitution de pas ; dimensionner et figer les retenues ; examiner ensuite le gabarit d’ensemble avant la commande.

Aucune autre mesure électrique n’est demandée maintenant. Après ces contrôles, adapter les cotes et retenues, relancer ERC/DRC/parité/exports et lever explicitement l’interdiction de fabrication. Ni achat ni fabrication ne sont autorisés par ce dossier.

## SPI3 et future tête

Longueurs de cuivre du candidat : SCK environ37 mm, MOSI/MISO environ33 mm, CS environ29 mm (somme des branches quand TP/rappel présent, voir JSON exact). Deux plans GND, quatre vias au total sur le support ; pas d’impédance contrôlée revendiquée. Les résistances série existantes sur la tête ST ne sont pas supprimées. Éviter toute branche de câble inutile ; maintenir le retour GND dans la nappe et la reprise d’effort avant les contacts.

**8 MHz non qualifié sur ce shield.** Les 30 cm de nappe ne sont pas déclarés fiables à cette fréquence par déduction. Commencer court, caractériser les lectures/configuration/FIFO sous la charge SD représentative ; toute réduction de fréquence serait une décision d’essai explicite, sans modifier silencieusement la référence firmware.

Carter peint relevé à56,5°C : la fixation vissée/magnétique, le couplage mécanique à la machine, la protection électrique et l’antichute restent une étude séparée. Aucun aimant, adhésif ou plastique n’est déclaré qualifié pour cet environnement.
