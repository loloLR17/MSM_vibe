---
mission_id: TR2-20261105-SHIELD-V25-KISS-FIXATIONS
status: READY
base_ref: main
base_sha: f59de5d9a279e62ab04a640729cf56ede87e1607
created_at_utc: 2026-11-05T20:20:44.365Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.5 : KISS, simplification ciblée de la visserie et du dossier d'impression

## Décision utilisateur et objectif

**KISS — Keep It Simple, Stupid.** L'utilisateur corrige notre cadrage : les **8 vis nylon Ajile VCY120 M2×20 et 16 écrous UHX001 déjà commandés sont réservés à l'empilage Nucleo/shield sur les six D126, deux vis et quatre écrous restant de réserve**. Ils n'ont jamais été destinés à la caisse. Pour **la caisse, les couvercles, les caches, les brides et les fixations structurales hors circuits**, une **visserie métallique M3 standard et bon marché (stock ou assortiment grand public/Amazon)** est autorisée ; aucune raison de chercher exclusivement du nylon ni d'imposer des cotes inhabituelles de tête si la géométrie peut recevoir du standard. Les fenêtres seront découpées dans de la récupération ; les petits colliers Colson nylon <=2,5mm sont des consommables courants. **Aucun nouvel achat de visserie nylon spécialisée sans nécessité électrique/mécanique démontrée.**

V2.4 a introduit **8 autres vis M2×20 +24 écrous M2** pour 8 points de maintien FRAM/microSD (2 par module, 4 modules sur 2 shields), via trois écrous par vis, ainsi que **32 M3×16, 8 M3×12, 40 écrous M3** proposés pour tout le boîtier. Ces références/quantités ne sont **pas** des prescriptions d'achat figées ; les simplifier en priorité. La V2.4 est une étude numérique complète mais son ZIP reste **partiel** : 2 étriers MKI et 2 Click exclus faute de zones d'appui réelles démontrées. L'utilisateur souhaite un seul dossier à transmettre à son ami imprimeur, sans reprise manuelle du logo ARNACO.

## Références / invariants

Lire la gouvernance actuelle sur `main` (`AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole, rapport V2.4 `DONE`, sa revue indépendante), le dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_4_IMPRESSION_3D/` et les références V2.3 PCB JLCPCB, V2.0 G12,25 et V2.1 D126. **Conserver** le shield réellement commandé, les droits, la géométrie critique D126, les appuis sans effort sur connecteurs et l'isolement du RS485. Ne pas modifier les Gerber, le firmware, le protocole, ni demander de nouvelles commandes par principe.

## Exécution attendue de Codex

1. **Séparer explicitement les fonctions de fixation** dans une nomenclature réaliste pour deux prototypes : (a) D126 Nucleo/shield = six vis nylon M2×20 et12 écrous M2 déjà achetés +réserve ; (b) supports FRAM/microSD ; (c) boîtier, couvercle, trappes, brides, retenues structurales ; (d) éventuels étriers MKI/Click distincts. Éviter de confondre maintien de carte et fermeture du coffret.
2. **Simplifier le montage des FRAM/microSD** : privilégier d'abord des petits appuis imprimés et une fixation positive simple / visserie ordinaire disponible plutôt qu'8 M2×20 et24 écrous nylon spécialisés. Les cartes doivent être maintenues même sous vibration, sans tirer sur les connecteurs. Des vis **métalliques ordinaires** peuvent être employées là où l'isolation entre la tête, la vis, les pads et les domaines électriques est démontrée, avec support/rondelle isolants si pertinent. Ne pas poser de métal en contact non maîtrisé avec pistes, pads ou parties isolées ; si une solution plus simple n'est pas qualifiable numériquement, garder la réserve localisée sans inventer une garantie.
3. **Utiliser des M3 métalliques standard pour le coffret**. Les emplacements de têtes/écrous doivent accepter un modèle industriel courant (par exemple tête cylindrique DIN912/ISO4762, écrou M3 standard) avec marge raisonnable d'impression. Réduire les fixations non nécessaires, sans compliquer le boîtier par des clips fragiles ou des filetages M2 imprimés. Séparer vis qui retiennent des cartes de vis purement structurales. **Sur le Click RS485 galvanique, préserver physiquement l'isolement : aucune pièce métallique créant un chemin entre domaine logique et bus isolé** ; n'interdire que les fixations qui portent un risque démontré, pas toute la visserie métallique.
4. **Les quatre étriers manquants** : reprendre leurs modèles existants et rechercher une fixation réglable sur zones nues et bordures validables. Si les vrais appuis restent inconnus, laisser le statut non libéré, sans produire un faux ZIP « complet » ; ne pas relancer une étude générale des modules. Pour rester économique, les cassettes et couvercles V2.4 acquis doivent être préservés si non affectés.
5. Mettre à jour **uniquement** la géométrie des fixations réellement nécessaire, la BOM et le ZIP imprimable dans un **nouveau dossier de révision ciblée**, ou dans un sous-dossier clairement versionné sans écraser les livrables antérieurs. Refaire les contrôles CAO/maillages/contacts adaptés aux pièces modifiées, vérifier taille des logements d'écrou et qu'aucune vis ne traverse la zone de cuivre. Quantités exactes, liste de quincaillerie **standard** courte (sans prix inventés), instructions simples pour l'imprimeur, différences V2.4 documentées.
6. Commit/push normaux sur `main`, `DERNIER_RAPPORT.md` du présent identifiant avec statut exact et preuves, relecture distante. Arrêter sans achat ni manipulation physique si une zone de contact électrique ne peut être jugée sûre numériquement.

## Critères d'acceptation et garde-fous

**Zéro réachat des vis nylon Ajile déjà commandées** ; conserver leur rôle exclusif D126. Pour le boîtier, métal standard admissible à condition de ne pas toucher/relier des circuits ; pas de contrainte « nylon partout ». Les vis FRAM/SD restent un problème de maintien mécanique à résoudre au plus simple ; une fixation doit rester sûre, jamais un simple effort de ressort des connecteurs sans preuve. ARNACO doit rester intégré en relief monobloc, sans devise, et ne pas imposer de travail à l'ami imprimeur. Les quatre étriers non qualifiés ne doivent pas être cachés dans un ZIP prétendu complet. Privilégier une petite correction de géométrie et une liste de courses courte plutôt qu'une refonte.

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Conception/tests numériques uniquement, pas d'achat, d'impression, d'envoi à l'imprimeur ni de mise sous tension. Le premier article sera contrôlé hors tension avant tout usage.
