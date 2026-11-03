---
mission_id: TR2-20261103-SHIELD-V12-PRECOMMANDE
status: READY
base_ref: main
base_sha: ebae78d21f1106ce0a4554f1399012478f8ffb59
created_at_utc: 2026-11-03T15:46:50.745Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V1.2 : finalisation ciblée du shield avant commande de PCB nus

## Décision et référence

L'utilisateur demande de **figer au plus tôt les derniers choix permettant de commander les PCB des deux prototypes**. La V1.1 a livré une CAO routée, ERC/DRC publiés, des plans d'interface et des pièces de retenue candidates, avec des inconnues de mécanique réelle. La mission V1.1 (`TR2-20261102-SHIELD-PROTO-V1-1`) a été **clôturée administrativement mais reste historiquement BLOCKED**, sans faux DONE ; voir son archive `CLOTURE_CHATGPT_PRECOMMANDE.md`.

**Référence normative de cette mission :** `Modbus RTU/00_gouvernance/DECISIONS_PRECOMMANDE_SHIELD_TR2_20261103.md` (écrit par ChatGPT et vérifié sur `main`). L'utilisateur abandonne le **détrompage automatique** des breakouts pour deux exemplaires, **pas les fixations ni la maîtrise d'assemblage**. Les décisions de conception qui y figurent sont prises ; **ne pas demander de validation humaine répétitive** sur les détails mécaniques réglables et les outils logiciels.

**But concret :** produire un dossier KiCad et CAM qui soit suffisamment juste pour **une commande future de circuits nus** (non un prototype électromécanique déjà qualifié), réduire les incertitudes à un contrôle physique minimal clairement documenté. La mission ne commande rien.

## Travaux techniques autorisés et attendus

1. **Retrouver la CAO constructeur Nucleo C05** : exploiter d'abord les archives officielles ST pour MB1549, notamment
   - `https://www.st.com/resource/en/board_manufacturing_specification/mb1549-bdp.zip` (MB1549 Board design project files, version annoncée 4.0)
   - `https://www.st.com/resource/en/board_manufacturing_specification/mb1549-manufacturing.zip` (MB1549 Manufacturing files, version annoncée 3.4).
   ST les publie sur la page de la NUCLEO-U575ZI-Q. Télécharger avec chemin réseau normal (pas de contournement d'accès), relever **contenu réel, révision exacte C05 ou autre, provenance, formats, hashes**, et extraire les cotes/centres mécaniques si elles sont présentes. Ne jamais assimiler C02/C03 à C05 sans preuve. Si inaccessible, le noter ; ne pas inventer de cotes. Comparer au dessin KiCad du shield : CN7/8/9/10, 2 pads CN12-69/70, 3 ancrages, contour, gabarits d'enfichage, orientation face dessous, rayon de la tête de vis et dégagement composants.
2. **Résoudre J5/PG4 avant libération** : le manuel officiel ST UM2861 Rev10 §8.2 précise **CN11/CN12 footprints non soudés d'origine**. Examiner le montage réel prévu (pins J5 sous shield) et documenter/finaliser la contrepartie **femelle deux contacts à ajouter sur les pads CN12-69/70** de chaque Nucleo (ou solution plus simple équivalente sans câbles Dupont), sa position, pas, hauteur, engagement simultané, contraintes et accessibilité de soudure. Confirmer numérotation PG4/PG6 et l'absence de courts-circuits. Si la paire n'est pas matériellement compatible avec la V1.1, ajuster CAO avant Gerber ; ne jamais qualifier virtuellement un connecteur absent.
3. **Vérifier l'essentiel des autres empreintes** : avec références constructeurs et fichiers connus, confirmer J6 FRAM Adafruit 4718, J7 SD 4682, J8 MKIGIBV2 DIL24 (entraxe mesuré 15,2 mm utilisateur, nominal 15,24 ; pin GND bas JP2 global13), J9 Click MIKROE3863 et sa révision mécanique v101/v102. Orientation, soudure et accès au lecteur SD doivent rester reproductibles. Vérifier l'alimentation FRAM par VIN sous 3V3 Nucleo (régulateur/compatibilité logique réelles) et l'isolement GND2/A/B, sans modifier aveuglément l'électronique. Garder SPI2 pour SD, SPI3 capteur et PG4 DE. Les pinouts fonctionnels ne doivent pas changer sans preuve et justification.
4. **Simplifier sans fragiliser** : retirer des exigences de libération les étriers clés asymétriques anti-retournement, qui sont abandonnés pour ces deux prototypes. Conserver des modules démontables et **positivement maintenus**, FRAM/SD par vis/entretoises, MKIGIBV2/Click par étrier simple ajustable ou retenue compatible sans pression sur les pins. Les supports mécaniques peuvent être ajustés après PCB si leurs trous d'ancrage, zones libres et enveloppes PCB sont correctement réservés. Ne pas transformer une pièce provisoire SCAD en validation physique. Fournir un **plan d'assemblage clair** avec orientation, repère broche 1, GND/3V3, contrôles hors tension, insertion sans décalage ni force, remplacement exceptionnel.
5. **Conserver les interfaces retenues V1.1** : microSD J7 fente vers -X et réellement accessible après montage dans un futur boîtier ; borne RS485 externe +X et outils accessibles, sans pont entre domaines isolés ; nappe capteur sortie -Y sans traction connecteur ; accès CN15 5V, ST-LINK, USER, LED et RESET. Un boîtier fini, le poussoir définitif et les découpes millimétriques des fenêtres LED **ne conditionnent pas** la commande de PCB nus lorsque l'encombrement a été réservé. Aucune batterie, aucun 230V sur shield ; bloc 5 V existant vers CN15.
6. **Revoir BOM, nomenclature et coût** : sélectionner une **référence réelle de sockets 12 points disponible** (pas, hauteur, engagement à comparer), J1–J5 mâles et J5 contrepartie Nucleo femelle, entretoises, vis isolantes, résistances/condensateurs de dimensions compatibles et retenues simplifiées. Documenter lot de 5 PCB pour 2 prototypes, distinction PCB nus vs ensemble équipé, prix d'appel vs devis et frais réels ; cible indicative environ 50 EUR / 2 assemblages, non une condition d'achat à négocier avec le fabricant sans l'utilisateur.
7. **Livrer la CAO et le pré-dossier fabricant** : corriger seulement les données effectivement fausses, éviter refonte du PCB. Fournir sources KiCad éditables, schéma, PCB rerouté le cas échéant, zones réservées, gabarit à échelle 1:1 exploitable sur **deux Nucleo C05**, PDFs, Gerber X2/Excellon candidats, BOM, plan de montage, check de pinout et fichier de synthèse de libération. Exécuter et conserver preuves ERC, DRC strict, parité schéma/PCB, comparaison netlist-empreintes/perçages, inspection de la face d'enfichage. Toute réduction de règles/masquage de contrôles est interdite.
8. **Si un relevé physique est encore indispensable**, fournir **la liste minimale et motivée** (une photo dessus de C05 avec règle, ou un gabarit papier à confronter, photo MKIGIBV2 / Click seulement si elle conditionne le PCB, confirmation des 2 pads Morpho non équipés) ; ne pas bloquer les livrables indépendants. Distinguer précisément `READY_FOR_PHYSICAL_TEMPLATE_CHECK`, `CAD_VERIFIED`, et `BLOCKED_FABRICATION`. **Ne lever `NE PAS COMMANDER` que si chaque cote et le raccordement J5 sont démontrés** ; sinon conserver l'interdiction et exposer seulement le dernier obstacle.

## Critères d'acceptation et sortie

- Explication documentée de la correspondance mécanique **C05→J1..J5**, y compris le besoin de soudure à CN12. Citer les sources réelles et distinguer les pièces constructeur des hypothèses.
- Élimination du faux besoin de détrompage sans abandon des retenues positives et reprises d'effort, et procédure simple d'assemblage pour l'opérateur.
- Révision KiCad contrôlée, exports réellement produits et cohérents, répertoire protégé et aucune altération d'archives/anciennes missions. Résultat **propre à la commande des PCB nus**, pas validation d'un produit fini.
- Prioriser la résolution autonome des obstacles documentaires et logiciels avant d'escalader des photos. Le rapport Codex `DERNIER_RAPPORT.md` doit porter exactement `mission_id: TR2-20261103-SHIELD-V12-PRECOMMANDE`, un statut fidèle, preuves, SHA, état Git, coût et reste réel, puis publication/relecture sur `main`.

## Permissions et sûreté

`allow_commit: true`, `allow_push: true`, `target_branch: main`, **`allow_flash: false`, `allow_debug: false`** (CAO/outillage seulement). Aucune intervention STM32/ports, aucune lecture/écriture FRAM/SD, aucun effacement/formatage, aucun achat ni paiement, aucune commande PCB ni impression de pièces. Préserver entièrement CN0/GI0/OBE, worktrees/stashes, spécifications gelées et code firmware. Aucune prétention de qualification vibratoire/électrique réelle sans assemblage et tests autorisés.

Respecter `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md` et le rapport V1.1 archivé. Circuit : **ChatGPT → GitHub → Codex → GitHub → ChatGPT**. L'utilisateur reste responsable des manipulations physiques et de la décision effective de commande.
