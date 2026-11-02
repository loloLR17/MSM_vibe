---
mission_id: TR2-20261102-SHIELD-PROTO-V1
status: READY
base_ref: main
base_sha: 4d45c9fdbc0031cc6df10a0b1bea1515e94b3b3c
created_at_utc: 2026-11-02T20:29:04Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission active — Shield économique de connexion robuste des breakouts TR2

## 1. Objectif et priorité

**CONCEVOIR ET LIVRER**, dans le dépôt Git, les sources KiCad et les fichiers de fabrication d'un **shield d'interconnexion pour NUCLEO-U575ZI-Q**, réutilisant les breakouts du banc MSM TR2 actuel sans fils Dupont, breadboard, connexions fragiles ni broches soumises à traction pendant transport et installation en machine.

Cette mission est **ACTIVE et exécutable** : passer effectivement de l'inventaire à la CAO, au routage, aux vérifications et aux exports fabricables, dans les limites des informations matériellement démontrables. Ne pas se limiter à une étude théorique si la CAO peut être exécutée.

**Priorité coût** : viser une enveloppe **≤ 50 EUR au total pour deux prototypes**, y compris cartes fabriquées en lot, connecteurs, visserie et port, en **réutilisant toutes les cartes et modules existants**. C'est une cible exigeante, non une garantie de prix ; si elle échoue, expliquer le chiffrage vérifiable et proposer l'alternative minimale. Aucune commande ni paiement dans cette mission.

La précédente mission `TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR` a été **clôturée partiellement par décision de ChatGPT**, sans faux `DONE` ni qualification 8 h. Mission, rapport et décision de clôture sont préservés à `Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR/`. Les données CN0/GI0/OBE et leur travail de récupération sont protégés. Cette nouvelle mission ne réalise aucune acquisition, récupération ou manipulation STM32.

**Cahier des charges détaillé déjà publié** : `Modbus RTU/00_gouvernance/PROJET_MISSION_SHIELD_PROTO_V1_20261102.md`. Ses exigences techniques sont le document de référence, mais ses mentions d'ancien statut « NON ACTIF » sont **historiques** : la présente mission dans `MISSION_EN_COURS.md` constitue seule le contrat exécutable. Ne pas déplacer/réécrire arbitrairement les preuves historiques.

## 2. Périmètre à exécuter en autonomie

1. **État des lieux actuel** : partir de la vraie branche `main`, vérifier `AGENTS.md`, la gouvernance et `ETAT_COURANT_TR2.md`, le rapport CN0, le firmware réel, les pinouts des modules et les références constructeurs. Dresser une matrice nette `broche Nucleo ↔ signal ↔ breakout ↔ tension ↔ masse ↔ fonction`. Distinguer pinout actuel et schémas candidats historiques. Confirmer les dimensions et connecteurs réels avant de figer les empreintes.
2. **Électronique existante à raccorder** : IIS3DWB / STEVAL-MKI208V1K (SPI3 sur PC9–PC12, capteur déportable), breakout microSD Adafruit 4682 **en SPI2 actuel** (et non l'ancien SDMMC2 de laboratoire), FRAM SPI1 / CS propre, RS-485 isolé tel que **monté réellement** (ne pas confondre anciennes EVAL et RS485 Isolator Click), et autres modules du prototype **effectivement présents**. Établir une liste `retenu/optionnel/non présent`. Respecter isolations, domaines de masse, alimentation 3,3 V et protection électrique.
3. **Robustesse mécanique** : aucun fil Dupont ni contact de breadboard dans la chaîne de campagne ; breakouts immobilisés (vis/entretoises) ou soudés sur support conçu ; connecteurs détrompés et retenus, supports/reprises d'effort, accès SD, USB/ST-LINK, bouton USER, reset/LED, points de test, encombrement pour un **futur boîtier 3D simple**. Pas de conception complète du boîtier dans cette tranche, seulement interfaces mécaniques et gabarit.
4. **Tête capteur déportée** : connexion courte et fiable, contrainte de SPI3 à 8 MHz à examiner, masse de retour, risque de nappe/câble long et tenue mécanique. Le carter réducteur peint a été relevé à **56,5 °C** ; fixation magnétique/vissée et sécurité antichute restent une **étude séparée**. Ne pas présenter une nappe longue ou un aimant comme qualifiés sans mesures. Prévoir un raccordement adapté à une petite tête fixée rigidement.
5. **Production CAO** : créer un projet **KiCad éditable** comprenant schéma et PCB routé, privilégier deux couches FR4 standard, connectique et composants traversants/soudables à la main, empreintes sourcées, repères de sérigraphie, DRC/ERC, compatibilité d'empilage avec Nucleo réelle, absence de collision d'accès aux éléments. Réutiliser des bibliothèques officielles fiables, contrôler numérotation, orientation et pas des connecteurs. Aucun design PCB final STM32 nu, Ethernet/PoE ni remplacement anticipé d'architecture.
6. **Livraison complète** : schéma, PCB, bibliothèque/empreintes nécessaires, Gerber/Excellon, PDF schéma, vue 3D/STEP si disponible, nomenclature avec références/fournisseurs/prix unitaires et alternatifs, dossier d'assemblage, plan de tests après réception (continuité/isolation/alimentations puis SPI/SD) et budget détaillé pour **deux équipements** avec transport, sans achat. Vérifier indépendamment les exports par rapport au projet et la suffisance de la documentation pour commander ultérieurement.

## 3. Critères d'acceptation

- CAO KiCad complète et contrôlée ; tous les nets du montage retenu identifiés sans conflit ni inversion ; documentation qui distingue les points réellement mesurés/connus des hypothèses ; preuve ERC/DRC et inspection du placement/empilage.
- Un montage de deux exemplaires reproductibles éliminant les contacts précaires et empêchant la traction directe sur les broches. Le coût est chiffré **pour deux TR2 complets en réutilisant les modules existants**.
- Fourniture d'un véritable jeu de fabrication et d'assemblage, pas de simples recommandations. **Si des empreintes/cotes essentielles sont impossibles à vérifier**, produire tout ce qui est sûreté-fiable et classer le reste `BLOCKED fabrication` avec la liste **minimale** de photos/mesures matérielles requises par l'utilisateur ; ne pas inventer un PCB fabricable.
- Préserver la topologie actuelle et les performances SPI/SD sans affaiblissement des contrôles de qualité ; aucune qualification physique revendiquée avant assemblage/essais.
- Tracer commits, livrables, validations effectivement obtenues, coûts, limites/risques et état Git dans `DERNIER_RAPPORT.md`, puis **push normal sur main et relecture distante effective**. ChatGPT contrôlera indépendamment le rapport et les sources.

## 4. Permissions, exclusions et conditions d'arrêt

`target_branch: main`, `allow_commit: true`, `allow_push: true` (autorisations permanentes de travail logiciel et gouvernance V2). **`allow_flash: false`, `allow_debug: false`**, car **aucune intervention STM32 n'est requise pour la conception CAO**. Aucun reset, branchement/débranchement, nouvelle campagne, programmation, lecture de média, remplacement de module ou manipulation physique dans cette mission ; celles-ci restent sous la responsabilité de l'utilisateur.

Protéger toutes les archives CN0/GI0/C29/OBE, FRAM, microSD, données et états des deux cibles. Récupération encore en attente sur TR2-002 : **ne jamais intervenir ni assimiler ce chantier à une récupération terminée**. Aucune modification firmware/API/format de campagne (notamment passage en `int32`) : ce dernier sujet constituera une **mission ultérieure indépendante**. Pas de commandes d'achat, pas de préparation d'écrasement, pas d'évolution PoE/MCU. Ne pas demander d'autorisations répétitives pour les opérations CAO/Git déjà permises ; signaler uniquement un risque électrique non maîtrisé, une donnée physique indispensable inaccessible, une décision structurante ou un achat.

Ne pas transformer des essais logiciels, un ERC/DRC vert ou un export Gerber en qualification réelle de tenue vibratoire, CEM ou température. Les critères de tenue définitive seront vérifiés après réalisation physique. En cas de risque d'isolation RS-485 ou brochage non démontrable, stopper **la portion concernée**, documenter les données manquantes et poursuivre les travaux indépendants possibles.

### Références obligatoires
- `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md`, état courant du dépôt.
- Cahier détaillé `Modbus RTU/00_gouvernance/PROJET_MISSION_SHIELD_PROTO_V1_20261102.md`.
- `CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md`, `CONCEPTION_FIRMWARE_P12H3H_B_MICROSD_SDMMC2_PINOUT.md` **seulement pour contexte historique**, `BRINGUP_RS485_RX_008.md`, et `R0_METIER_20261029_REVUE_ARCHITECTURE_MATERIELLE.md` dans `Modbus RTU/00_gouvernance/`.
- Dossier du code SPI2 actuel et captures matérielles CN0/GI0 ; impératif de distinguer preuves et hypothèses.

Mission exécutée selon la chaîne **ChatGPT → GitHub → Codex → GitHub → ChatGPT** ; Codex publie le rapport, ChatGPT assure sa revue indépendante.
