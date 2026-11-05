# Clips sans vis — étude discriminante

**NO-GO pour adopter maintenant un double clip FFF simple. D126 V2.1 est conservé comme candidat de prototype à risque explicité.** Ce verdict ne signifie pas que tout clip serait impossible : une solution à rattrapage de jeu peut être conçue, mais elle ajoute une fonction élastique, un débattement et une qualification qui ne sont pas démontrés ici. Pas de longue refonte, pas de STL prétendument utilisable.

## Contraintes communes reprises

Trois ancrages ST V1.9 aux coordonnées shield(32,616;21,192131),(33,89;97,39),(79,61;97,39). Trous nominaux shieldØ2,8 et NucleoØ3,2 ; budgets V2.1±0,13 utilisés comme hypothèses, pas tolérances constructeur garanties. Deux PCB1,6±10%, G=12,25±0,10. Ni trou agrandi, ni cuivre déplacé, ni isolation sacrifiée.

D126 est une demi-couronne ouverte Ø6/alésage2,4, ligament1,8, section11,875mm², ouverture CN15 au point haut et ST-LINK aux points bas, orientation±5°. La marge V2.1 de0,320338mm résulte de son profil orienté et de budgets précis ; **elle ne s'applique pas à un clip circulaire, à ses crochets ou à son volume balayé pendant l'insertion**. Un tube plein ou des ergots latéraux pourraient retrouver les collisions près de Zio déjà observées et conservées dans V2.0/V2.1.

## Contre-exemple calculé : capture rigide simple

Un clip doit accepter PCBmax1,76. Supposons une gorge de capture2,00mm donnant0,24mm de passage au plus épais. Sur PCBmin1,44, elle laisse0,56mm par extrémité. Deux extrémités non préchargées permettent jusqu'à **1,12mm de séparation supplémentaire**, donc G12,25..13,37 sous traction, très au-delà de la cible±0,10.

Au maximum de ce jeu, l'insertion J5 nominale deviendrait8,22+8,51−13,37=**3,36mm**, sous le minimum Samtec3,68. C'est un **contre-exemple pour cette gorge rigide**, pas une simulation d'un clip conçu ni une preuve universelle. Réduire la gorge sous1,76 empêche d'accepter l'épaisseur haute. Une languette de précharge ou un rattrapage axial d'au moins0,32mm par carte est nécessaire pour couvrir l'intervalle1,44..1,76 sans ce jeu ; sa raideur, son fluage et son effort sur PCB doivent alors être définis.

Pour rendre l'ordre de grandeur explicite : hypothèse de crochet hautØ3,30, colØ2,60 ; au trou shield minimal2,67, chaque branche doit s'effacer de0,315mm. Au trou maximal2,93, le crochet garde seulement0,185mm radial de prise. En bas, crochet distinctØ3,80/colØ3,00 pour trou Nucleo3,07..3,33 : effacement0,365mm et prise minimale0,235. Les cols ont seulement0,07mm de jeu diamétral au minimum ; retrait, bavures et précision FFF ne sont pas couverts.

Avec buse0,4, une hypothèse de deux lignes donne une branche0,8mm et une longueur libre4mm. La poutre uniforme à petit déplacement donne `epsilon=1,5*t*delta/L²`, soit **2,36%** à la racine du crochet haut. Formule dérivée dans [calcul-clips.json](preuves/calcul-clips.json), non limite admissible. L/t=5 est court : pied réel, géométrie de crochet et FFF invalident une utilisation comme calcul certifiant. Aucune valeur matériau ni capacité de plusieurs cycles n'est supposée. Le [constructeur BASF](https://ultrasimweb.basf.com/snapfit/blog/introduction/) rappelle que géométrie, forces et déformation admissible du matériau doivent être confrontées ; ses données de plastiques ne qualifient pas notre PETG imprimé.

## Procédés et démontage

| Critère | Double clip FFF | D126 + nylon | Maintien par boîtier |
|---|---|---|---|
| Impression PETG/PA | Fentes, cols et crochets fins ; couches et retrait critiques. PA absorbe l'humidité ; pas de grade/conditionnement qualifié. | Profil épais simple déjà éditable. Hauteur finie/planéité à accepter, coût de fabrication réel inconnu. | Appuis larges envisageables, mais dépendent du coffret final et de ses tolérances. |
| Orientation | Axe vertical : flexion au pied sollicite aussi les liaisons de couches. À plat : supports, fentes et hauteur à reprendre ; ni orientation ni réglage garanti. | Face d'appui et G mesurables ; maintien de l'ouverture±5° non prouvé en vibration. | Une fermeture doit retenir positivement les deux cartes sans charger les seuls contacts électriques. |
| Traction/vibration | Pas de force de retrait calculable sans géométrie/grade/friction ; rupture d'un ergot pourrait laisser le shield libre ou un fragment près des contacts. | Trois fixations/contreblocage, mais couple, fluage, maintien et endurance non qualifiés. | Nécessite une pièce de pression ou des logements, sans effort transmis à USER/modules. |
| Démontage | Il faut atteindre deux branches aux trois points, y compris dessous ; aucun accès sous Nucleo et près Zio démontré. | Dévissage accessible selon coffret, pas de flexion répétée d'ergot. | Démontage du boîtier peut libérer les cartes ; pas une retenue indépendante démontrée. |
| Isolation | Matériau isolant seulement ; ne dispense pas de vérifier voisinage cuivre et volumes balayés, ni l'isolement RS485. | Enveloppe nominale et trois points déjà étudiés ; nylon maintenu, pas de vis acier. | Aucun bossage ne doit réduire les séparations ou toucher soudures/queues. |

Un ergot n'est pas validé en lui appliquant seulement la marge statique D126. L'insertion doit aussi dégager les six bras comprimés, puis leur détente, sans déplacer les sockets. Ici la cinématique, les efforts de dépose et le rattrapage axial suffisent à refuser l'adoption immédiate. Aucun effort de retrait/vibration ni nombre de cycles n'est inventé ; ces résultats restent **inconnus**.

## Clip standard réellement documenté

Le [dessin Essentra MDLSP1-12M-01, P160344-2 issue2](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/7134/mdlsp1-12m-01.pdf), et non une photo générique, prévoit **deux trousØ2,50±0,05**, PCBmax1,65, hauteur12, diamètre enveloppe4,6, nylon6.6. Il ne correspond ni aux deux diamètres2,8/3,2 ni à PCBmax1,76 ni à G12,25±0,10. **NO-GO de cette référence** sans modifier le projet. Son corps circulaire n'hérite pas de la garde D126 ; pas de chiffrage d'une pièce incompatible.

## Décision proportionnée

Conserver les fichiers D126 V2.1 sans les reproduire ni les modifier. L'offre de visserie moins chère est étudiée séparément. Aucun modèle de clip n'est fourni pour fabrication ou revue3D : les conditions contractuelles d'une variante vraiment simple et plausible ne sont pas réunies dans ce concept. Le [schéma de principe](principe-clips.svg) est éditable, **sans échelle et non fabricable**, pour visualiser le problème axial seulement.

Premier exemplaire : contrôle à froid unique de G, emboîtement sans force, enveloppe des pièces et orientation D126 ; pas de papier supplémentaire. Le risque mécanique circonscrit accepté par l'utilisateur demeure possible pour un prototype, après décision humaine de dépense distincte. Il ne transforme pas un jeu axial trop grand en engagement électrique garanti.
