---
mission_id: TR2-20261104-SHIELD-V14-J5-2X2
status: BLOCKED
base_ref: main
base_sha: fa49923a124607bed2c3a4e9b40d86bec98eadd9
initial_head: 5890556a512de816a2251c77e2bc128578ba6750
result_sha: 5d5bc5f8d118eebea9267fc2dfe6bd6d7be0bdbe
created_at_utc: 2026-11-04T10:21:56Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield V1.4 — J5 femelle 2 × 3 autorisée, candidat vérifié

**CAD_VERIFIED (nominal), READY_FOR_PHYSICAL_TEMPLATE_CHECK, BLOCKED_FABRICATION. NE PAS COMMANDER.** Le format 2 × 3 et le déplacement de l’ancrage Click sont intégrés conformément à la décision explicite de l’utilisateur. Seule CN12-69/PG4 rejoint RTU_DE ; les cinq autres contacts sont NC indépendants côté shield. Aucune modification des Nucleo.

Publication et relecture distante effectuées sur `118363431d3eda9cce9943eda82bf3d46c293e7e` : les 107 fichiers modifiés ont été relus intégralement depuis la référence récupérée de GitHub et comparés octet pour octet aux fichiers locaux. Preuve : `archives/TR2-20261104-SHIELD-V14-J5-2X2/REMOTE_READBACK.json`. Le statut BLOCKED concerne exclusivement la libération en fabrication : hauteur naturelle Zio, ajustement sur les deux C05, enveloppes et tolérances non démontrés physiquement. La tranche logicielle autorisée est terminée ; aucune validation physique n’est revendiquée.

## Contradiction résolue et autorisation

ST UM2861 Rev8, tableau 19 p37 et figure 4 p10, reçu et inspecté : CN12 se présente par paires 65/66, 67/68, 69/70. Fonctions respectives : PD10, PG8, PG7, PG5, PG4, PG6. Les positions 66/67/68/69 ne forment pas un carré 2 × 2. L’utilisateur précise deux barrettes en quinconce 67/69 et 66/68 ; les positions 65/70 restent vides.

Une proposition isolée a été préparée avant escalade. L’utilisateur a ensuite approuvé explicitement « Oui, 2 × 3 et ancrage déplacé ; poursuite autonome », en réponse à la proposition femelle couvrant 65..70 et ancrage (94,5 ;112,5). Décision archivée dans `archives/TR2-20261104-SHIELD-V14-J5-2X2/DECISION_UTILISATEUR_2X3.md`. L’identifiant historique 2X2 est conservé ; le contrat et les décisions de pilotage, propriété ChatGPT, ne sont pas réécrits. L’adaptation ne nécessite plus d’approbation de format.

## Modifications réalisées

Dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/`. Nom de projet KiCad historique conservé, cartouche et sérigraphie 1.4. Sources et générateurs, schéma, PCB, matrice, nomenclature, budget, calcul Z, plans et exports actualisés.

- J5 `CN12_65_70_2X3_FEMALE`, empreinte `TR2:J5_CN12_65_70_2X3_FEMALE`, **Samtec SSW-103-01-F-D**, sous le shield, entrée vers Nucleo ; corps B.Fab et marquage miroir. Numéros locaux 1..6 = CN12-65..70. Local 5 seul connecté à DE. Les anciens footprints J5 conservés sont historiques, non instanciés.
- Positions globales nominales en mm, vues depuis le dessus du PCB shield :

| Pad local | CN12 / fonction | X | Y | Côté shield / mâle utilisateur |
|---|---|---:|---:|---|
|1|65 / PD10|85,96|106,28|NC / absent|
|2|66 / PG8|88,50|106,28|NC / présent|
|3|67 / PG7|85,96|108,82|NC / présent|
|4|68 / PG5|88,50|108,82|NC / présent|
|5|69 / PG4|85,96|111,36|RTU_DE / présent|
|6|70 / PG6|88,50|111,36|NC / absent|

- Quatre nouveaux trous J5 aux positions 65..68, perçages J5 1,04 mm conformément au dessin recommandé Samtec ; autres connecteurs 1,02 mm inchangés. Position de DE conservée. **32 trajets géométriques et nets inchangés**, seul libellé d’extrémité J5.1→J5.5. 19 empreintes, 191 pads dont 11 NPTH et 180 numérotés, 59 connectés / 121 NC, quatre vias ; contour 90 × 100 mm.
- Un seul ancrage Click inférieur déplacé de (91 ;108) vers **(94,5 ;112,5)**, après collision avec le nouveau J5. Étrier Click SCAD/STL adapté ; autres ancrages Nucleo, modules et retenues positives conservés. Pas de détrompage automatique réintroduit. Fixations restent nécessaires ; aucune qualification mécanique déduite.
- Exports candidats réels : schéma, implantation dessus et dessous miroir, cuivre dessous miroir, Gerber X2 et Excellon dans `exports/BLOCKED_FABRICATION`, STEP/PNG **carte nue seulement**, gabarit A4 1:1 V1_4 trois pages et CSV de 109 centres, interfaces et retenues SVG/PDF. Les fichiers nommés V1_2/V1_3 restent historiques. Régénération par les outils mise à jour, sans retour à J5 1 × 2.

## Empilage : données et limites

Cotes opérateur acquises : pointe à **8,22 mm**, plastique à **2,40 mm**, métal exposé **5,82 mm** ; approximations au pied à coulisse, incertitude non fournie. Aucun nouveau relevé de ces dimensions ni de l’entraxe MKI 15,2 mm n’est demandé.

Catalogue Samtec SSW traversant réellement reçu : mâle nominal carré .025 pouce (~0,635 mm), hauteur femelle 8,51 mm, queues style 01 de 2,64 mm, plage d’insertion **3,68..6,35 mm**, corps double 4,95 mm de large et 8,13 mm de long pour trois positions. Dessin de perçage double reçu : pas 2,54 mm, trou recommandé 1,04 mm. URLs, tailles et SHA-256 dans `docs/sources-v1-4.json` et `docs/checks/v14/independent-findings.json` ; originaux locaux conservés sous `/home/lolo/dev/msm/reports/TR2-shield-v14-20261104/`.

Avec G = distance entre dessus du PCB Nucleo et dessous du shield, insertion **I = 8,22 − G + 8,51**, jeu entre corps **C = G − 8,51 − 2,40**. À G = 11,00 mm, hypothèse **non mesurée**, I = 5,73 et C = **0,09 mm seulement**. L’intervalle nominal résultant de l’insertion constructeur et de l’absence de collision plastique est **10,91 ≤ G ≤ 13,05 mm**, sans marges ni tolérances. Il ne prouve pas le montage. G réel, section/tolérances des mâles et dégagements corps/queues/entretoises restent à confronter à l’assemblage naturel Zio/J5 ; ne jamais rattraper un écart en serrant ou en fléchissant les cartes.

Les deux ZIP ST MB1549 ont fait l’objet d’une tentative HTTPS bornée à 8 s chacun : timeout ~8,24 s, **aucune archive reçue** (`checks/v14/ST-bounded-retry-v14.json`). Contenu, formats, révision C05 et hash inconnus ; aucune équivalence entre numéros de publication et révision PCB supposée. Le manuel vérifie les noms et paires, pas le fit mécanique C05 ; la géométrie reste un candidat issu de la reconstruction documentaire antérieure.

## Nomenclature et budget

Référence choisie SSW-103-01-F-D : affichage constructeur **1,349 USD/unité**, minimum 1, deux unités nécessaires, stock affiché 4 597 au relevé. Ce sont des affichages, pas une réservation ni un devis livré. Sources référencées dans `docs/SOURCES.md`. Les autres connecteurs standard économiques V1.3 sont conservés. Total connectique pour deux shields : **7,558 USD**, contre 5,66 USD en V1.3 ; surcoût 1,898 USD pour disposer des données d’engagement.

Alternative Connfly DS1023-2*3S21 : minimum 10 × 0,10 USD, total connectique hypothétique 5,86 USD ; non sélectionnée car limites d’insertion non trouvées. Les dimensions Samtec ne sont pas transférées à Connfly. La femelle 2 × 2 est incompatible avec le quinconce.

Budget prudent : **54,30 EUR + 9,558 USD avant fiscalité**, comprenant provisions mécaniques 21,30 EUR, hypothèse de quatre ports 33 EUR et appel PCB 2 USD. Ni ports réels, ni devis livré, ni conversion de devise inventés. **Cible 50 EUR livrée non démontrée.** Aucun achat ni commande.

## Validations effectivement observées

| Contrôle | Résultat / preuve sous docs/checks |
|---|---|
| generate_schematic.py, generate_board.py, apply_routes.py, export_design.py, read_board.py | sorties réelles 0 ; `rebuild-v14.json`, `commands.json` |
| ERC KiCad 10.0.6, severity-all et exit-code-violations | zéro violation ; `ERC.json/log` |
| DRC strict avec schematic-parity, all-track-errors, severity-all et exit-code-violations | zéro violation, zéro non-connecté, zéro écart de parité ; `DRC.json/log` |
| verify_exports.py : netlist/PCB, attributs X2 des deux cuivres, tous Excellon, contour CAM | PASS ; `export-verification.json` ; pas d’audit indépendant de toute la topologie cuivre CAM ni d’impédance |
| verify_preorder.py natif : J5 face B, corps, miroir, local 5 DE et cinq îlots NC, trous, routage | PASS nominal ; `preorder-native.json` |
| verify_mechanical.py : géométrie, ancrages, corridors, cinq maillages | PASS nominal ; `mechanical-verification.json` |
| Étrier Click déplacé, compilation OpenSCAD et maillage | succès, 1 604 triangles, fermé et connexe, volume positif ; `v14/retainer-new-anchor.json`, click.log/STL |
| generate_preorder.py / generate_interfaces.py | succès ; `preorder-template.json`, `v14/interface-pdf-generation.json` ; gabarit et plan dessous inspectés visuellement |
| Comparaison ciblée à 5890556a | PASS ; `v14/scope-audit.json` : autres pads inchangés sauf un NPTH autorisé, 32 géométries conservées |
| Règles projet et hashes | fichier .kicad_pro octet pour octet conservé, SHA-256 8c077c4fc90dc8fe73b778e3fccfeb764cc4dca59667a8a04d85bc7cc1bb61f6 ; 73 artefacts relus selon artifact-hashes.json |
| Diff Git / périmètre | diff --check réussi ; aucune source firmware ni document humain actif modifié |

Erreurs conservées : la première proposition 2 × 3 causait trois erreurs DRC autour du NPTH Click (jeu 0,181 < 0,25 mm et courtyard). L’ancrage approuvé a été déplacé ; textes déplacés/redimensionnés à 0,8 mm pour supprimer les avertissements de sérigraphie, DRC final strict réussi. `DRC-before-retainer-move.json` garde l’échec initial. Le contrôle mécanique a d’abord lu la mauvaise base historique V1, dépourvue de retainer_mount_holes (KeyError) ; il lit désormais la V1.3 pour cette comparaison et passe. Aucun seuil, aucune sévérité ou exclusion abaissés. Les quatre maillages inchangés ont été réutilisés et relus, sans recompilation inutile. Les UUID KiCad régénérés expliquent une part du diff sans changement de géométrie fonctionnelle.

Validation proportionnée : mission purement CAO/documentaire, aucun changement firmware ; tr2_validate/build STM32 et campagnes physiques ne fourniraient pas de preuve utile ici et n’ont pas été exécutés. Compilation OpenSCAD, contrôles CAO et inspection documentaire ne constituent pas une qualification physique.

## Conservation, publication et suite

Dépôt principal historique propre à `82169af7e76797731358852bec83bfb0ceb54ccf` préservé ; aucun reset, merge, rebase ou nettoyage. Worktree isolé `MSM_vibe-SHIELD-V14-J5-2X2`, branche locale `codex/shield-v14-j5-2x2`, cible autorisée main, départ réel `5890556a512de816a2251c77e2bc128578ba6750`. Stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87`, autres worktrees, archives et campagnes préservés. Contrat copié dans la nouvelle archive de mission ; décision opérateur archivée. Aucun accès matériel, flash, debug, FRAM, microSD ou COM ; aucune soudure, impression ou commande.

Commit de sources évaluées : `5d5bc5f8d118eebea9267fc2dfe6bd6d7be0bdbe`, « Shield V1.4: adapt authorized staggered CN12 connection to female 2x3 ». Le journal `ETAT_COURANT_TR2.md` est actualisé : rectification du quinconce, décision 2 × 3/ancrage, validations nominales, cotes acquises, budget et réserves C05. Permissions exercées : modifications, contrôles logiciels et commits ; push normal réalisé vers main puis relecture distante complète à `118363431d3eda9cce9943eda82bf3d46c293e7e`, jamais force push. État du worktree propre après cette publication ; la présente clôture documentaire fait l’objet d’un dernier commit/push normal avec contrôle distant et SHA fourni dans la réponse finale. Le dépôt principal reste propre à son HEAD historique, ses deux stashes inchangés.

L’unique contrôle physique groupé restant avant libération : **gabarit 100 % sur les deux C05, emboîtement naturel Zio/J5 avec mesure de G et vue de profil**, puis dégagement des corps, queues, vis/entretoises et volumes MKIGIBV2/Click. À effectuer dans une intervention ultérieure autorisée ; pas de manipulation demandée dans cette mission. Les relevés 8,22/2,40 et 15,2 ne sont pas à refaire. Enveloppe de boîtier, guides LED, poussoir et détrompage automatique ne constituent pas des gates supplémentaires pour les PCB nus.

Les livrables candidats sont prêts pour revue indépendante ChatGPT. La fabrication reste suspendue à ce contrôle réel ; aucun fichier candidat n’est déclaré bon pour commande.
