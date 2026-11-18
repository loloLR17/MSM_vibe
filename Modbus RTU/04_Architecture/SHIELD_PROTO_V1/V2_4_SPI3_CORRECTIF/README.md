# Shield V2.4 électrique — SPI3 corrigé, CANDIDAT NON LIBÉRÉ

Mission `TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF`, départ `fb7167548dc1f55eb8250d3d52c8c85dd5caa8c9`. Cette variante distincte corrige uniquement les six contacts J8 concernés et les pistes SPI3 nécessaires. Aucun accès matériel, achat ou envoi au fabricant.

## Fichiers à revoir

- [Projet KiCad](cad/TR2_Shield_Proto_V1.kicad_pro), [PCB](cad/TR2_Shield_Proto_V1.kicad_pcb), [schéma](cad/TR2_Shield_Proto_V1.kicad_sch), bibliothèques et routes locales dans `cad/`.
- [ZIP CAM candidat unique](exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip), [SHA256 externe](exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip.sha256), [inventaire de ses9membres](preuves/CAM-manifeste.json). Sept Gerber, deux Excellon ; plans PDF et gbrjob informatifs hors ZIP, aucun vieux CAM incorporé.
- [Schéma PDF natif](exports/schema.pdf), [implantation](exports/implantation.png), [cuivre avant](exports/cuivre-dessus.png), [cuivre arrière miroir](exports/cuivre-dessous-miroir.png), PDF/SVG vectoriels correspondants.
- [Contrôles E-01/M-01](CONTROLES_ET_LIMITES.md), [180contacts](preuves/E01/MATRICE_E01_180_CONTACTS.csv), [17nets](preuves/E01/MATRICE_E01_17_NETS.csv), [diff V2.3/V2.4](preuves/diff-V23-V24.json).
- [Achats urgents mis à jour avec le stock utilisateur](achats/ACHATS_URGENTS.md), [BOMnative](achats/BOM_NATIVE_2_SHIELDS.csv), [liste de préparation](achats/PREPARATION_2_SHIELDS.csv).

## Correction explicite

| Net MCU conservé | V2.3 fautive | V2.4 | Fonction constructeur |
|---|---:|---:|---|
| PC9 / ACC_CS |J8-21|**J8-19**|CS|
| PC10 / ACC_SCK |J8-22|**J8-20**|SCL|
| PC12 / ACC_MOSI |J8-23|**J8-21**|SDA|
| PC11 / ACC_MISO |J8-24|**J8-22**|SDO|
| NC |19/20|**23/24**|aucune connexion intentionnelle|

J8 ne bouge pas : mêmes24pads, entre-rangées15,24mm, même sens de numérotation. Broches1/2 alimentées3V3 et13GND inchangées. Les autres nets, notamment J9/LPUART/PG4/J5 physiquement qualifiés sur l'ancien assemblage001, sont conservés exactement. Les marquages natifs historiques V1.9/DRAFT et ARNACO restent intacts pour respecterM-01 ; l'identité électriqueV2.4 et le statut candidat sont portés par ce dossier/ZIP/manifest, pas par une modification de la sérigraphie.

**Résultat numérique :** ERC0/DRC0/non-connectés0/parité0 ; E-01 180PASS,17nets, zéro court-circuit/coupure géométrique dans les deux vrais cuivres exportés. M-01 :19empreintes/191géométries de pads-trous/4vias exactement conservés ;232fichiers historiques V2.3/V2.7 intacts ;191interfaces de perçage recoupées au STEP gelé. Huit mutations interdites rejetées par les contrôles négatifs en mémoire.

**Cela n'est pas un GO fabrication.** Le futur PCB V2.4 n'existe pas encore physiquement. Montage à froid, références réellement montées, insertion/tenue, soudures, rails, SPI3 et coexistence matérielle restent à qualifier. Le défaut des V2.3 existantes n'est pas réparé par la publication. Revue indépendante ChatGPT puis décision utilisateur requises avant toute diffusion au fabricant.

## Reproduction et environnement

KiCad Windows10.0.6 sous `C:/Program Files/KiCad/10.0/bin`, PythonLinux et modules déjà présents via `PYTHONPATH=/home/lolo/dev/msm/cache/shield-pylibs`. Pour STEP/CadQuery, Python `/home/lolo/dev/msm/cache/shield-v15-cad-venv/bin/python`. Aucune politique Windows modifiée.

Les générateurs sont destinés à une **copie de travail isolée avec toutes les modifications manuelles préservées**, pas à écraser ce lot figé. Ordre : `correct_spi3.py` (sourceV2.3 immutable), `route_spi3.py` (5branches terminales, sans via nouveau), `fill_candidate.py` avec PythonKiCad vers un fichier temporaire neuf, `merge_zone_fills.py` (seuls polygones de cuivre dérivés), `verify_m01.py`, `verify_v27_step.py`, ERC/DRC natifs, puis `export_design.py`. L'exporteur exige ces gates et refuse de mélanger des CAM déjà présents.

Commandes ERC/DRC réellement exécutées dans `docs/checks/native-commands.json`. Pour les preuves constructeur : `build_reference.py` (ne lit aucun projet), `read_native.py` avec PythonKiCad, `audit_orientations.py`, `verify_symbols.py`. Après `package_cam.py`, `audit_e01.py` lit le vrai ZIP/les deux cuivres et Excellon ; `compare_before_after.py`, `negative_m01_controls.py`, `verify_stock_options.py` complètent les contrôles. Les sources fabricant externes sont épinglées par URL/version/hash dans `preuves/E01/reference-constructeurs.json` ; conserver leurs caches `/home/lolo/dev/msm/reports/` pour une reproduction complète. Aucun original fabricant redistribué à tort comme création du projet.

Le gbrjob natif conserve une taille90,05×100,05mm incluant le trait de contour ; les axes du vrai contour restent90×100mm. Son `Finish:None` n'est pas une sélection de finition de commande. Les paramètres de fabrication et le paiement seront une décision distincte.
