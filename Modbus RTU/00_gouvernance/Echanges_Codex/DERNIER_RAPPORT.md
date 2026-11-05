---
mission_id: TR2-20261105-SHIELD-V24-IMPRESSION-3D
status: IN_PROGRESS
base_ref: main
base_sha: a6887a2a938dfa3d098efbb7458fb43ddea9f67b
initial_head: dad65db35259da6db5e1a202cd4bc835879b258b
result_sha: null
created_at_utc: 2026-11-05T19:53:51Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.4 — deux prototypes ARNACO, transfert d’impression partiel

**Conception et contrôles numériques terminés ; publication/relecture distante encore à obtenir.** Ce statut provisoire porte sur la publication, pas sur un obstacle extérieur. Le contrat autorise des réserves localisées et un pack partiel sûr : les quatre étriers MKI/Click restent exclus de l’impression, leurs zones d’appui réelles n’étant pas démontrées. Aucun kit complet ni assemblage physique qualifié revendiqué.

## Livrable à contrôler

Dossier `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_4_IMPRESSION_3D/` : sources paramétriques CadQuery/JSON,17STEP individuels et assemblage de nos pièces uniquement,17STL répartis par statut, vues/coupes PDF/PNG, BOM globale, consignes/assemblage/réserves et preuves reproductibles.

**Un seul ZIP candidat pour l’ami** : `pack_imprimeur/TR2_V24_2_PROTOTYPES_PARTIEL.zip`, **23 entrées,15types,52pièces**, réserves comprises. Deux bases, deux couvercles ARNACO monobloc sans devise, panneaux/trappes/brides, D1268=6utiles+2réserves, appuis FRAM/SD12=8utiles+4réserves, bras Nucleo8=6utiles+2réserves. Le ZIP contient seulement ces15STL et les instructions/BOM/PDF/manifeste ; aucune variante provisoire, fichier ST tiers,3MF non validé, police ou logo séparé. **Les deux types / quatre étriers MKI/Click sont hors ZIP.**

SHA-256 ZIP : `2ff65c5660e9828853678e1f7e011ebd22d3528e6b8aa741a67081136095600a` ; taille2120518octets. Réouverture complète des23entrées, comparaison octet pour octet et contrôle CRCZIP réussis. `preuves/pack-manifeste.json` et `QUANTITES.csv` donnent les listes exactes. Aucun transfert à l’ami effectué ; revue indépendante ChatGPT puis décision humaine de transmission restent attendues.

## Conception et conservation

Reprise minimale du concept/générateur V1.5 avec paramètres et données V2.0/V2.1/V2.3. Référentiel unique : X=KiCadX, YCAO=−YKiCad, Z0=dessus Nucleo. **G12,25 / dessus shield13,85mm** ; aucun retour au scénario ancien G11. Modules représentés à11mm au-dessus du shield seulement comme scénario catalogue, jamais comme mesure physique.

Empreinte de caisse158×166mm conservée. Fond−14/plancher−11, bord47/couvercle47..50, relief51 : corps64mm de haut,65avec ARNACO. Deux portes dépassent latéralement de0,8mm ; enveloppe propre assemblée158,8×166,8×65mm hors vis/câbles/débattement. Garde nominale sous le point ST le plus bas0,9924mm. Poches de tête AjileØ6,4/profondeur3,2 sont des réservations de conception ; Ø/hauteur réels restent à confirmer.

Cuvettes ouvertes du côté des broches voisines, empilage déposable sans effort sur connecteurs ; bras antilevée avec jeu nominal0,8mm, sans précharge PCB. Appuis FRAM/SD : rondelle isolante et écrous nylon réglés à la hauteur naturelle d’emboîtement, deux points par module ; corps Connfly1×10 entiers représentés, dernière queue neutralisée selon V2.3. Capacité géométrique de réglage8..14mm, **pas validation d’accouplement sur cette plage**.

Baies ouvertes vers le haut et cassettes remplaçables : ST-LINK/CN1, CN15, fenêtre PC latérale, nappe, microSD, RS485. USER direct sous trappe, sans poussoir long ni exposition alignée de RESET ; trappe tournevis distincte. Nappe retenue par bride avec fentes/collier et garniture souple ; câble RS485 retenu hors du Click avec visserie isolante, aucun pont métallique vers le bus isolé. Les panneaux limitent les reprises d’impression si un vrai câble exige une adaptation.

**602fichiers historiques du shield vérifiés identiques**, dont PCB V2.3 commandé, V1.9 et tous CAM/Gerber/anciens modèles. `preuves/conservation-finale.json`. Aucun PCB, cuivre, schéma ou logiciel/firmware modifié. Sources ST et empreintes sources tracées sans redistribuer les solides tiers dans le nouveau pack.

## Validations réellement exécutées

Environnement existant `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Versions exactes dans `preuves/environnement-validation.json`. Depuis le nouveau dossier :

| Commande | Résultat observé / portée |
|---|---|
| `python sources/modele.py` | code0,17solides propres valides et monoblocs ; exports STEP/STL et assemblage. |
| `python sources/controle.py` | code0,17/17STEP réouverts et STL fermés/connexes, orientation cohérente, volumes positifs, bornes comparées au STEP dans la tolérance de discrétisation0,025/0,06mm. Erreur maximale de borne0,001494mm, volume relatif0,059672%. Pas de preuve universelle d’absence de toute auto-intersection par un outil indépendant. |
| Relief sur STEP réimporté | Six glyphes au-dessus deZ50, hauteur1mm, volume ajouté193,82327mm³, un seul solide avec le couvercle. ARNACO visible dans `vues/05_relief_reel.png`. Aucune devise/©. |
| Assemblage nominal | 24instances propres contre17enveloppes de référence : aucune intersection positive imprévue. Hauteurs/contours MKI/Click/IDC restent des enveloppes provisoires. |
| C05 officielle |1060solides ST, transformation/population documentées V2.0. Couverture de matière de la base par cellules prouvée, résidu0, pour écarter le volume vide de la chambre ; quatre paires nécessitent un booléen exact après filtres. **Aucune collision, même avec le modèle ST complet** ; scénario des deux modèles Morpho non peuplés également sans collision. Contact de portée intentionnel avec PCB d’épaisseur numérique4,42nm, volume0,00030743mm³, conservé et identifié, pas gommé. |
| D126 | Différence volumique symétrique avec STEP V2.1 =0. Les neuf cas antérieurs de tolérances ne sont pas rejoués ; interactions nouvelles avec caisse contrôlées. |
| `python sources/services.py` | code0,sept gabarits nominaux dégagés : USB×2, extraction SD, doigt USER, passage RS485, tournevis, nappe. Dix-sept tiges/têtes M3 bornées par kit sans collision avec pièces globales contrôlées ; appuis/écrous M2 hors corridor de carte, garde analytique0,3106mm. Ceci ne qualifie pas les câbles, la main ou l’outil réels. |
| Optique |66observateurs CAD communs sur88candidats franchissent les ouvertures et n’intersectent pas les AABB des obstacles retenus. Cibles =origines ST à hauteur du corps+0,1mm, **pas centres d’émission mesurés**. Aucun résultat de luminosité/visibilité physique revendiqué. |
| `python sources/vues.py` | code0, cinq PNG et PDF cinq pages issus des solides et enveloppes, vues/sections/relief inspectés. Les arêtes de triangulation du rendu ne sont pas des nervures du modèle. |
| `python3 sources/paquet.py` | code0, axes de tous ancrages recoupés avec11trous du PCB V2.3, son SHA-256 inchangé ; ZIP et602originaux comparés intégralement. |

Logs finaux et JSON sous `preuves/`. Le contrôle de blancs Git ne signale qu’un espace final déjà présent dans la mission source : sa copie d’archive est conservée strictement identique ; les autres changements passent le contrôle de blancs. Les contrôles ne sont pas des essais FDM, de résistance, de fluage, de vibrations, d’étanchéité ou d’isolement physique. Aucun build/flash/debug/accès MCU, aucun test de stockage ou acquisition répété. ERC/DRC non relancés car électronique inchangée.

## Échecs traités et limites

`preuves/corrections-intermediaires.json`, `rejet-01-base-C05.json`, `rejet-01-visserie.json` conservent les échecs : cadre de vitre contre rail20mm³ corrigé par retrait local ; cuvettes pleines contre ST437/894/900 corrigées par demi-cuvettes ouvertes ; trois M3×16 dans le fond corrigées en M3×12. Ancien fond/G11 non repris. Contrôle de bornes STL initialement trop strict pour des cordes circulaires corrigé par comparaison à la tolérance d’export documentée ; topologie et contrôles volumétriques conservés. Deux calculs C05 préliminaires sans conclusion ont été interrompus pour éviter les bornes répétées/booléens dans le vide ; le contrôle final complet après corrections est réussi.

Réserves localisées dans `RESERVES.md` : quatre étriers, engagement réel J5/Zio/G, têtes Ajile, tolérances PETG et tenue, vrais contacts/modules/IDC, câbles, course/ergonomie USER et visibilité réelle des trois LED. Montage à froid du premier article nécessaire avant alimentation ; aucune intervention physique demandée/exécutée aujourd’hui, aucun nouveau papier. Les réserves mécaniques acceptées pour les cinq PCB restent applicables.

## Impression / fournitures / budget

Plateau utile180×180×70mm minimum avec brim5mm,200×200 conseillé ; PETG une couleur, buse0,4 ; orientations STL déjà appliquées. Couches0,20générales et0,10D126 ; remplissage100% pour D126/appuis/bras, quatre à cinq périmètres ; petites poches d’écrous à vérifier au trancheur. Aucun G-code ou filet M2 imprimé. **Masse dense calculée718,16g**, scénario PETG1,27g/cm³ ; allocation budgétaire20%≈861,79g sans garantie contre reprises. Temps, énergie et masse tranchée **non estimés**.

Ne pas racheter commandes acquises : cinq PCB, TME17,60€ et Ajile16,20€ selon contrat, sans vérification de réception. Les8vis M2×20/16écrous commandés couvrent D126 mais pas tous les nouveaux appuis. Besoin global utile14vis/36écrous M2 ; cible avec réserves16/40, **complément8vis+24écrous**. Ajouter32M3×16 et8M3×12 avec réserves,40écrous M3, deux vitres50×18×1,5, colliers et garnitures, stock personnel non inventorié. Visserie des étriers exclue et non figée. Samtec demandé, réception inconnue. Aucun achat, contact commercial, justificatif/identifiant personnel ni prix global final inventé ; cible≈50€ non établie.

## État Git / publication / suite

Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V24-IMPRESSION`, branche locale `codex/shield-v24-impression`, départ propre à `dad65db35259da6db5e1a202cd4bc835879b258b`. Dépôt principal historique propre conservé à `82169af7e76797731358852bec83bfb0ceb54ccf`, worktrees historiques préservés, deux stashes `6251b988…` et `f3b829e6…` conservés. Aucun reset/clean/merge/rebase, aucune résolution de divergence. Commit/push normaux vers `HEAD:refs/heads/main` autorisés par cette mission uniquement. Aucun flash/debug, commande/impression réelle ni envoi tiers.

Journal `ETAT_COURANT_TR2.md` actualisé : pack partiel, résultats numériques, réserves, fournitures et prochaine revue. Prochaine étape ChatGPT : contrôler les livrables et le ZIP, puis décision de transmission du lot partiel et traitement des quatre étriers à réception. Le SHA du code évalué sera fixé au commit publié lors de la clôture documentaire ; l’état actuel non commité est décrit par les manifestes d’artefacts.

Publication distante : **à effectuer**. Aucun DONE avant push et relecture complète depuis GitHub.
