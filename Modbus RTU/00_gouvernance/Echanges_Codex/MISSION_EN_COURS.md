---
mission_id: TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF
status: READY
base_ref: main
base_sha: dcd3be60e147d610e171ff2ca37f099e35054dfe
created_at_utc: 2026-11-18
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2 — Nouvelle révision électrique corrective du shield, Gerber candidats et préparation urgente des achats

## Décision utilisateur, urgence et limites de libération

L'utilisateur demande de **générer dès maintenant un nouveau dossier Gerber/Excellon du shield corrigé**, puis de faire toutes les vérifications nécessaires avant de décider ultérieurement l'envoi au fabricant. Les composants nécessaires au montage des **deux shields corrigés** doivent être identifiés et commandés **au plus tôt** afin de viser une réception **avant samedi 21 novembre 2026**, sans promettre une livraison non confirmée. Cette mission doit fournir rapidement une liste de commande exacte et des options de livraison vérifiées lorsque les sources commerciales le permettent. **Ni commande/paiement de composants, ni téléchargement au fournisseur, ni fabrication PCB n'est autorisé par ce contrat ; ces décisions restent à l'utilisateur.** Ne pas demander d'accord pour les activités logicielles et Git autorisées.

La mission précédente `TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL` a été exécutée, son rapport `DONE` et sa revue indépendante bornée publiés ; contrat et rapport archivés sous `Echanges_Codex/archives/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/`. Son acquis RX/TX/Modbus sur TR2-001 doit être **préservé**, pas refait sans motif. La mission E-01 précédente est close uniquement pour l'audit documentaire, avec maintien du NO-GO physique global du shield actuel.

## Base de travail et objectif

Partir du HEAD réel `main` et des documents actuels `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, E-01, M-01, audit des 180 contacts et preuves physiques Click. Copier en **variante V2.4 électrique** distincte les sources natives de **`Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_3_ARNACO/`** ; préserver V2.3 historique et le kit mécanique V2.7 inchangés.

**Unique correction fonctionnelle attendue :** interface SPI3 J8 MKIGIBV2, en conservant le MCU PC9/PC10/PC11/PC12 et tous les autres nets/rails et composants dont la cohérence a été étudiée :
- ACC_CS → contact J8-19 (CS) au lieu de 21 ;
- ACC_SCK → J8-20 (SCL) au lieu de 22 ;
- ACC_MOSI → J8-21 (SDA) au lieu de 23 ;
- ACC_MISO → J8-22 (SDO) au lieu de 24 ;
- J8-23 et J8-24 doivent rester NC, sans faux contact ; vérifier le sens de numérotation constructeur.
Corriger dans la nouvelle variante symboles/types électriques, schéma, PCB, routage et sources de génération s'il y a lieu, puis régénérer **un seul** jeu Gerber/Excellon candidat dédié à V2.4, identifié `CANDIDAT_NON_LIBERE` sans ambiguïté. Aucun ancien Gerber V2.3 ou mélange de versions.

## GATE IMPÉRATIF M-01 — MÉCANIQUE 100 % IDENTIQUE

**La CAO d'impression `V2_7_KISS_ABS` est GELÉE et des pièces ont déjà été imprimées par Morvan en PLA. Aucune réimpression ne doit être imposée par la nouvelle révision PCB.**

Conserver à l'identique **contour 90 × 100 mm, épaisseur 1,6 mm et stack mécanique, tous perçages/NPTH/PTH/diamètres/positions, empreintes et coordonnées X/Y/rotations/faces J1..J9, R/C et points de test, connecteurs réellement montables, hauteurs et axes de tous les modules, interface Zio/Morpho/J5 et G12,25 ±0,10 mm, fixation D126, supports, étriers, patins, selles, jeux et passages câbles, accès CN15/ST-LINK/USER/LED/microSD/RS485**, et sérigraphie ARNACO existante. La position physique des pads J8 est inchangée ; **seuls les nets attribués à ses pads et les segments/vias de cuivre correspondants peuvent être redessinés**. Les nouvelles pistes doivent rester compatibles avec tous les dégagements mécaniques et électriques gelés.

Exiger comparateur **automatisé et indépendant de la génération** sur PCB natif V2.3 vs V2.4 avec liste exhaustive d'identité géométrique (contour, trous, stack, implantations et transformations, empreintes et sérigraphies) ; vérifier aussi interfaces STEP/enveloppes V2.7 sans régénérer ni modifier aucun STL/STEP/QUANTITES/PDF de Morvan. **Divergence mécanique = FAIL/NO-GO, pas de contournement ni ajustement des impressions.** Documenter la limite de la conformité purement numérique et les essais physiques de montage à froid ultérieurs.

## GATE IMPÉRATIF E-01 — ÉLECTRIQUE, indépendant des anciennes matrices

Reconstituer une référence constructeur **indépendante** des fichiers du projet pour ST Nucleo C05 / MKIGIBV2 / Adafruit FRAM4718 / SD4682 / Click RS485 MIKROE-3863, révisions identifiées, inclure NC, tensions, direction, /RE/DE, isolation GND2, protections, rails et straps. Ne pas supposer une référence d'après le nom d'une broche KiCad. Comparer exhaustivement **tous les 180 contacts et les 17 nets fonctionnels** de la nouvelle variante aux vrais pinouts, au schéma natif, aux bibliothèques, au PCB et aux **deux couches cuivre Gerber réellement exportées**, plus les trous Excellon. Comparer aussi avant/après V2.3 : les **seules modifications de net attendues** sont les quatre affectations J8 et routage requis ; préserver Click J9/LPUART/PG4/J5 ayant réussi les essais physiques, FRAM, SD, rails et passifs. Rejouer ERC/DRC/parité KiCad et comparateurs indépendants de CAM ; **0 FAIL constructeur non justifié, 0 court-circuit et 0 ouvert**, sinon bloquer le lot.

Ne pas traiter les anciens PASS E-01 comme vérification indépendante définitive ; remonter tout doute sur les sources constructeur exactes (ST miroir vs officiel, Click v101 mécanique/v102 schématique) et faire figurer précisément les preuves/codes/versions/hashes.

## Deuxième livrable prioritaire, exécutable immédiatement en parallèle : achats pour 2 shields

Établir à partir de la **BOM définitive correspondant à V2.4** et de la commande historique TME, du stock réellement probable **mais non présumé**, une **liste de composants à acquérir et quantités utiles pour deux shields complets**, références/MPN exactes, alternatives compatibles validées et répartition par fournisseur : J1..J9, résistances, condensateurs, éventuels accessoires et consommables. Distinguer :
- les deux Nucleo et les breakouts réutilisables **sans rachat** ;
- la visserie/boîtiers V2.7 déjà prévus ;
- les connecteurs **soudés sur les anciennes cartes** : récupération incertaine, chiffrer raisonnablement le scénario prudent de remplacement, mais ne commander aucun module complet inutile ;
- les pièces encore possiblement en stock de la première commande (à confirmer par utilisateur plutôt que déduire d'une BOM) ;
- les échantillons Samtec et leurs délais non maîtrisés.

Proposer **dès le premier checkpoint publiable**, sans attendre la revue finale des Gerber, un panier de préparation factuel avec liens fournisseurs exacts, MOQ, disponibilités/délais réellement consultés et frais si vérifiables ; distinguer un stock affiché d'une garantie de livraison. Viser une réception avant **samedi 21/11/2026**, très rapprochée ; expliciter les produits non livrables à temps et substitutions compatibles seulement après vérification E-01/M-01. **Aucun achat, paiement ni navigation de compte fournisseur sans décision utilisateur ; ne pas prétendre que l'achat est lancé.** Éviter de bloquer la correction CAO si les sources commerciales sont temporairement indisponibles.

## Exécution et livrables

Codex choisit les solutions techniques minimales, inspecte les sources réelles, modifie les fichiers CAO **dans la nouvelle variante seulement**, génère les Gerber/Excellon et preuves associées, lance toutes validations utiles, inspecte les diff et sauvegarde son travail par commits et push vers `main`. Ne pas réorganiser le dépôt, introduire refonte, ni modifier firmware, matériaux, valeurs passifs ou implantations sans nécessité démontrée ; en cas de conflit électrique insoluble à géométrie figée, émettre `BLOCKED` avec preuve au lieu de déformer la mécanique.

Livrer : projet KiCad V2.4 complet, netlist, schéma PDF, vues de routage, ZIP CAM candidat unique + SHA256 + inventaire, rapports E-01 et M-01 détaillés avec comparateurs et sorties brutes, matrice source-conformité des 180 contacts, diff V2.3/V2.4, **BOM/plan d'achat urgent avec liens et risques de délai**, rapport `DERNIER_RAPPORT.md` incluant SHA vérifié distant et limites. Le ZIP candidat est à relire à partir des octets publiés.

**Important :** publication et validation native par Codex ne constituent **pas** une libération de commande PCB. Après son rapport, ChatGPT réalise la revue indépendante du code/CAO, des sources constructeurs, de la matrice et des CAM ainsi que de la mécanique V2.7, puis décide du GO/NO-GO avec l'utilisateur pour la fabrication. Le paiement, l'envoi au fournisseur et les commandes composants relèvent d'une étape ultérieure explicite ; priorité à préparer les achats sans attendre inutilement la fin de la CAO.

Permissions : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Aucune intervention STM32 ni physique pour cette mission purement CAO/achats.
