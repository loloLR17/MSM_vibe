# Sources et reproductibilité — V1.6

Consultation le 04/11/2026, sans achat ni accès matériel. Les prix/stocks sont des relevés de pages, non des devis ou réservations.

| Source | Utilisation / limite |
|---|---|
|[ST MB1549-U575ZIQ-C05, 12 juillet 2023, feuilles3/5](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf)|Vrai schéma C05 consulté via l’index textuel du PDF : références/valeurs et transistors des LED. **Ni fichier CAO de placement ni PDF intégral téléchargé**. Aucune inspection graphique du schéma n’est revendiquée ; recoupement avec UM2861. L’accès Windows normal a expiré à12,12s, zéro octet ; preuve `preuves/PDF-Windows-access.json`. Aucune protection ou politique modifiée, pas de campagne répétée de téléchargement.|
|[ST UM2861 Rev8, exemplaire STJapan](https://www.stmcu.jp/wp/wp-content/uploads/2021/10/UM2861_Rev8.pdf)|Exemplaire complet déjà conservé localement, relu pages24,33,34,37. SHA256 **1eb153334dfffe947972ecd3065b36dfe7f0ddb294e30aa0bde252e72e5f35dd**. Table19 :140 contacts analysés ; tables15–18 : absence des trois GPIO dans Zio. Résultat dans `connecteurs_ST.json`. Le manuel ne donne pas les centres LED C05.|
|[ST STM32U575 datasheet](https://www.st.com/resource/en/datasheet/stm32u575ai.pdf)|DS13737, caractéristiques de sortie GPIO et domaine VDDIO2. Les bornes calculées restent inférieures aux conditions usuelles de pilotage de quelques mA ; vérifier mode/niveau/budget total avant schéma final. Aucun maximum absolu n’est traité comme courant d’emploi.|
|[Bivar PLP1-XXX-F revR](https://www.bivar.com/parts_content/Datasheets/PLP1-XXX-F.pdf)|Dimensions, longueur21,1mm disponible dans la famille (ne pas limiter par erreur à19), trou/panneau et bride. Un guide court n’atteint pas le couvercle actuel. Aucun rendement/couplage optique Nucleo n’est garanti.|
|[DigiKey PLP1-250-F](https://www.digikey.com/en/products/detail/bivar-inc/PLP1-250-F/586481)|Affichage0,69USD à l’unité, stock54 821 ; total six4,14USD horsport/taxes. La pièce6,4mm n’est pas déclarée compatible avec le trajet TR2.|
|[onsemi BC546/547/548/549/550, BC550/D, fév2026 rev6](https://www.onsemi.com/pdf/datasheet/bc550-d.pdf)|NPN THT de principe, BC547 VCEO45V, brochage à vérifier sur référence choisie ; saturation documentée à10mA/0,5mA. Le nouveau montage utilise des courants moindres mais doit être qualifié ; aucune luminosité déduite du seul gain.|
|[TME BC547B-ONS](https://www.tme.eu/en/details/bc547b-ons/npn-tht-transistors/onsemi/bc547b/)|Stock483, MOQ1 ; 0,21USD à1,0,17USD dès5. La fiche marchand indique65V, **en désaccord avec les45V du constructeur pour BC547** : seules disponibilité et tarification sont utilisées.|
|[Kingbright L-934MBDL chez TME](https://www.tme.eu/de/details/l-934mbdl/runde-tht-leds/kingbright-electronic/)|Exemple de bleue rejetée : Vf annoncée3,8..4,5V, inadaptée à3V3. Ne pas sélectionner une LED sur le seul diamètre/couleur. Aucune référence finale bleue adoptée dans l’étude.|

## Sources internes gelées

Référence initiale **6d97906d32ec6b30a6db9caa140fc02481aa142d**, contrat `base_sha=ee947e532fbd418cd787246d95f2237452198126`. Sources étudiées : `cad/TR2_Shield_Proto_V1.kicad_pcb`, `mechanical/V1_5/parameters.json`, `tools/build_integration_v15.py`, interfaces/BOM/docs V1.4/V1.5 et `experiments/r3_durable_fat32/src/standalone_board.inc`. La clôture humaine V1.5 et son rapport BLOCKED sont conservés, sans réécriture.

`preuves/conservation.json` compare **chaque fichier antérieur du dossier shield** octet pour octet à son blob Git initial et enregistre SHA256/taille. Les seules modifications suivies autorisées hors nouvelle étude sont le journal, le rapport actif et la nouvelle archive. Le diff global vérifie qu’aucun firmware/fichier de fabrication n’a changé. Les anciens STEP/STL binaires, KiCad, Gerber et manifests sont inchangés.

## Reproduction logicielle, sans matériel

Depuis la racine du worktree V1.6 :

1. L’extraction native est faite par `lire_pcb.py <racine-du-dossier-SHIELD_PROTO_V1>` avec le Python KiCad10 (`/mnt/c/Program Files/KiCad/10.0/bin/python.exe`, chemins Windows via `wslpath -w`). Elle utilise **LoadBoard sans SaveBoard**, produit uniquement `preuves/pcb_natif_lecture_seule.json` :191pastilles,396segments,2zones. Les zones ne sont pas réécrites/remplies.
2. Exécuter `analyse.py` avec Python/numpy/matplotlib. Dans l’environnement actuel : `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python 'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/ETUDE_LED_V1_6/analyse.py'`. Environnement existant réutilisé sans installation. Les résultats sont `preuves/*.json`, trois SVG et le PDF3pages. Le script vérifie les intersections, les rayons, les bornes et la conservation ; les hypothèses restent explicitement marquées.
3. Réouvrir/rendre les trois pages avec PyMuPDF existant (`/home/lolo/dev/msm/cache/shield-pylibs`), puis inspection visuelle. Réouvrir les SVG en XML. Ce contrôle est consigné dans `pdf-relecture.json` et `validation-finale.json`. Aucun rendu esthétique synthétique ni image générée.

Pas de cross-build/essai firmware ni nouvelle campagne ERC/DRC/CAM : les fichiers concernés sont conservés exactement et les validations V1.4/V1.5 déjà acquises sont réutilisées. Les seuls nouveaux calculs vérifient cette étude. **Aucune observation matérielle ni qualification optique/électrique réelle.**
