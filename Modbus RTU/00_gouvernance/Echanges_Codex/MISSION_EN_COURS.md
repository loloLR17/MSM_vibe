---
mission_id: TR2-20261031-R5-ANALYSE-PC-MACHINE-6H
status: READY
base_ref: main
base_sha: d053d70155aecfe5af9bf7059e3b0dd6ea24a7af
created_at_utc: 2026-10-31T09:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — R5-A/B/C — Première exploitation PC de la campagne réelle 6 h

## Référence, objectif et justification

**Lire obligatoirement la version courante de `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, le présent contrat, la roadmap métier, le cahier des charges V1-12/V1-13 et les notes R5 avant toute action. Vérifier HEAD réel de `main`, identité mission, historiques et tous les worktrees/stashes.** Ne pas repartir d'anciennes archives de code. La source vérité est le GitHub `main` courant et les données `.TR2` originales identifiées ci-dessous.

**Mission R5 : construire une première chaîne de mesure et de visualisation locale vérifiable sur les six heures réellement capturées au plancher du PC machine le 31/10/2026.** L'objectif premier est de **quantifier l'intensité des vibrations (RMS, crêtes, évolution temporelle)**. Le **spectrogramme et les fréquences caractéristiques** expliquent ensuite leur évolution. Préparer un chemin scientifique distinct pour **vitesse vibratoire en mm/s et comparaisons ISO/BV**, sans prétendre à une conformité à ce stade.

**Ne pas relancer R3.** L'ancien contrat et le rapport `BLOCKED` sont archivés textuellement dans `Echanges_Codex/archives/TR2-20261030-R3-8H-FAT32-AUTONOME/` avec clôture `CLOSED_WITH_RESERVATIONS`. La qualification A-01 [REVUE_R3_6H_PC_MACHINE_RECUPERATION_20261031.md](../../02_Validation/Qualifications/REVUE_R3_6H_PC_MACHINE_RECUPERATION_20261031.md) accepte l'exploitation exploratoire mais pas la chaîne producteur-vers-carte au sens industriel. Ne pas modifier rétroactivement son verdict. Un seul pilote actif : **la présente mission R5**.

## Entrées : réelles, locales, immuables et traçables

- Source à exploiter : `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2` ; **vérifier sa présence et son SHA256 entier avant lecture** : `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`. Taille physique 671 088 640 B (préallocation incluse). **351 312 816 B utiles** ; 750 669 records, **58 552 136 triplets XYZ** validés par l'importeur strict v3 au dernier rapport. Ne jamais considérer la queue préallouée comme des échantillons.
- Preuves publiées compactes : `Echanges_Codex/preuves/TR2-20261030-R3-8H-FAT32-AUTONOME/six-hour/recovery-powercut/`, en particulier `RECOVERY_RESULTS.json`, `COPIES.json`, `README.md`. Format v3, empreinte SHA des octets utiles : `5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41`, égale au digest du footer **mais non indépendante du fichier**. `SOURCE_RAIL=0`, `OUTPUT_CLIP=0`, 11 sorties warmup, 3 natifs déclarés en attente d'ancrage. Éviter de confondre `quality_or=1` warmup et un défaut qualité.
- Témoin QPC : **21601,0193586 s à l'intérieur d'une même session active**, autonomie temporelle prouvée **dans cette portée**. START annoté `2026-10-31T00:25:17.363316Z`, pas étalon d'UTC des échantillons. Ne jamais inférer une UTC précise des données à partir de START Windows ou des dates FAT32 ; `UTC_UNKNOWN` à défaut d'ancrage qualifié.
- Paramètres historiques **non étalonnés** : sensibilité nominale `244 µg/count`, ±8g logique, FIR 107 coefficients Q15 /DEC10, DC/gravité conservée, cadence indiquée `source_hz≈26609.795`, `output_hz≈2660.9795` **sur base capteur/MCU initiale**, pas 2000 Hz V1 ni cadence SI physiquement calibrée. Traiter ces valeurs comme *estimations* avec impact sur les axes Hz et les intégrations de vitesse. **Orientation XYZ ↔ axes navire inconnue** : les sorties sont XYZ capteur, pas « vertical/bâbord/avant » sans preuve.
- L'utilisateur a photographié la pose sur **plancher PC machine**, petit breakout IIS3DWB au double-face mince annoncé avec nappe vers carte de communication, breadboard et NUCLEO protégées provisoirement, alimentation USB ST-LINK depuis T1000. Montage et coupling **exploratoires, non qualifiés**. Pas de vibration reconnue ISO/BV du local déduite de ces seuls essais.
- Autres archives et carte : **lecture seule**, aucun écrasement, suppression, réparation de FAT32, renommage, `git clean/reset` destructif, ni transfert intégral de 640MiB sur GitHub. TR2-002 déclaré hors tension et microSD au T1000 au dernier rapport. **Pas de flash, debug, START/STOP, intervention sur STM32** dans cette mission.

## Travail demandé à Codex (découpage incrémental, autonomie et contrôles)

### R5-A : importer et identifier la campagne sans perte d'information

1. Examiner et **réutiliser autant que possible** le lecteur versionné `experiments/r3_durable_fat32/tools/read_campaign.py` et les scripts/outils de supervision déjà présents. Ne pas créer un deuxième parseur divergant ni requalifier silencieusement un préfixe corrompu comme complet. Lecture `COMPLETE` stricte depuis le fichier local réel, CRC/indices/session/qualité, SHA octets utiles, provenance versionnée. Si le fichier original est indisponible, publier `BLOCKED` en donnant le chemin réellement testé et les prérequis exacts, plutôt qu'analyser des données synthétiques à sa place.
2. Construire une **lecture/agrégation par blocs**, mémoire bornée pour 58,55M XYZ : ne pas charger les 351Mo utiles en Python sous la forme d'objets individuels ; tracer pic RAM et temps de calcul. Conserver l'original et toutes les preuves intégralement immuables.
3. Livrer un **manifeste de campagne** : identité, SHA, session/UID, source effective/firmware, point de mesure, chronologie MCU relative, qualité, nombre de points, interruptions, hypothèses d'échelle XYZ et statut UTC. Identifier clairement les données non qualifiées.

### R5-B : métriques prioritaires d'intensité vibratoire

4. Calculer sur **la totalité des 6h** les RMS par axe et global, crêtes par axe et global, en **mg** à sensibilité nominale, par fenêtres pertinentes **1s / 10s / 60s**, en priorité dans la **bande 2–25 Hz**. Distinguer mesure brute avec gravité/DC et vibration après filtrage approprié ; **documenter séparément** le nom de l'indicateur, la réponse du filtre (y compris aux bornes), l'impact des transitoires de bord, gain et unité, la convention `RMS_global = sqrt(mean(x²+y²+z²))`. Ne pas prendre la crête d'un axe pour un pic vectoriel.
5. Produire tendances pleine campagne (1min et 10s), maximums par fenêtre, médiane/quantiles pertinents et **épisodes de vibration élevée**, sans fixer arbitrairement un « seuil de gêne » ISO/BV. Afficher explicitement les lacunes, invalidités ou périodes indéterminées ; pas de substitution de zéro. Tester les fenêtres de frontière, le warmup, les cas d'absence et d'anomalie. **La mission réussit seulement si les courbes proviennent effectivement des données E6N0005.**

### R5-C : première lecture fréquentielle et présentation utile

6. Sur la même campagne réelle, produire un **spectrogramme temporel 2–25 Hz** (puissance/PSD XYZ convenablement combinée, unité et convention dB documentées), des PSD/FFT d'au moins trois plages justifiées et **un premier suivi de plusieurs raies**, sans raccorder arbitrairement des pics appartenant à des sources différentes. Paramètres Welch, largeur de fenêtre/recouvrement, pas temporel, réponse et fréquence d'échantillonnage estimée affichés. La vue 2–700 Hz peut être préparée seulement si les coûts et réserves aliasing le permettent ; ne pas la présenter comme bande métrologiquement qualifiée.
7. Livrer au moins **trois graphiques directement consultables** (PNG/SVG ou équivalent compact) : (a) RMS pleine campagne temporel, (b) spectrogramme 2–25Hz pleine campagne, (c) spectre(s) de plages comparées ou dynamique des raies ; avec titres, axes lisibles, unités, origine et réserves. Fournir CSV agrégés et un `README` expliquant les conventions ; si possible un paquet local navigable sans dépendance cloud. Priorité à l'**utilité métier** plutôt qu'à l'interface complète Web.
8. Si disponibles depuis journaux **externes** et avec précision justifiée, proposer repères d'exploitation (régimes/navigation/arrivée), sinon les laisser comme annotations **non synchronisées et descriptives** ; ne pas attribuer des fréquences à un équipement sans preuve.

### Préparation R5 confort ISO/BV — **étude contrôlée**, sans certification

9. Examiner la [fiche de métrologie vitesse et confort](../R5_METIER_20261030_METROLOGIE_VITESSE_CONFORT_ISO_BV.md) : identifier les données/algorithmes nécessaires à la conversion `accélération→vitesse` en mm/s par axe, 1–80 Hz, et à une **pondération Wm normative versionnée**. Produire au minimum des tests **synthétiques** d'intégration (sinusoïdes 1/2/5/10/20/40/80 Hz, multitons et offsets) et un bilan concret de faisabilité/risques de biais. Éventuels calculs *illustratifs non pondérés* sur quelques segments réels doivent rester explicitement `EXPLORATOIRE_NON_NORMATIF`. Ne publier **aucune** valeur présentée comme conforme ISO 20283-5 ou grade BV COMF-VIB sans lecture de l'édition applicable, filtre/pondération et étalonnage démontrés ; ne pas convertir un RMS mg en mm/s par coefficient constant.
10. Proposer la plus petite étape de qualification supplémentaire (chaîne de mesure à 1 Hz, conversion mm/s, pose) **seulement après** la livraison des indicateurs de base et des graphiques réels. Pas de nouvelle campagne longue requise dans cette mission.

## Gates et éléments de preuve

- **G1** Original identifié et SHA/CRC reconfirmés localement ; pas de modification des octets originaux ou de la microSD. Status de qualité R3 conservé (A-01 `accepté exploratoire`, non `R3-GO`).
- **G2** Algorithmes reproductibles et tests automatisés synthétiques DC/gravité, sinusoïdes (2/10/25/80Hz), impulsions, multiton, bord de fenêtre, calcul RMS/PSD et cas data gap. Analyse croisée indépendante ponctuelle sur E6N0005 (sans confondre deux copies du même algorithme).
- **G3** Résultats sur **6h réelles** avec couverture explicite, coûts mémoire/CPU raisonnables, données exportables et *trois graphiques cohérents*, pas de chiffres inventés ni d'étiquettes UTC non justifiées.
- **G4** Aucun engagement ISO/BV sans qualification ; qualité et limites scientifiques visibles ; anciens tests/scripts/supervision fonctionnels préservés.
- **G5** Preuves GitHub proportionnées, **pas** de push de 351Mo ou 640MiB ; publier sur `main` seulement code/source/tests/rapports/captures compressées/CSV agrégés raisonnables. Chemins **exacts** et empreintes des artefacts réels conservés localement. Relecture GitHub de `main` après push ; `DERNIER_RAPPORT.md` signé `mission_id` et `status: DONE` uniquement lorsque les gates techniques exécutés sont tous satisfaits, ou `BLOCKED` précis si source inaccessible.

## Autorisations et risques

Autorisations permanentes de développement logiciel `commit/push` confirmées. Cette mission est **exclusivement traitement PC** : `allow_flash: false`, `allow_debug: false`; n'interdit pas les autorisations permanentes de flash/debug pour des missions ultérieures pertinentes, simplement aucune intervention STM32 ici. Pas de Work, pas de déplacement/retouche physique, pas de modification des règles de gouvernance par Codex. Préserver tout worktree/stash/données préexistants, éviter la refonte et les installations globales non requises, suivre le principe A-06.

**Chemin de retour** : Codex exécute, commit/push, publie et relit `DERNIER_RAPPORT.md` ; ChatGPT effectue l'audit indépendant et décide de la tranche suivante.
