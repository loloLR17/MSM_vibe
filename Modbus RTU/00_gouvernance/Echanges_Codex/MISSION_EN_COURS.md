---
mission_id: TR2-20261105-SHIELD-V22-TME-CLIPS
status: READY
base_ref: main
base_sha: 5c29ce62f2026993d24fd53e3ca926131a74d3fb
created_at_utc: 2026-11-05T15:52:11.445Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.2 : optimiser le panier TME et comparer clips sans visserie vs D126

## Objet et décision utilisateur
L'utilisateur demande d'étudier sérieusement la solution à **un seul fournisseur TME** pour la connectique/passifs du shield de prototype et la possibilité d'entretoises imprimées **encliquetables sans visserie**. Il accepte un risque mécanique **circonscrit, explicité, pour un prototype**, et a rejeté le panier dédié de visserie nylon ~25,90 € dominé par les frais de port. Il demande également si des M2×20/écrous M2 nylon sont disponibles auprès de fournisseurs grand public, dont Amazon. **Étude et preuves seulement : aucun achat ni libération des Gerber.**

Préserver V1.9 C05, V2.0, V2.1 D126, la correction des 106 centres XY, la connectivité/pinout/isolement et les campagnes physiques. Lire AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, PROTOCOLE_ECHANGE_V2.md, ETAT_COURANT_TR2.md, la mission et le rapport V2.1 archivés, sa revue indépendante, l'arbitrage utilisateur du 05/11, les sources CAO ST C05, la BOM et la précommande V1.8.

## Travail autorisé à réaliser

1. **Panier TME pour 2 shields montés + lot 5 PCB.** Partir des lignes réelles de `REVUE_PRECOMMANDE/BOM_2_PROTOTYPES.csv`, prix et conditionnements réels ; confronter la disponibilité des variantes TME : J6/J7 femelles 1×9 Connfly `DS1023-1*9S21` (`ZL262-9SG`), C1 Kyocera AVX `SR205C104KAR` 100 nF/50 V/X7R/pas 2,54, et J5 femelle 2×3 Connfly `DS1023-2*3S21` (`ZL262-6DG`). Revoir quantités/MOQ, empreintes, tolérances, hauteur, queues et accessoires. Les pages TME identifiées le 05/11 sont des **pistes à vérifier**, non des commandes ni des substitutions approuvées. Recalculer TTC et port France sur lignes sourcées, stock/délai vérifiés ou expressément non vérifiables ; confronter à la cible historique ~50 € pour deux montages (matériels réemployés exclus), cinq PCB nus, mécaniques et expéditions distingués. Ne jamais additionner HT/USD et TTC/EUR sans conversion sourcée.
2. **Garde prioritaire J5.** Le `SSW-103-01-F-D` Samtec constitue l'acquis V1.9/V2.0 : **ne pas le remplacer au motif que Connfly a aussi un corps de 8,5 mm**. Comparer à partir de documents constructeur contrôlables profondeur réelle d'insertion/zone de contact, longueur des queues, footprint, implantation sous shield, engagement avec les quatre broches réelles CN12 66/67/68/69, stack G~12,25, tolérances, déplacement et courts-circuits. Si la profondeur utile du Connfly n'est pas démontrable, marquer **NO-GO substitution J5**, chiffrer l'alternative TME pour le reste + Samtec exact obtenu à coût livré minimal/stock personnel et garder la référence qualifiée ; aucune substitution silencieuse.
3. **Clips sans visserie : étude mécanique courte et discriminante.** Reprendre 3 ancrages, trous shield Ø2,8 et Nucleo Ø3,2, enveloppes ST C05, marge socket/Zio et garde V2.1, G=12,25±0,10, zones cuivre et voisinage des connecteurs. Comparer (a) D126 imprimés + 3 vis nylon M2×20 + 6 écrous par prototype, (b) supports à double clip plastique non métalliques, imprimables et démontables, (c) éventuel maintien positif simple par boîtier seulement s'il évite une complexité supérieure. Un clip standard fabricant (Essentra, autre) n'est pas automatiquement compatible : trou, épaisseur, hauteur et dégagement doivent correspondre ; ne pas réutiliser abusivement la marge CAO D126. Vérifier débattement des ergots à l'insertion, flexion/fatigue à plusieurs cycles, jeux, épaisseur minimale imprimable selon orientation/couches PETG/PA, retrait sous traction et vibration, isolation, fluage et risque de rupture. Si variante à clips vraiment simple et géométriquement plausible : fournir modèle éditable + STL de **revue non libéré**, plan coté, script de vérification/collisions à coût proportionné ; sinon documenter précisément son NO-GO et **conserver D126**. Aucune longue campagne ou refonte générale.
4. **Vis nylon grand public en repli.** Trouver au moins une offre réellement identifiable avec M2×20, matière isolante, diamètre tête, écrou M2 avec encombrement compatible (têtes nominales Ø3,8, écrous coins Ø4,619 et stack V2.1), quantités pour deux prototypes + réserve (8 vis et 16 écrous), prix TTC port France délai, et distinguer pages marchandes directes vs listings de recherche. Examens initiaux : Ajile vis nylon M2×20 (0,30 € TTC unitaire/minimum 10 pour version ronde, autre tête hexagonale), écrous nylon M2 ; kits Amazon trouvés sans preuve du **M2×20 exact**, ne pas les qualifier comme compatibles. Privilégier commande groupée ou stock existant ; refuser comme inutile une expédition onéreuse de pièces à faible prix.
5. **Décision pragmatique.** Tableau 1 fournisseur / 2 fournisseurs, coût acheminé réellement vérifié ou fourchette honnête et postes inconnus ; verdict J5 et clips `GO conditionnel/NO-GO` fondé sur preuve. Identifier seulement la vérification physique indispensable pour le premier exemplaire, à froid avant alimentation. Pas d'exigence d'impression papier répétitive. Pas d'ordre d'achat.

## Livrables et acceptation
Créer un dossier ciblé `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_2_ECONOMIE/` avec BOM à jour, sources/liens datés et quantités/MOQ, comparatif coûts livrés/transport, verdict technique sur J5, revue concise clips vs D126 et, si justifié, fichiers CAO du clip avec limites de qualification. Ajouter seulement les mises à jour factuelles utiles du journal/roadmap sans toucher aux règles de gouvernance. Contrôler l'absence de changement électrique, KiCad/Gerber/CAM V1.9 et la préservation de V2.1. Rapporter les vérifications effectivement exécutées et les limites ; publier `DERNIER_RAPPORT.md` portant le même `mission_id`, commits/push normaux, puis effectuer la relecture distante et clôturer honnêtement.

`DONE` veut dire **étude publiée**, jamais validation physique, go fabrication ou commande. Si une information constructeur essentielle manque, rendre le blocage **localisé** et proposer l'alternative moins risquée plutôt que démultiplier les investigations.

## Permissions / garde-fous
`target_branch: main`, `allow_commit: true`, `allow_push: true`. Flash et debug STM32 **false** (étude pure CAO/achats) ; aucune intervention matérielle, commande, paiement, transfert Gerber fabricant, impression physique, modification Nucleo/PCB, reset, force push, ni destruction de travaux préexistants. Respecter A-06, l'intégrité de l'isolement RS485 et le partage utilisateur/ChatGPT/Codex. L'utilisateur conserve la décision finale de dépense/fabrication et les gestes physiques.
