---
mission_id: TR2-20261105-SHIELD-V19-C05-OFFICIAL-CAD
status: DONE
base_ref: main
base_sha: 63451b2b4f87a1be0547406d76877b60114bdb20
initial_head: ad4b698f42a7b174e92227a88ce68d8c9c8935b1
result_sha: ca1a2484d735cfe8a4443e54bb80148fb8105152
created_at_utc: 2026-11-05T11:56:37.709851+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V1.9 — coordonnées ST C05 obtenues, CN7 corrigé, fabrication toujours NO-GO

**L’erreur relative démontrée concerne CN7/J1 : correctionY+1,016mm.** CN8/CN9/CN10 sont cohérents avecST dans un même repère. J5 est recaléY+0,025001mm et un centre de fixation de moins de0,005mm. Ce n’est pas une translation globale ni un changement d’échelle ; nous ne prétendons pas attribuer entièrement le constat qualitatif utilisateur de «plusieursmm» à cette seule erreur. Le candidatV1.9, ses gabarits et les preuves sont publiés et relus depuisGitHub. **Fabrication NO-GO : l’ancien papier ne valide pas le nouveau PCB.**

Dossier : [V1_9_C05/README.md](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/README.md). [Décision technique et portes séparées](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/DECISION_V19.md), [provenance](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/PROVENANCE.md), [comparaison106contacts](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/comparaison-C05.csv), [comparaison des trous](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/comparaison-trous-C05.csv).

## Sources exactes réellement reçues

Archives officielles ST téléchargées et ZIP/CRC contrôlés : **BDP4.0**,11 019 805octets, SHA256`ba9ce5359d9b5f81ca61423b11d6432d76e438e28af98259a64ab1eba5447f4c` ; **Manufacturing3.4**,30 596 347octets, SHA256`ecd9d543fa90b384ce741696ededc6d388f2d9154e4ef0bc626bece68c59c31f`. [ST annonce ces ressources](https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html). Inventaires et manifestes des membres conservés, sources complètes horsGit sous `/home/lolo/dev/msm/reports/TR2-shield-v19-20261105/`.

Le projetAltium déclare **Board_PCB_Revision=C / Board_Assembly_Revision=05**, date12July2023 ; manufacturing comprend `MB1549-U575ZIQ-C05.step` et `MB1549-C05_PCAD.NET`. Les dates2020 d’ODB/placement et2021 du PcbDoc sont signalées : pas une identificationC05 par leur seul nom. Le readmeBDP garde un ancien en-tête3.1, son historique annonce4.0/C05 ; celuimanufacturing3.4 corrige le nom de variante duMechanical. Aucune assimilation aveugleC02/C03/C05.

**Extraction métrique nativeCFB/Pads6** en1e-4mil, origineBoard6, ordre de broches et noms de nets natifs ; **240positions** deCN7/8/9/10/11/12 recoupées individuellement àl’ODB++ST (conversionpouces25,4), concordance à la précision des fichiers. PG4 deCN12-69 vérifié directementPads6→Nets6. Le STEP spécifiqueC05 a été réellement ouvert :**1060solides valides**, preuve `STEP-officiel-controle.json`, aucune preuve d’emboîtement déduite. Les originaux ne sont pas republiés massivement : readmeOPLA identifié, endpointlicence intégrale non relu avec succès, limite explicitée sans réattribuer les fichiersST.

Les premiers téléchargementsLinux/Windows expirent, Edge échoueHTTP2 ; miroirSTjaponais inspecté, liensJS renvoyant àST, chemins/download404. Nouveaux accès publics hôte/query réussis, HTTP200ZIP et mêmehashBDP par deux chemins. Ni protectionWindows, ni compte ou politique modifié. Un import natifKiCadAltium expire après correction de sa signature d’appel : aucune sortie partielle utilisée. FormatPads6 recoupé au code officielKiCad10 ; extraction directe et contrôleODB évitent de déduire des coordonnées d’une image.

## Identification et correction minimale

En vue dessus STXdroite/Yhaut, shieldXdroite/Ybas : `Xshield=XST+21,74999888 ; Yshield=126,60113030−YST`, ancreCN8-1/J2-1(32,62;35,16). **Distances relatives avant alignement** : CN7-1/CN8-1, ΔX45,72mm identique, ΔY9,144mmST contre10,16mmshield. La correction est donc relative àCN7, pas un recalage de toute la carte. Face inférieure des connecteurs ne signifie pas miroirXY du gabarit dessus.

|Élément|Correction nominale|
|---|---|
|CN7/J1 broche1|(78,34;25)→(78,34;26,016), soit+1,016mmY|
|CN8/J2,CN9/J3,CN10/J4|Positions conservées ; écart numérique éventuel1nm de conversionKiCad, pas déplacement mécanique|
|J5local1/global65|(85,96;106,28)→(85,96;106,305001),+0,025001Y|
|Premier ancrage|(32,62;21,19)→(32,616;21,192131), déplacement0,004532mm|
|Deux ancrages bas|Inchangés (33,89/79,61;97,39)|

**106centres de contact du PCB final comparés àST : écartmax1nm calculé**, seuil de vérification2nm. C’est une concordance numérique de données, pas une tolérance de fabrication ni preuve physique. Trois centres de fixation recoupés ; trousshield2,8 face trousNucleoST3,2, deux autres trousST non ajoutés au shield.

CAO éditable dans `V1_9_C05/cad/`, ancien candidat conservé. Schéma et projet/règles inchangés octetpour octet, modules/contour90×100/retenuesMKI/Click inchangés. J5femelle2×3 dessous inchangé électriquement : local5=69/PG4/DE, cinqNCindépendants, quatre mâles66/67/68/69 et65/70vides. Six fins de pistesCN7 adaptées en préservant les préfixes ; finDE ajustée25µm pour conserver l’espacement auNC. **25/32routes exactes**, dont toutes celles du capteur. Les sources/CAM/analyses antérieurs restent conservés :**324fichiers historiques identiques**, READMEparent seul complété par lienV1.9, contenu historique conservé.

## Résultats observés et portée

**KiCad10.0.6 : ERC0, DRC0, non-connectés0, parité0**,19empreintes/191pads/4vias. Toutes sévérités des contrôles activés, listes de contrôles ignorés héritées conservées ; aucun réglage assoupli. Commandes effectives `sch erc --severity-all --exit-code-violations`, `pcb drc --schematic-parity --all-track-errors --severity-all --exit-code-violations`, exportPDF/netlist/Gerber/Excellon/STEP/rendu, `read_board.py`, `verify_exports.py`, `verify_v19.py`, `generate_templates.py` ; arguments/sorties dans `docs/checks/commands.json` et logs.

Netlist/PCB/X2cuivre et tous les perçages/contour comparés ; Excellon apparié un-à-un à la résolution1µm, erreurcentremax0,000131mm ; aucun arrondi du modèle natifST. ZIP10fichiers neuf **NON LIBÉRÉ** réouvert et chaque membre comparé, SHA256**ddc0d5c45569dd48a3d06bd6a8ea0604a629c0ebb40b639d449320fa60acdbdd**. Gabarits2pagesA4 avec barres vectoriellesX/Y100mm rouverts, dimensions et longueurs vérifiées, premièrepage rendue et inspectée visuellement. JSON/Python parsés ; STEP du shield nu normalisé sans changement de tokens et rouvert valide.

**Échecs analysés et conservés** : retrait de zonesSWIG non exposé, corrigé par retrait des seuls noeudscuivre du candidat ; première adaptation de pistes remontant7violationsDRC, corrigée par contrainte de couche au raccord des préfixes et déplacement cohérentDE ; première comparaisonExcellon demandant4décimales à un format3décimales, lecteur corrigé et contrôlequantification ajouté. Aucune suppression de test ni modification des contrôles historiques. Validation proportionnée à la CAO ; pas de build/testsfirmware, flash/debug, COM ou média.

## Contrôles physiques restant nécessaires

[Nouveaux gabarits 100%](../../04_Architecture/SHIELD_PROTO_V1/V1_9_C05/exports/GABARITS_C05_V19_100POURCENT.pdf) : deuxpages, uneparC05, **vue dessus sans miroir**, barresX/Y100mm. Contrôle unique hors tension, sans déplacement de fils : cinqinterfaces, troisancrages et contour, sans force. Retour compact, sans relevé de toutes les broches ; si écart, localisation par une photo orthogonale du papier. Aucune nouvelle impression ni vérification réelle effectuée parCodex.

**Z/J5/G naturel reste séparément NON VÉRIFIÉ** : pointe8,22/corps2,40, MKI15,2 etUSER10,2/course0,4 restent acquis ; ne pas remplacer ces mesures par une hauteurCAO. Si témoins exacts déjà disponibles, contrôlerG/engagement et jeux, sinon noter nonmesurable. **USER simple accès protégé/RESET protégé**, vitragePC latéral conservés.

Les originesempreintes C05 sont désormais connues : LD3/LD2/LD1X51,72/54,92/58,12,Y20,92 ; B1(29,5212;120,3262), B2(84,2074;120,3262) hors contourshieldY117. Ces origines ne prouvent ni émission lumineuse ni effort bouton. Les calculsV1.7 fondés sur un domainehypothétique ne qualifient pas ces troispositions. CN1/CN15 sont localisés aux extrémités ; accèsUSB-C/STLINK, extractionSD,RS485,nappe,fixations/modules/isolement et visibilité dans le vraiempilage restent à confronter. Aucun conflit incontournable justifiant une autre découpePCB démontré ; ajustements du coffret après compatibilité, sans réouvrir une conception générale.

**NO-GO fabrication et aucun envoiGerber/achat** jusqu’au nouveau contrôle physique approprié et revue indépendanteChatGPT/décisionhumaine.

## Git, journal et publication

Code évalué **ca1a2484d735cfe8a4443e54bb80148fb8105152 — Correct shield V19 CN7 alignment using official ST C05 CAD**. Worktree `MSM_vibe-SHIELD-V19-C05-OFFICIAL-CAD`, branche locale`codex/shield-v19-c05-official-cad`, ciblemain. DépartHEADdistantad4b698f ; basecontractuelle63451b2b dans l’historique. Dépôtprincipal propre82169af7, deuxstashes6251b988…/f3b829e6… et autresworktrees préservés, pas de reset/merge/rebase/forcepush. Permissions exercées : logicielsCAO, documentation, journal, commits et pushnormaux ; aucune permission matérielle.

Journal `ETAT_COURANT_TR2.md` actualisé : constat papier, sourcesC05 effectives, correctionCN7/J5/ancrage, résultats et distinctionXYnumérique/XYphysique/Z/accès. À transmettre àChatGPT : contrôler indépendamment sourceetrepère, nouvellescomparaisons, gabarits et résultats physiques futurs, puis statuer sur libération. Archives de mission et rapport prévues sans toucher aux anciennes.

**Publication normale réussie sur main a820d1852af54dd16c5cebde3babe0cd1d3595c5**, confirmée par fetch/ls-remote ; **453fichiers relus intégralement depuis origin/main et comparés octetpour octet**, dont les livrables, preuves, historique du shield, trois ajouts humains, contrat et gouvernance. Preuve `REMOTE_READBACK.json` dans l’archiveV1.9. La présente clôture et son archive sont publiées par un dernier commit/push normal puis relues ; SHA final fourni à l’utilisateur après vérification. **DONE concerne analyse/correction/exports publiés, jamais libération de fabrication ni contrôle physique.**

## Mise à jour avant publication — main concurrent préservé

Pendant la préparation du push, origin/main a avancé de ad4b698f vers **7c184bc871c867533785ce3a4e7e77f9776e6182**, par trois compléments humains (rectification des accèsST, observation des espaces et descriptions de photos). Le push n’a pas été tenté. Les trois documents ont été lus, sans les modifier. La source métrique désormais obtenue établit **5,080mm côtéCN8/CN9 contre4,064mm côtéCN7/CN10 entre centres extrêmes**, corroborant l’hypothèse terrain d’environ1mm ; aucune mesure photographique prétendue. Un comptage8+17 dans la description photo est signalé : CN9natif est2×15, la photo n’est pas utilisée comme cote. Source et preuves dans `dissymetrie-recoupee.json`.

AGENTS.md§7 impose l’escalade de divergence ; commits locaux et worktree intégralement préservés, pas de merge/rebase/reset/pushforcé. **Autorisation utilisateur explicite reçue** pour intégration isolée et publication. Les trois commits sont réappliqués dans `MSM_vibe-SHIELD-V19-C05-PUBLICATION` basé sur7c184bc871c867533785ce3a4e7e77f9776e6182, branche `codex/shield-v19-c05-publication`. Le code corrigé correspond àca1a2484d735cfe8a4443e54bb80148fb8105152, identique octetpour octet au code testé7557cce1 ; tout le dossier et les preuves au checkpointac61190a sont identiques (442fichiers). Les trois ajouts humains restent intacts, les worktrees/commits originaux aussi. Preuve `INTEGRATION_AUTORISEE.json`. Aucun nouveau test électrique répété. Intégration et relecture terminées ; statutDONE de livraison logicielle, NO-GO physique inchangé.
