# V1.7 — vitre latérale ST-LINK et USER

**Recommandation prioritaire : une petite vitre fermée dans la paroi verticale côté ST-LINK, sous le shield et à côté de l’accès USB ST-LINK. Aucun changement électronique proposé.** La correction explicite de l’utilisateur pendant l’exécution prime : la visée observée est **latérale quasi horizontale**, pas depuis le dessus. Une photo est décrite comme montrant plusieurs LED ; cela ne démontre pas encore l’identité et la visibilité des **trois** LD1/LD2/LD3 dans l’empilage équipé. Aucun fichier photo coté n’est disponible ici pour en déduire leurs centres.

**Étude documentaire/CAO terminée, compatibilité physique non qualifiée, fabrication toujours non libérée.** Le PCB, les Gerber, le firmware et les fichiers V1.5/V1.6 sont préservés. [Vues et coupes, PDF4pages](../../mechanical/V1_7/REVIEW_ONLY/v17-vues-coupes.pdf), [modèles et paramètres éditables](../../mechanical/V1_7/README.md), résultats [optique](optique.json) et [USER](chaine_USER.json).

## Vitrage retenu comme candidat

Repère X/Y KiCad, nord=Y décroissant, Z=0 dessus shield ; Y CAD est inversé. Reprise du modèle V1.5 : shield X20..110/Y17..117, épaisseur1,6 ; G11mm hypothétique. Proposition locale de fenêtre **16×8mm utiles**, X29..45/Z−11..−3, paroi Y−20..−17. Plaque **PC transparent1,5mm**, 20×12mm ; cadre démontable26×18mm, X24..50/Z−16..2, quatre passagesØ2,2 pour fixation future M2. Joint plat0,8mm, scénario comprimé0,6. Aucune ouverture libre, aucun effort transmis aux LED, aucune classe IP revendiquée. PMMA possible mais plus cassant ; PC préféré. Modèle de principe, pas dossier d’impression/commande.

Le cadre garde **2mm nominaux** avec chacun des inserts aveugles ST-LINK et nappe V1.5 ; l’ouverture est à10mm en X de la réservation USB. Les vrais corps/fiches, rayons de courbure et tolérances restent à vérifier. La vitre n’occupe ni le port ST-LINK ni la sortie de nappe. Fixations du shield et des modules conservées.

Dans le domaine de test X45..65/Y12..28/Z−12,6..−9,6, les rayons quasi horizontaux passent **sous le shield, le MKI, la retenue et la nappe**. Aucun centre LD n’est inventé : ce domaine n’est pas une métrologie. **3 564 rayons** ont été calculés pour297points et12observateurs ; **976** entrent dans la fenêtre et évitent les enveloppes élargies de **1,5mm**, avec0,11..4,26° par rapport à l’horizontale. L’obstacle résiduel du modèle est principalement la réservation ST-LINK :1146rayons la coupent ; d’autres ratent l’ouverture. Aucune conclusion sur trois LED réelles ne se déduit de ces pourcentages. Une seule position d’observation testée couvre au plus204/297points, pas tout le domaine. Les composants C05 sous shield non modélisés peuvent encore gêner la vue.

Par rapport à la fenêtre nord-est du toit V1.6 (visée48..52° de la verticale, marge0,246mm), ce candidat répond à l’observation corrigée et évite les obstacles supérieurs. **La marge V1.6 n’est pas considérée robuste** ; son étude est réutilisée sans relance des tests sur code inchangé. La vitre latérale est désormais prioritaire sous réserve d’une vue réelle des trois LED. Si elle échoue, garder le plan B V1.6 trois LED tamponnées via Morpho, sans l’exécuter automatiquement. Ni fibres, ni nouvelle électronique, ni nouvelle découpe PCB dans cette étude.

## USER : dimensionnement ajustable de principe

La mesure utilisateur **10,2mm** donne, avec G11, le sommet B1 à **z−2,4**. Le complément publié donne le premier clic à **≈0,4mm** ; ce n’est ni une tolérance garantie, ni la course totale admissible. Avec un jeu au repos cible **0,6mm**, la pointe au repos est à **z−1,8** ; distance sous-face de couvercle→pointe **38,8mm**, déplacement nominal jusqu’au clic≈1,0mm. Formule générale : `z_B1 = −G − 1,6 + 10,2` ; un changement de G exige un nouveau réglage.

Proposition : tigeØ4 guidée deux fois dansØ4,4, patin flottant à pointe souple, **réglage axial de la pointe proposé±2mm**, cage ouvrable, bride captive et **butée structurelle à1,6mm** de course externe. La réaction d’un appui de doigt après butée va à la cage puis au panneau/boîtier ; le patin ressort reste le seul chemin vers B1. La tête garde0,4mm au panneau à la butée dans le modèle. Rappel doux indépendant, fixation de cage, filetage/contre-écrou et membrane anti-poussière sont des détails à finaliser, pas des composants qualifiés.

Avec les **scénarios non mesurés** jeu0,45..0,75 / clic0,3..0,5 / course externe1,5..1,7, la compression après clic à la butée varie0,25..0,95mm. Si B1 ne bouge pas, le ressort pourrait prendre **1,25mm** : réserve proposée≥1,5mm avant talonnage. **L’effort reste non qualifié** : vérifier `F0+k×0,25 ≥ F_clic_max` et `F0+k×1,25 ≤ F_admissible_min`, avec frottement/tolérances. Sans ces efforts ni la référence exacte de B1, aucun k/F0 n’est choisi et aucune protection absolue n’est affirmée. Un simple embout mou ne suffit pas à garantir la sécurité.

La recherche ST retrouve B1=`SW_BLUE` et HW17=`BLUEHAT`, pas une référence fabricant donnant sa courbe d’effort. Le manuel précise un capuchon démontable, mais **aucun retrait n’est proposé** : cela changerait la référence de la cote10,2. Course déjà acquise : **aucune nouvelle mesure utilisateur demandée**. Ne jamais chercher une butée de B1 en forçant.

## Avant commande et après

Il n’y a **pas de nécessité de modifier le shield démontrée** à ce stade. Pour l’établir avant commande : vérifier sur l’empilage réel les trois trajets LD1/2/3 et le couloir B1 hors shield/modules, ainsi que les autres gates J5/Zio/modules V1.5. Les centres restent null. Le USER est illustré sur un axe hypothétique(34;124), pas placé sur B1 ; couloir de test X28..40/Y119..130, au sud du shield. RESET reste sans accès extérieur.

La forme exacte de la vitre et les découpes de la paroi/panneau peuvent être finalisées après cette vérification si elles n’impliquent pas le PCB. Ne pas fabriquer les Gerber tant qu’une solution LED et USER compatible avec l’empilage n’est pas démontrée. [Checklist ciblée](CHECKLIST.md) et [sources/limites](SOURCES.md). Aucun geste physique, achat ou accès STM32 réalisé.
