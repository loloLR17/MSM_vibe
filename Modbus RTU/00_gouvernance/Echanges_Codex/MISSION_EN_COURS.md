---
mission_id: TR2-20261105-SHIELD-V20-EMBOITEMENT-3D
status: READY
base_ref: main
base_sha: 0ba0df1653bf8f7cac33f8b30ec0dba06517643a
created_at_utc: 2026-11-05T12:40:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.0 : revue finale ciblée de l'emboîtement mécanique et des tolérances, sans nouvelle impression par défaut

## Objet

La conception du shield V1.9 est recalée numériquement sur la CAO officielle NUCLEO-U575ZI-Q **MB1549 C05** : 106 contacts comparés ST Altium↔ODB++↔KiCad, anomalie CN7 Y+1,016 mm corrigée ; ERC/DRC0 et CAM candidats publiés NON LIBÉRÉS. Après un gabarit V1.8 imprimé correctement et ayant révélé cette anomalie, l'utilisateur souhaite **éviter une seconde impression papier si elle ne produit aucune preuve supplémentaire indispensable**.

**Mission technique ciblée : évaluer la faisabilité d'assemblage physique et les tolérances de l'empilage C05 + shield V1.9 + connecteurs + modules, avec sources métriques vérifiées, et établir précisément quels risques peuvent être levés sur CAO et lesquels exigent encore une preuve physique.** Ne pas relancer les anciennes études électriques/firmware ou de conception générale.

Lire AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, PROTOCOLE_ECHANGE_V2.md, état courant, missions/rapports V1.4–V1.9 et revue indépendante V1.9. Référence : HEAD réel de main. Sources autorisées : dossier V1_9_C05, fichiers ST BDP4.0/Manufacturing3.4 déjà téléchargés sous `/home/lolo/dev/msm/reports/TR2-shield-v19-20261105/` après vérification de leurs deux SHA256 documentés dans PROVENANCE.md, STEP `MB1549-U575ZIQ-C05.step`, modèles et dimensions catalogue vérifiés des connecteurs et modules.

## Contrôles prioritaires

1. **Assembler virtuellement en 3D** la Nucleo C05 STEP constructeur (avec véritable révision identifiée), le shield KiCad V1.9 corrigé, les connecteurs J1–J4 Zio, J5 Samtec SSW-103-01-F-D femelle **2×3 sous shield**, les quatre mâles existants CN12 66/67/68/69 en quinconce, les entretoises, modules MKIGIBV2/Adafruit FRAM/microSD/Click et retenues selon précision réellement disponible. Repères, face dessous/dessus, épaisseur PCB1,6mm et appuis à justifier ; ne pas inventer le port de la contrepartie ni des cotes de STEP manquantes. Contrôler signaux et séparations si une inversion/orientation pourrait remettre en cause la correction XY.
2. **Analyse Z et tolérances dimensionnelles au pire cas** : détailler la plage admissible de `G = dessus PCB Nucleo → dessous shield`, hauteur de plastique et queues des Zio, J5 corps8,51mm/profondeur engagement admissible3,68..6,35mm, mâles CN12 pointe8,22mm depuis carte et corps2,40mm, jeu réel sous le shield. Contrôler également passage entretoises isolantes diamètre≤4mm et têtes de vis≤4,5mm, dégagement près Zio, filetages et contacts soudés. G=11mm seulement hypothèse, jeu nominal J5 0,09mm et voisinage entretoise 0,04mm : **ne jamais qualifier comme robuste une marge quasi nulle**, intégrer tolérances d'implantation/PCB/usinage/contreparties. Trouver si un intervalle **non vide et robuste** de G et de dimensions de connecteurs/entretoises existe, en formulant les inégalités et sources vérifiables. Proposer les ajustements minimaux **sans modifier la Nucleo** si un écart prouvé existe.
3. **Collisions et usages influant sur le PCB** : fixations et chevauchement des modules, isolation du RS485 Click, nappe ST, port ST-LINK et USB-C CN15 avec fiches, extraction microSD et accès RS485, fenêtre latérale PC à visibilité réelle des trois LD1/LD2/LD3, accès direct protégé au USER (mesures10,2mm au repos et0,4mm au clic). Conclure ce qui nécessite **un changement de PCB avant commande** vs simples ajustements du boîtier imprimé plus tard. Favoriser bouton simple, pas de cage à ressort. Différencier modèle nominal/volumes provisoires et preuve physique.
4. **Évaluer une dispense argumentée du deuxième gabarit** : ne pas redemander d'impression comme automatisme pour les XY puisque 106 contacts et trois ancrages sont recoupés à ST ; expliciter si les deux cartes C05 physiques devraient encore être confrontées et par quelle méthode alternative efficace. **Une nouvelle impression n'est demandée que si elle apporte une preuve indépendante indispensable non obtenable par constructeur / vérification ciblée**, et précisément laquelle.
5. **Livrer une décision opérationnelle courte** : matrice `GO numérique XY / GO ou NO-GO Z / réserves coffret / décision de fabrication` avec résultats et incertitudes chiffrées, sources/hashes et calculs reproductibles ; éventuelles vues/coupes/STEP d'assemblage EDITABLES dans un dossier versionné V2_0_EMBOITEMENT, rapport de collisions et tolérances ; si une donnée constructeur ou physique manque, **une seule vérification humaine ciblée**, justifiée et non dangereuse, plutôt que de nombreuses mesures dispersées. Le cas échéant, proposer un **protocole de validation par premier exemplaire / première commande** en distinguant clairement décision de risque utilisateur et qualification réelle ; ne jamais annoncer que le montage a été testé si ce n'est pas le cas.
6. Préserver les sources et CAM V1.4–V1.9. **Ne pas retoucher PCB/routage/empreintes ou refaire les Gerber sans défaut démontré**, pas de refonte. Faire les contrôles logiciels pertinents réellement exécutables, conserver preuves/écarts, publier sur main et relire indépendamment le résultat distant ; `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261105-SHIELD-V20-EMBOITEMENT-3D`, statut fidèle, commits et portes non levées.

## Permissions / limites

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false` : **aucun accès STM32/flash/debug**, aucune intervention physique (réservée utilisateur), achat/impression/commande, pas d'effacement ni force push. Suivre A-06 : préserver acquis, ne pas réexécuter des tests inchangés, ne pas créer de formalités disproportionnées. Ne pas bloquer l'étude entière sur une donnée physique inaccessible ; publier honnêtement `BLOCKED` pour le seul gate qui ne peut être levé. Les Gerber demeurent **NON LIBÉRÉS**, sauf décision ultérieure expresse après revue indépendante de ChatGPT et décision de l'utilisateur.
