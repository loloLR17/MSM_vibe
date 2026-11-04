> **Intégration mécanique V1.5 disponible** : [assemblage éditable et vues](mechanical/V1_5/README.md), [décision avant PCB](docs/DECISION_LIBERATION_PCB_V1_5.md), [maturité du boîtier](docs/MATURITE_BOITIER_V1_5.md), [LED/USER](docs/OPTIQUE_USER_V1_5.md). Le PCB reste V1.4 inchangé. **READY_FOR_PHYSICAL_CHECK / BLOCKED_FABRICATION** : les guides LED/USER doivent être vérifiés AVANT commande s’ils impactent la géométrie du shield. Cette règle V1.5 remplace les phrases antérieures qui reportaient toute ergonomie après PCB. Aucune fabrication autorisée.

# Shield TR2 V1.4 — femelle2×3 pour mâles CN12 en quinconce

**BLOCKED_FABRICATION / NE PAS COMMANDER.** Mission `TR2-20261104-SHIELD-V14-J5-2X2` ; malgré son identifiant initial, le format2×3 et le déplacement de fixation ont été **autorisés par l'utilisateur durant l'exécution**. Aucun changementNucleo, achat, flash ou accèsmédia.

Lire [précommandeV1.4](docs/PRECOMMANDE_V1_4.md), [brochage et Z](docs/PINOUT_ET_Z_V1_4.md), [montage](docs/ASSEMBLAGE_ET_RESERVES.md), [BOM](docs/BOM.csv), [quantités/prix](docs/CONNECTEURS_ACHATS_V1_4.csv), [budget](docs/BUDGET.md). Les fichiers explicitementV1.1/V1.2/V1.3 restent historiques ; ne pas fabriquer leurs raccords1×2.

- `cad/TR2_Shield_Proto_V1.kicad_pro` : projetKiCad10 éditable, cartouche1.4,90×100mm,2couches/1,6mm/35µm.
- J5SamtecSSW-103-01-F-D dessous :6positions65..70 ; mâles66/67/68/69, alvéoles65/70 vides ; **local5=PG4→DE, cinq autres contactsNC**. Aucune brocheNucleo supplémentaire.
- `exports/gabarit-C05-V1_4.pdf` : deux gabaritsA4/1:1 pour les deuxC05 et miroir ; barres100mm. Ce n'est pas une extractionC05 fabricant.
- Schéma, implantationsdessus/dessous, cuivre dessous miroir, interfaces et retenuesPDF/SVG ; STEP/PNG **carte nue**. `exports/BLOCKED_FABRICATION/` : CAMX2/Excellon candidats interditsdecommande.
- `docs/checks/rebuild-v14.json`, contrôlesnatifs/CAM et `docs/checks/v14/` : sources/erreurs/corrections, autorisation de format et contrôles de conservation.191pads dont11NPTH,59connexions,4vias ; quatrePTH nouveaux et ancrageClick inférieur déplacé94,5/112,5.
-32trajets géométriques inchangés, autre module/Zio/ancrageNucleo conservé. J5trous1,04 selonSamtec ; autres connecteurs1,02. ÉtrierClickSCAD/STL adapté, autres modèles réutilisés. Aucun modèle physique ou détrompeur automatique qualifié.

Reconstruction en copie préservée : generate_schematic(Python), generate_board/apply_routes(pcbnewKiCad, argumentrépertoire), export_design(KiCadCLI), read_board(pcbnew), verify_exports/verify_mechanical/verify_preorder et generate_interfaces/generate_preorder(PyMuPDF). Projetstrict conservé ; les comparateurs contrôlent explicitement la nouvelle numérotation et les changements autorisés. Testsfirmware non concernés.

Mesures mâles8,22/2,40 et entraxeMKI15,2 acquises ; **ne pas les redemander**. EmpilageZio naturel, gabarit des deuxC05 et dégagement réel restent à confirmer avant libération. Boîtier/LED/poussoir finis différés.
