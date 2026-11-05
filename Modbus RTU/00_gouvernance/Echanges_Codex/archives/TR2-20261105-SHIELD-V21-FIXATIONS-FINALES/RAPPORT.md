---
mission_id: TR2-20261105-SHIELD-V21-FIXATIONS-FINALES
status: DONE
base_ref: main
base_sha: c23f5e0109d53a0aaad984fcff16399985b6ab24
initial_head: e8f240ba766ed5ce506b0c467772490ee0d589ad
result_sha: ea1ce5507d9386d7dfa4014d1c26162bb539c280
created_at_utc: 2026-11-05T12:52:44.429738+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.1 — candidat D126 dimensionné, qualification et approvisionnement incomplets

**Solution retenue pour un premier prototype : trois supports isolants en demi-couronne D126, G=12,25±0,10 mm, vis nylon M2×20 et deux écrous nylon contrebloqués au-dessus du shield.** Le résultat est un GO géométrique sous bornes. La résistance, la fabrication et l’approvisionnement des supports restent bloqués ; la fixation n’est pas définitivement qualifiée et les PCB restent non libérés.

[Décision](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/DECISION.md), [montage futur](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/ASSEMBLAGE.md), [approvisionnement](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/APPROVISIONNEMENT.md), [plan et coupe](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/cad/plan-et-coupe.pdf). STEP, SCAD, STL de revue, scripts et preuves sont dans `V2_1_FIXATIONS/`.

## Choix et résultats

Trois options ont été comparées : tube mince standard et cale, profil ouvert, reprise sur châssis. D126 conserve davantage de matière tout en dégageant le côté Zio : enveloppe Ø6, alésage Ø2,4, ligament radial1,8, section11,875 mm². Il s’ouvre vers CN15 au point haut et vers ST-LINK aux deux points bas, avec orientation à maintenir dans±5°.

Le calcul inclut désormais le déplacement libre réel du support sur la vis, borné à1,015 mm, et non une simple position imposée à0,15. Après budgets distincts de longueur du socket, bavure0,38, position ancrage/socket, forme et rotation, la marge calculée vaut **0,320338 mm**, au-dessus de la garde0,20. Toutes les bornes et leurs limites sont explicites dans `geometrie-et-limites.json` ; elles ne sont pas des mesures certifiées des pièces réelles.

**Neuf cas de supports contre les1 060 solides ST : aucune collision.** Un premier profil à méplat1 mm croisait deux solides près du point haut : rejet conservé. La visserie nominale ne montre aucune collision imprévue. Trois faibles recouvrements aux portées tête/PCB —4,42 nm d’étendue,1,458e-5 mm³— sont conservés et classés comme contacts numériques intentionnels par leur emplacement/profondeur ; aucun seuil global n’a été relevé.

Les têtesØ3,8 sont sous la Nucleo ; les écrous de clé4 ont des coinsØ4,619 et sont au-dessus du shield. Aucune rondelle large ni vis métallique au contact du cuivre. Le stack nominal18,65 mm laisse1,35 mm de vis après les écrous. La décision contient les cotes et bornes d’acceptation, sans les présenter comme mesures d’un lot.

À5 N, la contrainte moyenne serait0,421 MPa sur la section et≈1,47 MPa sur la demi-portée. **Ce n’est pas une capacité admissible ni un couple de serrage.** Excentricité, impression, fluage, température, maintien angulaire et vibrations exigent une qualification. Aucun serrage compensatoire ou montage forcé n’est recommandé.

## Approvisionnement et réserves

Les packs de25 vis DIN84 PA M2×20 `84PA220_25` et25 écrous DIN934 PA M2 `934PA2_9451` sont affichés disponibles chez SAM/ScrewsAndMore, à5,39 € chacun. Besoin avec réserve :8 vis et16 écrous. Port européen14,90 € TTC19% ; **25,68 € affichés, environ25,90 € budgétés à TVA20%**. Les pages annoncent1–3 jours, sans confirmation spécifique France ni quantité réservée. Les liens sont publiés, aucun achat effectué.

**Le prix livré et le délai des huit supports spéciaux ne sont pas obtenus.** PETG dense local est proposé pour un prototype, PA66 usiné/PA12 SLS comme alternatives non qualifiées. Le coût de matière brute n’est pas confondu avec un devis d’impression : le total rendu complet reste `null`. L’exigence d’une fixation immédiatement approvisionnable et définitivement robuste n’est donc pas déclarée satisfaite sans réserve.

G/J5, LED, USER, câbles et isolation restent des portes séparées. Un seul contrôle froid du premier exemplaire est décrit, suivi d’une qualification de tenue sous autorisations adaptées. Aucun geste physique n’est demandé maintenant, aucun nouveau papier. Cinq PCB nus peuvent seulement être envisagés comme prototypes à risque après revue indépendante et décision financière humaine distincte ; aucune commande ni libération ici.

## Validation et traçabilité

CadQuery/OCCT : `final_model.py`, `fasteners.py`, `views.py`, `validate.py`. Trois STEP rouverts valides, STL fermé à volume positif, PDF rouvert et texte contrôlé, JSON/Python parsés. Résultats et échec initial conservés dans `preuves/validation-finale.json` et les preuves associées. Aucun test historique affaibli.

Code évalué : **ea1ce5507d9386d7dfa4014d1c26162bb539c280**, «Dimension V21 D126 isolation supports and verify mounting clearance cases». HEAD initiale8f240ba766ed5ce506b0c467772490ee0d589ad, base contractuellec23f5e01. Worktree isolé `MSM_vibe-SHIELD-V21-FIXATIONS`, branche `codex/shield-v21-fixations`, ciblemain. **464 fichiers historiques du shield identiques**, aucun PCB, schéma, routage ou Gerber modifié ; V1.9 et campagnes antérieures préservés sans répétition d’ERC/DRC ou firmware.

Journal `ETAT_COURANT_TR2.md` actualisé avec le candidat, les marges, les essais, les réserves physiques et le devis incomplet. Dépôt principal propre82169af7, deux stashes historiques et autres worktrees préservés. Permissions exercées : CAO, calculs, documentation, journal, commits et push normaux ; aucun flash/debug/COM/média, achat, impression ou changement de sécurité.

Publication normale sur main **dc17c9ea9f1fada0233b41e41c89405d5ebd38b5** réussie, confirmée par fetch et ls-remote. **493 fichiers relus intégralement depuis origin/main et comparés octet pour octet**, couvrant livrables, preuves, historique du shield, journal, contrat et gouvernance. Preuve `REMOTE_READBACK.json` dans l’archive V2.1. Worktree propre après ce push. Cette clôture et son archive sont publiées par un dernier commit/push normal puis relues ; SHA final communiqué après contrôle. **DONE signifie étude livrée avec réserves, pas fixation physiquement qualifiée ni READY_TO_ORDER.**
