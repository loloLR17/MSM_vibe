# Préqualification hors ligne — résultats livrés

**FAISABILITÉ LIMITÉE**, aucune métrique normative, aucun score de confort.
[Note de décision (POC)](NOTE_DECISION.md) · [Figures hors ligne](index.html).

- [RMS par axe, direction et norme technique, 60/120s](metrics.csv), [182 paires alignées par méthode](aligned_pairs.csv).
- [Quatre bandes et chevauchement](band_shares.csv), [réponses amplitude/phase](filter_response.csv), [coefficients et définitions](definitions.json).
- [Résumé](summary.json), [période entière valide](additional-stats.json), [sensibilités](sensitivity_summary.json).
- [144 contrôles réels FFT/Parseval](real_controls.json), [tests ciblés](tests-final-ten.log).
- [Distribution des codes conservés](raw_distribution.json), [min/max par seconde](raw_1s.csv.gz), [épisode extrême](extreme-audit.json).
- [Simulation sur analogue connu et budget de résolution](resolution-simulation.json), [preuve SQLite et préservation intégrale](sqlite-proof.json).
- [Méthode détaillée](../../../../../experiments/r5_prequal_offline/METHOD.md), [audit FS et protocole futur](../../../../../experiments/r5_prequal_offline/FS_AUDIT.md), [code et commandes](../../../../../experiments/r5_prequal_offline/README.md).

MESURÉ signifie mesure des **codes de l'archive**, pas étalonnage instrumental.
Les g/mg utilisent la sensibilité nominale. ESTIMÉ concerne cadence, conversion,
réponse numérique et marges. SPÉCIFICATION_FABRICANT : ST DS12569 Rev8, note6,
bruit indépendant du FS mais caractérisé100Hz–6,3kHz. HYPOTHÈSE : modèles de bruit
blanc/quantification, filtre LP10 de sensibilité sans modèle de perception humaine.

Les quatre bandes Butterworth ne sont pas orthogonales : pourcentages rapportés
à leur somme, somme/total exportée. Le poids plat et le LP10 sont deux méthodes
distinctes, pas deux unités d'une même métrique. Aucun Wm/ISO/BV implémenté.
Les premières/dernières fenêtres invalides ne sont pas remplacées par zéro.
Les comparaisons60/120 portent aussi sur les mêmes périodes, pas seulement deux
histogrammes. Les valeurs non arrondies restent dans les exports et SQLite.

Le fichier source entier (640MiB), son payload utile, les anciennes DB v1/v2/v3
et leurs tables sont préservés. Une copie unique v4 est nécessaire car la mission
interdit aussi l'écriture dans v3 ; elle contient les anciennes cohortes et les
8 816 nouvelles métriques. Aucun XYZ brut ajouté à SQLite. Chemins/hashes dans
LOCAL_ARTIFACTS.json et sqlite-proof.json ; aucune DB/TR2/NPZ/PDF publiée.

Le FIR calculé 1–100Hz provient de la preuve PK précédente, réutilisée et
explicitement nommée fir_response_reused.json : aucun test historique répété.
Les suites numériques précédentes et les analyses PK/RMS historiques sont intactes.
[Ancienne analyse technique 2–25/PSD](../TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/index.html) ;
[anciens RMS1–80 non pondérés](../TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/index.html).

Source COMPLETE au sens du lecteur strict, SHA/CRC/index vérifiés avant/après ;
référence RAM indépendante perdue après coupure, R3 reste sous réserves. Axes navire,
fixation, gain, cadence et bruit1Hz non qualifiés ; température non archivée.
Aucun matériel accédé et aucune acquisition ni dépense effectuée.

Échecs logiciels conservés : collision de nom d'import catalog avec l'ancien
module, puis ResourceWarning de connexions SQLite non fermées. Imports nommés
et fermetures explicites corrigés, 10 tests PASS sans avertissement. Le découpage
naïf des requêtes SQL a aussi rencontré un point-virgule dans un commentaire ;
commentaire corrigé puis cinq requêtes réellement exécutées. Pas de recalcul
intégral nécessaire ; le parcours réel a réussi au premier essai.
