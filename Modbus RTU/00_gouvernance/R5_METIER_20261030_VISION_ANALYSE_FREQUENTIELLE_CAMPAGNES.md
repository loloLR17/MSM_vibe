# MSM TR2 — R5-MÉTIER — Vision d'analyse fréquentielle et campagnes de référence

**Date :** 30/10/2026. **Statut :** cadrage fonctionnel de ChatGPT issu des échanges utilisateur, non spécification normative gelée, non ordre d'exécution.  
**Référence :** `loloLR17/MSM_vibe/main`, gouvernance `AGENTS.md` / `GOUVERNANCE_CHATGPT_CODEX.md`, roadmap R0-MÉTIER (R4–R6), cahier des charges métier V1-01 à V1-11 et D1–D4.  
**Périmètre :** analyse PC, exports et campagnes d'observation à bord ; aucune modification du firmware, du matériel, des contrats Modbus historiques, ni de `MISSION_EN_COURS.md`.

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

**Gates :** reproductibilité sur fichiers originaux, interprétation scientifique honnête, gestion de défauts, performances sur archive longue, absence de régression de l'existant. La présente note est **fonctionnelle** et ne constitue **ni code implémenté, ni mesure à bord qualifiée, ni mission Codex**.
