# V2.7 — diffusion simplifiée à Morvan

La revue indépendante ChatGPT a accordé le GO **impression prototype ABS** à V2.7. Cette mission ne change aucun modèle ou quantité :25 STL distincts,61 pièces pour deux prototypes (46 utiles +12 alternatives +2 D126 de réserve +1 coupon). Le nombre de formes était gelé ; la simplification porte sur l'envoi et les tâches demandées à l'imprimeur.

**Seule archive de diffusion à retenir après revue ChatGPT :** [**TR2_V27_ABS_MORVAN_IMPRESSION_SIMPLE_2_PROTOTYPES.zip**](../pack_Morvan/TR2_V27_ABS_MORVAN_IMPRESSION_SIMPLE_2_PROTOTYPES.zip),1716575 octets, SHA-256 `0ce87b103fbc5eaa2602a9687628cc959d092cf5275700c7308e198f59c29c8f`.

Elle contient exactement **27 entrées** :les25 STL source identiques dans `STL/`, [une notice de3 pages](pdf/01_LIRE_EN_PREMIER_MORVAN.pdf) et [deux pages de vues](pdf/02_VUES_DES_PIECES.pdf). Aucune pièce ancienne, archive imbriquée, MD/CSV/JSON/STEP/BOM, notice de montage ou manifeste technique. Les preuves sont ici, **hors ZIP**.

Morvan imprime seulement :ABS non chargé, échelle100 % en mm, quantités déjà pour les deux prototypes. Pièces en vrac acceptées ; aucun tri, étiquetage, achat de quincaillerie, montage des cartes ou test de USER demandé. Montage, retouches locales et vérifications fonctionnelles restent au propriétaire. Le GO prototype n'est pas une qualification du montage ni une autorisation de première mise sous tension.

## Anciennes trappes et cales :constat binaire

[Inventaire old/new complet](preuves/INVENTAIRE_ET_COMPARAISON.md). ZIP V2.6 réellement extrait :8 fichiers obsolètes identifiés. ZIP technique V2.7 réellement extrait :38 entrées,25 STL, **aucun** de ces noms ; tous les STL comparés aux sources gelées. L'archive téléchargée/consultée par l'utilisateur n'est pas disponible :son identité précise reste inconnue. Le constat ne présume aucune erreur d'utilisateur.

L'ancien ZIP technique V2.7 et les950 fichiers historiques du shield sont conservés strictement. Les anciens dossiers restent archivés sur GitHub ; ils ne sont pas inclus dans le nouvel envoi. Ne transmettre aucun ancien ZIP V2.6.

## Vérifications et reproduction

Depuis la racine du dépôt, exécuter les scripts de `diffusion_Morvan/sources/` dans cet ordre :`audit_archives.py`, `documents.py`, `conditionnement.py`, `validation_finale.py`. Python existant :`/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`, `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs` pour PyMuPDF/Pillow/NumPy ; aucune bibliothèque CAO appelée. DejaVu incorporé aux PDF, aucune installation nécessaire. `audit_archives.py` décompresse réellement les anciennes archives dans `/home/lolo/dev/msm/reports/TR2-shield-v27-pack-morvan-pdf-20261106/`.

Le constructeur utilise une liste blanche explicite ; le second validateur retrouve indépendamment les25 fichiers et quantités dans Git gelé `50af554db63ab76664df63565f9203e04f2d963e`. Validation des octets/SHA/taille/CRC et des25 lignes de quantités extraites des PDF. Les5 pages ont été rendues en PNG puis réellement inspectées :[revue visuelle](preuves/revue-visuelle.json), [rendus](rendus_pdf/), [validation automatique](preuves/validation-finale.json).

`relecture_distante.py`, après `git fetch origin main`, compare les blobs complets et les27 membres de l'archive distante. Aucune transmission automatique à Morvan. La décision de remise du nouveau lien revient à ChatGPT après revue.
