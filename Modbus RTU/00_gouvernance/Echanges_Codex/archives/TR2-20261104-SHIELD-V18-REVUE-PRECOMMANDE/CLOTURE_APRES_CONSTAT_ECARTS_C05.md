# Clôture V1.8 et fait nouveau — écart physique de plusieurs millimètres

Date : 2026-11-05. Dépôt `loloLR17/MSM_vibe`, branche `main`.

V1.8 `TR2-20261104-SHIELD-V18-REVUE-PRECOMMANDE` : rapport Codex `DONE` pour audit documentaire et CAO seulement ; code/audit `7a09a88050aebe8be262247657057679222b3f5e`, clôture rapport `a2b4fd58a8488ae6a330a668451e1d6b1b36509b`. Revue indépendante ChatGPT `6ead15762b5f9a13670b8637abd462b9d3309d89`. Verdict antérieur de commande **NO-GO physique** maintenu.

**Fait nouveau utilisateur le 05/11/2026 :** contrôle gabarit V1.8 sur Nucleo avec impression dont barre étalon mesure **100 mm exactement** ; **certains connecteurs semblent décalés de plusieurs millimètres**. Les connecteurs fautifs, les axes et le décalage exact ne sont **pas encore identifiés**. Ne pas prétendre que les deux Nucleo ont été comparées ni que le défaut est déjà localisé. Observation physique suffisante pour **STOP fabrication / STOP achat** et rechercher la géométrie MB1549 C05 exacte plutôt que demander une série de corrections manuelles utilisateur.

Recherche documentaire ChatGPT : la page officielle [NUCLEO-U575ZI-Q](https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html) annonce `MB1549 Board design project files v4.0` (25/01/2024), `MB1549 Manufacturing files v3.4` (24/02/2025), et [schéma spécifique MB1549-U575ZIQ-C05](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf). **Existence de ressources confirmée sur site ST, leur ZIP, contenu et correspondance exacte C05 non encore téléchargés/qualifiés.** Vérifier licence ST et version/révision dans les fichiers.

Le tableau `docs/comparaison-C05.csv` contient des colonnes `ST_C05_x_mm`, `ST_C05_y_mm`, `delta_mm` **vides** et des statuts `C05_NOT_VERIFIED`. Le plan V1.4 était issu d'une reconstruction depuis UM2861, pas d'une implantation C05 industrielle vérifiée. La nouvelle observation rend une correction de source indispensable, même si ERC/DRC KiCad sont verts : ils ne contrôlent pas l'alignement à une carte externe.

L'ancienne mission et son rapport restent intacts. Prochaine mission spécifique : **obtenir/exploiter les CAO/fabrication ST officielles C05**, comparer XY et orientations avec PCB actuel, corriger le strict nécessaire, produire gabarits et CAM révisés **non libérés sans dernière vérification sur cartes physiques**. Chercher à éviter d'autres mesures manuelles dispersées. Aucun achat, aucune commande, aucune intervention STM32 sous cette décision.
