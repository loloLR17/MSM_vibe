---
mission_id: TR2-20261105-SHIELD-V26-IMPRESSION-COMPLETE-SANS-RECEPTION
status: DONE
base_ref: main
base_sha: c2a1cf83da6c85211656721de2731d83f169cbbd
initial_head: a4738855b1b66e81bbf46233f888f931d10a61fa
head_after_addenda_fast_forward: 72a28db9c96d35d7d421574f464cc0125c380a20
result_sha: 348e559cef823a57a86a99a98d2ef2376a6c5f7d
completed_at_utc: 2026-11-05T22:21:45Z
created_at_utc: 2026-11-05T22:17:39Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.6 — dossier complet imprimable, avec ajustement à froid

**Le kit numérique complet est terminé, publié et relu depuis GitHub :25 STL /142pièces pour deux prototypes, quatre étriers inclus.** DONE porte sur cette livraison numérique complète avec ajustement à froid prévu. L’objectif de cette mission est l’impression de prototypes avec un ajustement prévu au premier montage, **pas leur qualification physique**. Aucune attente de livraison ou nouvelle photo ajoutée comme préalable à la génération.

Le dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_6_IMPRESSION_COMPLETE/` contient les natifs éditables, les STL orientés, un seul ZIP imprimeur, une notice PETG, les quantités, la nomenclature, une fiche d’ajustement et un PDF de cinq pages avec plan et coupes. Les14formes du coffret V2.5 sont reprises octet pour octet. PCB V2.3 commandé, CAM, X/Y officielsST, G12,25, D126, firmware et électronique conservés. **747fichiers antérieurs du shield comparés sans différence.**

## Livraison et quantités

ZIP unique : `pack_imprimeur/TR2_V26_COMPLET_2_PROTOTYPES.zip`, **32entrées**,25STL,142pièces. Réouverture locale complète, CRC et comparaison de chaque membre réussis. SHA-256 : `2fee5474fc251d96a5d2b01eb2df2c50561965d72085f32319f07780f9a80230` ; 545997octets. Aucun STEP fabricant ni logiciel dans ce ZIP ; aucun envoi à l’ami imprimeur, aucune impression ou commande effectuée. La décision de transmission suit la revue indépendante ChatGPT.

| Groupe | Quantité pour deux prototypes | Rôle |
|---|---:|---|
| Quatorze types V2.5 identiques |40| Coffrets, couvercles ARNACO, cassettes, trappes, brides,8D126 et8bras avec leurs réserves |
| Supports H8/H10/H12 |24|8de chaque hauteur, **8seulement à choisir** ; les variantes ne se montent pas simultanément |
| Cales0,5/1/2mm |12/36/12| Les36cales1comportent24rondelles fixes,8calages et4réserves |
| Étriers MKI/Click |2+2| Les quatre retenues auparavant absentes sont incluses |
| Patins MKI avant/arrière/Click |4/4/6|8utiles+6réserves |

Masse du scénarioPETGdenseCAO :737,72g dans le scénarioPETG1,27g/cm³, **pas une consommation réelle de trancheur** ; supports, bordures et purge non évalués. Aucun temps/prix d’impression promis. Fichiers : `QUANTITES.csv`, `BOM_QUINCAILLERIE.csv`, `IMPRIMEUR.md`, `MONTAGE_A_FROID.md`, `vues/ASSEMBLAGE_AJUSTEMENT_V26.pdf`.

## Conception et chemin d’efforts

**FRAM/microSD** : vrai appui rigide du module jusqu’au shield, remplaçant le principe des écrous suspendus V2.4. Deux portées par module, quatre par TR2. Appuis PETG à section arrondie4,4×4mm, bore2,4 ; hauteurs8/10/12 et cales0,5/1/2. Couverture mécanique8..15,5par pas0,5, avec ponçage de la face supérieure de l’appui≤0,5mm, à plat et hors électronique. La largeur5mm initiale laissait seulement0,04mm de bord sur Adafruit ;4,4mm porte la marge nominale à0,34mm,0,14dans l’enveloppe de décentrage±0,2. Cette dernière est une allocation de conception, pas une précision FDM mesurée.

La fixation comporte M2×30 par-dessous, rondellePETG1mm sous tête, support/cales, PCB module, rondellePETG1mm et deux écrous métalliquesM2 en contre-écrou. La portée rigide reprend la compression ; la fixation retient le soulèvement. Aucune pression de vis utilisée pour emboîter les sockets. La notice impose d’emboîter d’abord à la main, de choisir le calage ensuite, sans fléchir le PCB. Les corps entiers1×10 sont conservés, y compris la position sans queue soudée.

**H n’est pas inventé** : femelleConnfly8,5 constructeur ; collier mâle standard2,5 =déduction H≈11, non mesure des mâles Adafruit. Le vraiSTEPClick indique2,54 de collier. La ficheConnfly ne donne pas la fenêtre interne de contact ; **la plage mécanique des appuis n’est pas une course d’engagement électrique qualifiée**. Aucun appui unique11mm imposé et aucun besoin de recevoir les connecteurs avant impression.

**MKI/Click** : deux étriers porteurs pris sur les axes existants du shield, quatre patinsPETG guidés par TR2. Guides carrés5,4/patins5, jeu nominal0,2par face ; M3×12 et DIN985M3 capturé, coupelle avec1,5mm fermé sous la pointe. Pas de vis contre un module, pas de filet imprimé. Patins0,8×2sur extrémités courtes MKI et bout logique Click ; aucun perçage de module, pastille, borne, composant, cavalier ou connecteur utilisé comme fixation. Pieds d’étriers en appui de compression au shield ; écrous M2 sur épaulementZ34,5. La partie métallique reste hors domaine isoléClick : aucun pont GND1/GND2 par la fixation. Ce n’est pas une qualification normative d’isolement.

Plage des dessusPCB retenusZ24,75..29,15. Au réglage : approcher doucement le contact puis reculer≈1/5tour, sans forte précharge. Le jeu de poche DIN9850,4mm et les tolérances s’ajoutent à ce déplacement0,1 ; la notice demande de vérifier réellement le petit soulèvement libre et la retenue. Aucune tenue en vibration déduite de la seule CAO.

## Sources constructeur et addenda récupérés

Sources et empreintes complètes : `SOURCES.md`, `preuves/sources-fabricants.json`, `sources/adafruit-derives.json`, `preuves/revisions-photos-addendum.json`. Les gros fichiers restent dans `/home/lolo/dev/msm/reports/TR2-shield-v26-20261105/`, pas redistribués inutilement.

- **Mikroe bon produit3863** : archive officielle36,16Mo ; STEP`RS485_Isolator_2_click_v101.step`,163solides. Empreinte STEP `ff4eed56c5ac02656b4a2f14e99e96460be65ddf18da9a824a52051984cf608e`. PCB57,15×25,4, vrais volumes de composants/borniers, patins prèsX84,2. Aucune substitution avec l’ancien RS485IsolatorClick.
- **ST** : archiveGerber officielleSTEVAL-MKI208V1K,94 528octets, SHA`265d576a827daf8b19fa7dea2c1308d388568538d920775fe7708d2b3510e4a7`. Le GKO MKIGIBV2 révèle≈18,00×30,93mm, plus étroit que l’enveloppe provisoire20,32×33,02. Drills, pâte GTP et assemblage GM15 servent à choisir les extrémités courtes, pas les côtés occupés par les rangées. AxesJP1/JP2 recoupés avec ceux déjà commandés, écart d’arrondi≤0,000254mm ; **aucun PCB déplacé**. Petit défaut de fermeture du tracé≤0,03mm signalé, pas transformé en tolérance fabricant garantie.
- **Adafruit** : Eagle exacts4718/4682 et STEP SD6solides déjà épinglé. Les trous sont Ø2,5métallisés/pastilles3,2flottantes, pasNPTHdesmodules ; lesNPTHdu shield restent2,7. Enveloppes composantsFRAM tirées des empreintes avec margeXY0,25etH4deconception ; ceHn’est pas une mesure fabricant. SDsourcePCB1,57corrigé dans l’assemblage nominal de rondelle supérieure.
- **Connfly/Samtec** : fiches primaires récupérées ; corps8,5, queue3±0,2, longueur1×10=25,9±0,3 ;SHF-SM5,46 de haut,5,85 de large,13,97de long. Enveloppe de calculIDCconnecté plus largeX46,5..58,8/Y25,8..41/Z0..8,5au-dessusPCB, avec départ de nappe ; pas de cote photo utilisée.

**Addenda distants396bda2e puis72a28db9 lus intégralement avant premier commit**. Ils ne modifiaient queMISSION_EN_COURS.md. LeHEADlocal sans commit V2.6 était ancêtre de leurHEAD ; avance rapide sûre effectuée, nouveaux fichiers locaux préservés, aucune divergence résolue par merge/rebase. Mission initiale et révisée archivées séparément.

ChatGPT rapporte des photos du Click réel **HWREV1.02/ADM2867E** ; images non jointes au dépôt et non vues directement par Codex. L’archive officielle ne contient aucun membrev102 ; **aucune équivalence intégrale1.01/1.02 inventée**. Les deux ouvertures photographiées ne sont pas qualifiées comme trous de fixation ; elles ne sont pas utilisées. Le modèle STEP simplifie aussi les perçages. Fonction précise de ces ouvertures non démontrée dans cette mission, sans incidence sur la retenue extérieure retenue. La comparaison de révision/portées sera faite à froid. L’estimation photoMKI21–22mm ne remplace pas le contourGKO et les15,24mm entre rangées. Aucun report en attente de photo ou matériel.

## Validation proportionnée réellement exécutée

Environnement existant CadQuery2.8/Trimesh/Shapely/Matplotlib ; Python`/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Scripts et sorties dans le dossierV2.6 ; aucun outil matériel utilisé.

| Contrôle | Résultat observé |
|---|---|
| `sources/modele.py` |25STL/25STEP individuels ;14repris exacts V2.5,11nouveaux ; assemblage propre nominal |
| `sources/controle.py --native-dir …` |**1 097contrôles BREP,0intersection positive indésirable** ; modules natifsClick163/SD6, contour/composantsST, enveloppesEagleFRAM, caisse et métalV2.5 |
| Nouveaux maillages |11STL réouverts : fermés, orientés,volume positif,un composant,STEP valide1solide ; écart relatif volume<1% |
| Pièces V2.5 |14STL/STEP comparés octet pour octet ; validations de leurs formes/D126/logo/visserie/accès réutilisées, pas relancées |
| Assemblages |STEP propres réimportés :44solides imprimés et32gabarits métal nouveaux, valides |
| FRAM/SD |Supports H8/11/15,5, tigesM2×30, rondelles/écrous et extractionSD sans collision dans les cas contrôlés |
| Patins |Trois dessusPCB×deux jeux, bornes analytiques des guides/filetages/têtes entre extrêmes ; marge minimale couvercleM3 **1,05mm**, recouvrement guide≥3,75 |
| Métal/Nucleo |Nouvelles têtesM2≥Z9,05nominal ; par enveloppeG/rondelle,≥Z8,85vsSTEPSTmax8,559996, marge≥**0,29mm** ;B1physique10,2hors axes concernés |
| Nappe |Trois routesBezier, chacune101points et100dallesAABB conservatives ; largeur8/épaisseur1, sortieX55,5,0collision avec étriers/caisse ; courbure échantillonnée min≈64,67mm, pas une limite fabricant qualifiée |
| Bords de contact |Marge nominale minimale0,489mmMKI /0,39Click ; allocation±0,2 laisse≥0,19mm aux contacts les plus proches du bord ; contrôle réel requis |
| ZIP et conservation |32entrées intégralement comparées,CRCOK ;25STL/142pièces ;747fichiers antérieurs inchangés |

**Échecs et corrections traçables** : le premier contrôle a refusé48intersections dues au gabarit de visM3 du test, dessiné à l’envers (tête sous la pointe alors que ces réglages se vissent vers le bas). L’orientation a été corrigée pour correspondre au chemin d’efforts prévu ; aucun contrôle affaibli. Preuve`controle-initial-refuse.json`. Les guides/coupelles sont découpés après union, pour ne pas reboucher leurs passages par une traverse. La section d’appui a été réduite seulement enXpour rétablir de la marge de bord. Un candidatM2×25 a passé les intersections, puis lesM2×30 ont été retenues partout : une seule longueurM2, réserve accrue de filetage sur toute la plage, dépassements recontrôlés. Dernier élargissement conservateur de l’enveloppeIDC : les1 097contrôles passent. Journaux finaux conservés, pas de matériel ou test firmware répété.

Aucune compilation/testhôte/cross-build/flash/debug : périmètre exclusivement mécanique, ces opérations n’ajoutent aucune preuve pertinente. Pas de nouvelleERC/DRC électronique sur PCB inchangé. Pas de prix/stock/délai revisité hors mission ; quincaillerie standard, pas d’achat.

## Quincaillerie et réserves de premier montage

**Ajile D126 exclusivement** :8visM2×20nylon+16écrous,6/12utiles. **Métal courant** :42M3×12utiles (34coffret+8patins),cible50 ;34DIN934M3,cible40 ;8DIN985M3,cible10 ;16M2×30,cible20 ;32écrousM2,cible40. LesM2n’utilisent aucun Ajile ; pas de nouvelle vis nylon spécialisée. Têtes d’enveloppeM3≤D6/H3,2 etM2≤D4,2/H2,2 ; ne pas substituer des pièces plus grosses sans contrôle.

À vérifier à froid, sans empêcher l’impression : vraies hauteurs de connecteurs etFDM ; zones nuesMKI/Click1.02 ; portées/rondelles/têtes et cuivre réel ; engagement/soulèvement retenu sans forçage ; microSD libre ; IDC/nappe et serre-câbles ; accèsCN15/ST-LINK/RS485/USER, visibilité réelleLD1/2/3 ; état desD126/G/J5 et fixations. Les nouveaux supports sont ajustables pour ce contrôle. Toute interférence reste un arrêt local de montage, jamais une permission de mettre sous tension une fixation douteuse. Tenue en vibration/fluage/température/isolement/IP **non qualifiée**.

## Git, journal et publication

Racine historique proprepréservée`/home/lolo/dev/msm/projects/MSM_vibe` à82169af7 ;stashes`6251b988…`et`f3b829e6…`inchangés. Worktree neuf`/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V26-COMPLET`,branche locale`codex/shield-v26-complet`,destination explicite`refs/heads/main`. Départa4738855 ;avant première publication avance rapide72a28db9 uniquement pour addenda, sans réécriture d’histoire. Les autres worktrees ne sont pas modifiés.

`ETAT_COURANT_TR2.md` actualisé factuellement : kit désormais complet/imprimable avec calage à froid, sources fabricant récupérées, réserves1.01/1.02 etabsence de qualification physique. Pas de changement de gouvernance. `archives/<mission_id>/MISSION_INITIALE.md` conserve le contrat initial et`MISSION.md` sa version avec addenda. Aucun historique écrasé.

**Publication technique vérifiée** : commit `348e559cef823a57a86a99a98d2ef2376a6c5f7d` — `Shield V26: complete adjustable print kit for two prototypes`, push normal `HEAD:refs/heads/main` réussi, puis `git fetch origin main`. HEAD etorigin/main identiques. Depuis les objets complets de cette référence GitHub fraîchement récupérée : **845blobs relus et comparés octet pour octet**, dont747historiques conservés et98fichiers modifiés/ajoutés depuis le départ, incluant le contrat révisé. Le ZIP distant a été rouvert et ses32entrées vérifiéesCRC/SHA,25STL/142pièces/quatreétriers. Empreinte du manifeste de relecture : `84c94f7452464c7cae38bbf364ab81227c0c41e13a24b82171ef938c01fb2426`. Preuve `preuves/publication-initiale.json` ; script reproductible `sources/relecture_distante.py` depuis la racine du dépôt aprèsfetch.

État après cette publication : worktree V2.6 propre, racine historique propre à82169af7, deux stashes inchangés (6251b988… etf3b829e6…),117worktrees recensés sans suppression. Le présent commit de clôture ajoute uniquement le rapportDONE, son archive, la preuve et l’outil de relecture, plus une actualisation factuelle du journal/README. Aucun STL/STEP/ZIP n’est retouché après la preuve technique. Son push et sa relecture finale seront vérifiés avant la réponse utilisateur, dont le SHA fera foi pour la dernière référence distante ; `result_sha` reste le codeCAO réellement évalué348e559c.

**À transmettre à ChatGPT** : revue indépendante du ZIP complet, en tenant compte du calageàfroid expressément autorisé et de l’écartClick1.01/1.02 ; décision d’envoi à l’ami imprimeur ensuite. Aucun contrôle supplémentaire ni réception n’est demandé avant la revue du dossier imprimable. Aucune qualification physique revendiquée.
