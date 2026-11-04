# Revue indépendante ChatGPT — V1.7 vitrage ST-LINK / USER

Date : 2026-11-04. Dépôt `loloLR17/MSM_vibe`, branche `main`. Mission `TR2-20261104-SHIELD-V17-VITRAGE-USER`. Sources évaluées : `80ec70effab4a12709c988401e19de8d8294be30`. Rapport final Codex : `07b1cfa16ce8680ce136d4efd89acd3f85bdcff9`, statut **DONE pour étude documentaire/CAO uniquement**.

## Contrôles indépendants effectivement réalisés

- Lecture sur GitHub de la gouvernance, mission, rapport, commits et fichiers `docs/ETUDE_VITRAGE_USER_V1_7/NOTE_DECISION.md`, `CHECKLIST.md`, `optique.json`, `chaine_USER.json`, `interfaces-nord.json`, `validation-finale.json`, `mechanical/V1_7/README.md` et `parameters.json`. Examen des objets commits : création du dossier d'étude V1.7 sans modification du PCB.
- Selon les preuves Codex publiées : les fichiers STEP3, PDF quatre pages et SVG4 ont été générés et relus ; `251` anciens fichiers du shield déclarés identiques octet pour octet ; `282` fichiers déclarés relus après push `2e7bccdba8c5581be8b830af753f596a472e3c44`. Ces validations ont été **contrôlées par lecture des preuves**, pas réexécutées localement par ChatGPT ; aucun essai physique réalisé.
- Cohérence de la mise à jour utilisateur : visibilité **presque horizontale dans le plan de la Nucleo depuis l'extrémité ST-LINK**, pas visée verticale ; hauteur USER au repos `10,2 mm` et course observée jusqu'au clic `0,4 mm`. Ne pas confondre photo montrant des LED avec identification certifiée des **trois** LD1/LD2/LD3 sur l'empilage équipé.

## Verdict technique

**Vitrage latéral : piste prioritaire proportionnée**, pas validation. La proposition vise une ouverture utile **16×8 mm** dans la paroi côté ST-LINK, une plaque PC **1,5 mm**, cadre démontable **26×18 mm** et joint. Les `3564` rayons sont issus de `297` points **hypothétiques** et `12` observateurs, dont `976` passages non obstrués sur l'enveloppe nominale majorée de1,5 mm. La plupart des rayons ne passent donc pas dans cette fenêtre, et **aucun point d'observation unique ne voit la totalité du domaine hypothétique**. Ce dénombrement ne peut prouver la visibilité simultanée de trois LED à leurs vraies positions, ni l'absence de conflit avec prise ST-LINK branchée, nappe ou composants réels. Les dégagements `2 mm` entre fenêtre et inserts sont nominaux, donc à vérifier avec une vraie fiche/câble. Aucun indice imposant aujourd'hui une découpe du shield, mais non-exclusion d'une gêne physique.

**USER : progrès paramétrique, effort non qualifié.** Le modèle introduit repose0,6 mm, déclenchement0,4 mm, déplacement extérieur1,6 mm et réglage axial±2 mm. Ces valeurs sont des **propositions d'étude**, pas caractéristiques garanties. La force et la course maximale sûres du vrai B1 `SW_BLUE/BLUEHAT` ne sont pas connues ; le ressort de la pointe et la force admissible restent inconnus. Le dessin `user-principle.step` ne constitue pas une pièce imprimable sûre. Des tolérances, un réglage sans précharge et une butée reprenant l'excès d'effort par le coffret doivent être démontrés ; éviter de surconcevoir le prototype si un poussoir simple avec patin souple et jeu maîtrisé s'avère suffisant.

**Maturité :** travail V1.7 conforme à sa mission d'étude ciblée, sans faux résultat matériel. **BLOCKED_FABRICATION / NE PAS COMMANDER** conservé pour shield et boîtier : vérifier les trois LED dans une même position de vision réelle, le passage de l'USB ST-LINK et de la nappe, la position/hauteur USER, puis les autres contrôles C05/J5/Zio/modules. Le volet approvisionnement final devra ajouter **stock, délai réel livré en France et prix rendu TTC**, pas seulement coûts hors livraison. Aucune mission nouvelle, achat ou modification de firmware n'est autorisée par cette revue.

## Contrôle utilisateur minimal à guider

1. Sur TR2-001, confirmer **séparément** que les trois LED LD1 verte/LD2 bleue/LD3 rouge sont visibles depuis **un même point de vue latéral côté ST-LINK**, sans confondre les LED ST-LINK, avec le port de maintenance accessible.
2. Mesurer/observer ensuite la visibilité **avec gabarit opaque du shield au niveau d'empilage réel**, module MKI et nappe simulés/confrontés sans poser ou appuyer sur les composants ; pas de nouvelles mesures de course0,4mm/hauteur10,2mm déjà acquises.
3. Ajuster sur le vrai centre B1 le poussoir et vérifier plus tard son appui/restitution sans précharge ni effort excessif ; ne pas rechercher la butée interne du bouton.
4. Avant commande, rassembler gabarit 1:1 sur les deux C05, hauteur naturelle G, fixation/volumes MKI/Click et accès USB/microSD/RS485/nappe, puis faire une revue de libération regroupée. Le boîtier conceptuel et les STEP d'étude ne sont pas une validation de pièces à imprimer.
