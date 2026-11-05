# V2.6 — dossier complet d’impression pour deux prototypes

**IMPRIMABLE AVEC AJUSTEMENT À FROID PRÉVU.** Aucun premier article ni comportement vibratoire qualifié. Le critère de mission est le kit complet, sans attendre les livraisons : les réserves de montage ne retirent aucune pièce du ZIP.

Le dossier `pack_imprimeur/TR2_V26_COMPLET_2_PROTOTYPES.zip` contient les **25 STL / 142 pièces** de `QUANTITES.csv`, les quatre étriers inclus, des supports rigides et leurs cales, les notices et un PDF de cinq pages. Les hauteurs alternatives et réserves expliquent le nombre : on ne monte pas les 142 pièces en même temps. Un seul ZIP de livraison ; la décision de le transmettre suit la revue indépendante ChatGPT. Aucun envoi effectué.

Le coffret, ARNACO, les D126 et les bras Nucleo sont les 14 modèles V2.5 repris octet pour octet. Aucun PCB, Gerber, schéma, firmware ou repère X/Y ST n’a changé. Les nouveaux éléments sont isolés dans cette révision.

- `IMPRIMEUR.md` : matière, orientation, couches, quantités.
- `MONTAGE_A_FROID.md` : ordre de montage et critères d’ajustement, sans forcer les sockets.
- `BOM_QUINCAILLERIE.csv` : Ajile exclusivement D126 ; M3×12 pour coffret/patins ; M2×30 unique pour les modules et étriers.
- `vues/ASSEMBLAGE_AJUSTEMENT_V26.pdf` : implantation et coupes ; `vues/etriers-3D.png` : géométrie des nouvelles pièces.
- `cad/assemblage-pieces-propres.step` : assemblage éditable de nos pièces, H11 illustratif. Les STEP individuels sont en coordonnées de montage ; les STL sont orientés et ramenés au plateau en millimètres.
- `cad/quincaillerie-standard-enveloppes.step` : nos gabarits de vis/écrous nouveaux, sans filets réels. Les modèles fabricants ne sont pas redistribués.
- `SOURCES.md`, `sources/`, `preuves/` : dérivations, scripts et observations effectives.

La sélection des hauteurs se fait au premier montage, hors tension. La plage mécanique des supports n’est pas une promesse d’engagement électrique. Les vérifications restantes sont intégrées à la notice de montage ; aucune mesure préalable n’est exigée pour lancer l’impression.

## Reproduction ciblée

Environnement utilisé : CadQuery2.8 / Trimesh / Shapely / Matplotlib, Python du cache `shield-v15-cad-venv`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Depuis la racine du dépôt, préfixer le chemin `sources/` par celui de ce dossier :

```
python sources/extraire_sources.py /chemin/vers/sources-fabricants-locales
python sources/modele.py
python sources/controle.py --native-dir /chemin/vers/sources-fabricants-locales
python sources/vues.py
python sources/paquet.py
```

Le répertoire natif doit contenir `Click2-cache.brep` et `SD-cache.brep`, issus des fichiers épinglés dans `SOURCES.md`. Ces caches tiers restent locaux. Les créer si nécessaire avec `cadquery.importers.importStep(str(fichier_source)).val().exportBrep(str(cache))`, après comparaison des SHA-256 des fichiers sources. Le Click vient du membrev101 identifié dans le manifeste ; le SD est `mechanical/V1_5/models/Adafruit_4682.step` déjà épinglé dans les preuves V1.5. `extraire_sources.py` attend les noms de fichiers privés enregistrés dans le manifeste et l’archive ST extraite avec son sous-dossieradapter. Aucune récupération automatique non épinglée ne remplace la comparaison des empreintes. Les scripts sont du périmètre CAO ; aucun build firmware, test matériel ou DRC électronique déjà acquis n’est relancé.

Publication technique relue sur GitHub : `348e559cef823a57a86a99a98d2ef2376a6c5f7d`,845fichiers/32membresZIP identiques. Voir `preuves/publication-initiale.json`. Pour vérifier une référence fraîchement récupérée : `python sources/relecture_distante.py /chemin/vers/preuve-externe.json` depuis la racine du dépôt (préfixer le chemin du script par celui du dossier V2.6). Aucun artefact imprimable modifié par la clôture.
