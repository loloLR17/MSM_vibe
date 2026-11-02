# Shield TR2 prototype V1 — candidat 0.1

**BLOCKED FABRICATION : ne pas commander ces Gerber.** Les réserves mécaniques et de détrompage sont détaillées dans `docs/ASSEMBLAGE_ET_RESERVES.md`. ERC/DRC sans anomalie ne lèvent pas ces réserves.

Mission `TR2-20261102-SHIELD-PROTO-V1`, départ `ad924a3d88ee9cd0908823aa333fec183c114dd3`. Projet éditable KiCad **10.0.6**, FR4 deux couches, 90 × 100 mm, 1,6 mm, cuivre 35 µm. Réemploi des cinq cartes du montage ; aucun firmware, média, banc ou câble modifié.

- Ouvrir `cad/TR2_Shield_Proto_V1.kicad_pro` ; schéma, PCB routé, symboles et empreintes locaux inclus.
- `exports/schema.pdf`, `implantation-dessus.pdf`, `cuivre-dessous-miroir.pdf` : relecture électrique et implantation.
- `exports/shield-nu.step` et `.png` : **carte nue uniquement**, aucun modèle complet d’empilage Nucleo/modules.
- `exports/gabarit.svg` : gabarit 1:1, enveloppes élevées provisoires, repères et cotes. Imprimer à 100 %, contrôler une cote avant comparaison.
- `exports/BLOCKED_FABRICATION/` : véritable jeu Gerber X2 et Excellon candidat, interdit de lancement en fabrication en l’état.
- `docs/MATRICE_INTERFACES.md`, `SOURCES.md`, `BOM.csv`, `BUDGET.md`, `ASSEMBLAGE_ET_RESERVES.md`, `PLAN_QUALIFICATION.md` : dossier de revue et prochaines opérations.
- `docs/checks/` : commandes exécutées, ERC, DRC avec parité schéma, comparaison indépendante netlist/PCB/CAM/perçages.

59 broches connectées, 19 empreintes, 187 pads dont 11 NPTH, 4 vias ; 32 liaisons routées hors plans GND. Aucun changement de protocole, de plage ±8 g, de filtrage AC1 ou de politique d’écrêtage.

Les connecteurs Nucleo se montent côté dessous ; les modules côté dessus. Les contacts individuels Dupont et la breadboard disparaissent de la **proposition**, mais sa tenue mécanique et son fonctionnement restent NON VÉRIFIÉS avant réalisation. La fixation antichute et thermique de la tête sur le réducteur reste une étude séparée.

Reconstruction : `tools/generate_schematic.py` (Python standard), `tools/generate_board.py` avec Python pcbnew de KiCad (argument : répertoire du dossier), `tools/route_board.py` (numpy/shapely), `tools/apply_routes.py` avec pcbnew, `tools/export_design.py`, `tools/read_board.py` avec pcbnew, `tools/verify_exports.py`. Ces générateurs sont réservés à la reconstruction du candidat : ne pas les exécuter sur des modifications manuelles non sauvegardées. Les exports passent seulement après ERC/DRC. Les bibliothèques sont relatives à `${KIPRJMOD}` ; aucun modèle 3D externe requis.

Les contrôles firmware `tr2_validate.sh` n’ont pas été lancés : aucune source firmware ni interface logicielle n’est modifiée. La validation appropriée est ici la CAO et ses exports, sans qualification physique.
