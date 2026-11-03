# Shield TR2 V1.3 — J5 femelle, connectique standard

**BLOCKED_FABRICATION / NE PAS COMMANDER.** Mission TR2-20261103-SHIELD-V13-J5-FEMELLE-BOM ; projetKiCad10,90×100mm,2couches,1,6mm,35µm. Aucun achat, flash, accèsmédia ou interventionmatérielle.

Commencer par [précommandeV1.3](docs/PRECOMMANDE_V1_3.md), [J5 et hauteurs](docs/J5_ET_EMPILEMENT_V1_3.md), [connecteursstandard](docs/CONNECTEURS_STANDARD_V1_3.md), [BOM](docs/BOM.csv), [quantités/prixMOQ](docs/CONNECTEURS_ACHATS_V1_3.csv), [budget](docs/BUDGET.md) et [montage](docs/ASSEMBLAGE_ET_RESERVES.md). Les documentsV1.1/V1.2 et exports explicitement nommésV1_2 sont historiques : **ne pas appliquer leur ajoutdefemelle surNucleo**. L'avenantopérateur etV1.3 prévalent.

- Ouvrir `cad/TR2_Shield_Proto_V1.kicad_pro` ; nomhistoriqueconservé, cartouche1.3. J5 actif est la variante `_FEMALE` sousshield, l'ancienne empreinte sanssuffixe reste historique/non utilisée.
- `exports/gabarit-C05-V1_3.pdf` : deuxpages1:1pourlesC05 + miroir ; barres100mm. Cotes issues du PCB sauvegardé, **pasCAOC05 vérifiée**.
- Schéma, implantationdessus, implantationdessous et cuivredessousmiroirPDF ; interfacesSVG/PDF, STEP/PNG **cartenueuniquement**. Aucun modèle d'empilage complet revendiqué.
- `exports/BLOCKED_FABRICATION/` : véritables GerberX2/Excellon candidats, interditsdecommande.
- `docs/checks/` : ERC/DRCstrict/parité, netlist/PCB/CAM/perçages et contrôlefaces, projet/routagepréservés. `docs/sources-v1-3.json` : dessinsréels/hashes et stocks/prixaffichés.
- RetenuesSCAD/STL deV1.2 inchangées, positives et démontables, REVIEW_ONLY ; clésautomatiquesabandonnées. Fixations/perçagesetinterfacesSD-X/RS485+X/nappe-Y conservés.

Génération : Pythonstandard `generate_schematic.py`, PythonpcbnewKiCad `generate_board.py` puis `apply_routes.py` (argumentrépertoire), `export_design.py`, `read_board.py`, `verify_exports.py`, `verify_mechanical.py`, `verify_preorder.py`, `generate_interfaces.py`, `generate_preorder.py` (PyMuPDF). Les scriptspréservent le projetstrict ; gabarits ne qualifient pas le fitphysique.32trajetsV1.2 inchangés, aucun firmware modifié, aucun tr2_validate/cross-build nécessaire pour cetteCAO.
