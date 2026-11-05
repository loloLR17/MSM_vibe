---
mission_id: TR2-20261105-SHIELD-V22-TME-CLIPS
status: IN_PROGRESS
base_ref: main
base_sha: 5c29ce62f2026993d24fd53e3ca926131a74d3fb
initial_head: 6542a4a5fdc970f3467658cc151e710677633d6f
result_sha: null
created_at_utc: 2026-11-05T16:18:02Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.2 — étude TME/visserie et clips

**Étude réalisée ; publication et relecture distante encore en cours.** Les réserves sont localisées et n'empêchent pas de livrer l'étude. Aucun achat ni libération de PCB. La cible historique d'environ 50 € pour deux montages avec cinq PCB et mécanique n'est pas démontrée par les offres disponibles.

## Décision proposée à ChatGPT

- **Conserver J5 Samtec `SSW-103-01-F-D`. NO-GO substitution Connfly** : profondeur de contact utile non documentée, même si le corps mesure aussi8,5mm. Aucun emprunt des limites Samtec pour approuver Connfly.
- **Tout-TME non réalisable démontré aujourd'hui** : femelle1×9 Connfly affichée sans stock et réapprovisionnement non daté. Le prix théorique18,81€TTC livré ne qualifie pas un panier disponible ou compatible.
- **Repli à deux fournisseurs** : TME + DigiKey, J5 exact et quatre Harwin1×9 regroupés chez DigiKey. Budget électronique livré49,88–54,88€TTC avec AVX proposé, **50,25–55,25€ en conservant le Kemet C1 actif**. Les variantes Harwin/C2 restent conditionnelles au premier assemblage. L'AVX économise seulement0,37€ ; ses petits écarts Fab/courtyard sont signalés, pas corrigés silencieusement.
- **Clips simples non adoptés ; D126 conservé**. Une capture rigide2mm laisse jusqu'à1,12mm de jeu axial cumulé sur PCBmin1,44 : G et engagement J5 ne sont plus garantis. Ce n'est pas une impossibilité universelle des clips ; un rattrapage élastique et sa qualification ajouteraient une conception hors de cette étude courte. La référence Essentra examinée ne convient pas aux trous/épaisseurs/G. Pas de modèle arbitraire ni STL de clip produit.
- **Ajile : offre exacte économique, compatibilité partielle**. VCY120 M2×20 +UHX001 M2, PA6.6, MOQ1 vérifiés dans les variantes sélectionnées. Pour8vis/16écrous :9,12€TTC de pièces, **13,80€ livré sans suivi ou16,20€ suivi**. Écrou AF4/H1,6 coté, tête de visØ/hauteur non cotée : interchangeabilité complète non démontrée. Aucun kit Amazon qualifié.

Le scénario électronique disponible +Ajile sans suivi +PCB lent budgété donne déjà77,18–82,18€ avant les postes inconnus. Le total complet reste `null` : D126 réellement fabriqués, retenues/visserie modules, coffrets/vitrage, réemploi manquant et facture finale PCB/import ne sont pas chiffrés intégralement. Pas de budget incomplet présenté comme coût de deux TR2 opérationnels.

## Livrables et preuves

Dossier [V2_2_ECONOMIE sur GitHub](https://github.com/loloLR17/MSM_vibe/tree/main/Modbus%20RTU/04_Architecture/SHIELD_PROTO_V1/V2_2_ECONOMIE/) :

- `README.md` : décision à un ou deux fournisseurs, coûts séparés, inconnus et contrôle unique à froid.
- `BOM_2_PROTOTYPES.csv`, `POSTES_RESTANTS.csv`, `sources.json`, `budget-TTC.json`, `parametres.json` : variantes, quantités/MOQ, prix/devis partiels, transports et sources datées.
- `TECHNIQUE_J5_PASSIFS.md` : contact J5, NC/pinout, empreintes, queues, C1 AVX/Kemet et limites d'intégration.
- `CLIPS_VS_D126.md`, `principe-clips.svg` : contre-exemple axial, hypothèses FFF et comparaison au clip fabricant/boîtier. SVG éditable de principe, sans échelle et non fabricable.
- `VISSERIE_GRAND_PUBLIC.md`, `preuves/ajile-variantes-extraites.json` : variantes exactes, images cotées consultées, coûts de 8/16 et limite de tête.
- `tools/verify_study.py` et `preuves/` : calculs reproductibles, compatibilité nominale, manifeste des486 fichiers antérieurs, contrôles et étatGit initial.

Les HTML/JSON complets Ajile et images restent horsGit dans `/home/lolo/dev/msm/reports/TR2-shield-v22-20261105/`. Le dossier versionné contient références/liens, faits extraits et SHA256 des artefacts locaux, sans redistribuer intégralement les documents constructeurs protégés. Mission copiée sans modification dans `archives/TR2-20261105-SHIELD-V22-TME-CLIPS/MISSION.md`. Rapport V2.1 déjà archivé à l'identique avant remplacement du fichier actif.

## Contrôles réellement exécutés et portée

`python3 'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_2_ECONOMIE/tools/verify_study.py'` : **PASS pour étude/documentation seulement**. CalculsDecimal de conversion et TVA, contrôle prix/MOQ/quantités, relecture JSON/CSV/Python/SVG et conservation exacte des486 fichiers antérieurs à partir du HEAD initial. Résultats dans `preuves/validation.log` et `preuves/validation-finale.json` ; contre-exemple dans `preuves/calcul-clips.json`.

Lecture du natifPCB et du readback V1.9 : J5 local 5 seul relié à RTU_DE, cinq NC ; C1 pas2,54/perçage0,8. PreuvesST/C05, 106 centres, tolérances et collisions V2.0/V2.1 réutilisées **sans refaire les validations acquises**. Aucun ERC/DRC/build/flash/debug/accèsCOM/SD ni essai physique : ces opérations n'apporteraient pas de preuve supplémentaire pour une étude sans modification électrique ou firmware. Aucun ancien fichier du shield/KiCad/Gerber/CAM ou campagne physique modifié.

Consultations marchandes en lecture seule, aucun panier, compte, paiement, envoiGerber ou message commercial. Les refus403 des téléchargements locaux TME ont été conservés ; les documents étaient lisibles via outilWeb. Pas de contournement. Pages France Farnell/Mouser et page Amazon non vérifiables ; aucune économie de livraison inventée à partir d'un listing étranger.

Les stocks sont affichés/non réservés. PrixTME en USD HT convertis au taux[BCE 05/11](https://www.ecb.europa.eu/stats/policy_and_exchange_rates/euro_reference_exchange_rates/html/eurofxref-graph-usd.en.html)1 EUR = 1,1224 USD puis TVA 20 % ; DigiKey en EUR HT. Port TME7,08€TTC compté une fois ; port DigiKey affiché25€, budget25–30TTC tant que traitement TVA non résolu. Aucun devis de commande accepté. PCB : calculateurV1.8 du 04/11 réutilisé, pas nouvelle consultation ni facture TTC finale.

## Journal, limites et prochaine décision

`ETAT_COURANT_TR2.md` actualisé par ajout d'une entrée V2.2 : décision J5, stock 1×9, budget, refus local des clips, offres Ajile et tête manquante. Aucune règle de gouvernance changée.

À transmettre à ChatGPT : accepter l'étude avec ses NO-GO locaux, vérifier le stock personnel exact avant dépense, privilégier les regroupements, puis lever la cote de tête Ajile par source fournisseur ou contrôle du lot à froid. Pas de nouvelle impression papier. Au premier article : G/parallelisme/engagements sans effort, orientation et maintien D126, queues/courts-circuits, isolement et accès. Le risque prototype circonscrit accepté reste distinct d'une qualification de fluage/vibrations. Toute décision financière/fabrication reste humaine et les Gerber **NON LIBÉRÉS**.

## Git et publication

Worktree neuf `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V22-ECONOMIE`, branche locale `codex/shield-v22-economie`, basé sur le HEAD réel `6542a4a5fdc970f3467658cc151e710677633d6f`. Le dépôt principal est conservé propre à `82169af7e76797731358852bec83bfb0ceb54ccf`, sans reset/rebase/merge. Les113 worktrees observés après création et les deux stashes historiques sont préservés ; SHA dans `preuves/etat-git-initial.json`.

Permissions : lecture/développement documentaire et calculs exercés ; commit et push normaux vers `refs/heads/main` à effectuer après contrôles. Aucun flash/debug ni opération physique autorisé ou exécuté. `result_sha` reste provisoirement null avant attribution du commit d'étude ; il sera renseigné après sa relecture distante. Publication initiale puis clôture DONE prévues conformément au protocole V2.
