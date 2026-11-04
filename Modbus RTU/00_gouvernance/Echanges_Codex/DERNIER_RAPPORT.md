---
mission_id: TR2-20261104-SHIELD-V17-VITRAGE-USER
status: IN_PROGRESS
base_ref: main
base_sha: 4717839a68016bb840598bd37180106e978e2ae3
initial_head: 39c33c69a502d19c6590adffac0ccf847d9f0e34
result_sha: 80ec70effab4a12709c988401e19de8d8294be30
created_at_utc: 2026-11-04T15:46:58Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V1.7 — vitrage latéral et USER : étude conditionnelle, aucune fabrication libérée

**Solution prioritaire proposée : petite vitre fermée dans la paroi verticale côté ST-LINK, sous le shield et à côté du port USB.** La correction utilisateur pendant l’exécution est intégrée : la visée observée est **latérale quasi horizontale**, pas une vue depuis le dessus. La photo est décrite comme montrant plusieurs LED ; je ne prétends pas l’avoir inspectée ni y avoir identifié les trois LD1/LD2/LD3 dans l’empilage équipé. Les conclusions, modèles et checklist d’étude sont prêts ; IN_PROGRESS concerne la publication/relecture distante restante. **La visibilité des trois voyants et l’actionnement USER réels ne sont pas qualifiés. NE PAS COMMANDER.**

## Résultats et réserves décisives

**Vitre candidate16×8mm utiles**, X29..45/Z−11..−3, paroi nordY−20..−17 dans le repère V1.5 (Z0=dessus shield, YCAD=−YKiCad). Plaque PC1,5mm, cadre démontable26×18mm, quatre fixations réservées et joint simple0,8mm/comprimé0,6hypothétique. Cadre à2mm nominaux des inserts ST-LINK et nappe existants ; l’ouverture utile est à10mm enX de la réservation USB. Ces valeurs concernent les enveloppes V1.5, pas des fiches/câbles réels mesurés. Aucune classe IP revendiquée.

**Calcul XYZ nouveau** :3 564rayons pour297points d’un domaine de conception et12observateurs,976passages dans la fenêtre évitant les obstacles majorés de1,5mm, angles0,11..4,26° de l’horizontale. Les rayons passent sous shield/MKI/retenue/nappe dans ce scénario ;1146coupent la réservationSTLINK et d’autres ratent l’ouverture. Aucun pourcentage ne prouve la visibilité de troisLED : centres réels toujoursnull, domaine non métrique, aucun point fictif attribué à LD1/2/3. Composants C05 bas non modélisés, vraies fiches, tolérances et photométrie restent à vérifier depuis une zone d’observation commune. La petite vitre constitue une piste réaliste à vérifier, pas une ligne de vue physique déclarée validée.

La fenêtre nord-est du toit V1.6 est comparée en réutilisant ses résultats, sans répétition des tests inchangés. Sa marge0,246mm n’est pas considérée robuste. **Pas de nécessité de modifier le shield démontrée ici**, mais pas davantage de preuve suffisante pour libérer le PCB : trois trajets réels et couloirUSER doivent être confrontés à l’empilage avant commande. Si le vitrage latéral échoue, conserver le repli électriqueV1.6 viaMorpho sans l’exécuter automatiquement. Aucun guide/fibre compliqué ni nouvelle électronique.

**USER : mesures acquises intégrées**,10,2mm au repos et≈0,4mm au premierclic selon complément humain39c33c69. G11hypothétique donne sommetB1z−2,4 ; jeu cible0,6→pointe z−1,8, distance souscouvercle→pointe38,8. TigeØ4 / deux guidesØ4,4, cage en deux demi-coques/bride captive, butée structurelle à1,6mm de course externe, réglage de pointe proposé±2mm. La tête garde0,4mm au panneau en butée, après correction de sa hauteur lors de la revue géométrique. La charge d’appui supplémentaire du doigt passe à la cage puis au panneau ; versB1, le patin reste compliant.

**La limitation d’effort n’est pas qualifiée.** Les scénarios jeu0,45..0,75/clic0,3..0,5/course1,5..1,7 donnent une compression0,25..0,95 aprèsclic ; siB1 ne bouge pas, jusqu’à1,25mm, réserve ressort proposée≥1,5 avanttalonnage. Il faut sélectionner F0/k pour satisfaire simultanément force de clic et effort admissible, avec frottement et tolérances ; ces efforts restent inconnus. Le schémaST C05 désigneB1=`SW_BLUE` etHW17=`BLUEHAT`, pas un MPN donnant sa courbe mécanique. Aucun k/force sûr inventé. Rappel indépendant, filetage/contre-écrou, fixations de cage et pointe souple restent des détails de réalisation. Modèle rigide de principe sur axeXYhypothétique(34;124), jamais présenté comme position réelle deB1 ; RESET inaccessible. **Aucune nouvelle mesure utilisateur demandée**, la course au clic est déjà publiée ; ne jamais chercher une butée interne en forçant.

## Livrables publiables

Dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/` :

- `docs/ETUDE_VITRAGE_USER_V1_7/NOTE_DECISION.md`, `CHECKLIST.md`, `SOURCES.md` : arbitrage, réserves, contrôles physiques futurs ciblés, provenance et comparaisonV1.6.
- Résultats `optique.json`, `chaine_USER.json`, `interfaces-nord.json`, `CAO-controles.json`, `validation-finale.json`, `conservation.json` : faits calculés distincts des observations rapportées.
- `mechanical/V1_7/parameters.json`, `README.md`, `tools/study_v17.py` : sources éditables. Tous les détails ne sont pas pilotés par JSON : des constantes de construction sont éditables dans le script, limitation documentée.
- `mechanical/V1_7/REVIEW_ONLY/` : **troisSTEP rouverts**, assemblage d’étude18solides, fenêtre4solides, mécanismeUSER6solides ; **PDF4pages** avec vue3D/coupe/chaîneUSER/face nord et rayons enplan, quatreSVG. Le sous-assemblage reprend les contours/perçages exacts et dimensionsV1.5 ; il n’est pas la maquette complète67objets ni la géométrie C05 qualifiée. Les anciens fichiers restent intacts. **Aucun STL de fabrication.**
- Archive V1.7 : contrat copié exactement dans `MISSION.md`, correction de direction dans `OBSERVATIONS_EXECUTION.md`, empreintes des20artefacts dans `ARTEFACTS.json`. Le complément humain de course est inchangé. Rapport final/preuve distante à ajouter après relecture.

## Contrôles effectivement exécutés

| Contrôle | Résultat et portée |
|---|---|
|`tools/study_v17.py`, environnement CadQuery2.8/numpy/matplotlib existant|Succès : calculs nouveaux, exports, intersections tige/guide/panneau nulles au repos et enbutée ; cadre/plaque/joint sans recouvrement volumique. Ni simulation élastique ni qualification de mécanisme.|
|STEP, PDF et SVG|3STEP valides/volumes positifs relus ;4pagesPDF réouvertes/rendues et inspectées,4SVG XML relus.|
|Conservation initiale|**251fichiers antérieurs du dossier shield comparés octet pour octet à39c33c69**, empreintes consignées. Aucun PCB/schéma/Gerber/firmware/V1.5/V1.6 modifié.|
|Sources et liens|SchémaST C05 textuel consulté, UM2861Rev8§6.6 relu localement ; complément0,4 et revueV1.6 lus ; liens locaux vérifiés ; sourcePython parsée et formule paramétrée comparée aux résultats.|
|Git|Diff indexé et périmètre examinés ; `git diff --cached --check` réussi avant commit.|

Les évolutions locales avant validation finale sont tracées : priorité corrigée vers la paroi latérale à la demande utilisateur ; cadre déplacé2mm pour garder le jeu aux inserts ; tête relevée pour ne pas rencontrer le panneau avant butée ; capture haute et cage ouvrable ajoutées pour la captivité/assemblage. Aucune ancienne preuve réécrite et aucun contrôle affaibli.

Validation proportionnée : pas de nouvelle campagne firmware/build/flash, ERC/DRC/CAM ou testsV1.6 sur code inchangé. Les nouveaux calculs et contrôles portent uniquement sur l’étude mécanique. Aucun accès STM32/GDB/COM/FRAM/microSD, achat, impression ni manipulation physique ; aucune protectionWindows modifiée.

## Git, journal et suite

Source évaluée **80ec70effab4a12709c988401e19de8d8294be30 — Study lateral ST-LINK glazing and adjustable captive USER mechanism**. Worktree isolé `MSM_vibe-SHIELD-V17-VITRAGE-USER`, branche locale `codex/shield-v17-vitrage-user`, cible autorisée main. HEADinitial39c33c69 comprend déjà la donnée de course. Dépôt principal propre à82169af7, autresworktrees et deuxstashes6251b988…/f3b829e6… préservés ; aucune résolution automatique de divergence, reset ou nettoyage.

Journal `ETAT_COURANT_TR2.md` actualisé par ajout factuelV1.7 : direction corrigée, nouvelles cotes/acquis, vitrage candidat, USERajustable, résultats et réserves. À transmettre à ChatGPT : vérifier trois identités/trajetsLED et couloirB1 sur l’empilage réel, identifier les effortsB1 puis sélectionner le mécanisme compliant ; finaliser ensuite les seules piècesboîtier nécessaires. Les découpesboîtier peuvent attendre cette compatibilité ; gatesJ5/Zio/modules/LED/USER avantcommande demeurent.

Permissions exercées : documentation, calculs/CAO d’étude, journal et commits. Push normal/relecture distante encore à effectuer ; le statut final sera publié après comparaison complète depuis GitHub. Le SHA final et l’état Git seront fournis après vérification, sans auto-référence dans le commit contenant le rapport.
