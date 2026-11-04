---
mission_id: TR2-20261104-SHIELD-V14-J5-2X2
status: READY
base_ref: main
base_sha: fa49923a124607bed2c3a4e9b40d86bec98eadd9
created_at_utc: 2026-11-04T09:38:35.059Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — Shield V1.4 : correction du raccord Morpho J5 vers les quatre broches CN12 existantes

## Source du changement
L'utilisateur a vérifié physiquement sa **NUCLEO-U575ZI-Q C05** et fourni des photos dessus/dessous et de détail. Un **groupe de quatre broches mâles disposées en 2×2** est déjà soudé à l'extrémité CN12 ; selon son relevé la population est **CN12-66/67/68/69**, la **broche CN12-70 n'étant pas présente**. Cette observation **invalide la solution V1.3** de J5 femelle 1×2 aux positions 69/70. Les photos à partir de la discussion utilisateur ne sont pas présumées accessibles à Codex sur le poste local : les observations et cotes ci-dessous constituent les données confirmées par l'utilisateur ; aucune cote photogrammétrique inventée.

Cotes opérateur, depuis la surface supérieure du PCB Nucleo : pointe des broches **8,22 mm**, hauteur du corps plastique mâle **2,40 mm** ; métal exposé au-dessus du corps **5,82 mm** (valeurs approximatives au pied à coulisse, pas fiches constructeur). La géométrie des quatre broches est 2×2 au pas Morpho nominal 2,54 mm à recouper avec la source ST. Les deux Nucleo sont des C05 et l'on souhaite **conserver les groupes mâles déjà soudés**.

**Décisions de pilotage :** `Modbus RTU/00_gouvernance/DECISIONS_PRECOMMANDE_SHIELD_TR2_20261103.md`, section `Rectificatif matériel du 04/11/2026` ; archives V1.3 et `CLOTURE_CHATGPT_RECTIFICATIF_CN12.md`. Respecter la gouvernance actuelle. La V1.3 est clôturée **administrativement**, avec un dernier rapport `BLOCKED` authentique, sans réécriture de l'historique.

## Objectif et travaux autonomes
**Livrer les sources et fichiers de fabrication candidats corrigés**, en éliminant la contradiction des broches CN12 sans changer l'architecture du shield ou ses connecteurs économiques.

1. **Vérification nominative du brochage** : relire sur la documentation officielle ST UM2861 les positions **CN12 66, 67, 68, 69, 70** et la correspondance fonctionnelle, les deux rangées (numérotation impaire/paire), ainsi que l'ordre physique vu du dessus. Identifier sans ambiguïté l'emplacement **CN12-69 / PG4** pour l'unique connexion **RTU_DE** ; toutes les autres positions 66/67/68 dans le bloc femelle doivent être des contacts **NC** côté shield, sans liaisons parasites. Ne pas se fier à une image miroir ou à une position déduite d'un texte non recoupé ; si la source contredit le relevé utilisateur, exposer exactement la contradiction et ne pas fabriquer sur hypothèse.
2. **Remplacer J5 femelle 1×2 par une femelle économique 2×2** au pas 2,54 mm, placée **sur la face inférieure** du shield, entrée dirigée vers Nucleo, accueillant le bloc de quatre broches déjà soudé. Choisir une référence disponible et vérifiée par dessin dimensionnel (section admissible des mâles, corps, queues et profondeur d'engagement), non une désignation approximative ; contrôler l'empreinte KiCad, le sens d'implantation dessous, l'effet miroir, le sens de broche 1, les coordonnées précises de chacun des quatre trous et les collisions éventuelles avec les autres pastilles/connexions Morpho. La broche PG4 doit rejoindre **RTU_DE et elle seule**. Ne pas dessouder de broches de la Nucleo et ne pas choisir une femelle 1×1 sans démontrer absence de collision avec les trois mâles voisins.
3. **Calcul d'empilage en Z avec les cotes réelles** : reprendre le schéma J1–J4 mâles shield sur Zio Nucleo, la femelle J5 2×2 et le bloc mâle mesuré (pointe8,22mm ; sommet corps2,40mm). Produire calcul clair d'engagement à hauteur naturelle des Zio, en distinguant valeurs nominales et inconnues réelles. Privilégier une solution qui n'impose pas de mesure supplémentaire de hauteur de broche déjà acquise. Signaler seulement les cotes restant réellement nécessaires : longueur/profondeur admissible du contact femelle, hauteur réelle d'empilage Zio, dégagement composants/entretoises. Ne pas déclarer un montage physique qualifié sans essai.
4. **Corriger effectivement KiCad** : schéma, empreinte J5 2×2, PCB, générateurs/exports, libellés de broches et sérigraphies, nomenclature et budget ; conserver tous les autres modules, nets fonctionnels, vis/entretoises et retenues positives (détrompage automatique non requis). Minimiser les déplacements et reroutages, vérifier que le modèle est régénérable sans restaurer l'ancien J5. Produire gabarit C05 V1.4 A4 1:1, vue dessus et miroir dessous, repères de quatre pins clairs. Conserver `J1..J4` et les options d'alimentation CN15, SPI1/2/3, LPUART/RS485, microSD côté -X, nappe capteur côté -Y, Click vers +X, accès USER/LED/ST-LINK.
5. **Réaliser les contrôles** ERC/DRC stricts KiCad10, parité schéma-PCB, matrice nets/pads, Gerber X2/Excellon, perçages, contour 90×100, calques de pose et prix/quantités de la nouvelle femelle2×2 ; conserver les traces contrôlables et l'historique. Interroger normalement les sources ST ou leurs archives si accessibles ; ne pas bloquer toute la mission sur leur téléchargement. Ne pas prétendre que ces tests prouvent les cotes C05.
6. **Sortie précommande ciblée** : indiquer distinctement `CAD_VERIFIED`, `READY_FOR_PHYSICAL_TEMPLATE_CHECK`, `BLOCKED_FABRICATION`. Le contrôle du gabarit sur **les deux Nucleo**, l'emboîtement Zio/J5, les fixations et les enveloppes MKIGIBV2/Click restent des préalables physiques à la libération si aucune CAO C05 vérifiée n'est trouvée. Les guides LED, boîtier complet, poussoir, retenues finales et impression 3D peuvent attendre le PCB nu. Si un élément bloque encore, consigner l'unique mesure/photo utile, sans demander de refaire les relevés 8,22/2,40 ou 15,2mm MKI.

## Contrat de résultat et autorisations
Effectuer les modifications et commits/push sur `main` autorisés, publier `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261104-SHIELD-V14-J5-2X2`, statut factuel et preuve de relecture distante. ChatGPT révisera indépendamment les sources et les preuves. `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. **Aucune intervention sur STM32, aucun flash, aucune modification FRAM/microSD, aucune soudure, aucun achat ni commande de PCB**. Les données opérateur sont factuelles mais ne constituent pas une qualification physique. Préserver les archives, worktrees, stashes, campagnes et spécifications gelées. **NE PAS COMMANDER** les Gerber de V1.3 ni lever V1.4 sans preuve d'ajustement.
