---
mission_id: TR2-20261012-M015-O-ST-OFFICIAL-DEMO-PORT
status: READY
base_ref: main
base_sha: f14183c81c9ff744ebb1d2c6d5f61537e9c1ddb0
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-O — Exécuter fidèlement une démonstration FIFO IIS3DWB officielle ST sur notre NUCLEO

## Décision utilisateur et rectification méthodologique
L'utilisateur rappelle que sa demande depuis le début est de **prendre un exemple constructeur ST qui fonctionne et de l'exécuter sur notre NUCLEO**, pas seulement de reprendre le pilote ST et de reconstruire une séquence inspirée de l'exemple. Priorité à un défaut d'initialisation/configuration, sans présumer que le capteur est défectueux. **Ne pas emprunter le capteur du TR2 principal** à ce stade. Cette mission doit rendre vérifiable l'origine de l'exemple, sa plateforme officiellement supportée, les changements strictement nécessaires au portage et l'équivalence fonctionnelle réelle. Ne jamais qualifier de « démo ST identique » une adaptation modifiant la logique capteur.

Lire AGENTS.md courant, vérifier main HEAD et état Git/worktrees/stashes, relire M015-K à N et leurs preuves, archiver mission/rapport M015-N. Respecter gouvernance, preuves, autonomie, commits/push normaux sur main. Périmètre isolé experiments/iis3dwb_st_reference et documentation/tests/preuves associés ; firmware TR2 permanent M014-H, FRAM, SD, Modbus et spécifications gelées hors périmètre.

## Phase 1 — Trouver un exemple constructeur réellement traçable
1. Rechercher prioritairement dans **les dépôts officiels STMicroelectronics**, packages ST, exemples de carte d'évaluation IIS3DWB/STEVAL-MKI208V1/STEVAL-MKIGIBV2, X-CUBE-MEMS1, STMems_Standard_C_drivers, notes d'application, et projets officiellement pris en charge. Identifier **un exemple FIFO XYZ** complet, sa version/commit/hash, licence, README, carte cible, bus, prérequis et source exacte ; distinguer exemple officiel ST, code dérivé et preuve de fonctionnement physique sur plateforme supportée. Ne pas affirmer qu'un exemple a été exécuté matériellement par ST si seule sa publication est attestée.
2. Examiner datasheet IIS3DWB et notes d'application officielles pertinentes, errata et révisions. Reconstituer la séquence complète reset/boot/BDU/ODR/FS/batching/mode FIFO/timestamp et les contraintes temporelles. Auditer notamment le sens des champs FIFO_CTRL3/BDR, CTRL1_XL, FIFO_CTRL4, les tags timestamp vs XYZ, conditions de batching et la possibilité d'une erreur d'API, d'enum ou d'ordre. Comparer **ligne à ligne** l'exemple ST original avec les adaptations M015-K..N et les valeurs réellement relues. Distinguer différences de version et bugs de getters de la cause de l'absence XYZ.
3. Si aucun exemple officiel FIFO XYZ **vérifiable** n'est trouvé, documenter honnêtement l'absence, les sources examinées et demander une décision structurante seulement si nécessaire ; ne pas inventer un « exemple qui fonctionne ». Si un exemple officiel n'a pas de preuve de test matériel publiée, dire explicitement « exemple officiel publié, fonctionnement matériel non indépendamment prouvé ».

## Phase 2 — Portage minimal et tests hors cible
4. Créer un **portage séparé et traçable** de l'exemple retenu : conserver une copie de référence inchangée avec SHA, patch/diff lisible, et isoler dans un fichier plateforme uniquement horloges, SPI, CS, délais, logs et déclenchement STM32U575. Éviter de réutiliser par commodité la machine à états FIFO actuelle si elle change le comportement ; ne pas transposer les profils STATUS10/word7 comme s'ils étaient la référence officielle. En cas d'incompatibilité avérée d'un getter, conserver le code officiel comme témoin et fournir un correctif local séparé, justifié et testé.
5. Construire un tableau d'équivalence **source officielle → portage → transactions et readbacks attendus**. Tester mocks, cross-build, capacité traces et cleanup sûr, réception SPI O2 démontrée en M015-M. Les variantes diagnostiques éventuelles doivent être distinctes et ne pas masquer les défauts de la référence. Publier un checkpoint logiciel et preuves de provenance avant tout essai physique.

## Phase 3 — Essai physique constructeur sur banc isolé
6. Banc secondaire uniquement : NUCLEO-U575ZI-Q STLINK SN `0032001F3235510C37333439`, UID `001B00463842501820333157`. Refuser le principal SN `003500463235510B37333439` et UID `004A003D3842501820333157`. Vérifier identité, image, SHA, cible avant chaque flash ; NORMAL SWD adresse 0x08000000, verify/readback, debug borné. **Aucune sauvegarde Flash2MiB ni restauration initiale sur le banc secondaire jetable** ; aucun mass erase/Option Bytes/RDP. Gestes physiques à l'utilisateur ; ne pas toucher au TR2 principal.
7. Exécuter **une campagne officielle de référence**, avec configuration capteur strictement équivalente à l'exemple constructeur, capture des registres écrits et relus, WHO, niveaux FIFO, payloads bruts, tags, XYZ, horodatages, erreurs et cleanup. Démontrer que le code réellement exécuté provient de l'exemple choisi et lister toutes les adaptations. Si la référence échoue, préserver l'échec et expliquer précisément ce qui reste à départager (portage, hardware, exemple, révision), sans requalifier une variante en réussite constructeur.
8. Si la référence ne produit pas de XYZ, autoriser au plus **deux variantes diagnostiques indépendantes**, uniquement après audit des faits et avec hypothèse discriminante écrite, un seul facteur à la fois et preuves séparées ; ne pas improviser des dizaines d'essais. Si une anomalie nouvelle ou un risque matériel apparaît, arrêter la campagne concernée, sécuriser le capteur et publier un BLOCKED motivé.
9. Si FIFO XYZ est démontré, réaliser une comparaison contrôlée avec M015-N (transactions, registres, ordre, tags) et isoler la différence causale ; confirmer avec une fenêtre bornée répétable, puis 512 XYZ si sûr et dimensionné. **Ne pas poursuivre jusqu'à 4096 par inertie** : priorité à comprendre pourquoi l'exemple fonctionne. Aucune qualification V1/FFT/production sans campagne dédiée.

## Critères de clôture et restitution
- Traçabilité officielle, source/version/hash, cible d'origine, preuve ou limite du caractère « fonctionnel », diff minimal, séquence et registres attendus, tests host/cross-build.
- Preuve physique FIFO XYZ issue du portage officiel, avec raw/tags/erreurs/readbacks et cleanup ; sinon BLOCKED, cause/hypothèses hiérarchisées et essai discriminant suivant.
- Publier analyse comparative intelligible pour l'utilisateur, code et tests, preuves brutes, manifestes, roadmap/journal et DERNIER_RAPPORT.md ; commit/push main puis **relecture distante** des livrables.
- Ne pas réclamer de validations répétitives ; demander seulement geste physique ou décision réellement structurante. Les autorisations permanentes sont explicites ici : allow_commit/push/flash/debug true, avec garde-fous ci-dessus.
