# Décision proposée avant PCB nu — V1.5

**NON LIBÉRÉ / BLOCKED_FABRICATION.** La maquette et ses contrôles sont prêts pour une vérification physique ciblée. Le PCB V1.4 est conservé octet pour octet ; aucune découpe ou translation n’est justifiée à des coordonnées C05 encore inconnues. **Cela ne signifie pas qu’aucune découpe ne sera nécessaire.**

La revue révèle un défaut de réservation documentaire : le plan V1.4 regroupait LED et USER au sud. Dans UM2861 Rev8 figure4, LD1/LD2/LD3 sont au **nord**, USER/RESET au sud. Le nouveau plan corrige ces secteurs sans inventer de centres. Le secteur d’inspection nord (rectangle de travail, non une mesure) recoupe le shield 90×100 et partiellement l’enveloppe provisoire MKI. Une remontée optique droite vers le dessus ne peut donc pas être déclarée libre. Aucune ligne de vue réelle ou découpe ciblée n’est qualifiée.

## Ce qui est acquis

- Modèle éditable d’assemblage avec PCB exact, sockets/contreparties, modules distingués par provenance, fixations et volumes réservés. STEP relu, solides et maillages contrôlés ; vues et demi-coupes BRep fournis.
- SD fabricant alignée sur neuf contacts et deux trous, extraction rectangulaire nominale libre du boîtier après retrait de sa trappe. Accès outil RS485 nominal libre après retrait du capot local. Ces contrôles n’incluent ni les doigts réels ni un bornier de révision inconnue.
- Fond/couvercle démontables et inserts rapportés permettent d’ajuster beaucoup de détails de boîtier sans changer le circuit ; cela ne dispense pas de résoudre les passages qui traverseraient le shield.
- ERC/DRC/parité/CAM V1.4 réutilisés uniquement après preuve de conservation exacte du PCB/schéma/projet/Gerber/Excellon. Aucun test firmware ou flash nécessaire.

## Gates qui empêchent la commande

1. G naturel Zio/J5 et tolérances : exemple G=11 donne seulement 0,09 mm entre plastiques, sans marge de fabrication. Confirmer également le gabarit sur les **deux** C05.
2. Centres et hauteurs LD1/2/3, trajet d’entrée/sortie, fixation des guides et encombrement MKI réel. Guides droits, flexibles ou fenêtre restent des options non couplées. Résoudre la nécessité d’un passage PCB ou d’un déplacement MKI AVANT commande.
3. Centre/hauteur B1 et course/effort admissibles : projeter tout le volume du poussoir, avec jeu au repos et butée ; vérifier RESET à part. La seule localisation générale au sud ne prouve pas le dégagement du capuchon.
4. Enveloppes MKIGIBV2/IDC et Click réels, appuis d’étriers et dégagements ; positions et fiches USB réelles, sortie nappe, isolement et visserie. Les détails d’inserts sont adaptables au boîtier si ces volumes restent libres du PCB.

La fiche `mechanical/V1_5/REVIEW_ONLY/CONTROLE_PHYSIQUE_MINIMAL_2P.pdf` regroupe les informations minimales. Les cotes CN12 8,22/2,40 et MKI15,2 ne sont pas redemandées. Aucune manipulation n’est demandée sous cette mission sans matériel. À réception des informations, mettre les centres vérifiés dans parameters.json, résoudre les trajets, puis appliquer uniquement la correction KiCad démontrée si nécessaire et refaire alors ERC/DRC/CAM/gabarit. Revue ChatGPT et décision de commande humaine restent nécessaires.

Le boîtier complet, la tenue thermique/vibratoire et la fixation de tête au réducteur ne sont pas ajoutés comme qualifications préalables au PCB nu ; seules leurs interfaces susceptibles d’en modifier la géométrie constituent les gates ci-dessus.
