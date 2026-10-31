# Voie spectrale PK — méthode dérivée v1, 31/10/2026

**Exploratoire, prototype non étalonné.** BV_NR467_F_2026_07_PK ; méthode
`pk-hann-idft-10s-60s.v1`, distincte des RMS2–25Hz et vitesses physiques1–80Hz.
Aucun Wm ni équivalent ISO pondéré n'est développé ici.

## Source de comparaison vérifiée

[Texte BV officiel, NR467 F juillet2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf),
SHA256 local5ae75d30f6b38a1331431e16bcc3dce11bc42c90b393f344be89b42cb050ceef,
688pages. Lecture du texte **et des pages rendues localement**, sans publier
les pages complètes sous droit d'auteur. IndexPDF0based238/246/247, pages
imprimées237/245/246 ; les numéros d'affichage1based sont donc239/247/248.

Ch6Sec1§2.1.3(b) : voie spectrale avecCF=1 ; pour **une composante sinusoïdale**,
crête=√2RMS de cette composante, arrondi au dixième seulement à l'affichage.
Sec3table5/6 : pour PCmachine occupé en permanence enmer, vitessecrête5–100Hz
4/4.5/5mm/s et accélérationcrête1–5Hz125/141/157mm/s². Correspondance locale,
axes prescrits, pose et instrumentation **non vérifiés** ; référence contextuelle,
aucun grade attribué. La table4RMSpondérée1–80Hz est une autre grandeur.

[ABS Ship Vibration2023](https://ww2.eagle.org/content/dam/eagle/rules-and-guides/current/conventional_ocean_service/147-guidance-notes-on-ship-vibration/147-ship-vibration-gn-sep-23.pdf),
§6/9.5.3 et définitions, distingue maximum répétitif/enveloppe et peak-hold
spectral ; l'égalité n'est pas garantie pour modulation rapide. Le
[guide LR](https://www.imorules.com/GUID-B6CCBEB7-2799-4E1F-9EAE-0A2181CEF867.html)
a été récupéré enHTTP malgré une erreur de décodage du navigateuroutillé ; il
souligne ces mêmes distinctions. Leurs facteurs/conventions ne sont **pas**
substitués auCF=1 du texteBV. L'ISO6954:1984 historique est retirée ; aucune
conformité formelle à l'ensemble de cette norme n'est revendiquée.

## Grandeur, fenêtre et estimation

Original XYZnominal .244mg/count, aucune conversion d'un RMSlarge bande.
Fenêtres non recouvrantes60sMCU, chacune divisée en6fenêtres10sMCU. À1Hz,
10s donne environ10cycles, à100Hz environ1000 ; compromis explicite entre
résolution et quasi-stationnarité. Le peak-hold de ces estimations n'est pas
un maximuminstantané du signaltotal ni une mesure garantie du MRA.

Après retrait de la moyenne constante (pas de suppression irréversible de
l'archive), Hann périodique `w[n]=(1−cos(2πn/N))/2` ; gaincohérentΣw/N=1/2.
FFTunilatérale, pas de zéro-padding donnant une prétendue résolution meilleure.
Résolutionnative≈1/T :0.1Hz(10s),0.0167Hz(60s), axesHz **estimés**, nonSI.
Les deuxhypothèses de cadence reprennent les mêmesfenêtres/XYZMCU.

Pour lemaximum local enbin k, estimation fractionnaire Hann :
`δ=2 Re[(X[k−1]−X[k+1])/(2X[k]−X[k−1]−X[k+1])]` bornée±0.5.
Cette identité suit la réponse de Hann pour une raieisolée à grandeN ; les
ratiosvoisins sont approximativement(δ−1)/(δ+2) et(δ+1)/(δ−2), leurcombinaison
donneδ/2. L'erreur àNfini et l'imagefréquence négative sont contrôlées par tests.

RéponseDTFT exacte normalisée deHann :
`W(δ)=[.5D_N(δ)−.25D_N(δ−1)−.25D_N(δ+1)]/(N/2)` avec
`D_N(t)=N exp(jπt(N−1)/N) sinc(t)/sinc(t/N)`.
La crête de raie vaut `A_pk_mg=2|X[k]|/(Σw·|W(δ)|)` ;
`f=(k+δ)fs/N`. Ceci corrige gaincohérent et scallopinghorsbin.
La PSDauditeur au bin vaut `2|X[k]|²/(fsΣw²)` enmg²/Hz, Σw²=3N/8.
LeRMSdecomposante=A_pk/√2 est stocké, sans sommer les harmoniques.

Unités : `a_pk=A_pk_mg×9.80665` enmm/s² ;
`v_pk=A_pk_mg×9.80665/(2πf)` enmm/s, intégration **de cette fréquence**.
Sélection parmaximum danschaquebande/axe, incluant5Hz danslesdeuxbandes mais
avecstatutfrontière. Trois candidats conservés enmémoire pourassocierla raie
surles6sous-fenêtres, même si descomposantes échangent leur rang.

## Conditions explicites avant comparaison indicative

- Entrée COMPLETEstricte ; pas de préallocation utilisée. Clip/gap/NaN/warmupFIR
  et fenêtre partielle invalident l'extraction ; pas d'amplitude0 substituée.
- Contraste amplitude>12dB parrapportàlamédiane desbinsdelabande, **pas** preuve
  debruitphysique calibré. Plancher1e-6mg uniquement numérique.
- Résiduelcomplexe deformeHann sur5bins≤0.15 ; raies trop proches ou forme
  incompatible ⇒ nonrésolu/nonsinusoïdal. Pairesde pics symétriques significatifs
  autour du candidat ⇒ modulationpossible/multisource, pas attribution causale.
- Proximité d'uneborne≤0.00679f+1/T ⇒ BAND_EDGE_UNCERTAIN : incertitude de
  scénario/résolution, pas une borne métrologique certifiée.
- Associer la fréquence60s à une candidate danschaquesous-fenêtre (écart≤0.2Hz),
  exigeant6/6valides. Étalementfréquentiel≤0.1Hz, max/minamplitude≤1.25,
  médianesousfenêtre/estimation60s∈[0.8;1.2]. Sinon METHODOLOGY_INDETERMINATE.
- Pour une composantequasi-stationnaire compatible, valeurretenue=maxdes6crêtes
  spectrales10s dela **même candidate**. Chaqueaxe est exporté. Pour qualifier
  uneminute/bande entière, **les3axes** doivent être recevables ; aucune omission
  d'un axeindéterminé pour obtenir une valeurplusfaible.
- Si la valeur est àmoins5% d'unrepère, réserve de méthode explicite ; pas de
  catégoriesrassurantes àla précision apparente du dixième. Les nombres bruts
  restent non arrondis. Égalitéexacte àunrepère identifiée séparément.

Méthode indépendante réelle : ajustementtemporel sin/cos +offset+dérivelinéaire,
fréquence optimisée dans±1/T autourdel'estimation, fenêtre rectangulaire sans
correctionFFT. Comparaison sur60s et surle10s qui porte lepeakretenu quandilest
recevable : tolérances **fixées avantessais**5% amplitude et0.02Hz fréquence.
Échec d'uncontrôle réputérecevable bloque toute lacatégoriehorloge/bande
correspondante, pas seulementunpoint gênant. Les divergences sont exportées.

Les minima/médianes/maxima desminutes et proportions parzone ne sont décrits
que pour les minutesrobustes, jamais unpourcentage delocauxconformes. Les zones
sont indicatives ; ni correspondance delocal ni calibration ne sont démontrées.
**Le PKspectral ne doit pas être assimilé au MRA d'un signalmodulé.**

## Tests et limites non levées

Sinus1/2/4.9/5/5.1/20/80/99/100/101Hz, phases/bins/horsbins : erreurisolée
amplitude≤1%,fréquence≤0.002Hz ; LSindépendant≤1%/1e-5Hz sursinusconnu.
Multitons/harmoniques, prochecouple, AM/FM, impulsion/bruit, DC1g, clip/gap/NaN,
courtesfenêtres et différenceraie/largebande testés. Le pruning emploie une
majoration1.3coarse (correctionHann≤1.179 etrapportbin/f≤1.059) ; recoupement
avec rechercheexhaustive, pas de suppression d'untest difficile.

Sensibilité, horloges, orientationnavire et couplage nonétalonnés. 0.68% entre
basesMCU/QPC, grille régulière approchée et UTC_UNKNOWN restent explicites.
Le FIR107Q15 est vérifié numériquement jusque100Hz, sans qualifier le MEMS,
lebruit ou l'ensemble anti-alias. L'intégritéinterne .TR2 est démontrée mais
le digestproducteurRAM indépendant reste perdu. Aucuneincidence structurelle,
expositionA(8), santé, causalitémachine, conformité ou gradedeclasse n'est déduite.

ArrondiFIR : ±0.122mg/échantillon nominal. Pour une fréquence **fixée**, la
majorationamplitude de l'effetdeserreurs d'arrondi est2×0.122/|W(δ)|mg
(≤environ0.288mg àδ=0.5), convertie dansl'unitédelacomposante etexportée.
Ce n'est pas uneincertitudetotale, ni unbruitblanc constaté, ni une borne de
l'erreur de fréquenceestimée. Les petitescomposantes/1Hz gardent cette réserve.
