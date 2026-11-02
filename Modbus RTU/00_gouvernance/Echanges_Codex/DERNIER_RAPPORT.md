---
mission_id: TR2-20261102-SHIELD-PROTO-V1-1
status: IN_PROGRESS
base_ref: main
base_sha: a99d9be86e0e20e9fb8a1362315bb360c9ba2002
initial_head: d0433fb5352d857344d892ed86099df6a09a8f25
publication_base: 77d67460b38f1152b97e6e5849db90fe2e140fb4
result_sha: null
created_at_utc: 2026-11-02T23:26:44Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield V1.1-A — candidat CAO, accès et retenues

Révision KiCad réalisée, contrôles et exports obtenus ; publication/relecture distante encore à consigner (`IN_PROGRESS`). **Fabrication toujours BLOCKED_FABRICATION** : les dimensions C05, les contacts/détrompages réels des modules et l’ergonomie assemblée ne sont pas démontrés. Aucun banc sollicité.

## Livrables

Dossier actif [SHIELD_PROTO_V1](../../04_Architecture/SHIELD_PROTO_V1/README.md), révision **1.1** dans le projet et le schéma. Nom de projet conservé pour continuité ; V1 préservée au commit `1d2aa6683e972a3751f69229db4022283f55b3fb`, son rapport `44fa2c68` et sa clôture ChatGPT inchangés. Pas de transformation rétrospective de V1 en DONE.

- **microSD** : J7 déplacé de **3,5 mm vers -X**, à (40,5 ; 106), sans rotation ni changement électrique. Fente vers -X confirmée par l’empreinte Eagle, la vue d’éjection du dessin 4UCON15882 et les modèles Adafruit. Première interprétation erronée d’une photo rectifiée avant modification : le sens V1 était correct. Plan de poche doigts, zone de retrait et vis latérales ; pas de simple promesse d’accès fondée sur le header proche du bord. Positions constructeur de dépassement 0,60 / 1,60 / 5,30 mm distinguées des différences de course ; hauteur et retrait de bouche du module réel restent à vérifier.
- **RS485** : bornier à l’extérieur, passage des fils et accès supérieur au tournevis réservés. Selle fixée au futur boîtier, pas aux pins du Click. Aucun A/B/GND2 sur le shield ; cuivre limité à X110. Géométrie v101 et schéma v102 identifiés séparément, leur concordance réelle **non démontrée**.
- **Capteur et câbles** : sortie de nappe protégée, couloir et gorge de selle proposés, reprise d’effort indépendante du raccord. Conservation du ST 12 voies/1,27 mm ; aucune substitution ou qualification 30 cm/8 MHz. Rayons/épaisseurs réels non inventés.
- **LED/USER/USB** : baie opérateur, fenêtre/guides et poussoir mécanique captif proposés, RESET séparé. Centres précis C05 laissés inconnus dans le JSON ; régions fonctionnelles ne valent pas empreintes de découpe. Accès ST-LINK réservé pour maintenance, sans présence nécessaire en acquisition ; accès CN15 et reprise USB réservés.
- **Retenues** : FRAM/SD à deux vis/entretoises chacun, Nucleo à trois ancrages conservés, étriers J8/J9 démontables à perçages dissymétriques M2,5/M3. **Cinq modèles paramétriques compilés** : étrier MKI, Click, selle plate, selle ronde, poussoir. La dissymétrie de fixation d’un étrier ne prouve pas le détrompage du module : celui du MKI reste bloqué faute de forme réelle relevée. Renversement Click calculé sur l’enveloppe v101 seulement.

[Plan d’interfaces SVG/PDF 1:1](../../04_Architecture/SHIELD_PROTO_V1/exports/interfaces-boitier.pdf), [vues des modèles](../../04_Architecture/SHIELD_PROTO_V1/exports/retenues-isometriques.pdf), sources [SCAD et STL de revue](../../04_Architecture/SHIELD_PROTO_V1/mechanical/retainers.scad). **Pas de boîtier industriel complet ni de pièces libérées pour impression de campagne.** STEP et rendu KiCad du support nu fournis ; aucune simulation complète d’un assemblage réel prétendue.

Le [tableau de sûreté](../../04_Architecture/SHIELD_PROTO_V1/docs/SURETE_CONNEXIONS_V1_1.md) traite **J1 à J9, nappe, RS485, USB CN15 et ST-LINK**, avec retenue, détrompage, effort, démontage et donnée manquante pour chaque connexion. Procédure d’assemblage, plan de qualification, BOM et réserves mis à jour. Bibliothèques locales et générateurs fournis ; [Gerber/Excellon](../../04_Architecture/SHIELD_PROTO_V1/exports/BLOCKED_FABRICATION/NE_PAS_COMMANDER.txt) restent **NE PAS COMMANDER**.

## Avenant V1.1-A effectivement appliqué

`main` a reçu `77d67460` pendant le travail : **rallonge secteur à distance → adaptateur externe 5 V existant → CN15**. Seul le 5 V entre dans TR2 ; aucun 230 V sur le shield ou dans son boîtier. Powerbank et interrupteur USB ne sont plus exigés ; leur étude et l’alimentation du produit final sont différées. Aucun circuit batterie/charge ou nouveau rail ajouté. JP6 et les procédures Nucleo sont conservés ; coupure d’énergie jamais assimilée à STOP propre du writer.

L’avenant est appliqué au JSON, plans, procédure, qualification, budget et journal. Aucun branchement ni essai de source exécuté. Installation secteur suivant exigences du bord, sans conception d’une alimentation secteur dans cette mission.

## Contrôles observés et portée

[Commandes et retours KiCad](../../04_Architecture/SHIELD_PROTO_V1/docs/checks/commands.json), [comparaison des exports](../../04_Architecture/SHIELD_PROTO_V1/docs/checks/export-verification.json), [contrôles mécaniques nominaux](../../04_Architecture/SHIELD_PROTO_V1/docs/checks/mechanical-verification.json).

| Contrôle réellement exécuté | Résultat |
|---|---|
| ERC, toutes sévérités, échec sur violation | code 0, aucune anomalie |
| DRC complet `--schematic-parity --all-track-errors --severity-all --exit-code-violations` | code 0, 0 violation, 0 non-connecté, 0 écart de parité |
| PDF/netlist/Gerber/drill/STEP/rendu KiCad | tous codes 0 |
| Nets et pads réels comparés à V1 | **59 broches connectées identiques**, 117 NC ; seul J7 déplacé ; J1–J5 et ancrages Nucleo inchangés |
| Netlist/PCB/CAM/perçages | PASS : deux cuivres X2, tous centres/diamètres Excellon et contour 90 × 100 identiques |
| OpenSCAD 2021.01 `--hardwarnings` | 5 exports STL réussis sans avertissement |
| Vérification indépendante des maillages | fermés, une composante de surface chacun, volumes signés positifs |
| Vérification mécanique nominale | ancrages SCAD/PCB cohérents, colonnes hors enveloppes nominales ; vis SD hors trajectoire de carte avec tête proposée Ø5 |
| Régression générateur | projet strict et schéma conservés octet par octet après génération |
| Inspection des vues | plan d’interfaces et vues des cinq pièces examinés, sans prétendre contrôle C05 réel |

**Erreur logicielle détectée à la relecture du diff :** le générateur schématique réécrivait le projet en format minimal, entraînant la remise de seuils aux valeurs KiCad par défaut. Correction : le générateur **ne réécrit plus le projet existant**. Projet V1 conservé, puis ERC/DRC/parité et exports relancés avec seuils stricts **0,20 mm isolation minimale, 0,25 mm largeur minimale, 0,60 mm diamètre via minimal**. Les premiers retours sous valeurs par défaut ne sont pas le gate final. Aucun contrôle affaibli pour passer ; assertion des seuils et preuve de préservation ajoutées. Les catégories ignorées par défaut de KiCad restent visibles dans les JSON, sans nouvelle exclusion spécifique.

187 pads dont 11 NPTH, 4 vias, pistes 0,25 mm, deux plans GND ; aucun net 5 V ou GND2. Les comparaisons CAM ne reconstruisent pas indépendamment tout le cuivre ni l’intégrité du signal. Les tests mécaniques contrôlent les **hypothèses nominales**, pas leur exactitude physique ni la résistance/fluage des pièces. `tr2_validate.sh` non exécuté : aucun firmware ni cœur partagé modifié, compilation STM32 sans preuve utile ici. Aucun flash, GDB, reset, COM ou accès média.

## Budget et critères non satisfaits

[Budget pour deux exemplaires](../../04_Architecture/SHIELD_PROTO_V1/docs/BUDGET.md) : **scénario 53,27 €**, soit **+8,16 €** par rapport à V1, retenues, visserie et fenêtre incluses. Réemploi des modules et bloc externe prévu. La cible de 50 € n’est pas tenue dans ce scénario et **aucun coût livré n’est démontré** : postes imprimés, visserie, transport et fiscalité provisionnels ; prix d’appel PCB et sockets disponibles à confirmer. Alternative proportionnée : approvisionnement/transport groupé réduisant le poste port de 17 à 14 € donnerait 49,67 €, hypothèse à deviser, sans supprimer aucune retenue.

Les [réserves minimales](../../04_Architecture/SHIELD_PROTO_V1/docs/ASSEMBLAGE_ET_RESERVES.md) restent :

1. Confronter gabarit/hauteurs à C05 ou obtenir sa CAO ; relever positions LED/USER/USB et engagement des headers pour ancrages, ouverture, poussoir sans flexion ni appui permanent.
2. Relever enveloppe/hauteur du MKIGIBV2, débord du raccord et rives utilisables pour le détrompage réel. **Ne pas redemander** entraxe 15,2 mm et GND bas droit déjà rapportés.
3. Vérifier Click réellement détenu v101/v102, ses rives et bornes ; confirmer bouche/hauteur SD, épaisseur/rayon de nappe et diamètres des câbles avant de figer les retenues et découpes.

Ce sont des obstacles réels à la **libération fabrication**, pas un motif de remplacer les livrables CAO par une simple étude. Les travaux indépendants possibles ont été réalisés. Mesures/photos minimales contextualisées dans la procédure, à obtenir lors d’une intervention ultérieure autorisée ; aucune manipulation demandée maintenant sous ce contrat sans matériel. Ni 8 MHz, tenue vibratoire/CEM/température, ergonomie réelle, powerbank, ni budget ferme qualifiés. Fixation de tête à 56,5 °C et écrêtage restent distincts.

## Git, préservation et journal

Départ `d0433fb5`, worktree isolé `MSM_vibe-SHIELD-PROTO-V1-1`. Avant création de tout commit local de mission, `main` a avancé uniquement par l’avenant `77d67460`. Les modifications non commitées ont été réappliquées dans **un second worktree isolé basé sur ce HEAD**, sans merge/rebase/réécriture de l’historique ; aucun historique divergent à résoudre, premier worktree conservé. [Traçabilité](preuves/TR2-20261102-SHIELD-PROTO-V1-1/integration-main-avenant.json).

Publication depuis `codex/shield-proto-v1-1-publication` vers `main` autorisée. Dépôt principal inchangé à `82169af7`, deux stashes historiques préservés ; aucun nettoyage destructif. Contrat initial et avenant archivés séparément, archives V1 et CN0/GI0/C29/OBE intactes. Firmware, FRAM, cartes SD et deux cibles non sollicités, état matériel non relu sous cette mission. Récupération TR2-002 non déclarée achevée.

Journal actualisé par append factuel : résultats CAO V1.1, réserves, budget et décision secteur V1.1-A ; aucune règle de gouvernance ou spécification normative changée. Revue indépendante ChatGPT attendue. SHA des commits, push et relecture distante à compléter après publication, sans référence circulaire.
