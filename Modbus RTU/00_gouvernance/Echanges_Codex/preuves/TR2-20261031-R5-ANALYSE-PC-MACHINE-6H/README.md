# Campagne PC machine — résultats exploratoires R5

Ouvrir [index.html](index.html) localement : paquet autonome sans réseau.
Les PNG s'affichent directement sur GitHub. [Synthèse métier](SYNTHESE_POC.md).

- [Intensité brute et filtrée](01_intensity.png)
- [Spectrogramme 2–25Hz](02_spectrogram.png)
- [Trois PSD et fréquences candidates](03_spectra_ridges.png)
- [Fréquences et puissances candidates](04_ridge_power.png)
- [CSV1s compressé](windows_01s.csv.gz), [CSV10s](windows_10s.csv), [CSV60s](windows_60s.csv)
- [Manifeste](manifest.json), [résumé](summary.json), [filtre](filter.json),
  [contre-calcul indépendant](independent_checks.json), [preuve SQLite](sqlite-proof.json).
- [Méthodes et reproduction](../../../../../experiments/r5_campaign_analysis/README.md),
  [étude vitesse/ISO/BV](../../../../../experiments/r5_campaign_analysis/VELOCITY_FEASIBILITY.md).

Les CSV et figures proviennent intégralement du fichier réel E6N0005 identifié
par SHA ; les onze sorties FIR warmup sont exclues du brut statistique, et les
5s supplémentaires de stabilisation du filtre des indicateurs vibratoires.
Aucun zéro de substitution : invalides NaN en CSV, NULL en SQLite ; couverture
et fenêtres de bord partielles signalées. La première minute et la dernière
minute incomplète sont grisées sur la tendance. La première trame spectrale
est invalide et grisée ; la dernière fraction de fenêtre non calculable n'est
pas extrapolée. Pas de trou horodaté >10périodes dans ce fichier.

Bande Butterworth à gain−3.0103dB aux bornes2/25Hz ; filtre non rectangulaire.
Spectrogramme : Hann2048 périodique après décimation16, pas6.156s MCU,
fenêtre12.312s, résolution0.08122Hz estimée. Puissances XYZ additionnées,
référence1mg²/Hz ; le plancher−120dB est uniquement visuel. Réjection finie
avant décimation :−63.31dB à141.34Hz se repliant à25Hz ; pas de garantie d'alias
nul ni bande2–700 qualifiée. PSD de segments : Hann32768, recouvrement50%,
fs2661.462Hz estimée, résolution0.08122Hz, moyenne Welch sur60s. La résolution
numérique n'est pas une précision physique de fréquence.

Chronologie : durée relative21999.988926sMCU (environ6.1111h), distincte de la
borne indépendante21601.0193586sQPC. Le témoin actif estime2679.508–2679.524Hz,
environ0.678% au-dessus de l'estimation MCU2661.462Hz ; écart à qualifier,
pas de correction arbitraire. Écart maximal grille régulière/endpoints archivés
0.606sMCU sur toute la campagne. UTC_UNKNOWN, aucun journal de conduite
synchronisé : aucune annotation « arrivée », régime ou machine n'est ajoutée.

R3 reste CLOSED_WITH_RESERVATIONS : le footer concorde avec les octets utiles,
mais le digestRAM indépendant a été perdu lors de la coupure. La phrase du
lecteur historique «producer digest matched» désigne ici le footer, sans
réhabiliter cette preuve indépendante. Pose, orientation, sensibilité et horloges
non étalonnées. Aucun résultat de confort humain, conformité ISO/BV, risque
structurel ou diagnostic de machine n'en découle.

Le catalogue local contient49134 lignes agrégées, zéro XYZ brut, un import et
un réimport identique sans doublon, quatre requêtes vérifiées. Schéma1 indépendant
et application_id propre ; aucune ouverture de la DB Windows de supervision.
Les chemins/SHA exacts du fichier SQLite19.29MB, des PSD XYZ11MB et du cache
décimé43.91MB restent dans LOCAL_ARTIFACTS.json et sqlite-proof.json.
Ces fichiers locaux et le .TR2 de640MiB ne sont pas téléversés.

Le calcul complet a pris24.52s chronométrées dans l'outil (hors imports Python),
pic RSS397320KiB, dont pages mappées. Lots4096records, aucune collection de
58552136 objets Python. Les graphiques v2 ont été rendus **depuis les exports**
pour lisibilité et puissance des raies ; aucun filtrage/acquisition refait.
Code numérique réel bb16202f ; provenance exacte dans manifest.json et render.json.

Quatorze tests ciblés PASS ; erreurs de préparation/dépendances et du premier
prototype SQL restent visibles dans les logs. Pas de build firmware/.NET :
aucune modification de ces composants. Aucun accès STM32/SD/COM effectué.

VALID_ESTIMATED : durée nominale entière et coverage99–101%, pas assurance de chaque point ; la fenêtre1s à5s comporte environ995ms valides, explicités dans coverage.
