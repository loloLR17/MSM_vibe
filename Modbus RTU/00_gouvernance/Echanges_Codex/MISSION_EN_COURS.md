---
mission_id: TR2-20261031-R5-BV-PK-ALTERNATIF-6H
status: READY
base_ref: main
base_sha: 95ef8c8bc03b35e0ad9d7ad889ca370070292de3
created_on: 2026-10-31
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — R5-PK : indicateur spectral alternatif BV pour la campagne PC machine 6 h

## Décision métier et résultat attendu

L'utilisateur souhaite **une mesure de vibrations compréhensible, reproductible et comparable entre campagnes et aux repères de confort indicatifs d'une source extérieure crédible**. Il valide pour cette tranche la **voie alternative de confort BV (grades 1PK/2PK/3PK)** ; il étudie séparément un éventuel achat d'ISO20283-5/ISO2631-2 pour la voie RMS pondérée. **Ne pas lancer le chantier ISO Wm** dans cette mission et ne pas prétendre que la voie PK est une « conversion » de nos RMS non pondérés.

**Mission PC exclusive :** calculer sur les **6 h réelles** les amplitudes crêtes de **composantes spectrales individuelles**, dans les bandes BV **1–5 Hz en accélération** et **5–100 Hz en vitesse**, avec une méthode répétable et justifiée, puis proposer une **lecture comparative indicative** de ces grandeurs face aux références de la bonne catégorie de local. Des graphes intuitifs et une base SQLite queryable sont exigés. Conserver intégralement les résultats R5 (2–25Hz techniques, 1–80Hz physiques non pondérés) ; ne pas ajouter la moindre conclusion structurelle, de santé ni notation formelle de classe.

Avant tout travail : relire **en direct** sur GitHub `main` : `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, contrat présent et rapport précédent ; vérifier `HEAD`, worktrees, stashes et état des fichiers originaux. Respecter A-06. Ancienne mission `TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF` **clôturée avec `BLOCKED` sur P0 ISO** : les fichiers historiques d'origine/rapport sont archivés sous `Echanges_Codex/archives/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/`. La [revue A-01 indépendante](../../02_Validation/Qualifications/REVUE_R5_CONFORT_PRELIMINAIRE_P0_20261031.md) **accepte les valeurs non pondérées comme exploratoires**, sans lever P0.

## Références de comparaison **exactes**, vérifiées par ChatGPT le 31/10/2026

**Source primaire gratuite :** [Bureau Veritas NR467, Partie F, édition juillet 2026, chapitre 6](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf).

- **Ch. 6, section 1 §2.1.3(b), page PDF 238 (page imprimée 237) :** BV permet l'alternative à la grandeur globale RMS pondérée ISO : **`single frequency amplitude peak`** de 1 à 100Hz selon l'ISO6954:1984, avec **facteur de conversion `C_F=1`**, d'où pour une **composante fréquentielle individuelle quasi sinusoïdale** `amplitude répétitive crête = sqrt(2) × amplitude RMS de cette composante`. **Ne jamais appliquer sqrt(2) au RMS broadband 1–80Hz, ni au maximum RMS XYZ pour obtenir un KPI PK**. Les mesures BV sont arrondies au dixième de mm/s ou mm/s² selon la bande après calcul ; conserver la valeur non arrondie en base. Le choix `C_F=1` est celui déclaré dans ce texte BV, à ne pas remplacer par une hypothèse issue d'une autre norme.
- **Ch. 6, section 3, tableaux 5 et 6**, **pour `Engine control room or switchboard room (if continuously manned at sea)`** :
  - `BV_PK_VEL_5_100` **5–100Hz, vitesse crête de la composante**, unités mm/s : grade 1PK **4,0** ; 2PK **4,5** ; 3PK **5,0**. Tableau 5, page PDF 246 / page imprimée 245.
  - `BV_PK_ACC_1_5` **1–5Hz, accélération crête de la composante**, unités mm/s² : grade 1PK **125** ; 2PK **141** ; 3PK **157**. Tableau 6, page PDF 247 / page imprimée 246. **Erreur documentaire corrigée : 125/157/188 concerne d'autres catégories/sections et ne doit PAS être utilisé pour ce PC machine.**
  - Grades 1PK à 3PK sont des critères **deux bandes**, et non une seule barre de seuil avec une valeur RMS.
- **Ch. 6, section 3 §3.1.2** : les emplacements et axes de campagne BV sont spécifiques ; mesures principalement **verticales au sol**, quelques directions supplémentaires par pont ; notre montage au PC machine n'est **pas qualifié selon cette procédure**, axes XYZ capteur non orientés navire. Le critère `continuously manned at sea` reste à confirmer pour la correspondance exacte du local ; tableau affichable sous libellé **« Référence BV : PC machine occupé en permanence, correspondance à confirmer »** et non attribution formelle.
- **ISO6954:1984**, référence historique de la voie PK, **retirée** selon ISO, mais toujours **citée par la NR467 édition 2026**. Les informations librement accessibles de [Lloyd's Register, analyse ISO6954:1984](https://www.imorules.com/GUID-B6CCBEB7-2799-4E1F-9EAE-0A2181CEF867.html) et des [guidance notes ABS sur vibrations navales](https://ww2.eagle.org/content/dam/eagle/rules-and-guides/current/conventional_ocean_service/147-guidance-notes-on-ship-vibration/147-ship-vibration-gn-sep-23.pdf) indiquent des enjeux de **MRA/maximum repetitive amplitude, modulation, fenêtrage et spectre RMS/peak/peak hold**. Ces sources **peuvent aider à vérifier la méthode**, mais leurs conventions éventuellement divergentes **ne doivent pas être mélangées** ou baptisées automatiquement BV. Expliquer la définition exacte du `single frequency peak` retenu dans nos exports.

**Porte d'homogénéité des grandeurs :** nous pouvons viser une valeur **de même grandeur, même bande, même ordre d'unité et même facteur `C_F` que les repères BV**, et la comparer **à titre exploratoire** ; l'écart d'étalonnage matériel, de pose, d'axes et de méthode d'extraction doit rester visible. **Ne pas prédire des résultats ni supposer que les 1,424/2,197/2,990 mm/s RMS de R5 sont des valeurs PK.** Si l'extraction **de composante crête** ne peut pas être rendue suffisamment défendable, fournir seulement une `BV_PK_CANDIDATE` explicite, **ne pas** classer « favorable / intermédiaire / élevé », et signaler précisément la condition d'accès à la méthode manquante.

## Entrée : archive intégrale, immuable

- `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`; SHA256 fichier physique entier `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`. Format v3 `COMPLETE`, 58 552 136 XYZ selon preuves R3/R5. **Vérifier SHA/CRC et NE JAMAIS modifier la source**, ni recopier la queue préallouée comme données.
- Réutiliser le lecteur strict R3, le code R5 d'analyse/FFT et `experiments/r5_comfort_analysis/` autant que pertinent ; **pas de second parseur divergent, pas de nouveau traitement RMS 1–80Hz inutile**.
- Conditions historiques et réserves inchangées : PC machine, plancher, double-face expérimental, sensibilité `0,244mg/count` nominale non étalonnée ; orientation XYZ non rattachée aux axes navire ; `UTC_UNKNOWN` et cadence estimée MCU `~2661,46Hz` vs témoin QPC `~2679,52Hz` (~0,68% d'écart) ; digest producteur RAM indépendant perdu après STOP. Source Wm/équivalence ISO **toujours indisponible**, conserver `BLOCKED` sur la voie RMS pondérée.
- Le FIR embarqué 107 taps Q15 et son SHA sont documentés : contrôler réponse en **1–100Hz**, anti-alias, quantification, bruit/dynamique et incertitude relative ; priorité aux grandes composantes. **Ne jamais prétendre que toute la bande 1–100Hz est physiquement qualifiée uniquement parce que son DSP fonctionne**.

## Travail demandé (incrémental, première valeur utile rapidement)

### A — Algorithme BV-PK candidat, crédible et quantifié

1. Reconstituer une **fiche de méthode** : texte BV officiel/clauses/édition/valeurs, définition de l'**amplitude individuelle** en fenêtre, `C_F=1`, conventions RMS→peak, axes, traitement des harmoniques et cas modulés/transitoires ; bibliographie autorisée. Définir la fenêtre d'observation (tester au minimum 60s et sous-fenêtres plus courtes justifiées), FFT/PSD/transformée, fenêtre Hann et correction du gain cohérent, résolution, interpolation d'une raie hors bin, fuite spectrale, superposition de raies proches, bruit et `peak-hold` vs maximum répétitif. La finesse d'un bin ne vaut pas précision fréquentielle SI.
2. **Sur chaque axe**, sur les **XYZ bruts archivés** et deux variantes de cadence, construire :
   - composante candidate maximale `a_pk(f)` sur **1–5Hz**, en **mm/s² peak**, avec fréquence, axe, début/fin de fenêtre et réponse de filtre ;
   - composante candidate maximale `v_pk(f)` sur **5–100Hz**, en **mm/s peak**, obtenue par intégration fréquentielle **de la composante** (gain fonction de `f`), et non par conversion du RMS broadband. Conserver PSD/amplitude/composante et éviter d'utiliser un maximum instantané arbitraire du signal total.
   - Statuts séparés pour composante stationnaire, modulée/chirps, non résolue, clippée/gap/invalide, et reliabilité relative à 1Hz.
3. Pour prévenir la sous/surestimation liée à la **méthode de pic**, confronter au moins deux modes crédibles sur les vrais segments (une méthode spectrale contrôlée et une méthode indépendante de reconstruction de raie/passe-bande/estimation temporelle), au lieu de convertir un simple RMS de fenêtre qui contient plusieurs raies. Évaluer sensibilité aux durées, types de fenêtre, résolution, choix de lissage, fréquentielle et deux horloges. Mesurer et afficher les divergences : si elles sont trop grandes/instables, indiquer `METHODOLOGY_INDETERMINATE` plutôt que faux « proche du seuil BV ».
4. Tests synthétiques avec oracles analytiques et tolérances **établies avant essais** : sinusoïdes d'amplitudes connues sur/hors bins, 1/2/4.9/5/5.1/20/80/99/100/101Hz, multitons, raies très proches, modulation d'amplitude et de fréquence, impulsions, DC/gravité, bruit, fenêtre courte, clipping/gaps, cas où le maximum RMS broadband n'est **pas** l'amplitude individuelle. Vérifier facteur `sqrt(2)`, unités g/mg/mm/s²/mm/s, fréquences, l'absence de confusion composante/total. Préserver résultats précédents.

### B — Interprétation indicative, sans verdict réglementaire

5. **Comparer à la paire de seuils de la bonne catégorie BV**, dans deux tableaux indépendants pour **1–5Hz acceleration crête** et **5–100Hz vitesse crête**. Conserver les métriques numériques **non arrondies**, et appliquer l'arrondi BV à une décimale uniquement pour l'affichage. Déterminer pour chaque fenêtre si **ses composantes sont situées sous les repères 1PK/2PK/3PK ou au-delà**, **seulement quand** la méthode de sélection est assez robuste pour une comparaison indicative. Pour la paire, montrer les deux résultats simultanément ; un repère 1PK « favorable » n'est pas un grade attribué au MSM. Ne pas appliquer la tolérance de 20% des **points de locaux** BV à la proportion de **minutes** d'un capteur unique.
6. Fournir un **encart lisible par profane** expliquant les deux bandes, unités et repères : « Référence indicative BV pour un PC machine occupé à la mer ; résultats issus d'un prototype non étalonné ; ni classification ni conformité ». Des termes de proximité `sous seuil 1PK`, `entre seuils 1PK/2PK`, `entre 2PK/3PK`, `au-delà 3PK` sont acceptables si méthode démontrée ; préférer cela à `bon/passable/mauvais` non prouvés. Sans méthode robuste, **montrer la mesure avec statut de réserve**, sans superposer une barre de seuil trompeuse.
7. **Ne pas confondre** ces repères PK avec les **4/4,5/5 mm/s RMS pondérés 1–80Hz** du **tableau 4 section 3**, même si certains chiffres sont identiques ; préciser `BV_PK_VEL_5_100` vs `BV_WEIGHTED_RMS_1_80`, unités et définitions. Le premier est l'objectif de cette mission ; le second attend les ISO que l'utilisateur envisage d'acheter.

### C — SQLite et livraison

8. **Étendre la cohorte d'analyse SQLite existante** de manière versionnée et sans perte (`comfort-secondary-v2.sqlite` locale et/ou migration sûre), avec identifiant distinct des deux KPI PK, édition `BV_NR467_F_2026_07_PK`, `derived_method_version`, fenêtre/axe/source/cadence/hypothèses, fréquence et amplitude de la composante la plus forte, status/coverage et référence de seuil consultable. **Ne pas migrer ni altérer la base de supervision LIVE, ni écraser les catalogues v1/v2 précédents.** Si schéma secondaire v3 nécessaire, documenter la future unification ; ne pas multiplier les DB sans justification technique. Tous résultats doivent être référencés à un seul `campaign_sha` et identifiants d'analyses distincts.
9. Au minimum : une **page HTML locale ou équivalent** et deux graphiques lisibles (1–5Hz et 5–100Hz) sur 6h, suivi des fréquences candidates et repères contextuels **si validés** ; synthèse de 1 page avec période typique / épisode intense / degré de robustesse. Conserver liens vers PSD, RMS2–25 et vitesse physique 1–80 précédents. Données CSV et requêtes SQLite permettant de reconstruire les figures. Aucun Grafana/BigQuery obligatoire à ce stade.
10. Exécuter tests, validation de mesures réelles sur au moins 3 segments faibles/médians/forts + campagne entière, vérification de double import idempotent et intégrité DB, SHA originaux avant/après, coût mémoire borné. Publier **seulement code/tests/requêtes/méthode/CSV agrégés/PNG/HTML/rapports/preuves compactes** sur `main`. Fichiers brut TR2 et DB locales demeurent au T1000 ; pas de transfert de 640MiB sur GitHub.

## Gates de validation et retour

- **G0 référence :** lecture et identification précises des §/tableaux BV officiels, **bonnes valeurs de la section3**, C_F=1 et tests de conversion de **composante individuelle** ; s'assurer de l'équivalence de grandeur et des limites de l'ISO6954:1984, sans prétendre appliquer tous ses critères formels.
- **G1 scientifique :** tests analytiques et recoupements indépendants pertinents sur le réel ; comparaison FFT/passe-bande/raies, choix de fenêtre et modulation documentés, seuils affichés **uniquement si l'amplitude crête est raisonnablement compatible avec le sens BV** ; ambiguïté majeure = `INDETERMINATE`, pas valeur de confort inventée.
- **G2 opérationnel :** 6 h réelles, métriques des deux bandes, unités correctes, qualité/filtres et cadence explicites ; statistiques et transparence des pertes/anomalies.
- **G3 persistance :** cohorte SQLite PK interrogeable, idempotence, empreinte originale/anciens catalogues préservée.
- **G4 lisibilité :** affichage POC deux bandes, seuils **propres** (si G0/G1), statut indicatif non normatif et diagnostic fréquentiel accessible.
- **G5 publication :** rapport `DERNIER_RAPPORT.md` portant le `mission_id`, références, méthode, tests, commits, résultats numériques, limitations, preuves/chemins et validation de lecture distante. `DONE` uniquement si les gates numériques/présentation/SQLite réellement satisfaits ; `BLOCKED` motivé si la méthode PK elle-même est insuffisamment reconstructible. **Le blocage ISO Wm précédent ne bloque pas par principe cette voie PK autonome**.

Autorisations permanentes de développement, commit et push : **oui**. Mission PC-only, **`allow_flash:false`, `allow_debug:false`** ; aucun STM32, SD physique, modification globale système, opération destructrice, nouveau matériel ni demande de validation récurrente. Respecter toutes les procédures `AGENTS.md` et l'autonomie maximale. **ChatGPT réalisera une revue indépendante après publication du rapport**, sans attribuer d'avance de grade BV ni de jugement sanitaire.
