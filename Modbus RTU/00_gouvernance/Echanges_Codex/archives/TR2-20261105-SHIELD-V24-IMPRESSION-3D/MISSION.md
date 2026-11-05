---
mission_id: TR2-20261105-SHIELD-V24-IMPRESSION-3D
status: READY
base_ref: main
base_sha: a6887a2a938dfa3d098efbb7458fb43ddea9f67b
created_at_utc: 2026-11-05T18:59:23.183Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.4 : finaliser un seul dossier d'impression 3D, deux prototypes ARNACO

## Décision utilisateur, objectif industriel

Les premiers **cinq PCB nus V2.3 ARNACO** ont été commandés chez JLCPCB par l'utilisateur (confirmation commerciale montrée le 05/11/2026). La commande TME (17,60 EUR TTC) et la visserie Ajile (16,20 EUR TTC suivi, 8 vis nylon `VCY120` M2×20 et 16 écrous `UHX001` M2) sont également passées ; Samtec `SSW-103-01-F-D` demandé en échantillons, réception à confirmer. Ne pas republier de pièces justificatives, identités, numéros de commande ou adresses.

L'utilisateur souhaite **un unique transfert à son ami imprimeur**, en minimisant son travail de préparation, les impressions répétées et le nombre de pièces manuelles. Le dossier doit viser **deux ensembles mécaniques prototypes complets** : fixations, retenues, boîtiers, couvercles et pièces d'accès/services nécessaires. Le boîtier aura **« ARNACO » en relief directement intégré à la face supérieure du couvercle**, **sans devise**, sans incrustation ni deuxième couleur, ni pièce logo séparée ; l'imprimeur ne doit pas avoir à le modéliser. Respecter la démarche A-06 : réutiliser les acquis, ne pas surconcevoir.

## Références obligatoires, état acquis et limites

Lire HEAD réel de `main`, `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, état courant, dernier rapport V2.3 `DONE` et revue indépendante, puis les dossiers :

- `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_3_ARNACO/` : PCB réellement commandé, empreintes et coordonnées de référence, même géométrie électrique V1.9 corrigée d'après CAO ST C05.
- `.../V2_0_EMBOITEMENT/` : G=12,25 mm, tolérances et enveloppes/marges conditionnelles J5/Zio ; ne **pas** reprendre l'ancien scénario G=11 comme cote finale.
- `.../V2_1_FIXATIONS/` : pièce D126 demi-couronne, 12,25 ±0,10 mm, Ø enveloppe 6, alésage 2,4 ±0,2, face ouverte, orientation spécifique aux trois ancrages ; modèle et validation bornée existants. Prévoir **6 utiles +2 de réserve** pour deux cartes ; fixations nylon déjà achetées (6 vis/12 écrous utiles, 8/16 au total).
- `.../mechanical/V1_5/`, `.../mechanical/V1_7/`, `.../mechanical/retainers.scad`, `.../docs/MATURITE_BOITIER_V1_5.md`, `.../docs/INTERFACES_BOITIER_V1_1.md`, `.../mechanical/interfaces.json` : concept enveloppe boîtier **158×166×64 mm**, anciennes pièces d'étude `REVIEW_ONLY`, et interfaces encore incomplètes. Leur présence n'est pas une libération d'impression. 
- `.../V2_2_ECONOMIE/POSTES_RESTANTS.csv` et `VISSERIE_GRAND_PUBLIC.md` : autres fixations M2,5/M3, supports FRAM/microSD, retenues MKI/Click et réserves.

La qualification 3D nominale n'est **pas** un contrôle de pièces réelles : révision Click, volumes du MKI, IDC/nappe, hauteur d'accouplement des modules, connecteurs femelles Connfly1×10 avec corps dix positions et patte supplémentaire neutralisée, jeux de fiches USB, tolérances d'impression, vue des LED et course USER restent en partie inconnus. Les nouveaux vis Ajile ont une tête dont Ø et hauteur ne sont pas prouvés : compatibilité à contrôler à réception. Aucun contact avec le PCB, force sur les connecteurs ou shunt entre GND logique et bus isolé RS485.

## Travail technique autorisé à Codex — à accomplir, pas seulement proposer

1. **Inventaire et reprise minimale.** À partir du PCB V2.3 commandé, choisir le modèle maître mécanique existant le plus abouti, faire une revue ciblée des cotes/référentiels, éliminer les incohérences V1.5/V1.7/V2.0 et reprendre sans perte les volumes et géométries validés. Concevoir dans un **nouveau dossier autonome V2.4** ; préserver tous les originaux historiques, fichiers CAO/PCB/Gerber et logiciels/firmware.
2. **Supports et retenues** : produire/contrôler les STL et STEP paramétriques pour D126, supports isolants FRAM et microSD (2 points d'appui par module, **8 appuis pour deux prototypes**), et étriers vissés du MKIGIBV2 et du RS485 Click (**2 de chaque pour deux prototypes**), avec quantités/réserves raisonnables. Vérifier compatibilité nominale avec les deux sockets 1×10 intacts (position10 sans queue), volumes FRAM/SD/Click/MKI, ouverture microSD, bornes RS485 et isolation. Les cotes encore inconnues doivent être traitées par un réglage simple, une pièce de rechange paramétrique ou une réserve explicite ; **ne pas inventer une hauteur ni déclarer « prêt à imprimer » des appuis non démontrés**.
3. **Boîtier complet pour deux TR2** : finaliser la base, le couvercle, les petits panneaux/trappes et les brides utiles à partir de V1.5, en conservant autant que possible son encombrement. Intégrer directement au couvercle le texte `ARNACO` **en relief monobloc, même filament, sans slogan**, embossage visible et imprimable ; ni décoration complexe ni support manuel spécialisé. Prévoir réellement l'accès de maintenance ST-LINK/CN1, alimentation5V CN15 (230V exclusivement à l'extérieur), extraction latérale de la carte microSD avec dégagement pour les doigts, bornier A/B/GND2 et accès tournevis depuis une trappe, nappe capteur avec serre-nappe/chasse d'effort, visibilité des trois LED, accès USER simple mais protégé et RESET non actionnable par inadvertance. Pas de découpes arbitraires qui gênent les vrais connecteurs ; des **panneaux démontables/ajustables** sont préférables à une nouvelle impression de la caisse entière si une cote périphérique doit évoluer. Ne pas attribuer de certification IP, d'étanchéité ou de tenue industrielle sans essai. Prévoir démoulage/assemblage et démontage sans toucher aux isolations.
4. **Simplicité pour l'imprimeur FDM PETG** : sélectionner orientations d'impression, tolérances et jeux du procédé, parois, top/bottom, remplissage, supports nécessaires et surfaces d'appui, avec recommandations prudentes pour pièces de fixation chargées et D126. **STL directement orientés/nommés** si possible et fichiers STEP ou sources paramétriques conservés séparément. Indiquer taille minimale du plateau requise et, si l'impression d'un grand boîtier dépasse un plateau courant, prévoir une séparation de pièces réellement assemblable plutôt qu'annoncer une impression impossible. Le mot ARNACO ne doit nécessiter **aucun travail** en conception, changement de filament ou post-traitement particulier. Ne pas demander d'imprimer les filetages M2 : vis/écrous nylon achetés.
5. **Vérification technique reproductible** : géométrie cohérente de chaque pièce (solide fermé/maillage étanche, unités mm, orientation, dimensions, absence d'auto-intersection manifeste), non-collisions géométriques dans les sous-ensembles modélisés, cohérence axes/hauteurs/points d'ancrage avec V2.3, dégagements de maintenance ; fournir vues/coupes d'assemblage avec repères, BOM globale pièce imprimée / pièce achetée / pièce réemployée, quantités pour deux kits, estimation de masse/filament et temps/énergie seulement si calculés et fondés, sinon `non estimés`. Ne pas présenter une vérification CAO comme un essai mécanique/vibratoire ou une simulation de résistance prouvée.
6. **Dossier de transmission unique** : préparer une archive ZIP de fabrication 3D pour l'ami imprimeur, claire et autocontrôlée, comprenant uniquement pièces imprimables identifiées (STL, éventuellement 3MF validés), quantités exactes, README concis destiné à l'imprimeur (PETG, réglages raisonnables, plateau, pièces orientées, marquage déjà inclus), schéma ou quelques vues d'assemblage et tableau des pièces non imprimées/manquantes. Fournir hors de l'archive opérateur les sources éditables STEP/CAD, preuves, variantes non qualifiées. Si nécessaire, distinguer `PRÊT_A_IMPRIMER` et `À_VALIDER_SUR_MATÉRIEL` **au niveau des pièces**, et préparer une archive partielle sûre plutôt qu'un « kit complet » faussement qualifié. Il ne doit pas y avoir de STL `DRAFT`, de pièces redondantes ou de modèles ST tiers redistribués par inadvertance.
7. Publier sur `main` le dossier, manifestes et preuves, un rapport `DERNIER_RAPPORT.md` conforme au protocole avec le même `mission_id`, état honnête `DONE`/`BLOCKED`, commits et relecture distante. Codex conduit la conception et les tests en autonomie et n'escalade que les vraies impasses d'information, risques physiques non maîtrisés ou décisions structurantes.

## Critères d'acceptation

- **Un seul dossier pour deux prototypes**, nomenclature complète et ZIP explicite ; inclure les 2 boîtiers et 2 couvercles **ARNACO en relief sans devise** et les modules D126/FRAM/SD/MKI/Click, sauf réserves localisées justifiées.
- Marquage relief vérifié sur le **solide réellement exporté**, sans surcharge pour l'imprimeur ; aucune dépendance à une police externe sur son PC.
- Tous les STL fournis pour impression sont vérifiés en géométrie et correspondent à une liste de quantité ; les fichiers provisoires clairement exclus du pack imprimeur.
- Fonctionnalités d'accès et isolement prioritaires inchangés ; pas de contact dangereux, de forçage de connecteur ni de qualification physique inventée.
- Préserver le PCB réellement commandé V2.3, la référence C05, G/D126 et les cinq réserves mécaniques acceptées ; la validation finale de l'assemblage reste au premier article hors tension.
- Rapport, preuves et publication distante vérifiables pour permettre à ChatGPT une revue indépendante puis la décision de transmettre le kit à l'ami.

## Permissions, garde-fous

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. **Conception/validation numérique uniquement** : aucune impression physique ni envoi au tiers, aucun achat, paiement, modification du STM32, flash/debug, modification cuivre/PCB, retrait d'anciens travaux Git, force push. Limiter les dépenses et formalités ; le budget global proche de 50 EUR est un objectif utilisateur, **pas un total établi** avec le coût d'impression et accessoires encore inconnus. Conserver les validations humaines exclusivement pour le montage physique/risques non maîtrisés.
