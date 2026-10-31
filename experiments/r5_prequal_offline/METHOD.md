# Préqualification hors ligne — méthode figée avant parcours réel

Version `prequal-global-energy-v1`. Aucun KPI réglementaire ni seuil de confort.
Le filtre `TEST_LP10` est uniquement un **test de sensibilité numérique**, pas un
modèle de perception humaine. Il ne permet ni de prévoir un futur résultat ISO,
ni de choisir sa norme. Les résultats restent conditionnés à 0,244 mg/count nominal.

## Énergie globale et qualité

Lecteur strict et décodage en blocs R3/R5 existants, source entière hachée avant
et après, tous CRC/indices/footer vérifiés avant calcul. Aucun second parseur.
16 chaînes causales float64 : deux horloges estimées MCU et QPC, quatre bandes
1–2/2–5/5–25/25–80 Hz, total 1–80 Hz, test pondéré LP10, variantes basses 0,8/1,2 Hz.
Les mêmes échantillons et temps MCU définissent les fenêtres pour les deux horloges.
Pas de compensation de gravité dans la source ; le passe-bande retire DC dans
les métriques. Une inclinaison variable ou une marche d'offset peut néanmoins
produire de l'énergie en bande : le calcul ne sait pas séparer cette composante
physique des vibrations. Température absente du format archivé.

Chaque passe-bande est un Butterworth d'ordre prototype 4 (ordre total 8),
préwarpé aux deux bornes et réalisé en SOS. Réponse à −3 dB aux bornes ; les
bandes adjacentes **ne sont pas une partition orthogonale**. On exporte leur
somme d'énergie divisée par l'énergie totale, fenêtre par fenêtre. Les proportions
sont normalisées à la somme de ces quatre énergies, pas assimilées à des fractions
exactes de la variance totale. Réponses et coefficients sont publiés/hachés.

Test variant avec la fréquence : `H(s)=1/(1+s/(2*pi*10))` derrière le total 1–80,
transformation bilinéaire. Amplitude continue `1/sqrt(1+(f/10)^2)` ; déformation
bilinéaire publiée. La phase causale est incluse dans les contre-calculs.
Comparateur à poids plat : total 1–80. Aucune reconstruction de raie stationnaire.

État/settling/gaps réutilisés du Stream R5 : 11 premières sorties FIR invalides,
10 s d'établissement après initialisation, invalidité ou gap >10 périodes.
Pas de remplissage de trou. État stationnaire initial au premier XYZ valable.
Une fenêtre FULL exige toute sa durée et tous ses échantillons observés valides ;
partial/invalid n'est jamais remplacé par zéro. Les métriques comprennent les
RMS XYZ, max directionnel et norme énergétique XYZ (technique, non normative).
Les statistiques par seconde sont suffisantes pour recombiner 60/120 s par
somme des carrés et comptes, sans moyenne arithmétique des RMS.

## Oracles, contrôles et critères établis avant traitement

- Réponse SOS vs formule analytique Butterworth préwarp : rtol 1e-8, atol 1e-11.
- Sinus 0,3/0,5/0,8/1/1,2/2/5/10/20/40/80/100 Hz : amplitude/RMS à 0,5 % ;
  multitons à 0,1 %, DC, inclinaison, marches, AM/FM, impulsions, clips, gaps.
- Convolution FFT indépendante de la réponse impulsionnelle vs SOS : 1e-6 relatif,
  1e-8 absolu ; identité Parseval à la précision float64.
- Réel : trois périodes 120 s faibles/médianes/fortes, chacune aussi contrôlée
  en ses deux moitiés 60 s, pour les 16 méthodes. FFT complexe + transfert et
  Parseval ; contexte réel ±30 s et padding 60 s pour repousser les bords ;
  tolérance relative RMS 0,1 %, Parseval 1e-12. Les échecs sont conservés.
- Recombinaison exacte des paires alignées à 1e-11 relatif. Rapports max60/RMS120,
  rang Spearman et distributions sont des **résultats**, sans seuil favorable
  inventé. Comparaison des mêmes contenus ; les distributions globales FULL ont
  aussi des populations légèrement différentes aux bords, signalées explicitement.
- Tests de découpage des blocs, quantiles exacts et refus des écritures en place.

## Pleine échelle : portée distincte

Distribution exacte des codes int16 archivés par histogramme 65536×3, sans charger
les six heures en mémoire ; quantiles au rang supérieur, min/max et crêtes 1 s.
Distribution de tous les codes et distribution après exclusion des 11 warmups.
Les marges ±2/4/8 g sont **estimées** sur la sortie déjà FIR/décimée. Pas de
borne des chocs natifs perdus. Les flags SOURCE_RAIL/OUTPUT_CLIP sont interprétés
selon le code réel : rail numérique dans l'historique FIR contribuant à une sortie,
puis clamp int16 de cette sortie. Aucun flag ne prouve une absence de saturation
analogique, ni des crêtes entre instants, ni la queue native non émise au STOP.

Simulation séparée sur signal analogique connu : quantification aux pas nominaux,
FIR107 puis décimation /10, métriques RMS/spectre/vitesse. Jamais requantifier
l'archive ±8g pour prétendre retrouver une résolution ±2g. Le modèle de bruit
utilisé dans la simulation est déclaré et n'est pas une mesure à 1 Hz.

## Persistance et préservation

Les catalogues v1/v2/v3 existants sont intouchés. Le contrat interdit aussi toute
écriture dans v3 : une **copie unique dérivée v4** est donc nécessaire pour étendre
la cohorte. SQLite backup en lecture seule vers un fichier nouveau, conservation
et empreinte de toutes les lignes historiques ; ajout de tables préqualification
avec identités de méthode/calcul/source et refus de remplacer un résultat.
Un seul campaign_sha. Pas de XYZ bruts dans SQLite. Double import, intégrité,
FK et absence de changement des originaux contrôlés. Pas de nouvelle DB par KPI.
