<!-- Contrat V2.7 initial publié par ChatGPT le 2026-11-05 et retiré AVANT exécution. Remplacé à la demande utilisateur par une mission V2.7 KISS consolidée (06/11). Ceci n'est pas un rapport Codex. -->

---
mission_id: TR2-20261105-SHIELD-V27-USER-ACTIONNEUR-EXTERIEUR
status: READY
base_ref: main
base_sha: 036f682476cfbb9d8832cc95aff4989e4c6161ec
created_at_utc: 2026-11-05T22:43:04.289Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.7 : rétablir un bouton USER extérieur captif, préserver le kit d'impression

## Constat et correction obligatoire du contrat utilisateur

**Régression V2.4 détectée par l'utilisateur lors de l'examen FreeCAD du ZIP V2.6.** Le kit V2.6, quoique numériquement complet, **n'est PLUS libéré pour impression** : revue ChatGPT V2.6 rectifiée (NO-GO) sur `main`. Le bouton USER B1 de la Nucleo doit pouvoir être activé de **l'extérieur en exploitation**, par **une simple pression**, sans ouvrir, dévisser, manipuler ni retirer trappe ou capot. Une petite vis, un cache ou un actionneur non captif perdu dans les fonds sous un réducteur est un risque opérationnel inacceptable. La trappe pivotante V2.4/V2.6, qui n'impose pas nécessairement de dévissage mais impose son ouverture et supprime le poussoir extérieur, est une régression fonctionnelle non autorisée.

**Responsabilité de pilotage ChatGPT** : la mission V2.4 avait dit seulement « accès USER simple mais protégé », ce qui a effacé l'exigence plus précise du **poussoir captif extérieur** dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/mechanical/V1_5/parameters.json`, rubrique `USER_principle` (candidat initial : tigeØ5, guideØ5,5, têteØ10, collerette captiveØ12, entrefer repos≈0,6mm, arrêt de course≈1mm ; valeurs de **concept V1.5 non qualifiées**, ne pas recopier aveuglément). Codex avait généré en V2.4 la cassette sud `04_cassette_sud.stl` avec ouverture et la trappe pivotante `08_trappe_USER.stl`, sous `V2_4_IMPRESSION_3D/sources/modele.py`. V2.5/V2.6 en avaient repris la forme à l'identique. Revenir à l'exigence d'origine sans refondre le boîtier.

## Résultat attendu impératif : correction ciblée avant impression

1. **Concevoir un actionneur mécanique accessible directement de l'extérieur**, robuste et à course limitée, pouvant être pressé au doigt. Poussoir **captif en permanence**, même couvercle fermé et même en cas de vibration ; aucune pièce mobile libérable en utilisation, aucune petite vis devant être défaite pour déclencher USER. Protéger contre appui accidentel (bouton affleurant ou légèrement en retrait derrière une collerette robuste), sans nécessité d'ouvrir de trappe. Veiller à ce que le dispositif ne puisse pas toucher ni activer RESET par erreur.
2. Utiliser l'existant : la **cassette sud** et la zone USER, au plus quelques pièces de guidage/arrêt imprimées. Faire l'étude de course du **B1 réel Nucleo C05** depuis la documentation/ST STEP existant et les coordonnées établies (V2.4 `sources/parametres.json`, B1 XY≈29,5212/120,3262, top signalé10,2mm), sans inventer effort ou course fabricant. **Repos = pas de précharge sur B1**, butée mécanique indépendante qui empêche de l'enfoncer excessivement. Assurer un retour réel en position de repos sous vibration, pas seulement une tige libre pouvant peser sur B1 ; un petit ressort ou élément élastique captif standard est admissible seulement si nécessaire et explicitement montable, avec approvisionnement ordinaire et sans imposer du matériel spécial pour son principe. Comparer plusieurs architectures simples, choisir la moins risquée avec KISS.
3. **Aucun changement à la caisse, au couvercle ARNACO, au PCB V2.3 commandé, aux D126, aux axes de supports/étriers** sauf si la vérification géométrique prouve un défaut local qu'il faut corriger. Préserver la fenêtre LED, la prise CN15 USB-C, la nappe, le bornier RS485, la SD, la ventilation et le chemin vers USER. Préférer modifier uniquement la cassette sud `04_cassette_sud` et remplacer `08_trappe_USER` par une pièce de poussée captive ; si un guide est indispensable, l'intégrer à la cassette si fabricable. **Ne pas produire de trappe à dévisser**. Les autres pièces imprimées de V2.6 doivent rester byte-identiques autant que possible.
4. **Contrôle CAO explicite B1** : chemin d'actionnement de la surface externe au bouton ; guidage, axialité, jeu, capture, débattement, butée, retour, garde RESET, aucune collision en repos/pressé ni avec le capot/câbles/electronique ; essai numérique de tolérances réalistes impression PETG, pas une simple capture d'écran d'un solide. Le parcours et la notice doivent montrer les deux états *relâché* et *appuyé*, et ce qui empêche toute séparation d'une pièce. Conserver clairement comme réserve de premier article le vrai effort, la course électrique, et les tolérances réelles FDM/non mesurées.
5. Générer **nouveau ZIP V2.7 complet** pour les **deux prototypes** avec toutes pièces imprimables (y compris supports H, cales, 4 étriers, pièces de USER), quantités, README imprimeur, nomenclature standard corrigée si nécessaire, consignes de montage **sans élément perdable en utilisation** et vues 3D/section de la zone USER. Ne pas prétendre que le ZIP V2.6 est encore libéré ; conserver l'historique V2.6 et son hash, ne pas l'écraser. Pas de travail manuel de CAO demandé à l'ami imprimeur.
6. Tests numériques proportionnés des parties modifiées et conservation de l'existant prouvée, relecture ZIP (CRC/empreinte) et publication par commits normaux sur `main`, rapport `DERNIER_RAPPORT.md` avec mission_id de V2.7, SHA de code/CAO, preuves, réserves. ChatGPT contrôle indépendamment et décide du GO impression. Si la course/rappel ne peuvent être tenus sans information ou qu'un effort potentiel sur B1 ne peut pas être borné, documenter le blocage local clairement et proposer le plus petit remplacement d'architecture possible : ne pas transformer ce doute en validation silencieuse.

## Critères de succès / limites

**Le USER extérieur doit être visible, accessible et actionnable immédiatement d'une seule main par pression**, sans capot mobile à ouvrir, sans manipulation de vis, et protégé d'une pression involontaire. Aucune pièce pouvant tomber dans les fonds pendant cet usage. L'accès RESET reste indirect/protégé. Vérifier le chemin d'effort, la butée mécanique et le retour ; aucun contact permanent/forçage sur les broches du shield. V2.6 avait été imprimable mais était inacceptable ergonomiquement ; ne pas se limiter à reproduire cette erreur dans une nouvelle cassette.

Délai utilisateur : **ce soir 05/11/2026**, ne pas attendre la livraison des PCB. Travail logiciel/CAO et tests numériques uniquement. `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Aucun achat, impression ou transmission effectués automatiquement. Aucun appel à Work.
