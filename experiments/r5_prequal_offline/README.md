# Préqualification expérimentale hors ligne

[Définition, limites et tolérances](METHOD.md) · [Audit pleine échelle](FS_AUDIT.md).
Source, ancien lecteur/streaming et catalogues sont préservés. Aucun accès matériel.
Interpréteur : venv scientifique R5 existant, dépendances inchangées.

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -W error::ResourceWarning -m unittest discover -s experiments/r5_prequal_offline -p 'test_*.py' -v
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_prequal_offline/run.py /nouveau/dossier/resultats
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_prequal_offline/deliver.py /resultats /nouveau/dossier/presentation
```

Ne pas relancer le parcours déjà réussi pour relire les résultats : les moments
par seconde dans sufficient_statistics.npz permettent de nouvelles agrégations
(`additional_stats.py RESULTATS NOUVEAU_JSON`) sans filtrer à nouveau la campagne.
Le NPZ est local ; seuls agrégats CSV et preuves compactes sont publiés.

`resolution.py NOUVEAU_JSON` simule un analogue connu avant quantification,
avec/sans bruit blanc déclaré hypothétique, puis FIR nominal et arrondi de sortie.
`extreme_audit.py MANIFEST NOUVEAU_JSON` inspecte seulement l'archive en lecture
seule pour localiser les extrêmes et expliciter la borne L1 du FIR. Ce n'est
pas une nouvelle mesure ni une explication physique de leur cause.

`prequal_catalog.py` offre create_copy/import_results/register_artifacts ; copie
v3→v4 seulement dans un fichier nouveau, anciennes tables identiques, nouveaux
résultats identifiés par SHA source/code/méthode/export. Voir queries.sql pour
reconstruire les courbes sans mélanger les analyses. La v4 unique est justifiée
par l'interdiction contractuelle de modifier aussi la v3 ; pas de base par KPI.

Les filtres TEST_LP10/EDGE ne sont pas des modèles de confort. Ne pas appeler leurs
valeurs ISO/Wm/BV ni leur appliquer des seuils. Une bonne précision numérique
ne prouve ni la répétabilité de deux poses, ni la mesure de bruit à1Hz, ni une
échelle physique étalonnée. Les noms de méthodes, coefficients et hashes doivent
rester figés pour comparer ultérieurement le même point ; cadrer les différences
de FS, firmware, cadence, orientation et pose avant toute comparaison physique.
