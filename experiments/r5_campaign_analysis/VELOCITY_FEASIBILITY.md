# Accélération → vitesse et confort : bilan R5 du 31/10/2026

**Étude contrôlée, non normative.** Les résultats réels livrés sont des
accélérations nominales filtrées 2–25Hz ; aucun indicateur de confort Wm, grade
BV, exposition humaine ou dommage structurel n'est calculé ici.

## Démonstration numérique acquise

Pour une accélération sinusoïdale de crête A en mg à la fréquence f :

`v_RMS = A × 9.80665 / (2π f √2)` en mm/s.

Le facteur9.80665 convertit des mg en mm/s², **pas des mg en mm/s**.
L'intégration dépend de la fréquence. Le moteur synthétique utilise
`FFT(v)[f] = FFT(a)[f] × 9.80665/(j2πf)` seulement dans1–80Hz, DC exclue.
Il suppose ici un signal périodique à fréquences exactement sur les bins ; ce
n'est pas encore un algorithme qualifié pour transitoires arbitraires en campagne.

Les tests effectivement exécutés couvrent1/2/5/10/20/40/80Hz, crête10mg,
offset1000mg, ainsi que multitons et signaux hors bande0.5/100Hz. Le calcul
spectral est confronté à la formule et à une intégration temporelle indépendante
par trapèzes après retrait de moyenne et centrage de la vitesse. Les écarts
attendus de discrétisation temporelle ne sont pas masqués.

Sur les sept fréquences, le CSV réel `velocity_synthetic.csv` donne une erreur
relative spectrale maximale8.89e-16 ; la méthode par trapèzes atteint0.12553%
à80Hz pour fs4096Hz. Critères fixés dans les tests :1e-10 relatif spectral et
0.2% temporel. Multitons vérifiés par somme quadratique des contributions
orthogonales ; offset et composantes hors bande exclus. Ces preuves sont
**synthétiques uniquement**, sans qualification de sensibilité, de cadence ou
réponse physique du TR2 à1Hz.

## Pourquoi les RMS réels ne sont pas convertis par un coefficient

L'archive conserve XYZ/DC ; elle permet une nouvelle analyse1–80Hz sans perte
irréversible de la composante1–2Hz. Le présent résultat2–25Hz ne suffit pas : le
filtre vaut environ−26.29dB à1Hz et−43.13dB à80Hz. Il faut repartir de l'archive
brute, pas intégrer ce résultat filtré comme s'il couvrait1–80Hz.

Le bruit, le biais/DC, les transitoires, les lacunes et les dérives de temps
peuvent dominer une intégration. L'amplitude nominale non étalonnée, la pose au
double-face et la nappe ne constituent pas une chaîne de mesure qualifiée.
Sur E6N0005, le témoin QPC donne une cadence de sortie estimée2679.5084–2679.5240Hz
sur son intervalle actif, contre2661.4620Hz sur les endpoints MCU de toute
l'archive : différence de bases d'environ0.678%. La QPC n'est elle-même pas un
étalon SI ; aucune correction physique définitive n'est adoptée. Cet écart affecte
les fréquences et le facteur d'intégration, et interdit de revendiquer une
précision métrologique à partir de la seule finesse des bins FFT.

Les axes sont XYZ capteur, sans correspondance prouvée aux directions du navire.
Le RMS vectoriel technique additionne les puissances ; il ne doit pas remplacer
silencieusement les résultats directionnels prescrits pour un indicateur de
confort. Aucun seuil de gêne n'est appliqué aux mg non pondérés.

## Textes examinés et édition à verrouiller

La [fiche officielle ISO20283-5:2016](https://www.iso.org/standard/68125.html),
confirmée en2026, décrit des RMS pondérés1–80Hz et un domaine de voyages prévus
d'au moins24h. Elle ne démontre pas l'applicabilité automatique aux traversées
plus courtes du MSM ; le texte intégral applicable et ses renvois restent à obtenir.

Le [texte BV NR467 partieF, juillet2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf),
chapitre6 §2.1.2–2.1.3 (pages imprimées236–237), distingue instrumentation,
étalonnage et traitement ; la voie ISO20283-5 utilise une vitesse RMS pondérée
1–80Hz. Ses limites directionnelles/local/grade ne sont pas transposables au
RMS global mg de cette mission. Notation COMF du MSM et édition contractuellement
applicable non établies ; aucune attribution de classe n'est faite.

La [fiche ISO2631-2:2026](https://www.iso.org/standard/87847.html) définit Wm dans
son annexeA pour1–80Hz, dans le contexte des bâtiments. Cela n'autorise ni à
inventer ses coefficients, ni à substituer cette édition aux références d'une
méthode marine2016 sans revue. Il faut identifier les textes/éditions exacts,
la fonction de transfert prescrite, ses tolérances et le protocole de mesure.
**Wm non implémenté** ; aucun texte payant n'a été acheté, aucun filtre générique
n'est baptisé Wm et aucun résultat réel en mm/s n'est publié comme conforme.

Pour les calculs exploratoires exécutés, les descriptions des filtres et PSD
s'appuient sur les documentations de la version utilisée :
[SciPy1.17 Butterworth](https://docs.scipy.org/doc/scipy-1.17.0/reference/generated/scipy.signal.butter.html),
[SciPy1.17 Welch](https://docs.scipy.org/doc/scipy-1.17.0/reference/generated/scipy.signal.welch.html).
Ces bibliothèques ne rendent pas l'instrument conforme à une norme de mesure.

## Plus petite étape suivante, après les livrables réels R5

1. Verrouiller la méthode de référence et ses éditions, obtenir les coefficients
   Wm et tolérances autorisés ; définir quel résultat directionnel sera comparé.
2. Sur banc de durée courte, avec fixation rigide/orientation documentées et une
   référence étalonnée, vérifier amplitude/cadence et réponse1/2/5/10/20/40/80Hz,
   puis quelques signaux transitoires. Inclure la chaîne FIR/enregistrement,
   les marges bruit/rail, la tenue temporelle et le retrait DC.
3. Qualifier séparément l'intégration1–80 et la pondération versionnée sur
   références analytiques puis par confrontation physique ; choisir les bords,
   durées et règles d'invalidité avant de comparer des locaux équivalents.

Aucune nouvelle campagne longue n'est nécessaire pour cette prochaine étape.
Budget, devis et disponibilité d'un moyen étalonné restent à décider ; aucune
promesse de précision, de retour financier ou d'effet sur le confort n'est inventée.
