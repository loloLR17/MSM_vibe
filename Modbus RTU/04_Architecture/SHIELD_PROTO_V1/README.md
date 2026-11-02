# Shield TR2 prototype V1.1 — candidat non libéré

**BLOCKED FABRICATION : ne pas commander ces Gerber.** Les réserves mécaniques et de détrompage sont détaillées dans `docs/ASSEMBLAGE_ET_RESERVES.md`. ERC/DRC sans anomalie ne lèvent pas ces réserves.

Mission `TR2-20261102-SHIELD-PROTO-V1-1`, départ `d0433fb5352d857344d892ed86099df6a09a8f25`. Sources V1 préservées au commit `1d2aa6683e972a3751f69229db4022283f55b3fb` ; sa clôture administrative ChatGPT n’a pas levé ses réserves. Projet éditable KiCad **10.0.6**, FR4 deux couches, 90 × 100 mm, 1,6 mm, cuivre 35 µm. Réemploi des cinq cartes du montage ; aucun firmware, média, banc ou câble modifié.

- Ouvrir `cad/TR2_Shield_Proto_V1.kicad_pro` ; schéma, PCB routé, symboles et empreintes locaux inclus.
- `exports/schema.pdf`, `implantation-dessus.pdf`, `cuivre-dessous-miroir.pdf` : relecture électrique et implantation.
- `exports/shield-nu.step` et `.png` : **carte nue uniquement**, aucun modèle complet d’empilage Nucleo/modules.
- `exports/gabarit.svg` et `interfaces-boitier.svg`/PDF : même plan 1:1, microSD déplacée vers le bord, volumes de retrait/préhension, accès borniers et tournevis, nappe, LED/USER, USB et reprises d’effort. Imprimer à 100 %, contrôler la barre50mm.
- `mechanical/retainers.scad`, cinq STL `mechanical/REVIEW_ONLY/` et `exports/retenues-isometriques.svg`/PDF : modèles paramétriques de revue, **pas pièces libérées pour impression de campagne**.
- `exports/BLOCKED_FABRICATION/` : véritable jeu Gerber X2 et Excellon candidat, interdit de lancement en fabrication en l’état.
- `docs/MATRICE_INTERFACES.md`, `SOURCES.md`, `BOM.csv`, `BUDGET.md`, `ASSEMBLAGE_ET_RESERVES.md`, `PLAN_QUALIFICATION.md` : dossier de revue et prochaines opérations.
- `docs/checks/` : commandes exécutées, ERC, DRC avec parité schéma, comparaison indépendante netlist/PCB/CAM/perçages.

59 broches connectées, 19 empreintes, 187 pads dont 11 NPTH, 4 vias ; 32 liaisons routées hors plans GND. Aucun changement de protocole, de plage ±8 g, de filtrage AC1 ou de politique d’écrêtage.

Les connecteurs Nucleo se montent côté dessous ; les modules côté dessus. Les contacts individuels Dupont et la breadboard disparaissent de la **proposition**, mais sa tenue mécanique et son fonctionnement restent NON VÉRIFIÉS avant réalisation. La fixation antichute et thermique de la tête sur le réducteur reste une étude séparée.

Reconstruction : `tools/generate_schematic.py` (Python standard), `tools/generate_board.py` avec Python pcbnew de KiCad (argument : répertoire du dossier), `tools/route_board.py` (numpy/shapely), `tools/apply_routes.py` avec pcbnew, `tools/export_design.py`, `tools/read_board.py` avec pcbnew, `tools/verify_exports.py`. Ces générateurs sont réservés à la reconstruction du candidat : ne pas les exécuter sur des modifications manuelles non sauvegardées. Les exports passent seulement après ERC/DRC. Les bibliothèques sont relatives à `${KIPRJMOD}` ; aucun modèle 3D externe requis.

Les contrôles firmware `tr2_validate.sh` n’ont pas été lancés : aucune source firmware ni interface logicielle n’est modifiée. La validation appropriée est ici la CAO et ses exports, sans qualification physique.

Nouveautés V1.1 : déplacement J7 de3,5mm vers-X sans rotation ni changement de net ; perçages dissymétriques M2,5/M3 pour deux étriers démontables, modèles de selles/poussoir ; tableau de sûreté **J1–J9 et chaque câble** dans `docs/SURETE_CONNEXIONS_V1_1.md`, plan fonctionnel dans `docs/INTERFACES_BOITIER_V1_1.md`. Les positions réelles LED/USER restent inconnues, aucune géométrie C05 inventée. Budget scénario53,27€ pour2shields, cible50€ non tenue et coût livré non démontré ; avenant V1.1-A : bloc secteur externe5V existant sur CN15, powerbanks/switch USB différés.

Après changement CAO : exécuter les contrôles ERC/DRC complets et exports, `tools/read_board.py`, `tools/verify_exports.py`, puis `tools/verify_mechanical.py`. Ce dernier vérifie nominalement les perçages/maillages et préserve les nets V1 ; il ne qualifie pas les cotes hypothétiques. `tools/generate_interfaces.py` produit les plans. Le cache OpenSCAD et les modèles fabricants ne sont pas des dépendances cachées de l’ouverture KiCad : aucune reconstruction nécessaire pour consulter les fichiers livrés.
