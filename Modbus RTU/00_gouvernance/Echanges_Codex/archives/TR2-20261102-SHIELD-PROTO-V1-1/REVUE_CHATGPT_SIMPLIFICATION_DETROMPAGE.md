# Revue ChatGPT — simplification du détrompage sur les deux prototypes TR2

Date : 2026-11-03.
Référence : `loloLR17/MSM_vibe`, `main`; mission `TR2-20261102-SHIELD-PROTO-V1-1`, rapport Codex publié `BLOCKED`, code CAO évalué `d9b5416f000d68eeb5f4545be5aa1b65f80bb986`, bilan de publication `541024417ab2357876095414e0eccba4ed6d0a97`.

## Décision de périmètre, à portée prototype uniquement

L'utilisateur, unique assembleur des **deux prototypes**, propose de **renoncer au détrompage mécanique infaillible systématique** des breakouts J6–J9 : après l'assemblage initial, les modules resteront en place, sauf remplacement exceptionnel. ChatGPT considère cette **simplification proportionnée** et évite d'en faire un critère bloquant pour deux prototypes montés manuellement par leur opérateur, à condition de conserver une procédure claire de vérification avant mise sous tension.

**Le détrompage n'est PAS la retenue mécanique.** Sont **maintenues sans atténuation** les exigences de connexion robuste : pas de Dupont/breadboard, pas d'effort de traction ou de vibration repris par les broches d'extension, maintien positif et démontable de chaque breakout, vis/entretoises isolantes FRAM/SD, retenues J8 MKIGIBV2 et J9 Click par étrier simple ou autre appui dimensionné, trois ancrages Nucleo/shield sans flexion des headers, serre-câbles indépendants pour la nappe ST, RS485, USB CN15. Maintenir l'isolation électrique GND logique / GND2 du Click.

L'absence de clé physique est acceptable seulement sous **maîtrise d'assemblage documentée** :
1. Plan / photo / nomenclature de montage avec vue de dessus, orientation **réelle** de chacun des modules, repères broche 1, 3V3 et GND ainsi que références J6/J7/J8/J9 ; sérigraphie durable et sans ambiguïté ; nommer les risques de retournement/décalage d'un pas et ne pas masquer une orientation inconnue derrière un symbole.
2. Montage et éventuel remplacement **hors tension**, alignement et engagement sans forcer ni plier broches/modules, fixation des retenues avec entretoises adaptées.
3. Avant première mise sous tension : **inspection croisée plan / position réelle**, contrôle de continuité et de l'absence de court-circuit sur le support, correspondance 3V3/GND de chaque breakout et isolement RS485 ; contrôler ensuite l'alimentation à 3,3 V des sockets avec les modules protégés/retirés selon le plan de qualification. Le contrôle peut être réalisé par l'utilisateur suivant la procédure, sans lui imposer une prise de mesure inventée. Ne jamais tester le montage « à l'aveugle » en cas de doute.

**Ne pas rendre obligatoires** des pièces dissymétriques M2,5/M3, obstacles « anti-retournement » ou autres inventions spécifiques **uniquement** destinés à empêcher une erreur humaine d'orientation. Ne pas supprimer une telle pièce si sa forme assure déjà sans surcoût une retenue efficace ; préférer la variante la plus simple et robuste. Une pièce de revue SCAD/STL qui n'est pas adaptée à une enveloppe mesurée ne doit pas être réputée fabricable.

## Revue documentaire du dernier rapport V1.1

Contrôle depuis GitHub `main` du rapport Codex, des fichiers de sûreté `docs/SURETE_CONNEXIONS_V1_1.md`, d'assemblage, des JSON ERC/DRC, du plan de qualification, des modèles et du budget. Le rapport Codex indique ERC/DRC/parité sans violation, interconnexion inchangée (59 nets connectés), microSD J7 déplacée, cinq modèles SCAD exportés en candidat, et retenues J8/J9 bloquées par absence de dimensions physiques. Les JSON vérifiés montrent absence de violations ERC/DRC et `mechanical-verification.json` précise `PASS_NOMINAL_ONLY` / `physical_fit: NOT_VERIFIED`. Ce sont des preuves documentaires/CAO, **pas une requalification sur matériel ni une fabrication libérée**.

## Ce qui reste bloquant indépendamment du détrompage

- **Géométrie réelle C05** : correspondance connecteurs et ancrages J1–J5, hauteur d'empilage/engagement, absence de flexion, accès LED/USER/CN15/ST-LINK.
- **Encombrement et dégagements** : appui non destructif du maintien J8 sur MKIGIBV2, maintien J9 sur la révision réelle du Click, retrait microSD, câble nappe, bornier RS485 et ouvertures futures, sans risque de court-circuit ni pont sur isolation.
- **Fabrication / essais** : approvisionnement de connecteurs de bonne hauteur, gabarit 1:1 contrôlé, inspection mécanique avant commande, DRC/ERC sur révision finale, essais d'alimentation et de capture sur prototype construit ; aucune revendication de tenue à 8 MHz/30 cm de nappe, vibratoire ou thermique avant essais.
- **Coût** : le scénario V1.1 est de 53,27 EUR pour deux, pas un devis. La simplification peut réduire la visserie/les impressions, mais il serait faux de garantir moins de 50 EUR sans nomenclature et devis revus.

**Conclusion de gouvernance** : la mission V1.1 conserve son historique et son rapport Codex `BLOCKED`; le blocage ne doit plus être attribué à l'absence d'un détrompeur automatique **en tant que tel**. La fabrication reste interdite tant que les inconnues géométriques et la retenue mécanique effective ne sont pas validées. L'étude d'alimentation du produit final et le passage en int32 demeurent différés. Cette décision n'autorise ni achat ni intervention STM32 et ne réécrit pas le contrat exécuté a posteriori. Une éventuelle mission suivante pourra simplifier les pièces et plans conformément à cet arbitrage.
