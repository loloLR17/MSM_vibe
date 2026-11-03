> Historique V1.1 : pour la précommande courante, PRECOMMANDE_V1_2.md et ASSEMBLAGE_ET_RESERVES.md prévalent ; détrompage automatique et boîtier fini ne sont plus des gates PCB nus.

# Sûreté connexion par connexion — candidat V1.1

**Aucune ligne ci-dessous ne constitue une qualification physique.** CAO = géométrie/connexion candidate contrôlée ; BLOQUÉ = cote ou essai manquant. Toutes les pièces doivent se démonter sans dessouder un breakout. La sérigraphie ou la friction des sockets n’est jamais considérée comme retenue suffisante.

| Connexion | Retenue positive et reprise d’effort proposées | Détrompage | Montage / démontage | Preuve et réserve |
|---|---|---|---|---|
| J1 CN7 | Trois ancrages Nucleo/shield, entretoises isolantes ; header ne porte pas le shield | Implantation Zio globale asymétrique, orientation marquée | Engager tous headers sans forcer, puis visser ; déposer après déconnexion des câbles hors tension | Pads inchangés V1 ; ajustement C05/hauteur et insertion sans flexion NON VÉRIFIÉS |
| J2 CN8 | Même structure à trois ancrages | Idem, pas d’insertion individuelle décalée | Idem | Alignement C05 à contrôler |
| J3 CN9 | Idem ; dégagement sous SD élevé | Idem | Fixer l’ancrage caché avant SD | Empilage/soudure sous module à vérifier |
| J4 CN10 | Idem, pas d’effort du câble RS485 sur le header | Idem | Idem | Ajustement C05 à contrôler |
| J5 CN12-69/70 | Ancrage aval proche, appui isolant local à définir si déformation à l’insertion | Ensemble Zio+paire PG4, ne pas confondre numéros locaux1/2 et69/70 | Engagement simultané sans flexion ni ajout de câble Dupont | Paire électrique inchangée ; rigidité et dégagement local NON VÉRIFIÉS |
| J6 FRAM | Deux vis/entretoises M2,5 dans trous existants ; serrage avec butée de hauteur | Trous asymétriques par rapport à la rangée ; sens VIN documenté | Retirer deux vis, extraire droit du socket, remplacer module | Géométrie Eagle ; vis ne doivent pas plier le PCB ni solliciter les pins ; hauteur réelle à confirmer |
| J7 SD | Deux vis/entretoises M2,5, aucune sangle devant la carte | Rangée+deux trous imposent le sens retenu | Carte accessible par poche latérale sans déposer module ; remplacement module hors tension après deux vis | Orientation push-push sourcée, déplacement3,5mm ; accès/courses/vis vérifiés nominalement ; assemblage réel BLOQUÉ |
| J8 MKIGIBV2 | Étrier diagonal vissé, contacts de rive, colonnes hors enveloppes nominales ; nappe bridée ailleurs | Perçages M2,5/M3 et diagonale empêchent d’inverser l’étrier ; **clé du module à adapter à une forme réelle non mesurée** | Débrider nappe, enlever les deux fixations, soulever droit | SCAD/STL compilés, nominal cohérent ; détrompage module180°, surface d’appui et hauteur BLOQUÉS |
| J9 Click | Étrier vissé aux deux extrémités, doigt de rive et butée gauche ; serre-câble boîtier | Butée accepte le long côté isolé+X ; collision calculée du corps v101 renversé | Libérer câble au bornier, déposer deux vis, soulever droit sans tordre le module | Géométrie v101 ; concordance v102/révision réelle et rive d’appui BLOQUÉES |
| Nappe ST | Selle plate vissée sur structure, raccord libéré de toute traction ; chemin arrondi | Connecteur ST conservé, orientation/retention réelle à vérifier ; détrompage des12contacts non présumé | Dévisser selle avant raccord, manipuler corps du connecteur | Gorge8,4×1mm proposée, épaisseur/rayon et position du connecteur à mesurer ; SPI3à8MHz NON QUALIFIÉ |
| Câble RS485 | Selle ronde et boucle de mou sur boîtier ; aucun effort repris par les broches Click | A/B/GND2 marqués, contrôle de borne obligatoire ; pas de brochage normatif changé | Outil par accès supérieur, conducteur par accès latéral | Diamètre/cosse/courbure à définir ; aucune fixation conductrice traversant l’isolation |
| Câble CN15 | Selle sur structure, jeu autour de fiche ; adaptateur secteur externe5V viaCN15 | USB natif et source5V selon Nucleo ; JP6 inchangé par CAO | STOP/fin confirmé avant coupure, brancher selon procédure physique | Diamètre/fiche/coupe du boîtier à relever ; powerbank hors exigences V1.1-A ; bloc5V existant, pas de230V dans TR2 |
| Câble ST-LINK | Passage maintenance réservé, selle démontable si usage prolongé | Port ST-LINK distinct de CN15 | Connexion seulement en maintenance ; aucune présence requise en capture | Coque USB et dégagement C05 à confirmer |

## Pièces de maintien produites

`mechanical/retainers.scad`, exports binaires dans `mechanical/REVIEW_ONLY/`, vues `exports/retenues-isometriques.svg`/PDF. OpenSCAD2021.01 a compilé les cinq modèles sans avertissement (`--hardwarnings`) ; vérification indépendante : arêtes des maillages fermées et volumes signés positifs. Ce contrôle ne vérifie ni résistance, ni tolérances d’impression, ni ajustement matériel.

- **MKI** : ancrages(39;20) M2,5 et(71;54,5) M3, toit àZ22mm/épaisseur3mm, colonnesØ6mm ; patins nominalement sur rives du rectangle provisoire. La clé de rive est un placeholder ; une boîte rectangulaire symétrique ne suffit pas à empêcher l’inversion du module. Le détrompage final doit exploiter son connecteur ou une autre forme réellement relevée.
- **Click** : ancrages(91;75) M2,5 et(91;108) M3, même toit nominal, butéeX82,5..82,9. Le corps v101 correct commenceX83,41 ; renversé autour du connecteur il couvreX51,66..108,81 et rencontrerait cette butée. Rive libre de composants et tolérances à confirmer avant d’utiliser ce calcul comme preuve de détrompage réel.
- **Selles** : base24×8×8mm, trousØ3,4 espacés18mm ; gorge plate8,4×1 ou gorge rondeØ5 avec entrée ouverte. Modèles de principe à adapter au câble, avec butée évitant son écrasement ; aucune compression admissible inventée.
- **Poussoir** : têteØ8×2 et tigeØ3×12 ; capturer la tête sous la fenêtre/plaque du futur boîtier. Position/course/longueur finale non connues ; prévoir jeu et butée pour ne pas maintenir USER appuyé au repos.

Fixations isolantes prévues pour les étriers, avec écrou captif/freiné et rondelles ; longueur environ35mm à vérifier selon pile réelle, sans dépassement touchant Nucleo. Une géométrie compilée ne qualifie pas PA66/PA12/PETG, fluage, serrage ou tenue à56,5°C. Aucun ruban adhésif ou serre-fils non coté utilisé pour revendiquer le maintien.
