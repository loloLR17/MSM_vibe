---
mission_id: TR2-20261104-SHIELD-V18-REVUE-PRECOMMANDE
status: READY
base_ref: main
base_sha: 6c4b6b7de20cdd5d4def23db4891f532a7e65c6d
created_at_utc: 2026-11-04T16:45:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V1.8 / Revue finale ciblée de libération PCB et préparation approvisionnement

## Objectif opérationnel

**Préparer la décision de commande du premier lot de 5 PCB shield** (2 prototypes, 3 réserves), avec un diagnostic indépendant et auditable **GO PCB / GO sous réserves hors PCB / NO-GO motif précis**, sans rouvrir indéfiniment la conception. Il s'agit de la dernière revue **précommande visée**, pas d'une promesse de libération si des conflits réels subsistent.

Lire en premier `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole, état courant, V1.4/V1.5/V1.6/V1.7, revue indépendante V1.7 et arbitrage de simplicité USER `archives/TR2-20261104-SHIELD-V17-VITRAGE-USER/ARBITRAGE_SIMPLICITE_BOUTON_USER.md`. Source unique : **HEAD réel `main`**. Contrôle des sources, des cartes C05 et des règles PCB sur la réalité du dépôt, sans supposer des cotes exactes non fournies.

## Périmètre / décisions déjà prises

- Matériel : **NUCLEO-U575ZI-Q MB1549 C05**, shield deux couches FR4 1,6 mm environ 90 × 100 mm, cinq PCB nus, deux prototypes équipés ; modules existants MKIGIBV2/IIS3DWB sur nappe ST, Adafruit 4682 microSD, Adafruit 4718 FRAM, RS485 Isolator 2 Click. Respecter les empreintes et interfaces déjà figées, sans refonte générale.
- Contrainte géométrique importante : J5 femelle Samtec SSW-103-01-F-D sous shield, couvrant CN12 65–70 ; **4 broches mâles réellement implantées en quinconce**, seule CN12-69/PG4 électriquement connectée, les autres NC. Hauteur broches mesurée **8,22 mm au-dessus Nucleo**, corps isolant **2,40 mm**, G d'empilage réel non encore certifié ; G11 mm **hypothèse et non preuve**.
- Boîtier PETG imprimé ultérieurement, avec **fenêtre rapportée en polycarbonate transparent côté ST-LINK**, et **ports ST-LINK (maintenance firmware/debug), CN15 (5V alimentation), RS485, SD et nappe accessibles/adaptés**. Photo/observation : LED visibles en visée **latérale quasi horizontale** sur TR2-001 hors empilage définitif. Les trois LD1/LD2/LD3 doivent être distinguables à travers une zone commune, sans obstruer ports/fiches. Taille fenêtre V1.7 16 × 8 mm **candidate**, non spécification figée ; modifications **du boîtier seul** peuvent attendre si le shield n'est pas impacté.
- Bouton USER : **simplicité absolue, faible coût et assemblage rapide**. Hauteur au repos **10,2 mm** au-dessus PCB Nucleo ; course au clic **0,4 mm** ; pas de précharge, effort en butée repris par le boîtier. **Préférer une seule petite pièce imprimée avec collerette, ou simple accès protégé au bouton** si mécaniquement possible ; ne pas imposer cage + double guide + ressort de V1.7 sans nécessité démontrée. Conserver RESET protégé. NE PAS modifier le PCB pour faciliter USER sans conflit réel prouvé.
- Alimentation prototypes : **USB-C CN15 5V**, secteur via alimentation externe, pas de nouvelle architecture powerbank.
- Approvisionnement : le **coût TTC livré en France, le stock vérifié, le délai annoncé de réception, les frais de port et la fiabilité** font partie de la décision. Un article très bon marché mais livré dans ~3 mois est défavorisé. Réduire le nombre de boutiques/port, privilégier disponible et livraison raisonnable ; **ne pas inventer les stocks, prix ou délais**, horodater les sources, marquer NON VÉRIFIÉ si accès impossible. Aucune commande autorisée par cette mission.

## Travail autonome attendu de Codex

1. **Audit de libération PCB sans refonte** : inspecter **fichiers KiCad réels**, schéma/connectivité, nomenclature et fabrication candidate. Contrôler/ERC/DRC et fabriquer/revalider les fichiers de contrôle nécessaires si c'est sûr, vérifier netlist/pads J5 et les autres connecteurs, dimensions/encombrements, trous, empilage, composants, épaisseur et options JLC/PCBWay ou équivalents. Ne pas confondre DRC0 avec ajustement mécanique certain. Évaluer tous les **gates C05/J5/Zio/Morpho/MKI/Click** documentés, les cotes acquises et leur provenance ; distinguer conflit démontré, incertitude mesurable et tolérance réglable. Vérifier que les deux cartes C05 ont la même interface pertinente ; ne pas inventer de preuve physique. Comparer artefacts au modèle de référence et ne changer le PCB/empreintes que pour un **défaut démontré**, avec justification, diff minimal, tests, conservation de la version précédente et nouveaux CAM explicitement marqués non libérés tant que revue non achevée.
2. **Interfaces du boîtier en tant que contraintes PCB** : vérifier l'absence d'interférence incontournable des trois LED en visée latérale ST-LINK, du connecteur USB ST-LINK branché, de CN15, de la microSD extractible, du RS485, de la nappe et du couloir USER ; distinguer clairement **ce qui impose une modification PCB avant commande** de ce qui peut être réglé sur les pièces PETG plus tard. Ne pas redemander les mesures USER déjà acquises. Conserver fenêtre latérale PC, bouton minimal. Contrôle de C05 et de la géométrie basé sur des preuves physiques ou constructeur exactes ; si une mesure physique est indispensable, demander **un unique protocole compact regroupé** (gabarit imprimable1:1, points de contact, G naturel non forcé, centre B1 si nécessaire, visibilité LEDs) et expliquer les critères d'acceptation. L'utilisateur réalise seul gestes physiques.
3. **Achats réalistes** : établir BOM quantités **5 PCB nus / 2 jeux composants + marge de remplacement raisonnée**, références fabricant, empreintes, variante de connecteur J5, options de fabrication, sources et fournisseurs ; tableau **prix unitaire, quantité, TTC livré, stock, délai livraison France (fourchette/date), frais de port, devise/date de vérification** ; éventuellement deux scénarios (rapide vs économique) et coût total de **2 shields opérationnels + 5 PCB nus**, sans masquer transports/taxes ni confondre coût PCB et assemblage. Vérifier les alternatives réellement compatibles sur géométrie C05, pas de substitution silencieuse. Pas de fabrication, flash, debug, paiement ni achat.
4. **Un seul document de décision précommande**, court et vérifiable : matrice **GO / GO sous réserves liées au coffret uniquement / NO-GO** par interface essentielle ; recommandations et **points bloquants strictement nécessaires** avant envoi des Gerber au fabricant ; lien explicite vers le jeu CAM candidat, empreintes/projets, devis/sources et preuves. Favoriser l'autonomie : si un blocage ne peut pas être levé sans l'utilisateur, préparer à la place un gabarit et des instructions compactes, sans demander plusieurs retours photographiques indépendants. Finir les éléments logiciels/CAO autonomes dans cette tranche plutôt que les reporter en missions en cascade. La décision formelle de commander reste utilisateur après revue indépendante ChatGPT.
5. **Validation** : vérifier les exports/drills, dimensions, layer stack, BOM, DRC/ERC selon outils disponibles, script de contrôle d'intégrité/pinout, manifestes de CAM et diff du dépôt ; relecture après push sur `main`. Si KiCad/accès fournisseur n'est pas disponible, produire un **NO-GO clairement motivé** ou un statut conditionnel étroit, sans inventer les contrôles. **Ne pas déclarer `READY_TO_ORDER` sans preuve géométrique et électrique suffisante** ; un rapport `DONE` peut signifier **audit terminé avec NO-GO**, s'il remplit les livrables.

## Non-objectifs et conditions d'arrêt

Pas de nouvelle mission expérimentale STM32, pas d'intervention physique ou de flash/debug, ni refonte électronique sans défaut démontré, ni promesse d'étanchéité IP, ni mécanisme USER multi-pièces disproportionné, ni achat ou création de commande. Protéger les données et fichiers historiques ; aucune suppression/overwrite destructif, pas de force push, pas de changement de câblage. Si nécessité d'intervention STM32 imprévue : ne pas l'exécuter dans cette mission, signaler et solliciter un contrat distinct avec autorisations propres. En cas d'incertitude physique non maîtrisée, documenter le seul contrôle bloquant, **ne pas libérer artificiellement les Gerber**.

Livrables publiés sur `main` sous un dossier `REVUE_PRECOMMANDE` versionné près du shield, état technique et preuves de tests, et `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261104-SHIELD-V18-REVUE-PRECOMMANDE`, statuts fidèles, SHA, permis, preuve de relecture distante. Circuit obligatoire ChatGPT → GitHub → Codex → GitHub → ChatGPT.
