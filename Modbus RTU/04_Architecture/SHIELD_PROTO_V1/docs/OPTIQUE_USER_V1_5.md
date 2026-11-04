# Trois LED et USER — options documentées, couplage réel non démontré

## Identité et emplacement

Lecture du code conservé `experiments/r3_durable_fat32/src/standalone_board.inc` : LD1/PC7 verte = WAIT clignotant et fin normale fixe ; LD2/PB7 bleue = scan/démarrage/acquisition ; LD3/PG2 rouge = état d’erreur. B1 USER est lu sur PC13. Les identités concordent avec ST UM2861 et le BSP Nucleo144 ; les coordonnées mécaniques ne sont pas dans le code. Le schéma C05 consultable dans l’index documentaire ST confirme B1/B2 et les fonctions, sans fournir de centres de placement ; aucun fichier de fabrication C05 n’a été reçu.

UM2861 Rev8 figure4 situe les trois LED près du **nord** (côté ST-LINK), USER et RESET au **sud** (côté CN15). Le rectangulaire secteur d’inspection du modèle est une réservation large, **pas** la position d’une LED ni une tolérance. Sa projection croise le contour du shield Y17..117 et partiellement le MKI provisoire. Le tracé exact doit être établi avant toute découpe. Le modèle 3D ne place donc aucune pseudo-LED/pseudo-B1 sur la Nucleo.

## Trois guides standards : candidats, pas références définitivement retenues

| Option | Données constructeurs | Décision actuelle |
|---|---|---|
|3 × Bivar PLP1-250-F transparents, individuels|Corps6,4 mm, guideØ3, brideØ3,8 ; perçage nominal .121pouce=3,0734mm, tolérances constructeur à respecter ; panneau .047..093pouce (1,19..2,36mm). PC clair, panneau arrière pressfit.|Modélisés sur coupon distinct, panneau2 mm. Ne traversent pas un shield opaque par magie : trajet vertical réel non dégagé et positions inconnues. Longueur6,4 choisie pour illustrer le montage du coupon, pas pour le TR2.|
|3 × MENTOR1216.1001|Fibre150 mm / cœur1000µm, rayon **>9mm**, tête/boîtier et lentille séparés ; boîtier d’entréeØ11 et hauteur7 mm, fixation par deux ergots dans le PCB ; lentille panneauØ4.|Option de déport examinée mais **non directement montable** : trous requis sur Nucleo non autorisés, compatibilité LED et entraxes inconnue ; trois corpsØ11 peuvent se gêner. Un support indépendant porterait une adaptation à qualifier, pas une conformité constructeur présumée.|
|MENTOR1216.1002|Fibre80 mm / cœur2000µm, rayon **>20mm**, même principe de fixation PCB ; lentille panneauØ3,6 dans le dessin.|Alternative non retenue : rayon plus grand dans le faible espace sous shield et mêmes inconnues de fixation. Ne pas transférer son rayon à la1001.|

Les perçages d’un couvercle plastique doivent être finis/contrôlés selon les cotes des guides ; une imprimante ne garantit pas d’emblée un pressfit. Un joint local ou capot transparent peut limiter les projections ; ni pièce standard ni assemblage imprimé ne qualifient une protection IP globale. Les corps optiques sont **réellement transparents du commerce**, les cloisons opaques éventuelles ne font qu’isoler les trois canaux. Aucun tube opaque n’est présenté comme un guide.

Une trajectoire future doit nommer, pour chaque LD, XYZ entrée et sortie, section, rayon, fixation et dégagement au shield/MKI/J1–J5. Aujourd’hui **ces trois trajets ne sont pas démontrés**, et la CAO ne dessine pas de rayons à partir de centres inventés. Un guide droit dans le secteur couvert nécessiterait un passage PCB et peut aussi rencontrer le MKI ; un flexible nécessiterait un dispositif d’entrée fixé hors des composants et un rayon compatible. Aucun achat ni perçage n’est décidé sur cette base.

## Repli par fenêtre rapportée

`window-alternative-NOT-OPTICAL-VALIDATION.step` montre les pièces d’une fenêtre fermée PC/PMMA1,5 mm et cadre vissé, en alternative au panneau aveugle LED. Joint silicone1 mm proposé, fixation démontable. Ce modèle démontre seulement la **construction de la fenêtre**, pas sa ligne de vue : le shield peut toujours masquer les LED en dessous. Un emplacement latéral/guide coudé ne sera retenu qu’après relevé du secteur réel et contrôle de la trajectoire ; pas de certification IP.

## USER captif et RESET protégé

Le coupon séparé contient tigeØ5/guideØ5,5, têteØ10, brideØ12 et butée. Repos fictif : pointeZ3, bouton témoinZ2,4 => jeu0,6. Descente limitée1 mm : tête encore libre jusqu’au panneau, bride arrêtée sur le support, compression géométrique fictive0,4. Les contrôles vérifient les intersections aux deux positions. Le porte-guide comporte deux montants et une traverse inférieure vissée ; deux fixations au panneau ferment le chemin de charge de butée vers la structure, sans prendre appui sur le shield. Les passages de vis M2 sont des dimensions de concept, à adapter au matériau et à la tolérance d’impression.

**Ce coupon ne qualifie pas le vrai B1.** Sa position dans le TR2, longueur de tige, course/effort admissibles, rappel (ressort ou diaphragme) et embout compliant restent à dimensionner. Le dessin ne contient pas de ressort qualifié. Une butée rigide seule ne garantit pas l’absence d’effort dommageable ; la tolérance du jeu et la compliance sont des gates. Le panneau USER de l’assemblage principal reste aveugle. RESET ne reçoit aucune commande extérieure ; son éloignement de la tige réelle reste à vérifier sur la même photo cotée.

## Coût supplémentaire : options séparées de la BOM électrique

Relevé DigiKey de PLP1-250-F : **0,69 USD/unité**, stock affiché54 821 ; six guides pour deux boîtiers = **4,14 USD hors port/taxes** au tarif unitaire. C’est un prix affiché pour une option non qualifiée, pas un panier final ni un devis livré. Pas de prix fiable retenu ici pour le support/fibre MENTOR, ressort, embout, joints, fenêtre et impression ; ces postes restent inconnus, pas chiffrés à zéro. La BOM électrique V1.4 n’est pas modifiée, cible50EUR non démontrée. Aucun achat.

Sources : [Bivar PLP1](https://www.bivar.com/light-pipes/rigid-panel-press-fit/plp1/), [dessin fabricant PLP1-XXX-F](https://www.bivar.com/parts_content/Datasheets/PLP1-XXX-F.pdf), [copie catalogue fabricant](https://media.digikey.com/pdf/Data%20Sheets/Bivar%20Inc%20PDFs/PLP1.pdf), [MENTOR catalogue pp61](https://www.mentor.de.com/productpdfs/ll/ll14-61.pdf), [prix option Bivar](https://www.digikey.com/en/products/detail/bivar-inc/PLP1-250-F/586481). Dessins reçus et consultation des index sont distingués dans sources-v1-5.json.
