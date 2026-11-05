---
mission_id: TR2-20261105-SHIELD-V21-FIXATIONS-FINALES
status: READY
base_ref: main
base_sha: c23f5e0109d53a0aaad984fcff16399985b6ab24
created_at_utc: 2026-11-05T13:40:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V2.1 : solution de fixation simple et robuste, avant commande du shield

## Objectif

Lever **le dernier risque mécanique prioritaire clairement identifié** par V2.0 : fixation entre la NUCLEO-U575ZI-Q MB1549 C05 et le shield V1.9 corrigé, à la hauteur nominale **G≈12,25 mm** (intervalle **conditionnel** 11,92..12,57). Établir une **solution concrète, dimensionnée, compatible et approvisionnable**, avec un verdict GO/NO-GO justifié pour le risque d'entretoises, sans refonte générale ni nouvelle impression papier par défaut.

Lire le HEAD réel de main, `AGENTS.md`, gouvernance, protocole, l'état courant, dernier rapport V2.0 et `archives/TR2-20261105-SHIELD-V20-EMBOITEMENT-3D/REVUE_CHATGPT_INDEPENDANTE.md` ainsi que les preuves V1.9/V2.0 et les STEP/sources fabricant. Respecter la clôture documentaire V2.0 conservée dans `archives/.../CLOTURE_AVANT_FIXATIONS.md`.

## Ce qui est acquis, à préserver

- 106 centres X/Y shield recalés sur Altium/ODB++ officiels ST C05, ERC/DRC/parité à0 ; KiCad et CAM V1.9 candidats **non libérés**. Aucune deuxième impression papier exigée par principe.
- J5 Samtec `SSW-103-01-F-D` 2×3 sous shield, quatre broches mâles CN12 66/67/68/69 réellement implantées, CN12-69/PG4 seul signal `RTU_DE`. Pointe8,22mm depuisPCB, corps2,40mm acquis. Zio mâles Connfly modèle legacy précis ; sockets ST C05 en STEP officiel. Pour G12,25 sous bornes V2.0 : insertion J5 4,15..4,81mm, Zio 4,00..5,72mm ; jeux de corps J5 ≥1,01mm et Zio ≥0,63mm.
- Trois points d'ancrage shield↔Nucleo, trous shields Ø2,8mm et Nucleo Ø3,2mm. Ø4mm entretoise circulaire : jeu nominal0,285mm, pire cas−0,04 ; Ø3,5 : nominal0,535, pire cas+0,235 hors bavure ; bavure max constructeur 0,38 conduit à −0,145 dans scénario défavorable. **Aucun de ces scénarios n'est libéré**. L'ancien G11 n'est pas robuste.
- Modules réemployés, fixations/retentions MKI/Click, isolation côté RS485, CN15 et ST-LINK accessibles, extraction SD, nappe, fenêtre transparente PC latérale et USER par accès protégé **très simple** : préserver sans ajouter de pièces coûteuses.

## Travail à réaliser maintenant

1. **Reprendre les vrais volumes et ancrages depuis ST C05 et shield V1.9.** Identifier précisément quel trou/axe, quelle portion de corps Zio, quels plastiques/bavures et quelles têtes/rondelles créent la gêne ; distinguer l'impact réel de la tolérance du constructeur de celui de la position du support. Recalculer la marge avec hypothèses clairement tracées, sans addition incohérente de tolérances.
2. **Comparer 2 ou 3 solutions réalisables et sobres**, par exemple entretoise nylon mince à géométrie contrôlée, support à méplat local ou profil non cylindrique, tube isolant + vis/écrou et rondelle appropriés, ou fixation déportée sur châssis du boîtier **si c'est plus simple et mécanique sûre**. Ne pas compromettre le serrage, l'isolement, l'engagement J1–J5, le routage/cuivre ou la tenue vibratoire ; ne pas supposer que Ø3,5 ou une pièce imprimée très fine est automatiquement solide. **Ne pas modifier le PCB ou la Nucleo** sans prouver que toute solution hors cuivre est impossible ; si un changement minime du PCB est absolument indispensable, le signaler et quantifier d'abord, sans exécuter une refonte implicite.
3. **Sélectionner un candidat prioritaire reproductible et facile à monter**, détailler diamètre/encombrement, longueur liée à G (y compris empilement des surfaces), matériaux isolants, type et longueur de vis/écrous, perçages/tête/rondelles et un ordre de montage hors tension. Si un profil spécial imprimé3D est recommandé, produire une pièce simple éditable et un STL de revue **non libéré**, expliquer tolérances d'impression et résistance, et le comparer à une solution standard. Tester les interférences 3D et justifier les marges, y compris les cas limites documentés.
4. **Vérifier les références et l'approvisionnement réellement disponibles** : options compatibles, quantité pour2prototypes+réserve raisonnable, prix total livré en France TTC, stock et **délai de livraison**, frais de port et nombre de fournisseurs. Préférer composant standard/groupé aux frais de port disproportionnés, et livraison raisonnable à une économie dérisoire sur plusieurs mois. Mentionner explicitement les prix/stocks non vérifiables, sans inventer de devis ni acheter.
5. **Rédiger une fiche d'assemblage très concise et un verdict clair** `GO géométrique sous hypothèses / BLOCKED physique / NO-GO fabrication` selon preuve, en séparant résolution de la fixation et réserve d'emboîtement réel J5/G. Dire si la fabrication des 5 PCB nus peut raisonnablement être envisagée comme **lot de prototypes à risque circonscrit**, sans l'autoriser, et préciser le contrôle froid du premier exemplaire. Aucun retour utilisateur nécessaire sauf décision structurelle ou contrôle matériel indispensable précisément justifié. Pas de nouvelle impression par principe.
6. Publier les sources, modèles/coupes, preuves et comparaison dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/`, actualiser seulement les constats factuels utiles, puis publier `DERNIER_RAPPORT.md` avec `mission_id: TR2-20261105-SHIELD-V21-FIXATIONS-FINALES`, statut honnête et SHA distant vérifié. **Ne pas confondre `DONE` de l'étude avec libération du PCB.**

## Conditions / limites

Permis : développement, CAO, docs, tests logiciels, commits et push normaux sur main. Interdits : flash/debug STM32, gestes physiques, achat, paiement, commande, impression, modification de sécurité/Option Bytes, effacement massif, force push. Préserver l'historique V1.4–V2.0 et CAM précédents. Appliquer A-06 : **pas de sophistication disproportionnée, pas de nouvelle campagne d'essais inutiles**, rigueur des preuves inchangée. La libération de fabrication et les décisions d'engagement financier restent du ressort de l'utilisateur après revue indépendante ChatGPT.
