# MSM TR2 — R5-MÉTIER — Niveaux vibratoires, confort, analyse fréquentielle et campagnes de référence

**Date :** 30/10/2026. **Statut :** cadrage fonctionnel de ChatGPT issu des échanges utilisateur, non spécification normative gelée, non ordre d'exécution.  
**Référence :** `loloLR17/MSM_vibe/main`, gouvernance `AGENTS.md` / `GOUVERNANCE_CHATGPT_CODEX.md`, roadmap R0-MÉTIER (R4–R6), cahier des charges métier V1-01 à V1-12 et D1–D4.  
**Périmètre :** analyse PC, exports et campagnes d'observation à bord ; aucune modification du firmware, du matériel, des contrats Modbus historiques, ni de `MISSION_EN_COURS.md`.

## 0. Hiérarchie fonctionnelle impérative — clarification du 30/10/2026

**Priorité 1 : combien ça vibre, où et pendant combien de temps ?** L'objectif premier de TR2 est de **mesurer et comparer l'intensité vibratoire** pour objectiver les désagréments potentiels aux passagers et à l'équipage. Produire des séries RMS X/Y/Z/global, crêtes, enveloppes, temps de persistance, répartition des niveaux et comparaison entre locaux, manœuvres et régimes, avec bandes/algorithmes/unités/qualité explicites. L'analyse temporelle et son exploitation précèdent les visualisations sophistiquées.

**Priorité 2 : pourquoi cela vibre-t-il ?** FFT/PSD, spectrogrammes, fréquences caractéristiques, intensité et suivi des raies, familles harmoniques et corrélations machines sont des diagnostics complémentaires **attendus et conservés dans le périmètre**, jamais un remplacement des indicateurs d'intensité.

**Séparation des grandeurs obligatoire** : le **RMS technique 2–25 Hz (mg, non pondéré pour le confort)** mesure une intensité dans une bande et ne permet **pas à lui seul** de noter le confort humain. Étudier ensuite un **RMS accélération pondéré pour le confort** adapté au lieu, aux axes et aux personnes exposées, en conservant les deux mesures distinctes et une dénomination non équivoque. Les seuils, classes et notes de gêne ne sont **pas gelés** : pas de conformité, d'alarme confort ou de diagnostic sanitaire automatique avant qualification de la chaîne, des filtres, du montage et du protocole.

**Références à étudier, sans revendication de conformité** : [ISO 20283-5:2016](https://www.iso.org/standard/68125.html) remplace ISO 6954:2000 et traite l'habitabilité des navires avec RMS pondéré 1–80 Hz, mais annonce un champ d'application pour les voyages prévus de **24 h ou plus**, pas une applicabilité directe automatiquement acquise pour les traversées courtes du MSM. [ISO 2631-1:1997 amendée](https://www.iso.org/standard/7612.html) traite les vibrations du corps entier et le confort selon les surfaces de contact. Le texte complet en vigueur, les pondérations et les conditions d'essai devront être étudiés avant toute revendication normative. Un capteur fixé dans un local ou à une structure ne reproduit pas nécessairement l'exposition sur le siège, le plancher ou une surface de contact humain.

**Complément de référence du 30/10/2026 (situation ferry MSM) :** les traversées sont courtes mais les escales d'environ 1 h 30 et l'exploitation quasi permanente créent un contexte d'exposition prolongée, notamment pour l'équipage. La formulation « voyages prévus de 24 h ou plus » d'ISO 20283-5 ne peut pas être assimilée sans justification à une exploitation continue. Retenir cette ISO comme **base technique volontaire**, en attendant l'examen du texte intégral et de son applicabilité formelle.

**Référence de classification concrète :** [Bureau Veritas NR467 Partie F, ch. 6 (juillet 2026)](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf) inclut le cas des **ferries** parmi les navires >=1600 GT ; notations complémentaires **COMF-VIB Pax** et **COMF-VIB Crew**, avec règles de mesure par zone et par axe et critères en **vitesse RMS pondérée 1–80 Hz (mm/s)**. Ne pas appliquer les seuils BV aux RMS d'accélération TR2 2–25 Hz en mg ; étudier séparément un **indicateur de vitesse vibratoire pondérée**, dérivé des archives XYZ avec algorithme qualifié et montage représentatif. L'affectation effective d'une notation BV au MSM n'est pas connue ; l'application contractuelle et les règles de classe correspondant au navire restent à vérifier. Ne pas annoncer de résultat « conforme BV » ou « conforme ISO » sans capteur étalonné, protocole d'essais, textes applicables et validation appropriée.


## 0 bis. Ordres de grandeur de référence — revue documentaire du 30/10/2026

**Portée et sources** : il s'agit de repères de *comparaison technique* et de préparation de mesure, **pas** d'une affirmation de conformité ou de dépassement réglementaire sur le MSM. Référentiel BV [NR467 Partie F, chap. 6, édition juillet 2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf), sections 1, 3 (équipage >=1600 GT), 4 (passagers >=1600 GT) ; [ISO 20283-5:2016](https://www.iso.org/standard/68125.html) confirmée en 2026 ; droit social maritime français [décret n°2005-748 du 4 juillet 2005, en vigueur](https://www.legifrance.gouv.fr/loda/id/LEGITEXT000006051961). Il faut vérifier les notations effectivement détenues par le navire, la version contractuellement applicable et l'édition BV pertinente **pour ce ferry existant** : NR467 de juillet 2026 concerne normalement les navires dont le contrat de construction est postérieur au 1er juillet 2026, sauf disposition/reprise prévue par BV. Le fonctionnement presque continu du MSM n'établit pas qu'il effectue un « voyage prévu de 24h ou plus » au sens ISO.

### Confort BV/ISO : vitesses RMS pondérées, non doses quotidiennes

- Critère principal COMF-VIB grades 1/2/3 : **vitesse vibratoire RMS pondérée 1–80 Hz (mm/s), dans les directions prescrites**, avec **grade 1 le plus strict** ; méthode alternative COMF-VIB 1PK/2PK/3PK pour la crête, **à ne pas confondre** avec RMS.
- Quelques repères BV en mm/s RMS, par grades 1/2/3 : cabine passagers standard **2,0 / 2,5 / 3,0** ; cabine équipage **2,8 / 3,0 / 3,2** ; PC machine **occupé en permanence à la mer** **4,0 / 4,5 / 5,0**. Les seuils sont **propres aux locaux**, non une séparation « passager / équipage » universelle. Exiger la correspondance locale de l'emplacement de mesure.
- ISO 20283-5:2016 : valeurs guides maximales par types de locaux, relevées dans les publications techniques publiques : cabines/publics passagers **3,5 mm/s**, logements équipage **3,5 mm/s**, lieux de travail **6,0 mm/s**, passerelle/PC machine **5,0 mm/s**. Vérifier la table normative originale lors de l'étude de conformité ; ne pas substituer ces guides ISO aux grades BV.
- ISO 20283-5 prévoit des mesurages d'au moins **1 min** et recommande **2 min** si des composantes sensibles sous 2 Hz sont suspectées ; vérifier les détails de protocole. RMS sur une fenêtre représentative, **pas** un dépassement d'accélération instantané, **pas** une limite d'exposition professionnelle. BV NR467 §2.2.2(d) admet **un critère de tolérance sur la proportion de points** : au plus 20 % des mesures des espaces considérés peuvent *dépasser le critère de plus de 0,3 mm/s* ; cela n'est **pas** une tolérance absolue individuelle « +0,3 mm/s max ». Ne pas déduire le statut d'un navire d'une mesure isolée.
- Mesures officielles BV : conditions de marche/puissance/régime représentatives, local/directions/points prescrits, instrumentation vérifiée, spécialiste qualifié ; nos campagnes TR2 continues sont d'abord des **outils de diagnostic et de surveillance exploratoire**, sans notation automatique.

### Santé au travail : régime français maritime distinct du confort

- Le **décret n°2005-748** s'applique aux personnels employés à bord des navires : exposition **corps entier A(8)**, accélération pondérée en **m/s²**, et **durée réellement subie**, ramenées à une période de référence de 8 h. **0,5 m/s² = valeur déclenchant l'action de prévention** ; **1,15 m/s² = valeur limite d'exposition journalière**, avec dérogation maritime exceptionnelle encadrée (art. 9). Vérifier les textes associés (notamment l'arrêté du 6 septembre 2005 applicable aux gens de mer).
- Illustration théorique mono-épisode (pas valeur TR2) : `A(8)=a_pondérée*sqrt(T_exposition/8h)` sous hypothèses de méthode compatibles ; **1 m/s² pendant 2h** et absence d'autre exposition conduit à **0,5 m/s² A(8)**.
- **Ne jamais comparer** le RMS d'accélération 2–25 Hz en **mg** du TR2 ni le RMS de **vitesse en mm/s** du confort directement à la dose professionnelle A(8). L'accélération **au point de contact du corps**, les axes/pondérations ISO 2631-1, l'occupation effective et la représentativité du montage sont indispensables.
- Le bruit ressentí au PC machine, éventuellement justifiant des protecteurs auditifs, appelle un examen **acoustique distinct** (pression sonore dB(A), impulsions, exposition) ; une vibration structurelle ne donne pas à elle seule un niveau acoustique.
- Pour l'architecture logicielle, prévoir à terme trois vues **distinctes et tracées** : (a) RMS d'accélération technique TR2 en mg ; (b) confort BV/ISO vitesse RMS pondérée 1–80 Hz en mm/s avec origine normative et statut `EXPLORATOIRE` tant que la mesure n'est pas qualifiée ; (c) exposition professionnelle A(8) en m/s² **uniquement si** protocole de mesure humaine et durées d'exposition fiables, sans inférer une dose d'un capteur fixe dans un atelier.

**Exigence complémentaire validée le 30/10/2026 — V1-13 :** l'utilisateur fixe comme objectif du **TR2** la mesure de **vitesse vibratoire depuis les accélérations archivées** et la présentation de **RMS pondérés 1–80 Hz en mm/s**, comparables à des références de confort **ISO 20283-5 / BV COMF-VIB**, indépendamment d'une notation COMF-VIB détenue (non connue et jugée improbable pour le MSM). L'outil doit afficher ces métriques à côté du RMS technique d'accélération 2–25 Hz, sans les confondre. **Priorité d'exécution R5 :** après le socle RMS technique, développer et qualifier **intégration + pondération + indicateurs directionnels de confort** avant le raffinement cosmétique du spectrogramme ; maintenir l'analyse fréquentielle nécessaire à cette conversion et au diagnostic. Voir la [fiche technique et le plan de qualification référencé aux sources officielles](R5_METIER_20261030_METROLOGIE_VITESSE_CONFORT_ISO_BV.md). Aucune affirmation de conformité ni calcul 1–80 Hz certifié tant que la chaîne, l'instrument et le montage ne sont pas qualifiés.

## 1. Décisions métier exprimées par l'utilisateur

1. Le TR2 est un enregistreur autonome ; les campagnes peuvent durer indépendamment des traversées.
2. Première livraison : import et traitement **local** sur le T1000, robustes même sans connexion au réseau ; données exportables dans un format durable, avec réutilisation des acquis de la supervision PC. Les présentations Grafana **local** puis la corrélation **BigQuery/Grafana MSM** sont des extensions prévues, pas des prérequis bloquants.
3. Visualisations attendues : tendances RMS/crêtes XYZ et globales, **spectrogramme temporel**, suivi des **fréquences caractéristiques et de leurs amplitudes dans le temps**, FFT/PSD sur sélection, harmoniques, comparaisons et annotations. C'est l'une des présentations prioritaires voulues par l'utilisateur.
4. Métier fréquentiel D1 : **2–25 Hz prioritaire**, 2–700 Hz cible étendue à qualifier ; ne pas confondre domaine de calcul possible et bande métrologiquement qualifiée.
5. Datation D4 : **événements corrélables en UTC à la seconde**, cible indicative de justesse **de l'ordre de ±1 s** lorsqu'elle peut être démontrée ; conserver temps relatif, ancrages UTC, provenance, méthode, incertitude et dérive. `UTC_UNKNOWN` si aucune référence justifiée ; aucune conversion inventée des dates FAT32. Pas de déphasage intercapteurs exigé en V1.
6. Le matériel expérimental posé sur une table peut capter des vibrations structurelles provenant des machines du bord. Ne **pas** affirmer qu'il n'en capte pas ; ne **pas** attribuer pour autant une raie observée à une machine précise sans corrélation/contrôle. Les manipulations non documentées restent des sources de confusion.

## 2. Maquette fonctionnelle des représentations (à qualifier et valider)

### Tableau de bord principal

- Bandeau : campagne, capteur/UID, point de mesure, orientation, statut d'intégrité, couverture valide, durée **effective** et statut temporel UTC.
- **Vue de tête et KPI métier en premier** : intensité RMS X/Y/Z/global 2–25 Hz sur 1/10/60 s ou fenêtres qualifiées, crêtes distinctes, durée/persistance des niveaux élevés, quantiles, comparaison au même point et couverture valide. Ajouter **ultérieurement** un indice de confort humain pondéré et explicitement distinct, seulement après étude normative et qualification du site de mesure.
- Chronologie synchronisée : **RMS 2–25 Hz** et, si qualifiée, 2–700 Hz ; composantes XYZ/global, **crêtes et enveloppes**, annotations d'événements, tracé explicite des lacunes et anomalies de qualité.
- **Spectrogramme principal** : abscisse = durée écoulée ou UTC qualifiée, ordonnée = fréquence (zoom 2–25 Hz / vue 2–700 Hz), couleur = densité spectrale/intensité avec unités et référence documentées. Une raie horizontale indique une fréquence stable ; une diagonale indique une évolution ; l'amplitude change dans la couleur.
- **Suivi des raies** : courbes `f_i(t)` pour plusieurs raies candidates, et **courbes distinctes d'intensité** `A_i(t)` / puissance, persistance et interruptions. Ne pas raccorder arbitrairement deux pics dominants successifs : plusieurs sources peuvent coexister et échanger leur rang.
- Sur sélection : spectre PSD/FFT détaillé, XYZ séparés ou PSD globale définie explicitement, RMS/crêtes par sous-fenêtre, données source, qualité et provenance ; zoom temporel sans imposer le chargement de la totalité des millions d'échantillons.
- Plus tard : superposition de `1x = RPM/60`, `2x`, `3x` et autres ordres calculés depuis les **régimes machines effectivement disponibles**, alignés via UTC avec incertitudes visibles. Une superposition compatible n'établit pas automatiquement la causalité.

### Choix d'analyse provisoires (non gelés)

- Vue 2–25 Hz : fenêtres de l'ordre de 10 s avec recouvrement 50 % comme **point de départ à étudier**, et possibilité de zoom/paramétrage ; compromis résolution temporelle/fréquentielle à documenter.
- Grande bande 2–700 Hz : stratégie de fenêtres et de résolution distincte si nécessaire. Ne pas confondre filtre anti-alias embarqué, filtrage d'indicateurs RMS et PSD.
- Spectrogrammes comparables : paramètres FFT, fenêtre, taux d'échantillonnage estimé/qualifié, recouvrement, unités `mg²/Hz` (PSD), éventuelle échelle dB **avec référence explicite**, palette/échelle de couleur fixes pour une comparaison entre campagnes.
- Couverture de qualité : ne pas interpoler graphiquement des ruptures, pertes, saturations ou horodatages invalides ; distinguer les vrais zéros, l'inconnu et l'invalide.
- Contrôles scientifiques : signaux synthétiques et fichiers réels, DC/gravité, FIR warmup, cas incomplets, impulsions, erreurs de bord, signaux multitons, alias, conservation de l'énergie via PSD, limites de cadence/étalonnage, distinction des trois axes et du global.

## 3. Contrat de données préparées, indépendant de Grafana

Conserver les fichiers `.TR2` originaux intacts, avec empreinte SHA-256. Index de lecture et traitements en cache, résultats/métadonnées catalogue local (SQLite possible), exports ouverts (CSV V1, Parquet candidat). **Ne pas** transcrire systématiquement chaque échantillon XYZ dans SQLite.

Pour toute série, fenêtre ou raie exportée : `campaign_id`, fichier source + SHA, UID/session, format/firmware/filtre, point de mesure/orientation, `elapsed_start/end`, indices source début/fin, fréquence d'échantillonnage estimée/qualifiée, conversion nominale/étalonnée, algorithme/version/paramètres/bande, indicateurs avec unités, qualité + couverture/validité. Temps UTC **si prouvé** : `utc_start/end`, `utc_status`, `utc_source`, `utc_uncertainty_s` et méthode/version de rattachement. Aucun transfert BigQuery requis pour V1.

## 4. Campagne nocturne envisagée — préparation de terrain

L'utilisateur souhaite une campagne plus représentative, sans perturbations humaines volontaires, dans un emplacement significatif : **PC machine** ou **atelier électrique pont 2, proche moteurs**. Son atelier habituel se trouve **derrière la passerelle au pont 11**. Ces localisations servent à contextualiser l'observation et ne garantissent aucune excitation mécanique précise.

**Préférence fonctionnelle proposée :** un point fixe documenté dans l'atelier électrique du pont 2 si l'installation y est autorisée et sûre ; le PC machine reste une option si un point rigide et sécurisé convient mieux.

Fiche minimale de campagne à relever :
- Identité TR2, nom de fichier / session, date et lieu exact du point (pont/local, support structurel), orientation X/Y/Z (photo lisible), méthode de fixation, état des machines proches, éventuelles sources parasites (interventions, passage, manipulations).
- Conditions d'exploitation si connues (GE/moteurs/propulseurs, vitesse/régime, manœuvres) ; noter les changements de régime ou événements marquants **sans prétendre disposer automatiquement des données Dataplatform**.
- Heures UTC observées **et leur provenance/incertitude** autour du début/fin, si disponibles sans perturber le protocole ; les annotations opérateur ne sont pas des ancrages UTC qualifiés par elles-mêmes.
- Continuité de l'alimentation, retenue mécanique/anti-chute et cheminement du câble, éloignement des organes mobiles, chauds et électriques exposés ; aucune intervention sur machine/armoire sous tension ni fixation hasardeuse. Ne pas déplacer ni toucher au TR2 pendant l'enregistrement.

**Séquence de sécurité prioritaire :** la mission `TR2-20261030-R3-8H-FAT32-AUTONOME` et la capture autonome en cours doivent se terminer et être **vérifiées** (STOP/SD quiet, preuves en RAM avant coupure, lecture/copie et contrôle selon contrat) **avant toute nouvelle campagne** utilisant le même TR2-002, sa microSD ou son alimentation. L'heure de fin estimée ne suffit pas ; ne pas débrancher, retirer de SD, redémarrer/flasher ou déplacer le banc à cette seule échéance. Reprise éventuelle par Codex dans le **même contrat** et qualification indépendante par ChatGPT ; nouvelle mission seulement après clôture du pilote actif. Cette note **ne déclenche ni START, ni manipulation physique, ni mission concurrente**.

Les données de la première campagne de terrain demeurent **observatoires**, non étalonnées et non représentatives d'un niveau admissible/normal de vibration. Prévoir ensuite une acquisition répétée sur **le même point et la même orientation**, avant de comparer l'intensité absolue avec un point du pont 11 ; des différences de fixation peuvent expliquer une partie des écarts.

## 5. Roadmap d'analyse PC (découpage interne à R5-MÉTIER)

- **R5-A** — Import réel versionné v1/v2/v3, intégrité complète/préfixe récupérable, provenance, catalogue, aucune date UTC déduite de FAT32.
- **R5-B** — Méthodes RMS/crêtes par fenêtres 1/10/60s et bandes, vérification indépendante métrologique/synthétique, gestion warmup/qualité.
- **R5-C** — PSD/FFT/Welch, spectrogramme, multi-raies `f_i(t)`/`A_i(t)`, harmoniques, exports avec incertitudes.
- **R5-D** — Moteur local durable et scalable 8h puis 24h, lecture par blocs, index et cache, sans dépendance à Grafana/cloud.
- **R5-E** — Exploitation locale, annotations, rapprochements de campagnes, format d'export publiable, métadonnées UTC.
- **R5-F (évolution)** — Grafana local ; BigQuery/Grafana MSM plus tard, sans requalifier ou recalculer autrement le signal.

**Gates :** d'abord fidélité et reproductibilité des mesures d'intensité RMS/crêtes, conventions XYZ/global, qualité et comparabilité ; contrôle séparé du calcul pondéré confort et de son protocole d'implantation **avant tout jugement de gêne** ; ensuite FFT/PSD/spectrogrammes sans régression ; gestion de défauts, performance sur archive longue et absence de régression de l'existant. La présente note est **fonctionnelle** et ne constitue **ni code implémenté, ni mesure à bord qualifiée, ni mission Codex**.
