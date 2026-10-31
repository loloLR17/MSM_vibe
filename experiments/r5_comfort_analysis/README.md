# R5 confort — voie préliminaire non normative, P0 non satisfaite

Mission distincte de R5-A/B/C/D acceptée. Aucun fichier de l'ancienne chaîne,
archive, base SQLite v1, firmware ou supervision n'est modifié. Les classes et
fonctions du lecteur strict R3 et du moteur R5 sont importées, pas recopiées.

## Porte de méthode P0

Les sources primaires accessibles permettent de constater l'existence de Wm et
des grandeurs d'évaluation ; elles ne fournissent pas les parties nécessaires
pour **vérifier** l'implémentation exacte et son équivalence de présentation.
Les aperçus licites consultés s'arrêtent avant ISO20283-5:2016 §7/8, et avant
ISO2631-2 annexeA. L'utilisateur confirme le31/10/2026 n'avoir actuellement
aucun accès autorisé complémentaire. Aucun achat, copie depuis un miroir sans
provenance autorisée, ni reconstruction baptisée normative n'est effectué.

Voir `P0_METHOD.md` et le journal de sources publié. **Wm non implémenté,
équivalent ISO non implémenté, seuils/classes BV désactivés.** Un vrai blocage
sur P0 demeure après publication des calculs préliminaires autorisés.

## Calculs effectifs, hypothèses séparées

- Repartir des XYZ originaux, pas des RMS2–25Hz : strictCOMPLETE et SHA avant/après.
  Sensibilité0.244mg/count nominale, axes capteur, UTC_UNKNOWN.
- Deux scénarios de fréquence : endpoints MCU et estimation moyenne des bornes
  du témoin QPC déjà vérifié. **Mêmes échantillons/mêmes fenêtres MCU60/120s**
  dans les deux scénarios ; ce n'est pas une correction de datation en secondes SI.
- Indicateur1 : RMS d'accélération **non pondérée** après Butterworth1–80Hz,
  prototypeordre4, ordre total8, SOS causal float64, pré-gauchissement des bornes.
- Contrôle2 : RMS de **vitesse physique intégrée non pondérée**, filtre analogique
  B(s)/s dont un zéro à l'origine est annulé **avant** transformation bilinéaire.
  Cette réalisation stable équivaut à l'intégration trapézoïdale discrète de la
  bande filtrée ; elle évite une annulation numérique instable pôle/zéro àz=1.
  Ce n'est ni Wm appliquée à une vitesse physique, ni l'équivalent de présentation ISO.
- Relation complexe discrète `V/A=9.80665/(j·2fs·tan(πf/fs))` ; l'intégrateur
  physique idéal est `9.80665/(j·2πf)`. Rapport des modules :
  `(πf/fs)/tan(πf/fs)`, biais fréquentiel quantifié à1/2/5/10/20/40/80Hz.
  La comparaison FFT indépendante à l'intégration idéale reste un contrôle séparé.
- Gain du passe-bande−3.0103dB à1/80Hz ; rolloff fini, pas de bande rectangulaire
  idéale ni de réponse physique étalonnée à1Hz. Retard causal non compensé.
- Exclure les11warmups FIR et10s après chaque début/reprise. NaN/clip/gaps invalident
  et réinitialisent l'état ; pas de zéro de substitution. FULL exige durée nominale
  entière et autant de sorties valides que d'entrées observées dans la fenêtre.
  Première/dernière fenêtre partielles exclues des quantiles ; couverture exportée.
- XYZ et maximum des RMS directionnels sont distincts de la somme quadratique
  vectorielle technique. Le maximum XYZ capteur n'est pas une sélection validée
  de directions navire. Une valeur plus élevée décrit une vibration filtrée plus
  intense dans **cette méthode** ; elle ne démontre ni gêne ni dépassement.
- Signature : version/algorithme, filtreSHA, éditionNONE, unité/bande, fenêtre,
  horloge/sensibilité, support/orientation/site, firmware, qualité/UTC et provenance
  campagne. Comparaison d'équivalence entre campagnes **refusée** dès qu'une
  hypothèse manque/diffère, ou qu'horloge/pose/orientation/échelle n'est pas qualifiée.
  La comparaison de scénarios du même fichier est étiquetée comme telle.

## Validation ciblée et reproduction

Utiliser l'environnement privé R5 existant ; pas d'installation globale :

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -m unittest discover -s experiments/r5_comfort_analysis -p 'test_*.py' -v
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_comfort_analysis/run.py /home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2 /chemin/PC/nouveau-dossier
```

Tolérances définies avant exécution : transfert complexe contre prototype
Butterworth analytique1e-8 relatif/1e-9 absolu ; sinusoïdes RMS1e-4 absolu ;
intégration synthétique indépendante trapèzes vs formule0.2%. Réels : SOS temps
contre transfert FFT calculé algébriquement1e-6 relatif, et vitesse physique
fréquentielle idéale vs approximation trapèzes1% (le biais théorique jusqu'à80Hz
est documenté). DC1g, inclinaison0.1Hz, multiton, bruit/impulsion, clipping,
lacunes, frontières de lots/fenêtres et maxdirection≠normeXYZ testés.
Ces tests ne portent **pas** sur Wm, dont l'oracle normatif manque.

Réponse des107coefficients FIR Q15 calculée aux fréquences utiles et à la première
zone susceptible de repliement ; SHA des107int16 little-endian comparé à l'archive.
C'est une vérification numérique de gain/délai/quantification, pas de la réponse
IIS3DWB/pose/bruit. Aucune compensation silencieuse de la sensibilité ou du FIR.

Validation exclusivement PC : aucune compilation STM32/.NET ni répétition des
14tests R5 inchangés n'apporterait de preuve supplémentaire ici.

## Catalogue secondaire et restitution

```sh
/home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_comfort_analysis/comfort_catalog.py /chemin/PC/nouveau-dossier /chemin/PC/comfort-secondary-v2.sqlite
/home/lolo/dev/msm/cache/r5-analysis-venv/bin/python experiments/r5_comfort_analysis/render_page.py /chemin/PC/nouveau-dossier /chemin/PC/nouveau-index.html
```

`schema_v2.sql` crée uniquement une base secondaire marquée application_id différent,
user_version2. Une DBv1/live/étrangère est refusée **avant toute écriture**. Il ne
s'agit pas d'une migration de la base existante. La passerelle est une lecture
séparée **mode=ro**, pour comparer les inventaires/provenances des deux catalogues ;
aucune égalité entre les indicateurs 2–25 et1–80Hz n'est présumée.

Tables campaigns, metric_definitions, analysis_runs, window_metrics, artifact_refs ;
les définitions portent éditionNONE_NON_NORMATIVE, unité, bande, paramètresSOS et
hypothèses complètes. Identité d'analyse = campagneSHA + définitionSHA + codeSHA ;
une nouvelle cohorte ne remplace pas l'ancienne. Transactions/clés/index/FK/FULL,
échec explicite journalisé, contrôle intégrité et réimport idempotent. Les données
brutes restent exclusivement dans .TR2 ; seuls les agrégats60/120s sont en SQL.
Les imports qui prétendent activer le nom normatif ou un seuil sont refusés par
cette voie préliminaire. Quatre requêtes documentées/exécutées : inventaire des
séries, pire60s de vitesse MCU, axes120s, édition/statut des définitions.
