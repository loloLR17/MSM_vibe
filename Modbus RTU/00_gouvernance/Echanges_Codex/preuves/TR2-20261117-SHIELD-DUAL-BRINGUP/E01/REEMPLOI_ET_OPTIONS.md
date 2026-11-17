# Réemploi, coûts et options — aucune réparation ou dépense décidée

## Ce qui est acquis et ce qui ne l’est pas

Deux ensembles assemblés sont déclarés par l’utilisateur. Les modules ont pu être
permutés ; les identités Nucleo ne sont pas des identités de périphériques. La BOM
est un besoin nominal, pas un inventaire photographié de chaque pièce soudée.
J5 reste potentiellement absent et la reprise des masses002 n’est pas encore confirmée.

La mission V2.3 et son README rapportent une commande TME **historique de17,60€TTC
transport compris** : dix1x12, dix1x10, dix1x8, trois barres mâles2x40, dix résistances,
trois100n AVX, trois10µF Panasonic. Ce total est une information utilisateur historique,
**pas un devis actuel, une preuve de réception intégrale, ou une valeur récupérable**.
Les anciens prix de la BOM ne sont pas réutilisés comme prix du17/11. Aucun prix
actuel vérifié n’est nécessaire pour décider cet audit : coûts de remplacement,
PCB, transport et main-d’œuvre restent **INCONNUS**, sans estimation monétaire inventée.

| Élément | Quantité nominale pour2 / fait disponible | Catégorie prudente |
|---|---|---|
| Nucleo C05 |2 identifiées par UID etST-LINK |réutilisables sans dessoudage du shield ; préserver leurs mâlesCN12 et firmware |
| MKIGIBV2 + nappe + MKI208V1 |2 jeux attendus ; association actuelle non prouvée |modules enfichés réutilisables sans dessoudage ; capteurs non déclarés sains avantWHO |
| FRAM Adafruit4718 |2 modules lus, RDID et sauvegardes préservés |réutilisables en débranchant hors tension ; aucun effacement ; instabilité0011MHz à conserver comme réserve |
| Adafruit4682 |2 lecteurs attendus ; carte32Go unique transférée |lecteurs enfichés réutilisables ; défaut d’alimentation/masse002 distinct ; carte conservée |
| RS485 Isolator2 Click |2 attendus, J2 déclaré ponté ; révisionv102 seloncontrat |modules enfichés réutilisables sans désouder leurs mâles ; santé/configuration encore non qualifiées |
| microSD32Go |1 identifiée parCID, médias512Go exclus |réutilisable avec données ; aucun formatage demandé |
| PCB V2.3 |2 assemblés constatés ; lot commandé nominal5, reliquat non inventorié |conserver pour analyse/reprise potentielle ; les exemplaires nus du même dessin portent aussi le défautSPI3 |
| J1..J4 Zio |8 sections,100contacts parshield ; troisbarres2x40 historiquement commandées |soudés : récupération possible mais non garantie ; nombreuses soudures et risques de pads arrachés/plastique déformé |
| J5 Samtec |0..2 effectivement soudés, présence inconnue ; échantillons demandés |non montés : réemploi immédiat si réellement disponibles ; soudés : récupération thermique délicate, non garantie |
| J6/J7 femelles |4 sections prévues ; achat déclaré1x10 et non1x9 |soudées : récupération non garantie ; contact supplémentaire neutralisé et enveloppes à vérifier ; reste de stock non compté sans inventaire |
| J8 femelles1x12 |4 sections soudées attendues ; lot10 déclaré |mêmes risques thermiques ; éventuelles pièces non montées réutilisables, quantité restante inconnue |
| J9 femelles1x8 |4 sections soudées attendues ; lot10 déclaré |mêmes risques thermiques ; ne pas présumer la récupération sans dommage |
| R1..R4 |8 résistances10k ; lot10 déclaré |potentiellement récupérables, fils raccourcis et coût de main-d’œuvre peuvent rendre préférable un stock neuf déjà acquis |
| C1/C2 |2 de chaque ;3 de chaque commandés ; AVXSR205C104KAR/PanasonicEEUFR1H100 indiqués dansl’étude |potentiellement récupérables, longueurs de queues/polarité et échauffement à contrôler ; ne pas supposer interchangeabilité mécanique depuis la seule valeur |
| TP1..TP4 |8 accès prévus ; type réellement monté non inventorié |pads du PCB non récupérables ; boucles éventuelles récupérables sous réserve |
| fixations et pièces V2.7 |dossier prévu pour2 ; réception matérielle non confirmée |réutilisation mécanique à recouper avec option choisie ; aucune nouvelle impression ou commande lancée |

Les cinq connecteursNucleo + quatreinterfacesmodules représentent **164contacts
de connecteurs soudés parshield** dans le dessin. Déposer tous les connecteurs
pour sauver un PCB nu n’est donc pas une opération banale ; aucune garantie de
dessoudage sans dommage ou durée fictive n’est donnée. Les breakouts enfichés et
leurs broches restent, eux, déplaçables sans déposer ces164soudures. Toute dépose
réelle restera hors tension et exécutée par l’utilisateur après un plan retenu.

## Options comparées

| Option | Travail / composants | Risque et capacité de réalisation | ImpactV2.7 | Verdict pour décision |
|---|---|---|---|---|
| Reprise locale des2PCB |parcarte : isoler2liaisons erronées CS21/SCK22 puis ajouter4liaisons ; mêmeMCU et autresinterfaces ; fil isolé et maintien à définir |nécessite accès aux pistes, isolement vérifié et soudures propres ; ni couche/point de coupe ni outil utilisateur qualifiés ; ne pas compter de durée précise |G12,25 et axes peuvent être conservés ; passage/maintien des fils, hauteur et risque d’arrachement à vérifier |**première option à instruire**, car évite la dépose massive et réemploie les connecteurs ; pas autorisée à exécuter par cette note |
| Interposeur sousMKIGIBV2 |rediriger entréesbase21→sortie19,22→20,23→21,24→22 ; alimentations1/2/GND13 inchangées ; PCB/contacts/maintien supplémentaires |pas de découpe du shield ; plus de contacts et de hauteur, nouveau gateE-01 et fabrication nécessaires ; version imprimable non conçue |G Nucleo/shield peut rester12,25, mais hauteur duMKI/nappe et étrier/capot augmentent : compatibilité actuelle non acquise |repli si reprise locale inaccessible ; délai/dépense et nouvelle mécanique inconnus |
| Nouveau PCB corrigé |variante distincte, quatre affectations corrigées, revueE-01, ERC/DRC puisCAM ; réutiliserNucleo/modules et stock non monté compatible |solution propre à terme mais délai de production/livraison, nouvelle soudure et qualification ; pas de nécessité de racheter les breakouts |si positions/contour conservés, interfaceV2.7 probablement préservable ; vérifier les vrais composants/queues ; aucune promesse de compatibilité sans contrôle |repli si réparation trop risquée ; pas de commande automatique |

Une liaison externe temporaire par câbles ne qualifierait pas le shield intégré
et réintroduirait les fragilités que le projet veut supprimer. Elle n’est pas
retenue comme résolution du prototype robuste.

## Préconisation à soumettre à ChatGPT

Préserver Nucleo, tous les breakouts, les médias et les pièces non montées ; **ne
rien dessouder maintenant**. Faire contre-lire l’audit E-01 complet, puis préparer
le plan de reprise locale seulement si ses points d’isolement et de soudure sont
accessibles et fiables. Le schéma de correction logique déjà publié est une
topologie, pas une procédure de coupe. Ne pas ajouter un pont avant isolement
des anciens nets et ne pas décaler leMKIGIBV2 dans son support.

Le choix définitif, les capacités/outils de soudure et une dépense éventuelle
restent à l’utilisateur. La FRAM001 à1MHz et la masse002 doivent être traitées
séparément ; corrigerSPI3 ne les résout pas par déduction. Aucun nouvel achat,
dessoudage, frais de port, prix ou délai de fabricant n’est engagé par cet audit.
