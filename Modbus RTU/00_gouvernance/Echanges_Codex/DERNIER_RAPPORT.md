---
mission_id: TR2-20261105-SHIELD-V25-KISS-FIXATIONS
status: IN_PROGRESS
base_ref: main
base_sha: f59de5d9a279e62ab04a640729cf56ede87e1607
initial_head: 72b47ee78c03b09d4a3d6d9b8f69eeb8258d7c35
result_sha: null
created_at_utc: 2026-11-05T20:59:34Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.5 — nylon D126 réservé, M3 métal standard, appuis conditionnels

**Simplification numérique exécutée ; publication/relecture distante à terminer.** Le coffret utilise une seule longueur de vis métallique standard M3×12. Les 8 vis nylon Ajile et 16 écrous achetés restent exclusivement affectés aux D126 (6/12 utiles +2/4 de réserve). **Aucun achat de nylon supplémentaire prescrit, aucun achat effectué.** L’ensemble mécanique reste partiel : appuis FRAM/SD et quatre étriers non libérés, réserves explicitement permises par la mission. Ne pas transmettre ce candidat comme dossier complet sans nouvelle décision humaine, conformément à la revue indépendante V2.4.

## Livrables et décisions

Nouveau dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_5_KISS_FIXATIONS/` : sources ciblées, STEP, STL, nomenclature séparée en quatre familles, schémas de montage, preuves et un seul ZIP de revue. Originaux V2.4 et tous PCB/CAM/firmwares conservés.

1. **D126 Nucleo/shield** : nylon Ajile déjà acheté seulement, sans changement de profil, coordonnées, G12,25 ou montage. Aucune ponction du stock pour la caisse ou les modules.
2. **Coffret structurel** : vis M3×12 métal DIN912/ISO4762 et écrous M3 DIN934 AF5,5/H≤2,4, stock ou assortiment courant. 17 points par kit,34 utiles pour deux kits, cible40/40 avec réserves optionnelles. Pas de nouvelle quincaillerie nylon ni de modèle exotique. Les 17 points ont une fonction : 4 couvercle,3 bras Nucleo,2 vitre,2 bride nappe,2 bride câble,2 trappe outil,1 USER,1 SD. Aucun point utile supprimé sans raison ; simplification du nombre de références et du montage, pas une promesse de baisse chiffrée du coût.
3. **FRAM/microSD, arrêt local de libération** : solution simple préparée = un appui isolant de hauteur naturelle H +une vis M2 ordinaire +un écrou par point, avec portées/rondelles isolantes. Deux points par module,8 au total. **H réel, longueur et contrôle final des volumes électriques manquent** ; aucun appui à11mm arbitraires libéré. Le générateur exige H et référence de mesure, écrit hors ZIP et laisse toujours le statut non libéré. Les12 rondelles seules V2.4 sont retirées du candidat V2.5 : elles ne doivent pas être présentées comme des supports de compression complets.
4. **Deux MKI et deux Click, arrêt local de libération** : modèles/axes V2.4 conservés, recherche limitée aux contacts existants et aux enveloppes. Les disques candidats de rayon1,25mm débordent de0,56..0,61mm les bordures rectangulaires provisoires. Cela ne prouve pas que tout appui de bord est impossible, mais ne permet pas d’inventer une zone nue, un plein contact ni la révision réelle du Click. Aucun changement arbitraire de ces pièces, aucun nouvel achat spécialisé.

**ZIP** : `pack_imprimeur/TR2_V25_KISS_PARTIEL_REVUE.zip`, **21 entrées,14 STL,40 pièces pour deux coffrets**, avec réserves D126/bras ; aucun appui ou étrier non libéré caché. SHA-256 `bd1b6bee3cc7566d5464921eaef156adff31121b734342e0339d995c3cc42479`, 331995octets. Liste exacte dans `QUANTITES.csv` et manifeste interne ; nomenclature non imprimée dans `BOM_QUINCAILLERIE.csv`. Le ZIP réouvert est contrôlé intégralement, CRC et octets de chaque membre. Aucun envoi à l’imprimeur.

## Géométrie corrigée au strict nécessaire

- Bride nappe rapprochée de2,5mm de la cassette : seule sa position d’assemblage change, STL identique. M3×12 conserve au minimum1,4mm d’émergence après l’écrou dans la plage de longueur de conception11,5..12,5mm.
- Base : poche d’écrou au coin(13;141) réouverte, car un rail ajouté ultérieurement en V2.4 l’obstruait partiellement. Deux petites fenêtres ouest pour charger les écrous des bras, et prolongement des deux canaux intérieurs de chargement des écrous de couvercle à droite. **Neuf écrous captifs à charger dans la caisse nue avant les cartes/panneaux.** Pas d’ajout de clip fragile, de filetage imprimé, d’écrou spécialisé ou d’autre pièce.
- Retrait de matière seulement :163,200856mm³ par base ; volume ajouté0, différence avec la correction calculée0. G, plancher, supports Nucleo, contours/cotes du PCB et D126 inchangés. Les petites ouvertures ne donnent aucune IP.
- **13 STL et12 STEP individuels identiques octet pour octet à V2.4**, dont couvercle ARNACO, cassettes et trappes. Seul le STL de base et deux STEP individuels changent (base et position de bride). Relief ARNACO monobloc sans devise repris sans nouveau travail de police/logo ni réexécution inutile de ses tests. Le nouvel assemblage STEP contient18 occurrences de nos pièces imprimables ; le STEP de quincaillerie est notre propre gabarit, pas un modèle fabricant ni des filets exacts.

## Pourquoi les appuis FRAM/SD restent réservés

**Lecture mécanique de la V2.4** : deux écrous suspendus sous le module et un au-dessus assurent une retenue vers le haut, mais ne démontrent pas une butée de compression vers le shield. L’écrou le plus bas est à20,25mm,6,4mm au-dessus du shield ; la tête sous le shield s’en éloigne quand le sous-ensemble descend. En l’absence d’un appui porteur/contre-appui supplémentaire, l’effort descendant peut donc rester sur le connecteur. La validation V2.4 concernait les intersections et maillages, pas ce chemin d’efforts. Aucune qualification physique antérieure n’est effacée ou inventée.

**Faits constructeur** : les quatre perçages Adafruit des fichiers Eagle épinglés sont `MOUNTINGHOLE_2.5_PLATED`, Ø2,5/pastilleØ3,2, sans contactref à un signal, avec restrictions cuivre locales. Leur SHA correspond aux références déjà publiées. Ce ne sont pas des NPTH de modules. Le métal peut donc être étudié sans interdiction générale, mais ces seules données ne démontrent pas les vraies révisions, le cuivre rempli et tous les volumes composants/têtes/rondelles. Le shield conserve ses NPTH2,7. L’appui simple proposé résout le principe de compression ; sa hauteur et ses contacts doivent encore être contrôlés avant libération. Aucun réachat Ajile ni achat de visserie module demandé aujourd’hui.

## Contrôles exécutés et preuves

Environnement CAD existant Python/CadQuery/Shapely/Trimesh/Matplotlib, lanceur `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Depuis le nouveau dossier :

| Contrôle | Observation réelle |
|---|---|
| `sources/revision.py` | code0, génération ciblée et copies identiques,14 types/40 pièces. Aucun ancien livrable écrasé. |
| `sources/controle.py` | code0 après corrections. Base et bride valides/monoblocs ; maillage base étanche, connexe, orientation cohérente, volume positif, écart relatif volume0,003373%. Translation attendue et différence de correction0. |
| Métal coffret |17 fixations testées à trois longueurs11,5/12/12,5, tête gabaritØ6/H3,2, écrous AF5,5/H2,4. Aucun contact imprévu avec pièces globales ni volumes PCB/composants ; distance inférieure conservative minimale2,56mm.1060 boîtes ST C05 reprises de la preuve versionnée, toutes séparées du métal ; pas de booléens ST répétés sans nécessité. Cela exclut un pont par ces fixations dans le modèle, **pas une qualification électrique physique**. |
| Écrous | Poches AF5,9/H2,8 : jeu0,2par face et0,4axial ; chemin continu de chargement en X des neuf écrous dans la caisse nue vérifié par volume balayé, aucune intersection finale. Pas seulement un test à quelques positions. |
| `sources/controle_bras.py` | code0,9 cas de passage métal dans les bras PETG inchangés, aucune intersection ; le métal ne porte pas sur le PCB. |
| Appui conditionnel | Sans hauteur, refus argparse code2 attendu et aucun fichier créé. Géométrie hypothétique H11 vérifiée comme un seul solide, sans export ni libération ; H0 refusé. **Aucune hauteur réelle mesurée.** |
| Validations reprises | Identité des13 STL et12 STEP avec V2.4 prouvée. La base modifiée est un sous-ensemble de l’ancienne, donc n’introduit pas une nouvelle collision C05 ; D126/G/ARNACO et services inchangés réutilisés. Aucun test acquis rejoué pour prétendre un nouveau résultat. |
| `sources/vues.py` | code0, PDF deux pages et PNG inspectés : axes de fixation, coupe bride, principe d’appui H variable. Schémas, pas nouvel essai ni photo réelle. |
| `python3 sources/paquet.py` | code0,21 entrées ZIP comparées intégralement, quantités vérifiées et **683 fichiers historiques du shield identiques**, PCB V2.3 commandé/CAM/V2.4 compris. |

Dimensions usuelles vérifiées sur les sources primaires : [SFS/FERRONORM ISO4762 M3×12](https://www.sfs.ch/CH/de/dl/vlp/P_S010101_0185), tête nominale5,5×3 et clé2,5 ; [Böllhoff DIN934](https://eshop.boellhoff.de/out/media/pdf/DIN_934_Stahl_8_gv___en.pdf), M3 AF≤5,5/H≤2,4. Les plages11,5..12,5 et têteØ6/H3,2 sont nos enveloppes de conception, **pas des tolérances normatives déclarées**. Aucune exigence de grade12.9, offre ou prix France déduite de ces pages. Stocks personnels et prix globaux non inventoriés, pas de commande recommandée par défaut.

Échecs conservés : `rejet-logements-M3.json` (0,127927mm³ au coin gauche bas), `rejet-chargement-ecrous.json` (deux bras gauches et deux poches droites). Les corrections restent locales. Les tableaux `M3-controle.json`, `M3-distances.json`, `reprise-geometrique.json`, `appuis-et-limites.json`, `bras-pivots.json`, `etriers-reserve-localisee.json` et les logs sont sous `preuves/`. Un téléchargement de PDF vis a rencontré403, les outils curl/pdftotext sont absents ; aucune installation ou protection modifiée, dimensions obtenues via les autres sources constructeur accessibles. Aucun ERC/DRC/build/flash/debug, accès microSD/FRAM/COM ou essai physique effectué.

## État et suite

717,42g de PETG dense calculés pour le candidat partiel, densité de scénario1,27g/cm³ héritée de V2.4 ; masse tranchée, temps et énergie non estimés. **Ce chiffre ne constitue pas une économie sur un kit complet** : les appuis modules et étriers restent à ajouter une fois définis. Aucune tenue vibratoire, thermique, IP, précharge ou résistance industrielle qualifiée. Premier article hors tension avant usage ; les réserves physiques V2.3/V2.4 (J5/Zio/G, têtes Ajile, câbles/IDC, fenêtres/USER) demeurent.

Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V25-KISS`, branche `codex/shield-v25-kiss`, HEAD initial `72b47ee78c03b09d4a3d6d9b8f69eeb8258d7c35`, propre au départ. Le worktree V2.4 reste propre à `58d5fdd2…`, le dépôt principal historique propre à `82169af7…` et les deux stashes historiques sont préservés. Pas de reset/clean/merge/rebase ni résolution de divergence. Permissions exercées : développement CAO/documentation/tests et publication autorisée sur `main` ; flash/debug refusés et non utilisés. Mission active archivée sans modification ; le rapport V2.4 avait déjà son archive exacte.

Journal `ETAT_COURANT_TR2.md` actualisé : distinction définitive nylon D126/métal coffret, M3×12 unique, retouches locales, réserve de compression FRAM/SD et étriers. Son contenu antérieur était identique à la référence V2.4 déjà lue, identité Git vérifiée ; il n’a pas été remplacé par une copie historique. Prochaine étape ChatGPT : contrôler le présent lot, garder l’envoi complet en attente, puis lever H et les vrais contacts dans un cadre physique autorisé. Aucune manipulation demandée dans cette mission numérique.

Publication distante : **à effectuer** ; ce statut IN_PROGRESS porte uniquement sur le push/relecture restant, pas sur un obstacle extérieur pour la tranche livrée. Le SHA effectivement évalué sera fixé au commit publié à la clôture documentaire. Les réserves FRAM/SD et étriers sont des arrêts locaux expressément permis, pas une qualification complète masquée.
