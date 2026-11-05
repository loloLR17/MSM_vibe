---
mission_id: TR2-20261105-SHIELD-V23-ARNACO-SILKSCREEN
status: READY
base_ref: main
base_sha: b26c2ac0067112b87015abde9cb196ec8572cf45
created_at_utc: 2026-11-05T17:58:14.524Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.3 : sérigraphie ARNACO © et dossier PCB prototype

## Objectif

Livrer une variante propre du shield V1.9 pour les **cinq PCB nus prototypes** destinés à JLCPCB, en ajoutant uniquement la personnalisation demandée par l'utilisateur :

- Mot-symbole : **ARNACO** suivi d'un **véritable symbole « © » en exposant**, de taille réduite et visuellement surélevé près du coin supérieur droit du mot. Ne pas imprimer `(C)` en substitution ; utiliser au besoin un petit dessin vectoriel conforme à la forme © si le moteur de texte KiCad ne rend pas le glyphe.
- Devise exacte, accents compris : **« Votre crédulité est notre force »**.

Choisir un emplacement sobre, lisible et idéalement sur `F.SilkS`, sans couvrir les composants, pads, vias, percements, points de test, repères J1–J9, marquages de sécurité/isolement, accès microSD ou autres informations fonctionnelles. Faire réellement vérifier le rendu sérigraphie après export.

## État acquis et achats

Référence obligatoire : HEAD réel de `main` et dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V1_9_C05/`, **jamais** les anciens fichiers V1.4/V1.8. Lire `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, rapport `DONE` et revue indépendante V2.2, documents V1.9/V2.0/V2.1 et précommande.

L'utilisateur a **accepté les réserves mécaniques résiduelles d'une première fabrication expérimentale**, sans prétendre à une qualification industrielle. Les cinq PCB doivent encore être contrôlés comme exports cohérents avant leur éventuelle libération pour fabricant et paiement, qui restent des décisions humaines.

La confirmation commerciale TME du 05/11/2026 fournie par l'utilisateur atteste une commande **17,60 EUR TTC transport compris**, avec ces références/quantités confirmées en stock : 10× Connfly `DS1023-1*12S21`, 10× `DS1023-1*10S21`, 10× `DS1023-1*8S21`, 3× barrettes mâles `DS1021-2*40SF11`, 10× `MRS16000C1002FCT00`, 3× `SR205C104KAR`, 3× `EEUFR1H100`. **Ne pas réacheter ces lignes.** Aucune preuve d'expédition/réception. Ne pas publier le justificatif, adresse, identité, coordonnées, numéro de commande ni aucune information personnelle dans Git. Samtec `SSW-103-01-F-D` : échantillons demandés, réception non vérifiée.

Le remplacement prototype J6/J7 prévu par Connfly 1×10 conserve le corps à dix positions et neutralise la patte supplémentaire ; les contrôles 2D préliminaires sont favorables sous réserves du contrôle mécanique à froid. Ni adaptation physique ni validation en vibrations ne sont couvertes ici.

## Travail de Codex

1. Inspecter les sources natives KiCad V1.9, les empreintes et leurs sérigraphies, le contour et les zones libres ; identifier l'emplacement approprié pour le mot-symbole et la devise sans perte de lisibilité des repères fonctionnels ni de dégagement dans le futur boîtier.
2. Dans un **nouveau dossier versionné V2.3**, créer une copie clairement dérivée des sources V1.9 et ne changer **que les sérigraphies/annotations strictement nécessaires**. Conserver les sources originales V1.9, schéma électrique, cuivre, composants, centres, contours, trous, stack, pinout et contraintes. Ajouter `ARNACO`, un **© visuellement en exposant**, et `Votre crédulité est notre force`. Si l'outil n'encode pas ou ne trace pas correctement ©, choisir une voie de dessin vectoriel minimale reproductible (sans dépendance exotique), avec une preuve visuelle réelle du résultat.
3. Réexporter dans le dossier V2.3 les Gerber/Excellon/manifestes correspondant **exactement** à cette variante ; garantir que la sérigraphie destinée à JLCPCB contient les textes, que les couches cuivre/perçages sont byte-identiques ou numériquement équivalentes aux V1.9 à résolution export, et que l'électrique n'a pas changé. Vérifier ERC/DRC/contrôles KiCad pertinents, dimensions 90×100, 2 couches, FR4 1,6 mm, cuivre 1 oz, lead-free HASL envisagé. Inspecter rendus cuivre/sérigraphie et vues exportées ; archiver preuves compactes et écarts.
4. Documenter un **verdict de préparation à la fabrication de cinq PCB prototypes à risque accepté**, séparant ce qui est prouvé (CAO, exports) de ce qui reste à contrôler à froid (emboîtement J5/Zio, dégagement des Connfly 1×10 conservant le corps, fixations D126, isolation, modules réels, étriers, boîtier). Si un problème réel de position/collision sérigraphique ou de DFM apparaît, corriger dans ce seul périmètre et revalider ; ne pas engager de refonte électronique ou mécanique. Préparer une fiche de configuration JLCPCB claire pour la décision d'achat.
5. Publier le dossier, les preuves, les constats techniques utiles et `DERNIER_RAPPORT.md` sur `main`, avec `mission_id` exact, status honnête, SHA et relecture distante. Respecter la séparation entre `DONE` de la mission et qualification physique.

## Permissions et limites

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Aucune commande, paiement, création de panier commercial, upload des Gerber à JLCPCB, impression, geste physique, accès STM32, changement firmware, reconfiguration des périphériques ni opération Git destructive. Garder A-06 : **personnalisation minimale, pas de formalités ou campagne inutiles**.

L'utilisateur conserve la validation finale des coûts, de l'expédition et du lancement de fabrication. Ne pas prétendre avoir acheté, reçu ou testé physiquement des pièces non observées.
