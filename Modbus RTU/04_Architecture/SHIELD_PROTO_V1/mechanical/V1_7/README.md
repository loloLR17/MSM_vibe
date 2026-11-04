# V1.7 — modèles mécaniques d’étude, NON LIBÉRÉS

Reprise des contours/perçages et dimensions V1.5, du domaine documentaire V1.6 et des mesures utilisateur B1. **Ce dossier ne remplace pas la maquette V1.5 ni ses réserves.** Aucun fichier électronique ni ancien fichier de fabrication modifié.

- `parameters.json` : observations acquises10,2/≈0,4mm, hypothèses et emplacement candidat du vitrage latéral. Les centres réels C05 LD1/2/3 et B1 restent **null**. G11mm reste hypothétique.
- `REVIEW_ONLY/v17-study.step` : sous-assemblage de principe dérivé V1.5, avec contour/perçages du shield, contour Nucleo de référence, enveloppes MKI/nappe/retenue/ST-LINK, fenêtre et mécanisme USER **sur axe d’illustration**. Il ne reprend pas les67objets de la maquette complète : FRAM/SD/Click et autres détails non concernés restent dans V1.5 et ne sont pas prétendus intégrés ici. Aucun faux composant LED ou B1 placé.
- `window-principle.step` : paroi locale percée de principe, plaque transparente, cadre et joint. Cette paroi locale n’est pas une modification de la base V1.5. Quatre fixations sont réservées, sans quincaillerie achetée.
- `user-principle.step` : panneau local, guide en deux demi-coques pour assemblage autour de la bride captive, tige/tête et patin. Deux guidages, butées haute/basse et réserve de ressort. **Les ressorts, réglage fileté, vis de cage et pointe élastique ne sont pas des détails de fabrication finis.** Le cylindre rouge est une enveloppe de ressort, pas un ressort à raideur certifiée. Rappel indépendant à définir. Ne pas prendre le modèle rigide pour une simulation élastique.
- `v17-vues-coupes.pdf` / `vue-1..4.svg` : vue3D du sous-assemblage, coupe latérale, chaîne USER et face nord/plan des rayons. Millimètres ; X CAD=X KiCad, Y CAD=−Y KiCad, Z0=shieldtop. Les projections n’établissent pas seules l’absence de collision.

Les STEP sont éditables dans un modeleur solide et régénérables avec `tools/study_v17.py` et ce JSON. La chaîne de hauteur lit G/épaisseur/mesure et jeu dans le JSON ; les coordonnées de construction des pièces et des figures sont aussi des constantes éditables dans le script : **ce n’est pas un configurateur où toute modification du JSON reconstruit automatiquement tous les détails**. Adapter et vérifier les deux sources ensemble. Aucune dépendance nouvelle installée : environnement existant CadQuery2.8/numpy/matplotlib dans `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`. Depuis la racine du dépôt :

```
/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python 'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/tools/study_v17.py'
```

Le script écrit exclusivement V1.7 et `docs/ETUDE_VITRAGE_USER_V1_7/`, ne lance pas le générateur V1.5, n’accède à aucun matériel, rouvre les STEP et contrôle les intersections de mécanisme aux extrêmes. **Aucun STL exporté**, car l’ajustement et les efforts ne sont pas qualifiés. La description axiale est un principe ajustable, pas un kit imprimable fonctionnel.

Sources, décision et checklist : [note V1.7](../../docs/ETUDE_VITRAGE_USER_V1_7/NOTE_DECISION.md). Les observations de l’utilisateur sont distinguées des calculs, les réserves C05/J5/Zio/USER restent avant commande. **NE PAS COMMANDER NI IMPRIMER pour exploitation sur la seule base de ce dossier.**
