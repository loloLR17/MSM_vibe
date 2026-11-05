---
mission_id: TR2-20261105-SHIELD-V19-C05-OFFICIAL-CAD
status: READY
base_ref: main
base_sha: 63451b2b4f87a1be0547406d76877b60114bdb20
created_at_utc: 2026-11-05T11:40:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — V1.9 : corriger les positions du shield à partir de la CAO officielle Nucleo MB1549 C05

## Fait déclencheur — anomalie physique réelle

Le 05/11/2026, l'utilisateur indique avoir fait imprimer les gabarits V1.8 et vérifié **la barre d'étalonnage : exactement 100 mm**. **Certains connecteurs paraissent décalés de plusieurs millimètres** par rapport à la NUCLEO-U575ZI-Q MB1549 **C05** réelle. Les écarts précis et leurs axes ne sont pas encore identifiés et aucune correction relative n'est autorisée sans référence métrique. Ce constat **invalide l'hypothèse que l'implantation actuelle du shield soit mécaniquement suffisamment vérifiée**. Maintenir **FABRICATION NO-GO**, aucun achat/commande.

La conception V1.4 et les revues V1.5–V1.8 sont préservées. ERC0/DRC0 ne démontrent pas l'alignement sur la carte externe. `docs/comparaison-C05.csv` ne contient **aucune coordonnée ST vérifiée** ; elles ont été reconstituées depuis UM2861Rev4/Rev8. Le dernier rapport V1.8 est clos par ChatGPT dans `archives/TR2-20261104-SHIELD-V18-REVUE-PRECOMMANDE/CLOTURE_APRES_CONSTAT_ECARTS_C05.md`.

## Objectif ferme

**Obtenir les fichiers exacts du circuit Nucleo MB1549-U575ZIQ-C05 auprès de ST, en extraire les coordonnées réelles des connecteurs et trous, confronter au PCB TR2, puis corriger le shield au minimum nécessaire avec validation électrique et fabrication reproductible.** Ne pas demander à l'utilisateur des mesures manuelles de toutes les broches : rechercher d'abord la CAO et les Gerber professionnels du constructeur.

## Sources officielles vérifiées comme annoncées par ST, téléchargement à faire et qualifier

- Page produit : https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html (rubrique **Board Manufacturing Specifications**).
- **MB1549 Board design project files**, v4.0, 25/01/2024, ZIP annoncé.
- **MB1549 Manufacturing files**, v3.4, 24/02/2025, ZIP annoncé.
- **MB1549-U575ZIQ-C05 Board Schematic**, PDF : https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf.
- UM2861 Rev10 distingue bien les révisions C02/C03 et C05 ; **ne pas assimiler une autre révision à C05**.
- Existence des téléchargements confirmée par ChatGPT via site ST le 05/11 ; **contenu des ZIP NON VÉRIFIÉ** et fichiers non récupérés par ChatGPT. Tout lien/acquis doit être testé réellement par Codex, de manière proportionnée. Respecter la licence des documents ST et ne pas committer aveuglément une grosse archive tierce, binaire ou soumise à restrictions.

## Travaux Codex attendus (autonomie maximale)

1. Chercher et télécharger **réellement** les ressources CAO et manufacturing ST (environnement/outils accessibles), documenter URLs, versions, date, empreintes, structure et **révision des fichiers réellement contenus**. Si le téléchargement ST échoue, rechercher des miroirs **officiels ST** ou les distributions constructeur autorisées, vérifier les provenances et ne pas abandonner à la première erreur HTTP. Si toutes les sources métriques exactes sont inaccessibles, arrêter au statut `BLOCKED` avec un unique besoin d'aide ; ne pas inventer des données C05.
2. Extraire les centres **X/Y/angle/ordre de broches** et trous de fixation de **CN7/8/9/10, CN12 mâles 66/67/68/69**, ainsi que les contours/éléments C05 utiles aux collisions. Reconcilier repères, origine, axes, orientation, face dessus/dessous, unités et révisions ; comparer d'abord les distances relatives des centres et l'entraxe des rangées au shield, avant translation/rotation globale. Conserver un tableau de deltas **ST C05 réel / KiCad actuel**, par connecteur et perçage, accompagné de preuve source. Déterminer si l'écart réel est une translation rigide du gabarit, une erreur d'origine, un placement spécifique d'un ou plusieurs connecteurs, un miroir, une différence de révision ou une autre cause. **Ne pas présumer que toutes les positions sont fausses.**
3. Corriger la géométrie KiCad seulement après identification objective. **Préserver le schéma, la fonction et le routage existants autant que possible**, adapter empreintes/traces/contours/fixations au strict nécessaire, en vérifiant également J5 (quatre mâles quinconce et seul CN12-69/PG4 = RTU_DE), modules MKI/Click, microSD, FRAM, périphériques et passages mécaniques. Éviter de déplacer automatiquement les zones de modules si la correction est un simple recalage de repères. Garder versions antérieures et leurs CAM non libérés ; ne pas changer firmware. Si une refonte mécanique substantielle est indispensable, expliciter l'impact et les choix sans improviser une architecture nouvelle.
4. Régénérer ERC/DRC/parité, netlist, CAM Gerber/Excellon, manifeste/hash et **gabarits 1:1 corrigés** avec barres X/Y100mm. Préparer si utile un **contrôle sans réimpression** via le relevé de coordonnées depuis la source ST et l'observation utilisateur existante, ou un gabarit digital avec cotes, tout en prévoyant une ultime vérification physique ciblée de la révision corrigée **avant commande** ; ne pas prétendre que l'ancien papier certifie le nouveau PCB. Pour l'utilisateur, fournir un seul protocole concis, sans lui transférer la conception ni lui demander de dessiner les deltas manuellement.
5. Qualifier séparément les portes **XY** et **Z/J5/G naturel**, les accès USB-C CN15, ST-LINK, SD, RS485, nappe, fenêtre latérale LED, couloir USER. Les données déjà acquises **CN12 pointe8,22/corps2,40 ; MKI entraxe15,2 ; USER hauteur10,2/course0,4** restent valides. Le choix bouton USER très simple et vitrage polycarbonate latéral sont préservés. Les contrôles purement liés au boîtier peuvent attendre s'ils ne changent pas le PCB.
6. Publier la comparaison, provenance et corrections, gabarits, décisions restantes, validations et journal d'état sur `main` ; produire `DERNIER_RAPPORT.md` pour `TR2-20261105-SHIELD-V19-C05-OFFICIAL-CAD`, avec SHA exact, état technique, vérifications et limites, puis relire le résultat distant. Rapport `DONE` seulement si les livrables d'analyse/correction demandés sont effectivement publiés et relus ; **ne jamais déclarer READY_TO_ORDER sans nouveau contrôle physique approprié**.

## Autorisations / arrêt

`allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Pas de flash/debug, pas de manipulation STM32, pas d'achat ni d'impression par Codex, pas d'effacement, pas de force push, pas de désactivation de protections ni modification d'Option Bytes. Respecter l'état courant `main`, `AGENTS.md`, gouvernance, archives et principe A-06 d'amélioration incrémentale. Aucune demande à l'utilisateur sauf véritable nécessité de donnée physique non présente et non récupérable autrement.
