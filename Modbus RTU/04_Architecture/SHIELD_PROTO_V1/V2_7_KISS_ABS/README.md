# TR2 V2.7 — ABS KISS, dossier POUR MORVAN

Mission `TR2-20261106-SHIELD-V27-KISS-ABS-MORVAN`, départ `c2bb310b85e974a87048093575eb69afe0728264`.

**Livraison numérique pour revue indépendante ChatGPT avant remise à Morvan.** Aucun envoi, achat, impression, flash ou essai physique réalisé. L'ancien feu vert V2.6 a été retiré par la revue ; utiliser uniquement le ZIP V2.7 après cette nouvelle revue.

- [ZIP unique](pack_Morvan/TR2_V27_ABS_POUR_MORVAN_2_PROTOTYPES.zip), avec SHA externe et manifeste interne SHA-256/CRC32.
- [Vue éclatée numérotée d'un seul prototype](vues/VUE_ECLATEE_V27.png), [plan PDF de six pages](vues/PLAN_MONTAGE_V27.pdf), [coupe USER](vues/COUPE_USER.png).
- [Instructions ABS](IMPRIMEUR.md), [quantités](QUANTITES.csv), [quincaillerie](BOM_QUINCAILLERIE.csv), [montage hors tension](MONTAGE_A_FROID.md), [sources et limites](SOURCES.md).
- STEP complet et éclaté dans `cad/`, paramètres et générateurs éditables dans `sources/`. Objets nommés ; électronique représentée par enveloppes dérivées/citées, pas par redistribution des solides fabricants. Réouverture STEP contrôlée avec CadQuery/OCC, pas de session FreeCAD revendiquée.

## Réduction réelle

| Pour deux prototypes | V2.6 | V2.7 |
|---|---:|---:|
| Pièces imprimées totales |142|61|
| Types STL |25|25|
| Pièces montées | — |46 (23 par prototype)|
| Variantes non montées | — |12 (8 selles +4 tiges)|
| Réserves D126 | incluses |2|
| Coupon unique |0|1|

**81 pièces de moins, soit 57,04 %.** Le nombre de types reste25, mais le tri, l'ébavurage et les petites pièces diminuent nettement. Les60 cales imprimées disparaissent ; **24 rondelles isolantes standard distinctes restent indispensables** (16 sous les têtes M2 et8 au-dessus des modules Adafruit pour les deux TR2). Les selles réunissent deux appuis en une pièce. Les trappes USER, SD et bornier, les cassettes sud/ouest et la trappe supérieure RS485 sont supprimées. Deux panneaux propres à leurs câbles restent remplaçables : nord LED/ST-LINK/nappe, est RS485. Ils évitent plusieurs découpes manuelles différentes et une réimpression de la caisse pour une retouche de passage.

La fonction essentielle USER ajoute par prototype une cage, une tige choisie, un ressort et une butée M3 freinée. La tête est en retrait de0,6mm et captive dans les deux sens. Pas de trappe ni d'outil en utilisation ; régler hors tension avant le premier usage. Le réglage court de transport montré en CAO n'est pas un réglage opérationnel.

## Invariants et réserves

PCB/Gerber V2.3 commandés, électronique, firmware, X/Y ST, Zio, J5 et G12,25 inchangés. D126 inchangés, Ajile nylon exclusivement D126. Deux portées par module reprennent la compression au shield ; ne pas charger les sockets. Les patins isolants touchent uniquement du PCB nu ; aucun pont métallique ajouté au domaine isolé Click.

H10,6/11,2/11,8 avec ponçage à plat≤0,6 couvre **mécaniquement10..11,8mm**, pas une course électrique de connecteur. Le modèle H11,2 est un exemple d'assemblage, pas une mesure. Click officiel1.01 et carte réelle1.02 ne sont pas déclarés identiques. Vérifier leurs portées à froid.

Mesures B1 rapportées : dessus10,2mm, premier clic≈0,4mm ; course maximale/force admissible inconnues. Tiges alternatives + butée positive réglable ; ajuster au premier clic réel, aucune précharge ni appui sur RESET. Aucune qualification de force, retour réel, gants, retrait ABS, vibrations, fluage, tenue thermique ou IP.

## Reproduction numérique

Environnement déjà présent : `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`, Python `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python` (CadQuery2.8, VTK pour les vues hors écran). Depuis cette racine, exécuter dans l'ordre `sources/modele.py`, `sources/controle.py`, `sources/supplement.py`, `sources/assemblage.py`, `sources/vues.py`, `sources/qualite_livraison.py`, `sources/paquet.py` avec ce Python. Les modèles précédents sont lus sans modification.

Les contrôles natifs et la régénération des enveloppes fabricants nécessitent les caches épinglés V2.6 dans `/home/lolo/dev/msm/reports/TR2-shield-v26-20261105/` (SD-cache.brep, Click2-cache.brep et leurs tables de solides). Ces dépendances locales ne sont pas incluses dans le ZIP imprimeur ; les sources/empreintes et les enveloppes STEP dérivées sont publiées. Ne pas confondre un dossier imprimeur autonome avec la redistribution intégrale de la chaîne CAO fabricant.

Preuves :5477 contrôles BREP initiaux finaux, puis229 contrôles supplémentaires avec enveloppes R/C élargies ; aucune intersection positive indésirable dans ces scénarios.25 maillages/STEP réouverts ou preuves exactes V2.5 réutilisées pour4. Les66 lignes de vue LED communes restent libres géométriquement. Le montage réel reste à démontrer. Ne pas relancer les tests déjà acquis sur des STL inchangés.
