# V2.4 — dossier d’impression de deux prototypes ARNACO

**Candidat d’impression partiel, géométrie numérique contrôlée ; aucun assemblage physique qualifié.** Le ZIP opérateur contient les deux caisses, deux couvercles ARNACO, panneaux, trappes, brides, huit D126 et douze rondelles d’appui réglables. Les deux types d’étriers MKI/Click sont exclus : leurs zones de contact réelles restent à vérifier. Il manque donc quatre retenues pour parler de deux kits complets. Cette réserve locale est permise par le contrat ; elle ne doit pas être perdue lors du transfert à l’imprimeur.

- `pack_imprimeur/TR2_V24_2_PROTOTYPES_PARTIEL.zip` : **unique transfert candidat**, sans STEP tiers, variantes ni fichiers provisoires. Aucun envoi effectué.
- `sources/` : générateur CadQuery, contrôles reproductibles, paramètres et coordonnées repris du PCB V2.3 commandé.
- `cad/` : STEP éditables en coordonnées d’assemblage, plus ensemble de **nos pièces uniquement**. Deux étriers réservés présents pour étude, pas à imprimer.
- `stl/PRET_A_IMPRIMER/` : STL orientés, unités **mm**, quantités dans le ZIP ; aucune mise à l’échelle.
- `stl/A_VALIDER_SUR_MATERIEL/` : deux modèles exclus du transfert imprimeur.
- `preuves/` : réouverture STEP/STL, collisions, relief réellement exporté, conservation et manifestes.
- `vues/ASSEMBLAGE_ET_COUPES.pdf` : vues et sections nominales ; enveloppes modules explicitement provisoires.
- `ASSEMBLAGE.md`, `RESERVES.md`, `BOM_NON_IMPRIMEE.csv` : premier article à froid, limites et achats complémentaires non effectués.

## Choix de reprise

Reprise du générateur paramétrique V1.5, de ses interfaces et des scénarios V2.0, sans modifier les originaux. Le référentiel est maintenant unique : X=KiCad X, Y CAO=−Y KiCad, Z=0 dessus Nucleo ; shield dessous **12,25**, dessus **13,85 mm**. Le scénario d’accouplement modules reste 11 mm au-dessus du shield, **pas G=11**, et n’est pas une mesure. Le réglage des appuis se fait par deux écrous contrebloqués sous chaque module, sans imposer cette hauteur.

L’ancien fond aurait empiété sur l’enveloppe inférieure de la C05 officielle. Nouveau plancher Z=−11, fond externe −14, bord supérieur de caisse47, couvercle47..50, relief51 : corps **158×166×64 mm**, relief compris **65 mm**. Les portes dépassent latéralement de0,8mm : enveloppe des pièces propres assemblées **158,8×166,8×65 mm**, hors vis, câbles et ouverture des portes. La tête des vis M3 peut augmenter la hauteur ; ses dimensions réelles restent à confirmer. Le plus bas composant ST est à−10,0076001 : garde nominale au plancher **0,9924 mm**, pas une marge de tolérance industrielle.

Panneaux démontables coulissant depuis le haut : une évolution d’une fiche ou de la nappe ne demande pas de réimprimer toute la caisse. Jeu latéral0,2mm par face, rainure2,8/panneau2,4 ; jeu sous couvercle0,25. Ces jeux sont des choix FDM à confronter au premier article, pas des capacités de machine démontrées. USER accessible directement sous une trappe pivotante ; aucun poussoir ni effort permanent sur B1. RESET n’a pas d’ouverture alignée.

## Matière et effort de préparation

PETG, une seule couleur, buse0,4mm, fichiers déjà orientés. Plateau utile recommandé≥180×180mm, hauteur≥70mm, avec bordure5mm ; 200×200mm apporte plus de confort. Pas de séparation de la grande caisse nécessaire sur ce plateau. Le mot ARNACO est déjà fusionné dans le couvercle : six glyphes, hauteur1mm, trait minimal nominal1,125mm, aucune police externe, aucune devise/© ni pièce à coller.

Les conseils PETG sont prudents : ce matériau demande une attention particulière aux ponts et aux surplombs ([Prusa, PETG](https://help.prusa3d.com/article/petg_2059), [ponts](https://help.prusa3d.com/article/poor-bridging_1802)). Les baies sont ouvertes vers le haut, les pièces plates orientées à plat ; les petites poches d’écrous demandent une vérification dans le trancheur. Aucun G-code, profil machine ni impression n’a été produit. Les temps et l’énergie sont **non estimés**.

Le volume CAO total des pièces autorisées, réserves comprises, est converti avec1,27g/cm³, densité du [PETG Prusament](https://prusament.com/wp-content/uploads/2022/10/PETG_Prusament_TDS_2021_10_EN.pdf) utilisée **comme scénario de calcul**, pas comme identification du filament de l’ami. Voir `preuves/masse.json`. Il s’agit de matière dense, sans supports, purge ni brim, et non d’une masse tranchée. Les propriétés mécaniques du fabricant ne sont pas transférées au prototype.

## Reproduction

Depuis ce dossier, avec CadQuery2.8, Shapely, Trimesh, NumPy et Matplotlib installés dans l’environnement CAD existant :

```sh
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/modele.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/controle.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/services.py
PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs /home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python sources/vues.py
python3 sources/paquet.py
```

Le contrôle C05 utilise le BREP officiel déjà conservé **localement** dans `/home/lolo/dev/msm/reports/TR2-shield-v20-20261105/C05-cache.brep`, et la transformation/population tracées en V2.0. Aucun modèle ST n’est redistribué dans nos STEP/ZIP. Si le cache n’est pas disponible, cette partie du contrôle doit être déclarée non reproduite, pas remplacée par une boîte simplifiée.

Ni PCB/cuivre/Gerber, ni firmware, ni protections du poste modifiés. Aucun ERC/DRC/build STM32 répété : ces outils n’apporteraient pas de preuve sur des pièces mécaniques nouvelles. Les neuf cas D126 V2.1 sont conservés, pas rejoués ; l’identité de son nouveau STEP est vérifiée par différence volumique.
