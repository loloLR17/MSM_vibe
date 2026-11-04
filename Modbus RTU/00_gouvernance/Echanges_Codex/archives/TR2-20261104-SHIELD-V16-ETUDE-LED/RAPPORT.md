---
mission_id: TR2-20261104-SHIELD-V16-ETUDE-LED
status: DONE
base_ref: main
base_sha: ee947e532fbd418cd787246d95f2237452198126
initial_head: 6d97906d32ec6b30a6db9caa140fc02481aa142d
result_sha: a526c1c162aa365c9701a2bcbc3f1c0bcddabcf6
created_at_utc: 2026-11-04T14:32:56Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V1.6 — étude de visibilité LED terminée et publiée

**L’étude aboutit à une recommandation conditionnelle, recevable selon le contrat : fenêtre fermée en priorité ; trois LED déportées avec buffers via Morpho en seul repli.** Les contrôles logiciels et documents sont terminés ; publication `6e9017ea1ed99375f8904fd47ee21a73fb3fac1e` récupérée de GitHub et relue intégralement : **260 fichiers comparés octet pour octet**, dont tous les livrables modifiés et les232fichiers antérieurs du shield. Preuve dans `archives/TR2-20261104-SHIELD-V16-ETUDE-LED/REMOTE_READBACK.json`. **Aucune qualification physique ni libération de fabrication.** La V1.5 administrativement clôturée conserve son rapport BLOCKED et ses réserves.

## Arbitrage

**Fenêtre transparente au nord-est du couvercle**, avec visée oblique devant le bord nord du shield, à côté de la nappe : moins de pièces, aucun raccordement électrique et aucun changement PCB si les vraies positions/luminosités le permettent. Le scénario propose une ouverture utile23×10mm, plaque1,5mm et cadre vissé/joint. Les centres C05 restent inconnus/null ; les positions de calcul sont un domaine de conception hypothétique et non des LED artificiellement placées. Angle48,4–51,7°, marge résiduelle0,246mm après élargissement0,5 des obstacles modélisés : **la proposition doit être vérifiée sur le vrai empilage et en éclairage d’usage**, sans quoi elle est rejetée. Pas de certification IP.

Les guides courts/droits ne sont pas retenus dans le boîtier courant : famille Bivar examinée jusqu’à21,1mm, distance hypothétique46,6–49,6mm jusqu’au toit. Un toit local abaissé et les vrais centres seraient nécessaires. Une ouverture exploratoire X45..65/Y17..28 touche quatre pastillesJ8 et +3V3/ACC_MISO/ACC_MOSI ; ce rectangle n’est ni une position réelle des LED, ni une découpe proposée. Le passage est contraint par MKI/supports. Aucune découpe aveugle, fibre ou renvoi compliqué développé.

**Seul repli : trois LED sur le shield, buffers actifsHIGH conservant les LED natives.** Vérification du vrai schéma C05 sous forme textuelle et recoupement UM2861 : NPN9013 ; résistances LED510/680/1000Ω, bases47k et B–E100k. PC7→CN12-19, PB7→CN11-21, PG2→CN11-42. **Ces GPIO sont absents des quatre connecteurs Zio du shield et du J5 actuel.** La population des contacts Morpho requis reste à vérifier ; des mâles supplémentaires pourraient devoir être soudés dans une mission ultérieure. PA5/D13 est déjà FRAM_SCK : aucun changement de GPIO ou de sémantique firmware proposé.

Principe ajouté : trois étages NPN, chacun Rb4,7k/Rbe100k/RLED1k initiale ; alimentation3V3 et masse logique, jamais GND2. Charge ajoutée calculée<0,807mA/GPIO, total3V3<13,79mA, sans considérer les limites absolues comme régime normal. Vf/luminosité des LED, choix/brochage NPN, budget total et VDDIO2 de PG2 à vérifier avant intégration. Aucune LED parallèle considérée sûre par supposition. Les courants natifs sont donnés par formule et scénarios, **pas mesurés**.

Comparaison prix/montage détaillée : guides seuls4,14USD pour deux ensembles, six NPN1,02USD ; provisions fenêtre2–6EUR/boîtier, composants électriques2–5EUR/shield hors fenêtre/PCB/port. Prix fournisseurs datés, provisions explicitement non devis ; connecteurs et achats minimaux peuvent dominer le coût. Cible globale50EURlivrée toujours non démontrée. Aucun achat.

## Livrables et preuves

Sous `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/ETUDE_LED_V1_6/` :

- `NOTE_ARBITRAGE.md` : décision concise, comparaison, conditions et plan minimal futur. `ANALYSE_DETAILLEE.md` : géométrie, raccordements, courants, coûts et limites.
- `ETUDE_LED_V1_6.pdf` : trois pages plan coté, coupe projetée et schéma de principe ; sources éditables `01-plan.svg`, `02-coupe.svg`, `03-principe-electrique.svg`. **Ce sont des documents d’étude, pas des fichiers de fabrication.**
- `analyse.py` et `lire_pcb.py` : calculs et extraction native KiCad en lecture seule ; aucun SaveBoard. `SOURCES.md` : liens primaires, accès et reproductibilité.
- `preuves/pcb_natif_lecture_seule.json`, `impact_ouverture_hypothetique.json`, `optique_conditionnelle.json`, `electrique.json`, `connecteurs_ST.json`, `conservation.json`, `validation-calculs.json`, `pdf-relecture.json`, `validation-finale.json` : résultats effectifs et hypothèses.
- Archive de mission : contrat exact dans `archives/TR2-20261104-SHIELD-V16-ETUDE-LED/MISSION.md`, empreintes des19fichiers d’étude dans `ARTEFACTS.json`. Le rapport final est copié dans `RAPPORT.md` et la preuve distante dans `REMOTE_READBACK.json`.

## Validation réellement exécutée

| Contrôle | Résultat et portée |
|---|---|
|Python KiCad10 natif Windows : LoadBoard sans SaveBoard|191pastilles,396segments,2zones ; extraction seule, aucun ERC/DRC relancé.|
|Python/numpy/matplotlib existants : `analyse.py`|Succès ;1 232rayons sans intersection avec les enveloppes de scénario élargies, contrôles d’ouverture/bornes et calculs de courant. Pas une simulation photométrique ni une preuve sur C05 réelle.|
|UM2861Rev8 complet local, pages24/33/34/37|140contactsMorpho analysés ; absence des3GPIO dans les tablesZio vérifiée. SHA du manuel dans SOURCES et JSON.|
|PDF/SVG|Trois pages réouvertes/rendues et inspectées, trois SVG XML relus. Légende du plan déplacée pour ne pas masquer la fenêtre.|
|Conservation du shield|**232 fichiers antérieurs comparés octet pour octet** à6d97906d, SHA256/taille consignés ; PCB/schéma/Gerber/STEP/STL/boîtier existants conservés.|
|Périmètre Git/format|Diff indexé inspecté, `git diff --cached --check` final réussi ; seuls étude/contrat archivé/journal puis rapport/archive nouveaux concernés.|

Validation proportionnée : aucun build STM32, essai hôte firmware ou nouvelle campagne ERC/DRC/CAM ; sources correspondantes inchangées et preuves acquises réutilisées. Aucun accès STM32/GDB/COM/FRAM/microSD, flash, impression ou intervention physique.

Incidents documentés : curl Linux absent avant transfert ; une tentative Windows normale bornée12s échoue sans octet, alors que le schémaC05 est lisible via l’index web. **Aucun PDF C05 intégral ni placement CAO obtenu**, aucune inspection graphique C05 revendiquée. Le précontrôle Git a détecté les espaces de fin de ligne SVG Matplotlib et CRLF du JSON Windows : correction limitée à ces textes, tokens identiques, générateurs corrigés ; PDF/binaries hérités intacts. Aucun test ni contrôle affaibli.

## Conditions pour la réalisation ultérieure

Une vue dessus cotée des surfaces émettrices LD1/2/3 relativement à CN8-4 et un profil hauteur/obstacles nord dans l’empilage réel suffisent à trancher le trajet proposé, puis contrôle de lisibilité à travers la plaque depuis une position utilisable. **Aucune nouvelle manipulation demandée ici.** Ne pas redemander8,22/2,40mm CN12 ni15,2mm MKI. Si la visée échoue, mandat distinct pour les trois contacts et composants, placement/routage LED puis ERC/DRC/CAM/revue3D et qualification réelle. USER/RESET, fixations et autres gates V1.5 demeurent applicables. Aucun seuil/exigence normative implicitement modifié.

## Git, journal et publication

Étude évaluée et commitée dans **a526c1c162aa365c9701a2bcbc3f1c0bcddabcf6 — Study closed-case LED visibility and buffered Morpho fallback without CAD changes**. Worktree isolé `MSM_vibe-SHIELD-V16-ETUDE-LED`, branche locale `codex/shield-v16-etude-led`, cible autorisée `main`. Dépôt principal propre au HEADhistorique82169af7 préservé sans pull/merge/rebase/reset ; autres worktrees et stashes6251b988…/f3b829e6… intacts. Aucune divergence résolue automatiquement.

Journal `ETAT_COURANT_TR2.md` actualisé : distinction étude/qualification, proposition optique conditionnelle, signauxMorpho et repli, contrôles réussis, prochaines preuves requises ; aucune ancienne décision réécrite. À transmettre à ChatGPT : arbitrer la fenêtre après vérification minimale ou mandater le repli électrique, avant fabrication.

Permissions exercées : lecture, nouvelles pièces documentaires/calculs, journal et commits logiciels. Push normal vers main réussi : `6e9017ea1ed99375f8904fd47ee21a73fb3fac1e`, SHA confirmé par ls-remote et fetch ; relecture complète réussie sur260fichiers, manifeste des19artefacts vérifié. Worktree propre après cette publication. Cette clôture DONE concerne **l’étude conditionnelle**, pas une validation matérielle ; elle est publiée par un dernier commit/push normal puis relue. Le SHA final du commit contenant le rapport et l’état Git sont fournis dans la réponse après cette vérification, sans auto-référence dans le commit.
