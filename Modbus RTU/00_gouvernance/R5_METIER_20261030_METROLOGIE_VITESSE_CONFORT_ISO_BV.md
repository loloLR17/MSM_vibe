# MSM TR2 — R5-MÉTIER — Mesure de vitesse vibratoire et métriques de confort ISO / BV

**Date :** 30/10/2026. **Statut :** exigence métier explicite et feuille de qualification **à réaliser** ; étude de faisabilité fondée sur sources constructeur, ISO et BV. Ce document ne certifie **aucune** mesure TR2, n'attribue **aucune** notation de classe au MSM et ne déclenche pas de mission Codex concurrente.  
**Gouvernance :** `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, [cahier des charges V1](R0_METIER_20261029_CAHIER_DES_CHARGES.md), [vision R5](R5_METIER_20261030_VISION_ANALYSE_FREQUENTIELLE_CAMPAGNES.md) et [roadmap](R0_METIER_20261029_ROADMAP.md).  
**Décision utilisateur du 30/10/2026 :** la possibilité de présenter des **vibrations en mm/s, confrontées à des références de confort naval ISO/BV**, est un **objectif explicite et important de TR2**, indépendamment d'une notation COMF-VIB du MSM (jugée improbable mais non vérifiée par l'utilisateur) et sans hypothèse de dépassement des valeurs de référence. Objectif de communication : données factuelles, traçables et défendables auprès des Commandants et de l'Armement.

## 1. Hiérarchie de résultats imposée au logiciel

1. **Mesure principale** : niveaux RMS et crêtes d'**accélération** en mg/m/s², XYZ et global technique, priorité métier **2–25 Hz**, chronologies, épisodes, quantiles et comparaisons par emplacement/régime.
2. **Confort naval** : **vitesse vibratoire RMS pondérée 1–80 Hz**, en **mm/s**, par direction de mesure, avec procédure et coefficients de pondération conformes à la référence choisie, statut `EXPLORATOIRE` tant que l'ensemble de mesure et le protocole de terrain ne sont pas qualifiés. Fournir une comparaison **indicative** aux valeurs guides ISO 20283-5 et/ou grades BV COMF-VIB adaptés à **l'usage exact du local** ; aucun verdict de conformité de navire.
3. **Diagnostic complémentaire** : FFT/PSD, spectrogramme, suivi des raies/harmoniques, analyse de variation en régime et corrélation UTC aux machines. Ne pas sacrifier les mesures d'intensité pour les graphiques spectraux.
4. **Santé au travail A(8)** : domaine distinct ; ne pas inférer une exposition corporelle personnelle à partir d'un accéléromètre fixe sur une structure. Une éventuelle étude A(8) exige un protocole d'exposition humaine spécifique (interfaces corps/support, durées et pondérations).

**Point majeur :** l'évaluation normative ISO/BV utilise les valeurs **dans les directions prescrites**, notamment la **valeur la plus élevée parmi les directions mesurées** selon ISO 20283-5, et non automatiquement la norme euclidienne `sqrt(RMS_x²+RMS_y²+RMS_z²)` que nous utilisons pour l'indicateur technique global. Distinguer ces deux usages par des noms/colonnes séparés.

## 2. Faisabilité physique et calcul numérique

Pour un échantillon accélération `a(t)` (m/s²), la vitesse est l'intégrale `v(t)=∫a(t)dt` (m/s). Par composante sinusoïdale à `f>0`, `v_rms=a_rms/(2πf)`. Dans la transformée de Fourier `V(f)=A(f)/(j2πf)` ; la **PSD vitesse** vaut `S_v(f)=S_a(f)/(2πf)²`. Il faut donc **traiter les fréquences une à une** ; aucun multiplicateur universel ne transforme un **RMS 2–25 Hz global en mg** en **RMS vitesse 1–80 Hz en mm/s**.

Exemple analytique **non pondéré et non issu du MSM** : 10 mg RMS (avec `g_n=9,80665 m/s²`) donnent une vitesse RMS de **3,12 mm/s à 5 Hz**, **1,56 mm/s à 10 Hz**, **0,78 mm/s à 20 Hz**. La pondération normative modifie ces résultats.

**Deux implémentations indépendantes à comparer** :
- Référence A, temporelle : conversion ADC en m/s² avec sensibilité/étalonnage connus, correction des offsets et filtrage contrôlé (sans altérer le fichier source), intégration stable dans la bande utile, pondération Wm applicable et RMS par direction sur fenêtre.
- Référence B, spectrale : calcul PSD d'accélération de qualité contrôlée, transformation `S_v=S_a/(2πf)²` pour `f>0`, application du **carré de la réponse de pondération prescrite**, intégration correcte sur **1–80 Hz** pour obtenir le RMS, et contrôle Parseval/tests synthétiques. Pas de traitement artificiel ni de division au voisinage `f=0`.
- La **méthode de conversion/référence accélération-équivalent-vitesse** explicitement décrite par l'ISO 20283-5 devra être examinée dans son texte intégral et confrontée à la méthode générale d'intégration physique : ne pas les confondre ou substituer une constante unique hors de son cadre. Les réalisations devront être versionnées et tests croisés.

**Contraintes prioritaires** :
- **Bande 1–80 Hz** : le firmware expérimental et les premiers indicateurs 2–25 Hz ne prouvent **aucune** qualification à 1 Hz. Vérifier gains du capteur **et du FIR embarqué**, filtre(s), bruit, réponse à 1/2/5/10/20/40/80 Hz, couplages du montage, anti-aliasing et transitoires. Archives XYZ brutes/DC conservées : candidat technique, **pas preuve métrologique**.
- Bas-fréquence : retirer gravité/DC/offset sans faire disparaître la vibration à 1 Hz ; quantifier dérive d'intégration, bruit, fuite spectrale, fenêtre de mesure ≥1 minute (et durée supérieure si requise), transitoires de bord. Tester l'effet de roulis/tangage, de changement d'orientation et des mouvements non stationnaires.
- **Cadence et unités réelles** : horloge MCU/ODR non encore étalonnées physiquement, conversion ±8g nominale `0,244 mg/LSB` non vérifiée en gain/bande/bruit. Une fréquence erronée induit une erreur de conversion en vitesse ; une amplitude erronée est reportée à l'identique.
- **Axes et montage** : orientation repérée par rapport à vertical/longitudinal/transversal, capteur immobilisé rigidement sur une surface représentative de l'emplacement humain à évaluer ; les essais PC machine/atelier proches des moteurs renseignent d'abord des **vibrations structurelles locales**, non automatiquement un confort de passager assis ou d'équipier au poste.
- **Qualité de campagne** : CRC/sequence/session/footer, lacunes, saturation/rail, warmup FIR, métadonnées UTC + incertitudes, délais de filtrage, fenêtres incomplètes. Ne jamais combler une perte silencieusement ; `COMPLET` ne veut pas dire `ÉTALONNÉ`.
- **Instrumentation** : prévoir la comparaison in situ sur plusieurs fréquences/niveaux à un appareil triaxial **étalonné et adapté aux vibrations de confort**, incluant vérification périodique et contrôle avant/après essai. Le seul capteur ST sur NUCLEO et un code de traitement non qualifié ne donnent pas une conformité ISO 8041.

## 3. Références normatives : conditions et statut

### ISO 20283-5:2016

- Norme **maintenue/confirmée en 2026** par ISO, remplaçant ISO 6954:2000 : `1–80 Hz`, **RMS global pondéré en fréquence**, normes de mesure/traitement/lieux. Son champ officiel vise les **voyages prévus de 24 heures ou plus**. Le MSM enchaîne de courtes traversées de jour/nuit avec ~1h30 d'escale ; ce contexte d'exposition prolongée est pertinent, **mais ne rend pas automatiquement la norme formellement applicable**.
- La pondération **Wm** selon ISO 2631-2 est à appliquer **dans les directions demandées** ; ne pas importer sans revue les coefficients de la version 2003 alors qu'une **nouvelle ISO 2631-2:2026** est publiée. Examiner la relation des éditions et la référence normative pertinente pour ISO 20283-5 ; le texte intégral, notamment la relation entre vitesse et accélération pondérées, les tolérances, les conditions de pose et les durées, est un **prérequis** à toute implémentation revendiquée conforme.

### Bureau Veritas NR467, Partie F, chapitre 6, juillet 2026

- **Notations additionnelles COMF-VIB-Pax / COMF-VIB-Crew** pour les zones passagers/équipage ; grades 1/2/3 fondés sur le RMS pondéré 1–80 Hz, grades `1PK/2PK/3PK` distincts pour critères de crête.
- Repères **strictement informatifs** extraits des tableaux BV de l'édition citée, en mm/s RMS pondérés, grades 1/2/3 : **cabine passagers standard 2,0 / 2,5 / 3,0** (Pt F, Ch 6, Sec 4, Tab 4) ; **cabine équipage 2,8 / 3,0 / 3,2** et **PC machine occupé en permanence en mer 4,0 / 4,5 / 5,0** (Pt F, Ch 6, Sec 3, Tab 4). Les valeurs dépendent de l'**usage précis** et de la catégorie du local.
- L'édition NR467 publiée **juillet 2026** est applicable en principe aux contrats de construction à partir du **1er juillet 2026**, sous réserve de renvois/règles spécifiques. Elle est **une référence de benchmark technique**, pas une obligation contractuelle prouvée pour le MSM existant. À confirmer auprès des certificats de classe et de BV si qualification formelle souhaitée ; l'utilisateur n'a pas connaissance d'une notation COMF-VIB du MSM.
- Protocole BV : appareils étalonnés, points/axes de mesure et états d'exploitation définis, conditions de mer/puissance et rapport documentaire ; ne pas déduire la conformité d'une mesure isolée, d'une fenêtre arbitraire ou d'une carte de chaleur TR2.

### Source fabricant et principes DSP

- ST IIS3DWB : capteur triaxial **±8 g** possible, réponse annoncée depuis le continu ; la fiche **ne qualifie pas** la chaîne `IIS3DWB → FIR/10 → fichiers TR2 → PC` à 1 Hz.
- ISO 8041-1:2017 : spécifications/tolérances/validation pour instruments de réponse humaine, comprenant explicitement des systèmes d'acquisition et d'analyse informatisés ; voie technique compatible avec notre architecture, **non équivalence automatique à un appareil validé**.
- ISO 13373-2:2016 et références fabricant d'analyse : passage accélération/vitesse par intégration temporelle ou spectrale, prudence en bas de bande.

## 4. Plan de qualification pratique et critères d'acceptation

**Q0 — Référentiel** : accès licite aux méthodes complètes applicables, préciser pondération Wm/édition, définitions RMS directionnelles, filtres, fenêtres, points de pose et classes locales. Pas de calcul baptisé « ISO » avant levée de cette porte.

**Q1 — Référence numérique** : signaux synthétiques DC et sinusoïdes précisément fixées (1/2/5/10/20/40/80/100 Hz), multisinus, transitoires, bruit, offset, tilt, lacunes, saturations ; calculs intégration par deux voies indépendantes, comparés aux valeurs théoriques dans des **tolérances définies avant les essais**. Vérifier explicitement le RMS directionnel vs somme vectorielle, réponse Wm, erreur temporelle, durée, Parseval et limites à 1 Hz.

**Q2 — Chaîne instrumentée** : caractériser matériel+FIR+fichier sur excitations contrôlées, bande 1–80, gain/phase, bruit/fond, horloge, orientation, sensibilité, incertitude. Comparer à un vibromètre triaxial calibré/traçable et documenter les écarts. Si 1–2 Hz insuffisant, afficher `NON_QUALIFIÉ_1_80_HZ` plutôt que falsifier une vitesse « ISO/BV ».

**Q3 — Terrain représentatif** : documenter point et fixation (photo, axes), date UTC avec qualité, configuration machines, stabilité, météo/mer si possible. Campagne exploratoire fixe en atelier/PC machine puis lieux passagers/équipage adaptés. Répétition au même point avant comparaisons multi-sites. Ne pas perturber ni réutiliser le TR2-002 tant que sa campagne autonome R3-8H et ses preuves ne sont pas clôturées et sécurisées.

**Q4 — Restitution défendable** : rapport avec RMS accélération mg, **vitesse RMS pondérée mm/s par axe** + axe retenu + pondération/version + fenêtre/provenance/qualité/incertitude ; valeurs guides BV/ISO citées et statut d'applicabilité ; vue temporelle (percentiles, épisodes, conditions), spectrogramme et corrélation machines. Afficher un bandeau `COMPARAISON INDICATIVE NON NORMATIVE` tant que la chaîne n'est pas qualifiée. Aucun badge `Conforme BV` sans démarche reconnue.

## 5. Sources officielles et documentations associées — URLs vérifiées en consultation (30/10/2026)

1. **[ISO 20283-5:2016 — fiche ISO officielle et domaine d'application](https://www.iso.org/standard/68125.html)** ; [aperçu détaillé ISO (OBP)](https://www.iso.org/obp/ui?_escaped_fragment_=iso%3Astd%3Aiso%3A20283%3A-5%3Aed-1%3Av1%3Aen) : habitabilité et RMS pondéré 1–80 Hz ; voyages prévus ≥24 h.
2. **[Bureau Veritas NR467, Partie F, juillet 2026 — PDF officiel](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf)** : Ch 6 Sec 1 méthode/conditions/étalonnage, Sec 3 Tab 4 équipage et PC machine, Sec 4 Tab 4 passagers ; [page officielle de publication BV](https://marine-offshore.bureauveritas.com/nr467-rules-classification-steel-ships). Respecter portée d'édition pour navire ancien.
3. **[ISO 2631-2:2026 — fiche officielle](https://www.iso.org/standard/87847.html)** : pondération `Wm` 1–80 Hz, version mise à jour, édition exacte à étudier avant codage d'une pondération normative (remplace ISO 2631-2:2003).
4. **[ISO 8041-1:2017 — fiche officielle](https://www.iso.org/standard/70648.html)** : tolérances d'instrumentation, validation, vérifications périodiques/in situ ; systèmes acquisition+traitement sur PC explicitement admis.
5. **[ISO 13373-2:2016 — traitement et présentation des vibrations](https://www.iso.org/standard/68128.html)** : analyse de signaux dans les domaines temporel et fréquentiel et intégration accélération-vitesse.
6. **[STMicroelectronics IIS3DWB — fiche produit](https://www.st.com/en/mems-and-sensors/iis3dwb.html)** et [datasheet DS12569](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) : sensibilité, bande, bruit, configurations.
7. **[ONO SOKKI, fabricant d'analyseur FFT — intégration accélération → vitesse](https://www.onosokki.co.jp/English/hp_e/c_support/faq/fft_common/fft_general_10.htm)** : filtrage et intégration temporelle/fréquentielle ; ressource pédagogique, non texte normatif.

**À ne pas faire dans cette note** : publier des valeurs réelles de confort du MSM à partir des 10 min tests sur table ; promettre une certification, une exactitude 1–80 Hz déjà acquise ou une notification juridique de dépassement ; modifier l'échantillonnage, les filtres firmware ou la mission R3 autonome en cours. Les sources ISO sont sous droit d'auteur ; seuls leurs principes généraux et références sont consignés, pas des copies du texte.
