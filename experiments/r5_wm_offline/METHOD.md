# TR2-Wm v1 — indicateur local, déterministe et non certifié

Indicateur principal unique `TR2_WM_A_MAX`, mm/s² : appliquer le même filtre aux
XYZ nominaux, RMS de chaque axe sur la même fenêtre, puis maximum des trois RMS.
Exporter axe gagnant et XYZ. Ni norme vectorielle, ni pic, ni RMS global×constante.
Secondaire dépendant `equivalent_v_mm_s=0.028*a_max`, pas une seconde mesure ni
l'intégration physique non pondérée calculée dans l'ancienne mission.

## Fonction numérique retenue, figée avant essai réel

Produit de trois étages analogiques : passe-haut Butterworth2 à10^−0.1Hz,
passe-bas Butterworth2 à100Hz, passe-bas1 à1/(0.028×2π)Hz. Réalisation originale
zéros/pôles, bilinéaire sans préwarping, SOS float64. Les deux limitations font
partie de la fonction Wm : aucun second Butterworth1–80 ajouté. Bande utile
1–80Hz **avec raccordements lisses**, pas fenêtre rectangulaire qui changerait
la réponse à1Hz/80Hz. Des fractions PSD hors bande sont diagnostiquées séparément.
Sous bilinéaire, l'oracle est la fonction analogique évaluée à
`f_equiv=fs/pi*tan(pi*f/fs)` ; module/phase et déformation sont exportés.

Les quatre points publics sélectionnés sont aux centres **réels** des tiers
d'octave :1, 10^0.6,10,10^1.9Hz ; valeurs arrondies0.833/0.818/0.494/0.0604.
Comparer79.4328Hz à l'étiquette nominale80Hz évite de corriger à tort le filtre.
Ces points contrôlent une fonction analytique, ils ne servent pas d'interpolation
ni de transcription exhaustive d'une table protégée. Les petites variantes
LOW08/LOW12 multiplient uniquement f1 par0.8/1.2 : diagnostics, pas KPI principal.

## Provenance et réserves d'édition

GOST R ISO20283-5:2023 déclaré IDT à2016 : registre officiel Rosstandart et
texte national public, §7–8/annexeA ; choix du maximum d'axe/RMS, unités et
relation vitesse. La définition mathématique historique est contrôlée contre
l'annexeA du texte national GOST31191.2-2004 (adoption **MOD**, pas IDT global,
de2631-2:2003), et le projet ISO/FDIS2631-2:2026 publiquement lisible chez le
distributeur. Le catalogue ISO confirme la nouvelle édition2026. Le texte final
public ISO/OBP masque son annexe normative ; le contenu public du distributeur
présente le même produit d'étages, mais ses formules finales sont perdues dans
le rendu texte. **Concordance numérique des relations accessibles, pas preuve
exhaustive d'identité de toutes les exigences finales2003/2026/8041.**
Cette réserve d'édition reste explicite ; elle ne bloque pas le calcul local.
Aucun PDF/texte intégral/figure/table exhaustive n'est publié, aucune norme achetée.
Les références exactes et éléments effectivement visibles seront dans REFERENCES.md.

## Qualité, contexte et tolérances fixées avant inspection des données Wm

Réutiliser lecteur strict et Stream/Moments R5 inchangés : 11warmups, 10s settling,
reset après invalidité/gap>10périodes, aucun remplissage. FULL exige toute durée
et tous échantillons valides ; fenêtres60/120s alignées sur MCU, mêmes données
pour MCU/QPC. Période entière valide inclut la dernière fenêtre partielle.
Une variante diagnostique exclut17600–17630s **seulement des sommes d'énergie**,
sans modifier la source ni l'état du filtre ; fenêtres affectées restent PARTIAL,
pas de suppression d'épisode dans le résultat principal. Ce périmètre a été fixé
avant le calcul, sur l'épisode déjà documenté par la mission précédente.

- Quatre valeurs publiques : écart absolu≤0.0005, ou0.00005 pour le petit gain80Hz.
- SOS complexe vs formule rationnelle indépendante : rtol1e−8/atol1e−10.
- Bilinéaire vs analogique aux points0.5–100Hz : écart module<1.2%, quantifié aussi
  par grille dense1–80. Ce n'est pas la tolérance ISO d'un instrument.
- Sinus connus : RMS≤0.1% ; unités1mg=9.80665mm/s²/axe gagnant/vitesse dérivée.
- Multitons, AM/FM/bruit/chocs : SOS vs convolution FFT indépendante,1e−6/1e−8.
- DC/offsets/inclinaison, gaps/clips/vides, causalité et invariance au chunking.
- Réel : trois périodes faibles/médianes/fortes120s plus épisode s'il est distinct,
  chacune avec ses deux60s ; FFT+fonction rationnelle+Parseval, contexte±30s,
  padding60s, limite relative0.001/Parseval1e−12. Conservation des échecs.
- Agrégation exacte des deux énergies60 vers120 par compte : seuil1e−11 hérité,
  pas de moyenne arithmétique des RMS. Rapports/rangs sont des résultats.

Les dérives/inclinaisons variables peuvent produire de l'énergie ; aucune
séparation physique garantie de la gravité dynamique. FIR107/réserves R3/FS/gain/
axes/pose/UTC et bruit1Hz non qualifiés restent ceux de l'archive. Aucune action
matérielle. Lot BV **après** validation numérique, métadonnées séparées ;
catégorie initiale NON_CLASSÉE et aucun seuil automatique.
