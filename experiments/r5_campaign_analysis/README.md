# R5 — analyse exploratoire PC d'une archive TR2 complète

Ce moteur PC est isolé du firmware et de la supervision Windows. Il ne possède
aucune commande matérielle. Les entrées restent en lecture seule. Le lecteur
strict R3 existant décide seul de l'acceptation COMPLETE ; le décodage NumPy par
4096 records intervient après cette vérification, sans accepter de préfixe.

## Reproduire

Python 3.13 ; dépendances gelées dans `requirements.txt`. Utiliser un environnement
privé, aucune installation globale requise. Depuis la racine de ce worktree :

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -m unittest discover -s experiments/r5_campaign_analysis -p 'test_*.py' -v
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_campaign_analysis/run_analysis.py /home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2 /chemin/PC/nouveau-dossier --qpc /home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/six-long.json
/home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_campaign_analysis/catalog.py /chemin/PC/nouveau-dossier /chemin/PC/r5.sqlite
```

Le dossier de calcul doit être nouveau : aucun écrasement d'un résultat existant.
Le catalogue accepte un second import identique sans écriture de série ni doublon.
Les erreurs d'import sont explicites et journalisées dans `r5.sqlite.errors.jsonl`.
Les premières tentatives d'installation/test de dépendances absentes sont tracées
localement ; aucune modification de la sécurité Windows n'est nécessaire.

## Méthode et limites

- `.244 mg/count` nominal ; XYZ capteur, orientation navire inconnue. Le brut
  conserve DC/pesanteur. Aucun étalonnage d'amplitude ni du montage n'est supposé.
- DSP régulier à `(N−1)/(dernier−premier temps MCU)` ; horloge MCU nominale,
  fréquence estimée et NON SI. Le manifeste distingue la cadence initiale du header,
  les bornes QPC indépendantes et l'écart à la grille régulière. Pas d'UTC reconstruite.
- RMS global = racine de la moyenne de `x²+y²+z²` ; crête vectorielle = maximum
  de la norme XYZ simultanée, et non somme quadratique des trois crêtes d'axes.
- Vibration : Butterworth prototype ordre4 passe-bande 2–25 Hz, ordre total8,
  SOS causal float64 ; réponse et coefficients exportés dans `filter.json`.
  Gain −3.0103dB aux deux bornes. Ce RMS est **pondéré par cette réponse**, pas
  une intégrale rectangulaire à gain plat. Retard causal non compensé.
- Initialisation à la valeur constante du premier XYZ valide ; exclusion des
  11 warmups FIR puis de 5s de stabilisation. Un trou de temps supérieur à dix
  périodes attendues réinitialise l'état et ouvre une nouvelle stabilisation.
  Les invalides restent NaN, les absences SQL NULL ; jamais remplacées par zéro.
- Fenêtres 1/10/60s ancrées à la première sortie, temps MCU interpolé par bloc
  selon la convention du lecteur strict. Statistiques regroupées par somme des
  carrés et nombre de points, pas moyenne non pondérée des RMS. Fenêtres
  partielles explicitement marquées ; pas de comblement de lacune.
- PSD : après le filtre documenté, décimation16. Fenêtre Hann périodique2048,
  recouvrement50%, densité unilatérale, sans detrend ; PSD globale = somme des
  PSD XYZ, `mg²/Hz`, couleur `10log10(PSD/[1mg²/Hz])`. Le plancher d'affichage
  n'affecte pas les valeurs. Paramètres et réjection avant décimation exportés.
  La bande 2–700 Hz n'est pas étudiée/qualifiée.
- Raies candidates : trois maxima moyens espacés d'au moins1Hz, puis bandes
  fixes ±0.30Hz ; contraste minimum6dB relatif à la médiane spectrale, sauts
  >0.20Hz par pas masqués. Ce suivi descriptif ne prouve ni identité d'une source,
  ni causalité ; aucun raccord arbitraire de machines.
- Trois minutes faible/médiane/forte sélectionnées selon le RMS global des
  fenêtres60s complètes. Contrôle réel indépendant par convolution FFT : transfert
  calculé algébriquement depuis les SOS, contexte12s ; tolérance relative0.1%.
  Contrôle brut par Parseval, tolérance absolue1e-8mg. Ce ne sont pas deux appels
  à `sosfilt`. Le signal échantillonné reste soumis aux réserves matérielles.
- Seuil d'épisodes = P90 des minutes valides de cette campagne ; **aucun seuil
  de gêne, de santé, de conformité ISO/BV ou de fatigue de structure**.
- Intégration vitesse synthétique séparée : `V(f)=A(f)/(j2πf)`, conversion
  `1mg = 9.80665 mm/s²`, bande1–80Hz, DC exclue. Sinusoïdes, multitons et offsets
  testés ; pas de coefficient constant mg→mm/s, pas de Wm, pas de vitesse réelle
  revendiquée normative. Voir l'étude livrée avec les résultats.

## SQLite : frontière avec l'existant

Inspection : `TR2.Persistence/Sqlite/SqliteDatabase.cs` utilise schema7, WAL/FULL,
B3/live_observation, temps de réception UTC et métriques du service ;
`SqliteLiveArchiveReadSource.cs` accède aux observations LIVE et aux JSON.
`TR2.Campaigns/AssemblyMarker.cs` est un squelette, sans import massif .TR2.
Les tests existants `TR2.Persistence.Tests/SqliteDatabaseTests.cs` et tests B3/B5
portent sur ces contrats LIVE. Le catalogue R5 ne réinterprète pas ces valeurs,
ne migre pas cette base et ne modifie ni ces composants ni leurs tests.

Réutilisation : même discipline SQLite transactions/clés/index/versions,
paramètres liés, source immuable et lecture séparée. La frontière d'intégration
future est un adaptateur de lecture de **ce catalogue agrégé**, par `analysis_id`
et temps relatif ; aucune refonte .NET ni duplication des XYZ n'est nécessaire.

`schema_v1.sql` crée une base séparée, marqueur application_id propre et
user_version1 ; une base existante sans ce marqueur est refusée avant écriture.
Tables : campaigns, analysis_runs, window_metrics, artifact_refs. Identité campagne
= SHA entier ; analyse = hash de la source, de la version, du filtre, de la cadence,
de l'échelle et des empreintes du code d'analyse. Résultats contradictoires sous
la même identité refusés. Une autre méthode produit une autre analyse sans
écraser la première. PSD/spectrogrammes restent des fichiers externes référencés
par SHA, pas des millions de lignes. Aucun XYZ brut n'est stocké en base.

Requêtes exécutables : dictionnaire `catalog.QUERIES` ; liste des campagnes,
RMS global par minute, pire tranche10s valide, XYZ sur l'intervalle600–660s.
La CLI affiche le nombre de résultats et les trois premières lignes de chacune.
Les tests utilisent des fixtures de **persistance seulement** ; la preuve réelle
est le double import de E6N0005 et les sorties de requêtes conservées.

Aucune compilation STM32/.NET ni revalidation physique n'est pertinente pour
cette tranche PC isolée ; tests ciblés Python, intégrité stricte réelle et
contre-calculs numériques constituent les validations applicables.
