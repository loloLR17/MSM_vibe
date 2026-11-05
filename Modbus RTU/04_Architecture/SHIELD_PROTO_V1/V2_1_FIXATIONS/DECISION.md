# V2.1 — fixation D126 retenue pour un premier prototype

**GO géométrique sous hypothèses ; qualification physique et approvisionnement des supports bloqués ; fabrication des PCB non libérée.** La fixation n’est pas définitivement qualifiée. Aucun changement de l’électronique V1.9, des trous ou des Gerber.

## Solution retenue

Trois supports isolants identiques en **demi-couronne**, hauteur **12,25 ±0,10 mm**, enveloppe Ø6, alésage Ø2,4. Le profil est ouvert jusqu’à l’axe : il n’a pas de mince paroi avant susceptible de casser. Ligament radial : 1,8 mm ; section comprimée : 11,875 mm² ; volume : 145,47 mm³.

L’ouverture regarde CN15 au point haut, ST-LINK aux deux points bas. Elle doit rester orientée à ±5°. La vis traverse le support ; celui-ci ne reçoit aucun filetage. Les fichiers [STEP](cad/D126-support.step), [SCAD](cad/D126-support.scad), [STL de revue](cad/D126-support.stl) et [implantation](cad/implantation-3-supports.step) sont fournis, sans impression effectuée.

Vis nylon DIN84 M2×20, tête Ø3,8 sous la Nucleo ; deux écrous nylon DIN934 M2 contrebloqués au-dessus du shield. Les écrous ont une clé de 4 mm et un diamètre aux coins de 4,619 mm : ils ne sont pas assimilés à une tête Ø4,5. Aucune rondelle large ni vis métallique n’est ajoutée.

## Comparaison des options

| Option | Résultat |
|---|---|
| Tube mince standard et cale | Économique, mais longueur, jeu des vis et marge au socket ne sont pas garantis ensemble. Non retenu. |
| Demi-couronne D126 | Davantage de matière porteuse et dégagement du côté Zio. Candidat prioritaire, sous conditions de fabrication et de tenue. |
| Reprise sur le châssis | Repli possible, mais ajoute des pièces et dépend du coffret. Aucune nouvelle conception générale exécutée. |

PETG dense est proposé pour un prototype local ; PA66 usiné ou PA12 SLS sont des alternatives utilisant le même STEP, sans qualification implicite de ces procédés. Six supports sont nécessaires, plus deux de réserve. Prestataire, prix et délai de fabrication restent non vérifiés.

## Marge et essais

Un premier profil avec méplat à 1 mm, orienté pareil partout, croisait deux solides ST près de l’ancrage haut : essai rejeté et conservé. Le profil final supprime une demi-couronne et se retourne à cet ancrage.

Le déplacement libre n’est pas supposé limité à 0,15 mm. En tenant compte des deux cartes et du jeu sur la vis :

`D = min((3,33−1,90)/2 ; (2,93−1,90)/2 +0,15) +(2,60−1,90)/2 =1,015 mm`.

Ces valeurs sont des budgets d’acceptation, pas des mesures certifiées des deux cartes. Avec R≤3,10, face avançant au maximum de 0,10 et rotation≤5°, l’avance maximale du profil est 0,369662 mm. La marge au bout du socket vaut alors :

`2,285−0,125−0,380−0,075−1,015−0,369662 =0,320338 mm`, au-dessus de la garde de 0,20.

La demi-tolérance de longueur du corps (0,125), sa bavure (0,380), la position ancrage/socket (0,075), le déplacement et la forme du support sont distingués. L’erreur de forme n’est pas soustraite deux fois. Les neuf cas CAO testés — trois ancrages, nominal et deux positions limites — ne montrent aucune intersection avec les 1 060 solides ST. Ce n’est pas un balayage exhaustif de toutes les translations et rotations.

La visserie nominale ne présente aucune collision imprévue. Les petits recouvrements aux portées tête/PCB sont conservés dans les résultats : profondeur de 4,42 nm et volume de 1,458e-5 mm³, classés comme contacts d’appui numériques par leur position et profondeur. Aucun seuil global de collision n’est relevé.

Le stack nominal mesure 18,65 mm sous la tête ; une vis de 20 mm dépasse de 1,35 mm après les deux écrous. Tête à Z−2,90 ; sommet des écrous à 17,05 ; pointe à 18,40 ; bas des modules à 24,85. Avec Lvis≥19,8, G≤12,35, PCB≤1,76 et écrous≤1,6 chacun, le dépassement reste≥0,73 mm. Dimensions réelles à confronter à ces bornes.

## Limites de résistance et décision

L’effort cible de calcul de 5 N donnerait 0,421 MPa sur la section et environ 1,47 MPa sur la demi-portée sous la tête. **Ce n’est ni une capacité prouvée ni un couple de serrage prescrit.** Excentricité, friction, couches, température, fluage et vibrations restent à qualifier. Le maintien à ±5° dépend du montage et du contreblocage, pas d’une clavette démontrée.

G et l’engagement J5 restent des portes distinctes : les résultats conditionnels V2.0 sont conservés, sans nouvelle qualification. LED, USER simple, câbles et isolation RS485 ne sont pas requalifiés par cette étude.

Un lot de cinq PCB peut être envisagé comme **prototypes à risque circonscrit**, après revue indépendante et décision financière humaine explicite. Il faudra contrôler à froid un premier exemplaire avant alimentation, puis qualifier la tenue. Aucune commande n’est autorisée ici et aucune nouvelle impression papier n’est nécessaire.
