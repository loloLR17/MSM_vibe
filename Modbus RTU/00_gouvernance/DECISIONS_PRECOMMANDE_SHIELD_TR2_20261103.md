# MSM TR2 — Arbitrage précommande PCB shield — deux prototypes

**Décision ChatGPT du 03/11/2026 à la suite de la revue utilisateur.** Référence `loloLR17/MSM_vibe` / `main`. Document de pilotage : ne vaut ni approbation du bon à tirer, ni achat, ni validation mécanique physique. Application du principe A-06, et de la revue `Echanges_Codex/archives/TR2-20261102-SHIELD-PROTO-V1-1/REVUE_CHATGPT_SIMPLIFICATION_DETROMPAGE.md`.

## Décisions techniques figées pour la version PCB de prototype

| Élément | Choix retenu |
|---|---|
| Nombre | Deux ensembles TR2 ; fabrication envisagée en lot standard cinq PCB, dont trois réserves |
| Technologie PCB | FR4 standard, deux couches, épaisseur nominale 1,6 mm ; cuivre standard 35 µm et règles DRC/largeurs V1.1 conservées, sous réserve DFM fabricant |
| Format | Candidat 90 × 100 mm ; **à confirmer géométriquement** par compatibilité avec NUCLEO-U575ZI-Q révision C05 et les ancrages. Ne pas changer ce format sans justification |
| Architecture | Nucleo et tous les breakouts réemployés, aucun nouveau MCU/PoE/électronique batterie ; protocoles SPI1 FRAM, SPI2 SD, SPI3 IIS3DWB et LPUART RS485 inchangés |
| J1–J4 | Embases mâles sur face inférieure du shield vers Zio CN7/CN8/CN9/CN10, assemblage sans flexion ni traction sur contacts |
| J5 | **Nouvelle observation opérateur, 03/11/2026 : les deux Nucleo C05 disposent déjà de broches MÂLES soudées aux positions CN12-69/70.** Conserver ces connecteurs et **adapter le shield avec une femelle à deux contacts côté inférieur** pour recevoir la paire existante, sans dessoudage de la Nucleo. Fonction inchangée : CN12-69/PG4→DE du RS485 ; CN12-70/PG6→NC, sans pont. Vérifier réellement implantation transversale, pas exact, sens des broches, hauteurs de corps/pointes, longueur utile d'engagement, compatibilité J1–J4 Zio, trou/empreinte/face de montage KiCad et dégagement mécanique avant libération. Aucune reprise du J5 mâle actuel sans correction CAO et nouveau DRC. Le manuel ST indique Morpho non équipé par défaut, mais **ce ne sont pas les deux cartes physiques de l'utilisateur** ; l'observation opérateur prévaut pour ses exemplaires |
| J6/J7 | Modules FRAM Adafruit4718 et microSD Adafruit4682 dans sockets femelles face dessus, **retenue par trous existants et entretoises vissées**, démontables |
| J8/J9 | MKIGIBV2 et RS485 Isolator2 Click dans sockets femelles face dessus ; retenir par étrier simple démontable ou prise d'appui protégée, **pas de détrompage mécanique automatique obligatoire** sur les deux prototypes montés manuellement par l'utilisateur. Marquage de l'orientation et vérifications hors tension obligatoires |
| MicroSD | J7 en bord -X, lecture et extraction carte depuis l'extérieur après arrêt propre, trajet de carte sans obstruction ; le boîtier imprimé s'adaptera si nécessaire |
| RS485 | Click vers bord +X, bornier et tournevis accessibles, GND2/A/B isolés du GND logique ; reprise d'effort sur boîtier |
| Capteur | Nappe ST d'origine 12 voies, sortie vers -Y avec protection des arêtes/retour de masse et serre-nappe fixé au boîtier ; pas de qualification SPI3 à 8 MHz déduite de la CAO |
| Exploitation | LED Nucleo visibles via fenêtre/guides simples, bouton USER actionnable, RESET protégé, ST-LINK accessible pour maintenance ; ajustement du boîtier et dimensions définitives des guides/poussoirs **après PCB** si les zones d'accès sont préservées |
| Alimentation | Externe secteur230V hors boîtier → adaptateur5V existant → CN15 USB-C, ou autre source 5V appropriée selon montage ; aucun 230 V, chargeur batterie, powerbank, switch secteur ni régulateur de batterie ajouté au PCB |
| Boîtier | Impression 3D par un tiers après réception/validation du shield ; ce n'est **pas une condition d'émission de Gerber PCB** si interfaces/keepouts mécaniques restent libres |
| Coût | Cible indicative environ 50 EUR pour **deux shields équipés** (hors modules réutilisés, powerbanks et boîtier). Le scénario V1.1 documenté est de 53,27 EUR, hypothétique ; privilégier solution simple et devis réel. Aucune dépense nouvelle sans décision utilisateur |

## Conditions impératives avant commande des PCB nus (et non de toutes les pièces du boîtier)

1. **Empilage PCB sur Nucleo MB1549 C05** : vérifier sur source constructeur **correspondant à la révision C05** les centres, numéros et orientation des Zio CN7/8/9/10, la paire J5/CN12-69/70, les trois trous d'ancrage, le contour et les dégagements. ST publie les fichiers `MB1549 Board design project files` (https://www.st.com/resource/en/board_manufacturing_specification/mb1549-bdp.zip) et `MB1549 Manufacturing files` (https://www.st.com/resource/en/board_manufacturing_specification/mb1549-manufacturing.zip), **contenu non encore inspecté par ChatGPT** ; Codex doit vérifier version/révision réelle incluse, et ne jamais assimiler C02/C03 à C05 sans justification. Si aucun CAD C05 fiable, faire un gabarit 1:1 et demander à l'utilisateur une comparaison sur chacune des Nucleo, hors tension.
2. **Empreintes et dégagements PCB critiques** : confirmer les brochages, pas, alignement et sens effectifs de MKIGIBV2 (DIL24, entraxe ~15,2 mm utilisateur déjà acquis), Click, Adafruit SD/FRAM ; vérifier qu'aucun module ne force, que les réservations de retenue ne rencontrent ni composants ni isolation et que la carte SD puisse sortir côté -X. Les dimensions d'un étrier et des futurs passe-câbles peuvent être figées **après réception du PCB** si aucune cote manquante ne modifie ses trous/son placement/son contour.
3. **Réalité de J5 et approvisionnement des embases** : confirmer le Morpho CN12 non peuplé sur les exemplaires physiques ; choix et hauteurs d'engagement des femelles 2 points sur Nucleo et des mâles sur shield, des sockets 12 points (référence V1.1 temporairement hors stock), empilage final et dégagement de soudure. Commander les accessoires séparément, seulement après inspection/référence correcte.
4. **Contrôle final des fichiers de fabrication** : ERC, DRC avec parité et règles strictes, netlist vs PCB, Gerber X2/Excellon, centres de perçages et contour, comparaison visuelle du 1:1, BOM / coûts. Pas de marque `FABRICABLE` tant qu'un des trois points précédents manque. Le constructeur reçoit les Gerber uniquement après feu vert de commande de l'utilisateur.

## Ce qui N'EST PAS un blocage préalable à la fabrication des PCB nus

- Un détrompeur asymétrique anti-inversion universel sur J8/J9, devenu facultatif.
- Le boîtier 3D fini ; ses découpes USB, guides de lumière et poussoir définitif.
- La preuve de 8 h, la validation thermique/vibratoire, le choix de batterie du produit final ; ils exigent le prototype réel et des essais ultérieurs.
- La conception finale de la fixation de la tête du capteur sur le carter.

## Prochaine exécution Codex

Produire un **dossier de précommande** ciblé avec exploitation des ZIP constructeur, solution vérifiable pour J5, empreintes/broches/hauteurs, correction minimale éventuelle des sources KiCad, plan gabarit avec vrais repères et BOM révisée, ERC/DRC/export validés. **Ne pas commander** et ne rien modifier sur les STM32, FRAM, microSD ou firmware. Si la géométrie C05 reste inaccessible, demander exclusivement la **photo ou comparaison gabarit 1:1 indispensable**, sans réinventer les informations déjà mesurées. Les données de campagne et historiques restent intacts.


## Avenant opérateur — J5 mâle déjà présent / achat économique (03/11/2026)

Deux Nucleo **physiques** portent déjà une paire mâle soudée aux positions Morpho CN12-69/70 selon confirmation explicite de l'utilisateur. La solution retenue initialement (« femelle à ajouter sur Nucleo et mâle J5 sur shield ») est désormais **abandonnée pour ces deux exemplaires**. Ne pas dessouder les broches existantes par défaut. **Concevoir et vérifier une femelle deux contacts sur le dessous du shield** ; la vraie hauteur d'empilage avec Zio reste un contrôle à effectuer quand les cartes seront accessibles. Éviter de supposer la taille exacte du corps ou des broches d'après leur seul caractère « mâle ». Les photos/mesures ultérieures doivent être limitées à ce qui change réellement l'empreinte ou l'engagement.

L'utilisateur confirme également que les **connecteurs spécialisés et coûteux ne sont pas une exigence de qualité** pour deux prototypes : optimiser la BOM avec des **barrettes mâles/femelles 2,54 mm standard, de marque ou distribution fiable, éventuellement sécables**, compatibles électriquement, avec perçages, dimensions, montage face dessous, hauteur utile d'emboîtement et disponibilité démontrés. Le seuil cible d'environ 50 € pour deux shields reste une orientation budgétaire, pas un prix inventé ni une raison d'affaiblir les fixations. Conserver les retenues vissées et le contrôle mécanique. Aucun achat automatique ; la commande des PCB nus reste bloquée jusqu'à vérification physique des points réellement déterminants.
