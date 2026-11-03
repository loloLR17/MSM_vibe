# Shield TR2 V1.2 — dossier de précommande candidat

**BLOCKED_FABRICATION — NE PAS COMMANDER.** Révision1.2, mission TR2-20261103-SHIELD-V12-PRECOMMANDE, base c32ca3ec. Projet KiCad10.0.6,90×100mm,2couches,1,6mm,35µm. Aucune intervention matérielle ni achat.

Commencer par [synthèse de libération](docs/PRECOMMANDE_V1_2.md), [J5/empilage](docs/J5_ET_EMPILEMENT_V1_2.md), [assemblage](docs/ASSEMBLAGE_ET_RESERVES.md), [FRAM](docs/ALIMENTATION_FRAM_V1_2.md), [BOM](docs/BOM.csv) et [budget](docs/BUDGET.md). Les décisions V1.2 remplacent les gates détrompage/boîtier final des documents V1.1 historiques, conservés avec leur contexte.

- `cad/TR2_Shield_Proto_V1.kicad_pro` : projet éditable ; suffixe historique conservé, cartouche1.2. Bibliothèques locales incluses.
- `exports/gabarit-C05-V1_2.pdf` : deux pages1:1 pour les deux C05 et vue dessous miroir ; impression100%, barres100mm. Coordonnées tirées du PCB réellement sauvegardé, **pas d'une CAO C05 inaccessible**.
- `docs/comparaison-C05.csv` : centres attendus, source réelle et colonnes constructeur nulles ; aucune concordance C05 fabriquée.
- `exports/schema.pdf`, `implantation-dessus.pdf`, `cuivre-dessous-miroir.pdf`, `interfaces-boitier.pdf`, STEP/PNG **carte nue uniquement**.
- `exports/BLOCKED_FABRICATION/` : CAM GerberX2/Excellon candidat ; marque NE_PAS_COMMANDER conservée.
- `mechanical/retainers.scad` et `REVIEW_ONLY/` : retenues positives, clés automatiques retirées ; aucun modèle physique qualifié.
- `docs/checks/` : ERC/DRC strict avec parité, readback, contrôles netlist/PCB/CAM/perçages, nominal mécanique, gabarit et logs réseau publics ST.

59 broches connectées,19empreintes,187pads dont11NPTH,4vias. Nets et positions J1–J9/ancrages inchangés ; courtyards sockets/perçages corrigés selon pièces, pas passifs adaptés. Firmware, campagnes CN0/GI0/OBE, FRAM et médias préservés. Aucune compilation firmware nécessaire pour cette CAO.

Reconstruction (dans une copie préservée) : `tools/generate_schematic.py` Python standard, `tools/generate_board.py` puis `apply_routes.py` avec Python pcbnew KiCad et argument répertoire du dossier ; trajets existants `cad/routes.json`. `tools/export_design.py` appelle les contrôles réels KiCad ; `read_board.py` pcbnew, `verify_exports.py`, `verify_mechanical.py`, `generate_interfaces.py`, `generate_preorder.py` (PyMuPDF). Règles projet conservées octet pour octet ; aucune réduction de DRC autorisée. Générer les deux retenues modifiées via OpenSCAD avant contrôles maillages ; autres modèles inchangés réutilisés. Les scripts ne prétendent pas vérifier la réalité C05 ni autoriser la fabrication.
