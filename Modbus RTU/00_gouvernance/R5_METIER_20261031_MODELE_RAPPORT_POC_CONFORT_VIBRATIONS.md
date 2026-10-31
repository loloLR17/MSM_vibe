# MSM TR2 — Modèle de rapport de démonstration POC pour l'Armement et les Commandants

**Version de travail : 31 octobre 2026.** **Statut : trame de restitution métier à remplir après traitement R5 réel — aucun résultat vibratoire calculé ici.**  
**Périmètre : Mont St Michel, enregistreur de vibrations TR2 expérimental.** Destinataires envisagés : Commandants, service technique, Armement, responsables du confort et de la maintenance.  
**Gouvernance :** [vision métier R5](R5_METIER_20261030_VISION_ANALYSE_FREQUENTIELLE_CAMPAGNES.md), [plan métrologique ISO/BV](R5_METIER_20261030_METROLOGIE_VITESSE_CONFORT_ISO_BV.md), [cahier des charges V1](R0_METIER_20261029_CAHIER_DES_CHARGES.md), [qualification A-01 récupération 6h](../02_Validation/Qualifications/REVUE_R3_6H_PC_MACHINE_RECUPERATION_20261031.md). Les mesures et figures manquantes restent explicitement **À PRODUIRE** ; ne pas transformer cet exemple en rapport de mesure signé avant vérification.

## Positionnement : un POC crédible, pas une expertise de conformité

**Message central :** « Un prototype autonome peut enregistrer des vibrations à bord sur la durée d'une traversée et produire une représentation objective de leur intensité et de leur composition fréquentielle. Une instrumentation industrialisée permettrait d'étudier le confort à bord, de comparer les régimes d'exploitation et de constituer un historique utile à la maintenance. »

- **Confort** : quantifier les vibrations observées en plusieurs locaux, comparer les conditions d'exploitation et les effets éventuels sur passagers/équipage. Les indices de confort humain restent un **objectif futur de calcul et de qualification** distinct du RMS technique.
- **Compréhension des sources** : identifier périodes et fréquences caractéristiques, rechercher leur corrélation aux régimes de propulsion/GE, à la conduite et à la mer, sans attribution automatique de causalité.
- **Préservation des équipements et structures** : constituer un suivi de tendance, détecter des changements inhabituels et cibler des investigations de maintenance. **Ne jamais déduire la fatigue, les contraintes mécaniques, l'endommagement ou une durée de vie résiduelle des seules accélérations TR2** ; analyses de structure/contraintes dédiées nécessaires.
- **Demande de soutien** : financer une **étape de validation** (boîtiers et fixations, alimentation sûre, connectique, répétabilité multi-points, étalonnage/référence indépendante, traitement local et datation UTC) ; un déploiement industriel sera décidé sur résultats et budget, pas présupposé.

## Rapport court proposé : 3 à 5 pages pour décision, annexes techniques distinctes

### Page 1 — Synthèse exécutive : pourquoi le projet ?

**Titre proposé :** *TR2 — Objectiver les vibrations à bord du Mont St Michel*. Sous-titre : *Démonstrateur de mesure autonome — premières observations au PC machine, campagne du 31 octobre 2026.*

**Constat métier** : certaines vibrations sont nettement ressenties dans les locaux machines et de vie ; leur intensité varie selon les conditions d'exploitation. Cette appréciation est qualitative et n'est **pas** une valeur métrologique. Une campagne exploratoire sur un poste du PC machine a été réalisée.

**Ce que le prototype a effectivement déjà démontré dans la portée de la revue A-01 du 31/10 :**
- Acquisition autonome et témoin indépendant **≥ 6 h 00 min 01 s** ; enregistrement de **58 552 136 triplets XYZ**.
- Fichier source `E6N0005.TR2` copié et relu par un importeur v3 strict, **351 312 816 octets utiles** ; CRC/index/temps internes et empreinte footer cohérents selon les preuves publiées ; **0 rail / 0 clip** signalés.
- **Réserve majeure :** la preuve indépendante de digest producteur en RAM n'a pas été récupérée avant coupure d'alimentation ; cette mission ne qualifie **pas** complètement la chaîne producteur→microSD. Cadence, amplitude et montage ne sont **pas** étalonnés ; heure UTC de chaque échantillon non qualifiée.

**Ce que le projet peut apporter** : chronologies des niveaux, cartographies ponctuelles des locaux, signatures fréquentielles, comparaisons entre conditions d'exploitation et éventuellement indicateurs de confort inspirés de référentiels publiés, sous qualification ultérieure.

**Demande de décision (à définir, sans montant inventé) :** autorisation d'une phase pilote instrumentée avec moyens matériels et temps d'essais, éventuellement budget de composants et recours ponctuel à un appareil étalonné. **Budget et devis : À CHIFFRER, pas d'estimation arbitraire.**

### Page 2 — Première traversée : intensité vibratoire dans le temps

**Figure A — à insérer depuis le moteur R5 :** RMS d'accélération **2–25 Hz** en **mg**, série temporelle de toute la campagne, idéalement séries X/Y/Z et globale ; montrer une courbe lissée 1 min, un zoom 10 s sur événement, et les crêtes de manière distincte. Indiquer le lieu/plan de pose et que les axes sont **ceux du capteur**, correspondance aux axes navire encore à confirmer.

**Chiffres à remplir uniquement après calcul reproductible :**
- RMS global médian, P95 et maximum par tranche : **À CALCULER** ; fenêtres et filtre explicités.
- Durée et proportion de périodes les plus vibrantes selon un seuil **exploratoire interne défini et affiché**, sans le confondre avec un critère de confort normatif : **À CALCULER**.
- Couverture valide, corrections warmup/effets de bord, qualité, incertitudes : **À RENSEIGNER**.
- Annotation contextuelle éventuelle (régime propulsion, manœuvre, arrivée) : **À JUSTIFIER** par journal indépendant et ancrage horaire, pas de superposition fictive.

**Message attendu pour l'exploitant** : visualiser les épisodes vibratoires et comparer leurs amplitudes entre des périodes réellement observées. Une campagne unique ne représente ni la moyenne du navire ni tous ses régimes.

### Page 3 — Comprendre les vibrations : fréquences caractéristiques

**Figure B — à insérer** : spectrogramme temporel **2–25 Hz**, axe temps relatif MCU (ou UTC seulement si traçable), axe fréquence Hz estimée, couleur PSD **avec unités/référence**, palette stable ; marquer les lacunes plutôt que les interpoler.

**Figure C — à insérer** : comparaison de PSD/FFT sur au moins trois plages sélectionnées et/ou suivi de plusieurs raies `f_i(t)` et de leurs amplitudes. Les mouvements de fréquence et d'intensité ne doivent pas être assimilés sans preuve à une machine précise.

**Interprétation autorisée** : présence de composants dominants, de raies persistantes, de transitions et de familles harmoniques candidates. **Interprétation à éviter :** conclure « cette raie vient de tel GE/propulseur » ou « le navire est dangereux » sans mesures de régime et recoupements adaptés.

### Page 4 — Perspective confort et comparaisons de sites

**Grandeurs à distinguer dans tous les graphiques et tableaux :**

| Grandeur | Unité | Statut actuel |
|---|---|---|
| RMS accélération technique filtrée, XYZ/global | mg | Cible immédiate R5 sur archive réelle, sans étalonnage |
| PSD / raies / spectrogramme | mg²/Hz, Hz | Diagnostic fréquentiel R5 à vérifier |
| RMS vitesse **pondérée 1–80 Hz**, directions prescrites | mm/s | **À DÉVELOPPER et QUALIFIER** ; aucune mesure normative TR2 acquise |
| Exposition humaine corps entier A(8) | m/s² | **HORS PÉRIMÈTRE** sans protocole spécifique au corps/temps de présence |

**Références pour situer les ordres de grandeur, jamais pour certifier le MSM** : [ISO 20283-5:2016 (fiche ISO)](https://www.iso.org/standard/68125.html), domaine annoncé des voyages prévus de 24 h ou plus ; [Bureau Veritas NR467 Partie F, ch. 6 (juillet 2026)](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf), notations optionnelles COMF-VIB-Pax/Crew, limites de **vitesse RMS pondérée 1–80 Hz** selon local et grade. La notation COMF-VIB du MSM **n'est pas établie** et serait selon l'utilisateur probablement absente ; l'édition de règle 2026 n'est pas contractuellement applicable d'office au ferry existant. **Ne pas apposer de ligne « seuil BV/ISO » sur un graphique en mg non pondéré ou un indicateur non conforme à la bande/pondération.**

**Pilotage comparatif proposé** : répéter le même point PC machine avec montage fixé et orientation documentée ; ensuite cabines/espaces équipage et passagers à points représentatifs, puis comparaison de conditions équivalentes. Un pic de fréquence mesure une vibration locale ; il ne démontre pas à lui seul une gêne perçue, un risque sanitaire ou un défaut structurel.

### Page 5 — Valeur d'exploitation et phase pilote proposée

**Objectifs mesurables d'une phase pilote financée** :
1. Réaliser un ensemble limité de mesures **répétables et non perturbatrices** dans plusieurs locaux (PC machine, lieu de vie équipage, local passagers pertinent), avec positions/orientations/fixations et conditions de fonctionnement documentées.
2. Produire automatiquement les tendances RMS/crêtes, spectrogrammes, listes d'épisodes, métadonnées et archives vérifiables ; exploiter **SQLite local** pour comparer les campagnes, sans dupliquer les millions d'XYZ.
3. Obtenir un chemin de temps **UTC à la seconde, avec source et incertitude** pour relier les événements aux journaux techniques disponibles.
4. Étudier/implémenter/qualifier l'intégration accélération→vitesse et la pondération normative Wm sur 1–80 Hz ; **confrontation à un appareil étalonné** et vérification sur place avant référence de confort défendable.
5. Mettre au point une mécanique et une alimentation sûres (boîtier, connectique, autonomie, microSD, contrôles START/STATUS/STOP/export) pour campagne ordinaire sans Codex.

**Postes budgétaires à chiffrer après devis réels :** électronique finale et spare, boîtiers/fixations/connecteurs, microSD et alimentation, accès ponctuel à une référence étalonnée, qualification pratique, temps d'installation et d'exploitation, éventuellement réseau/supervision. **Ne pas donner de ROI monétaire inventé**, de nombre de pannes évitées ou de réduction de fatigue démontrée.

**Décision sollicitée** : autorisation/budget d'un pilote borné, point de contact technique à bord et dans la compagnie, modalités d'essais et de restitution ; choix d'un éventuel passage à plusieurs TR2 **après** résultats du pilote.

## Annexes techniques facultatives, distinctes du rapport court

A. Photo du point de mesure et de la chaîne prototype ; orientation navire si documentée, fixation double-face et alimentation ST-LINK/T1000 ; réserves de pose et sécurité.  
B. Fiche de campagne : UID, session, SHA256, fichier original conservé, durée QPC, bande/échelle/cadence estimées, format, filtres, version calcul, CRC/flags.  
C. Méthodes RMS, PSD et tests de référence ; conventions, limites, qualité et incertitudes.  
D. Sources : textes BV/ISO, notes de calcul, fiches produit, séparation entre résultats exploratoires et conformité ; accès à des textes normatifs complets si étude formelle.  
E. Export CSV de séries et schéma/catalogue SQLite ; reproduire chaque figure depuis les données agrégées versionnées.  
F. Feuille d'écarts et tâches suivantes : mesure 1–80 Hz, Wm, calibration/couplage, UTC, qualité et preuve producteur persistante.

## Règles de publication du rapport utilisateur / Direction

- **Règle F1 — Source et vérité** : mentionner pour chaque résultat le statut « mesuré », « calculé sur données brutes contrôlées », « estimé », « hypothèse » ou « objectif » ; ne jamais remplir une valeur « À CALCULER » par une valeur plausible ou reprise d'une autre campagne.
- **Règle F2 — Deux niveaux** : une **synthèse de 3–5 pages destinée aux décideurs**, puis les preuves et protocoles en annexes. Privilégier graphique RMS intelligible, spectrogramme et court plan de pilotage.
- **Règle F3 — Portée comfort/structures** : la vibration mesurée peut motiver l'examen du confort ou du fonctionnement mécanique, mais **pas prouver seule un dépassement normatif, une dégradation de structure, la fatigue ou l'imputabilité à une machine**.
- **Règle F4 — Non commercial** : présenter un POC interne expérimental de pertinence industrielle, sans logo d'approbation BV/ISO, sans invoquer une notation non démontrée ni prétendre une métrologie certifiée.
- **Règle F5 — Financement** : la phase initiale est financée sur fonds personnels et temps libre selon l'utilisateur ; **ce point n'est pas requis dans le rapport de démonstration technique**, mais peut être expliqué oralement si l'utilisateur veut motiver une prise en charge par la compagnie. Ne pas présumer un droit ou montant de remboursement. Il n'existe pour l'heure **aucun budget chiffré approuvé**.
- **Règle F6 — État de production** : la présente trame **n'est pas encore un rapport de traversée achevé**. La mission R5-A/B/C/SQLite est encore à produire ; ses courbes et valeurs feront l'objet d'une revue indépendante par ChatGPT après rapport Codex. La création de cette trame documentaire ne change ni le contrat actif de Codex ni l'état de ses tâches.
