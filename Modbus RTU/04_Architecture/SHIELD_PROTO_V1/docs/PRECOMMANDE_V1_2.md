# V1.2 — synthèse de libération des PCB nus

Mission TR2-20261103-SHIELD-V12-PRECOMMANDE, 03/11/2026. **NE PAS COMMANDER.**

| État | Sens et résultat |
|---|---|
| CAD_VERIFIED | Contrôles électriques/CAM du candidat uniquement ; voir sorties ERC/DRC et comparateurs. Ne signifie pas géométrie C05 vérifiée |
| READY_FOR_PHYSICAL_TEMPLATE_CHECK | Dossier et gabarit 1:1 disponibles pour une comparaison ultérieure sur les deux C05 |
| BLOCKED_FABRICATION | Compatibilité C05 et engagement J5/Zio non démontrés ; enveloppes réelles MKI/Click à confronter aux réservations |

## Ce qui est livré

Projet éditable KiCad10, schéma, PCB90×100×1,6mm, deux couches35µm, symboles/empreintes locaux ; CAM GerberX2 et Excellon **candidats**, PDFs dessus/dessous, STEP/PNG carte nue. Le nom technique des fichiers reste TR2_Shield_Proto_V1 pour préserver les liens ; le cartouche et la sérigraphie indiquent1.2.

Aucun connecteur Nucleo, ancrage ni piste de signal déplacé. Connexions identiques V1 : SPI1FRAM, SPI2SD, SPI3IIS3DWB, PG4DE. Correction des courtyards des sockets selon la famille Harwin choisie ; perçages connecteurs1,02mm selon dessin M20-782 (tolérance trou fini±0,05mm à exiger au devis). Pas C1→2,54mm et C2→2,00mm pour références réellement identifiées ; seules leurs bornes GND se déplacent, zones recalculées. 31 des32 trajets V1.1 conservés ; seule branche +3V3 TP1→C2 écartée localement du nouveau padGND, après deux violations DRC conservées dans checks/v12/DRC-C2-before-fix.json. Aucun trajet de signal modifié. La sauvegarde pcbnew a révélé une réécriture des seuils projet : générateurs corrigés pour conserver le projet octet pour octet, puis contrôles stricts relancés.

Les étriers simples restent vissés dans les quatre trous V1.1. Les deux petites barrières de détrompage ont été retirées du SCAD/STL ; cela ne crée pas de nouvelle fonction de protection contre l'inversion. Repères broche1, contrôle hors tension et montage manuel obligatoire. Les anciens documents V1.1 décrivent l'historique ; les présentes décisions V1.2 les remplacent pour les critères de précommande.

## Pourquoi la géométrie constructeur C05 reste NON VÉRIFIÉE

La page ST publie MB1549 Board design project files4.0 et Manufacturing files3.4. Ces numéros sont ceux des publications, **pas la preuve de présence C05**. Quatre téléchargements HTTPS ST/STChine expirent (~18s chacun), curl Windows expire ; Edge normal renvoie ERR_HTTP2_PROTOCOL_ERROR sur les deux ZIP et le schéma C05. Le repli HTTP/1.1 normal expire lui aussi sur la navigation. Aucun ZIP reçu, donc **aucun format interne, fichier, révision réelle ou hash d'archive n'est connu**. Le navigateur documentaire signale un contenu ZIP non exploitable ; il n'en a pas inspecté l'intérieur. Logs dans checks/v12. Aucun contournement d'accès, de certificat ou de protection.

La reconstruction disponible vient du dessin UM2861Rev4, non d'un PCB C05. Les sources Adafruit/Click sont conservées et identifiées dans SOURCES.md. Aucune égalité C02/C03→C05 n'est déduite.

## Dernier contrôle minimal — à autoriser dans une suite physique

Aucune manipulation demandée/exécutée dans cette mission. La voie documentaire reste préférable si les ZIP ST peuvent être obtenus normalement : conserver les ZIP originaux et hashes, inspecter leurs révisions, extraire coordonnées/contour et comparer le CSV fourni. Sinon :

1. Sur **chacune** des deux Nucleo C05 hors tension, comparer les pages1/2 de `exports/gabarit-C05-V1_2.pdf` imprimées100%, sans ajustement ; mesurer les barres100mm X/Y, superposer tous les centres CN7–10, CN12-69/70 et les trois ancrages. Une photo dessus avec règle et repérage C05 permet de tracer le résultat. Le papier ne prouve ni profondeur d'engagement ni absence de collision sous carte.
2. Dans la même inspection, confirmer les pads CN12 libres et soudables, le dégagement dessous pour deux queues3mm, et relever hauteur Zio/diamètre trous : comparer la femelle M20-7820242 proposée8,50±0,15mm. Engagement simultané J1–5, tête de vis et rondelle isolantesØ≤4,5mm aux ancrages Nucleo, aucun composant sous carte ni flexion : nécessaire avant libération. Voir J5_ET_EMPILEMENT_V1_2.md.
3. Une vue avec règle MKIGIBV2 et Click réel suffit à confronter **uniquement** les contours, révision et zones des quatre ancrages aux réservations du plan. Entraîxe MKI15,2mm et GND bas JP2 déjà acquis : ne pas les redemander. Pas de demande de détrompeur, de dimensions finales d'étrier, de LED, de poussoir ou de boîtier.

Ces constats pourront lever ou préciser les seules incertitudes PCB critiques. Ce n'est pas une permission de fabrication ni d'achat. Si un centre ou une hauteur diffère, corriger la CAO et régénérer les contrôles avant Gerber libéré. Le dossier ne peut honnêtement être déclaré prêt à commander aujourd'hui.

## Hors gates PCB nus

Dimensions finales des patins/étriers réglables, boîtier et selles, fenêtres LED/poussoir, détrompage automatique, qualification thermique/vibratoire, campagne8h, alimentation produit final. Les réservations de passage doivent cependant rester libres : SD-X, RS485+X, nappe-Y, CN15/ST-LINK/USER/RESET. Aucun gain de robustesse réel n'est revendiqué avant assemblage.
