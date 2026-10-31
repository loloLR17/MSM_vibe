---
mission_id: TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE
status: READY
mission_revision: R1-20261031-PREFERENCE-BV
base_ref: main
base_sha: 4192f4cbe0436a30cfe57a0033bbd869612931d5
created_on: 2026-10-31
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — Implémenter TR2-Wm, indicateur local et indicatif de confort vibratoire

## Décision et finalité utilisateur

**Précision R1 du 31/10/2026, avant tout démarrage Codex :** l'utilisateur préfère les **seuils de confort Bureau Veritas**, exclusivement *si la méthode TR2-Wm calculée est physiquement et dimensionnellement comparable à la grandeur et au protocole associés au seuil BV*. **Calcul et validation numérique d'abord ; étude des seuils ensuite.** Ne pas adopter les seuils GOST par défaut, ni chercher à forcer l'équivalence. Cette révision du même contrat READY est traçable ; les permissions et l'identifiant ne changent pas.
Le 31/10/2026, l'utilisateur confirme que **le TR2 n'est pas un instrument métrologiquement étalonné** et qu'aucune revendication de conformité ISO/BV, ni certification du navire, n'est visée. Il veut néanmoins une **métrique physique, stable, simple, reproductible**, inspirée de la pondération fréquentielle Wm, pour comparer les vibrations de confort dans des locaux et traversées et les rapporter à des **repères publiés d'acceptabilité**. Ne pas bloquer le développement numérique faute de certification ni introduire un « score de confort » arbitraire.

Cette mission est **strictement hors ligne/PC** sur l'archive réelle existante ; elle implémente, teste, versionne et publie une première chaîne réutilisable. Elle ne qualifie pas la sensation d'un passager individuel ni l'état réglementaire du MSM.

## Préambule, état et source
Lire `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, `ETAT_COURANT_TR2.md`, la présente mission, la revue indépendante `Modbus RTU/02_Validation/Qualifications/REVUE_R5_PREQUAL_ISO_FS2G_20261031.md` et les preuves de la mission précédente archivées dans `Echanges_Codex/archives/TR2-20261031-R5-PREQUAL-ISO-FS2G-OFFLINE/`. Vérifier HEAD réel, git status, worktrees et stashes ; préserver les travaux existants. Ne pas répéter à l'identique les validations historiques.

Base logicielle : `experiments/r5_prequal_offline/`, lecteurs R3/R5 stricts, `experiments/r5_comfort_analysis/`, `experiments/r5_bv_pk/`. Source locale originale en **lecture seule** :
`/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`
SHA256 `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`, 58 552 136 XYZ int16 ; contrôle avant/après des SHA/CRC/index/qualité par lecteur strict. Ne pas exploiter la queue préallouée, ne jamais modifier l'archive. Données R3 `CLOSED_WITH_RESERVATIONS` ; `FS_PHYSIQUE_NON_VÉRIFIÉ` sur toute la durée ; sensibilité nominale ±8 g/244 µg par compte sans étalonnage, cadence MCU/QPC incertaine d'environ 0,68 %, orientation/navire/pose non qualifiées, réponse physique à 1 Hz non démontrée. Les 41 sorties Z >2 g / pic nominal +7,676728 g exigent un audit d'influence, sans suppression silencieuse.

## Sources scientifiques et règles d'utilisation
- Enregistrement officiel russe : https://protect.gost.ru/gost/details/afd5be76-acef-4aab-b688-0c0173a64e99
- Texte GOST R ISO 20283-5:2023, déclaré **IDT** à ISO 20283-5:2016 : https://meganorm.ru/mega_doc/norm/gost-r_gosudarstvennyj-standart/7/gost_r_iso_20283-5-2023_natsionalnyy_standart_rossiyskoy.html ; §7–8 et annexe A : pondération Wm, max des axes, RMS 1–80 Hz, 60 s au minimum (120 s si composantes <2 Hz), repères d'espaces habitables.
- Fiche ISO : https://www.iso.org/standard/68125.html ; champ d'application documentaire pour voyages prévus >=24 h, **hors garantie d'applicabilité à notre traversée ~8 h**.
- ISO 2631-2:2003, annexe A mathématique (ancienne édition), et ISO 2631-2:2026 (nouvelle édition) : vérifier concordance de la définition Wm accessible avant figer méthode. https://www.iso.org/standard/23012.html et https://committee.iso.org/standard/87847.html?browse=ics . ISO 8041 fournit des exigences de mesure **non revendiquées**.
- Respecter le droit d'auteur : ne pas verser de PDF ISO/GOST, texte intégral, figures d'origine ni transcription exhaustive de tables protégées dans le GitHub public. Implémenter du code original à partir des relations vérifiables, référencer liens/édition/clauses ; conserver seulement les paramètres factuels nécessaires et les preuves de calcul originales. Si un détail est indisponible, indiquer une incertitude et produire la meilleure méthode démontrable, sans prétendre qu'elle est normative.

## Lot A — Calcul original Wm, prioritaire
1. Concevoir et **implémenter** un traitement Wm déterministe sur accélération XYZ physique nominale, distinct de `TEST_LP10`, du RMS plat 1–80 et de BV-PK. Vérifier mathématiquement les étages de limitation de bande et de pondération : passe-haut Butterworth ordre 2 (`f1≈0.7943 Hz`), passe-bas Butterworth ordre 2 (`f2=100 Hz`), pondération passe-bas ordre 1 (`f3≈5.684 Hz`), avec **réponse totale et limitation utile 1–80 Hz**. Calculer module/phase, réponse digitale et influence de la transformation discrète ; ne pas appliquer deux fois des limitations de bande, ne pas multiplier un RMS global par un facteur fixe et ne pas transformer arbitrairement des coefficients tiers d'octave en filtre continu. Comparer la réponse calculée à des points publics de l'annexe A (par ex. |Wa| ≈0.833 à1 Hz ; 0.818 à4 ; 0.494 à10 ; 0.0604 à80), et à la définition analytique complète, en conservant la provenance et la distinction entre centre tiers d'octave et gain ponctuel.
2. Définir un seul **indicateur principal** `TR2_WM_A_MAX` : RMS de l'accélération Wm en **mm/s²**, calculé séparément sur X/Y/Z, puis **maximum des trois RMS d'axes sur la même fenêtre**. Produire les trois RMS et l'axe gagnant ; ne pas confondre avec norme vectorielle ni pic transitoire. Vérifier `1 mg = 9.80665 mm/s²`, propagation des unités et stabilité des calculs. Mettre à disposition, en secondaire/contrôle, `v_w` en mm/s selon la relation du référentiel `v_w≈0.028 a_w` dans ces unités, ou par chaîne indépendante de vitesse soigneusement justifiée ; **éviter de présenter deux grandeurs liées comme deux mesures indépendantes**.
3. Réutiliser traitement streaming et qualité R5, état des filtres, warmup/settling/bords et fenêtres FULL ; calculs 60 s et 120 s, résumé de période entière, P50/P95/max, comparaisons alignées et sensibilité 1 Hz/horloges. Ne pas rendre artificiellement FULL une fenêtre partielle/invalidée, ne pas supprimer les chocs/gaps pour rendre le résultat favorable. Distinguer résultat principal et diagnostic de sensibilité à l'épisode extrême documenté, ainsi qu'aux dérives/inclinaisons et filtre FIR107 existant.
4. Restreindre ce lot à l'analyse locale ; livrer une CLI ou API de calcul réutilisable pour futures archives, sans lancer de nouvelle capture ni créer une dépendance Grafana/Windows.

## Lot B — Étude ultérieure de comparabilité aux repères BV (après calculs validés)

1. **Ne pas retarder ou conditionner le Lot A aux seuils.** Sur la méthode Wm versionnée et les résultats vérifiés, consulter et identifier la version actuelle, le tableau et la catégorie *pertinents* du Bureau Veritas NR467 Partie F (référence documentaire déjà étudiée dans `REVUE_R5_BV_PK_ALTERNATIF_20261031.md`). Examiner notamment la distinction entre **RMS global pondéré** (voie potentiellement applicable) et **composantes individuelles crêtes BV-PK** (voie distincte, antérieurement `BLOCKED` sur ce signal). Vérifier directement les prescriptions de bande, grandeur, pondération, valeurs, unités, fenêtre/durée, axes, conditions et locaux ; les anciennes valeurs de PK ne sont **pas** interchangeables avec Wm.
2. Privilégier **les repères BV** dans l'affichage *uniquement si* l'alignement des grandeurs/méthodes est établi et tracé point par point. Séparer comparabilité **mathématique** du résultat et applicabilité **réglementaire ou métrologique** (non recherchée). Si l'équivalence n'est pas démontrable, consigner clairement `COMPARABILITE_BV_NON_ETABLIE`, présenter les résultats TR2-Wm sans seuil ni verdict, et lister ce qui manque. Ne jamais « corriger » le calcul pour le faire entrer dans un tableau de seuils.
3. Le GOST R ISO 20283-5:2023 conserve son utilité comme source de définition/contre-vérification Wm, mais ses seuils **ne sont pas sélectionnés par défaut**, ni substitués silencieusement aux repères BV. Si un jour une comparaison alternative GOST est utile, l'isoler et identifier explicitement sa source/édition.
4. Une catégorie de local doit être **renseignée explicitement**, jamais déduite de « PC machine ». Ne pas attribuer un repère de local habitable au compartiment machines sans champ d'application démontré. Pour cette première campagne `NON_CLASSÉE`, produire prioritairement figures et distributions Wm sans classement automatique du local ; comparer les références BV dans un tableau séparé si les critères sont remplis.
5. Si une comparaison BV indicative est défendable, livrer valeur, seuil, ratio/marge, source/édition/catégorie, et mentions visibles « comparaison documentaire indicative ; TR2 non calibré ; aucune conformité ISO/BV revendiquée ». Sinon résultat numérique seul ; ne pas produire `SOUS_REPÈRE` ou `AU_DESSUS_REPÈRE` sans correspondance établie. Les seuils restent des métadonnées versionnées distinctes des résultats physiques.

## Lot C — Validation, persistance et publication
1. Tests synthétiques **indépendants** : réponse magnitude/phase aux fréquences pertinentes (0.5/0.8/1/2/4/10/25/80/100Hz), sinus et multitons, AM/FM, bruit, offsets et inclinaison variable, échelons et chocs, clipping, gaps, fenêtres invalides, frontières, coupure des blocs, invariance au chunking, phase/causalité, unités et relation vitesse. Définir les tolérances par rapport à l'oracle AVANT d'inspecter les résultats. Sur vraie archive, contre-calcul séparé temporel / FFT+Parseval sur segments représentatifs et périodes 60/120 s ; diagnostiquer honnêtement les différences de conditions aux bords.
2. Calculer **l'ensemble des six heures** si G0 valide, sans devoir attendre une campagne nouvelle. Refaire SHA avant/après et confirmer l'absence de régression v1/v2/v3/v4. Les anciennes SQLite restent read-only ; si une nouvelle cohorte exige une écriture, **une unique copie dérivée v5** depuis la v4 est autorisée, après contrôle des invariants, 12 tables historiques et résultats cumulés conservés, import idempotent, intégrité/FK, identifiants versionnés et provenance claire. Aucune donnée XYZ brute ni DB énorme dans GitHub.
3. Code et tests dans `experiments/` en réutilisant les modules ; conserver traces des réponses du filtre, oracles, contre-calculs réels, seuils et catégories, statistiques et diagnostics. Publier des preuves compactes, README, graphiques, exports CSV/JSON/HTML simples et MANIFEST SHA ; pas de normalisation présumée ou de promesse de précision absolue.
4. Temps/mémoire bornés, méthodes et résultats reproductibles, pas de refonte ou dette opportuniste A-06. `./tr2_validate.sh` / ARM / .NET uniquement si impact réellement identifié.

## Critères d'acceptation
- **G0** : source valide/intacte, gouvernance et état Git vérifiés, aucune modification physique/firmware et aucun achat.
- **G1** : réponse Wm originale vérifiée contre fonctions et points publics, calcul XYZ/max axe/60–120 et unités corrects, oracles indépendants PASS ; distinction test numérique vs conformité métrologique.
- **G2** : six heures traitées, reproductibilité/robustesse des fenêtres, vrai contrôle croisé sur signal réel, effet anomalie/horloges/basse fréquence documenté, échecs et limites conservés.
- **G3** : après validation du calcul Wm, étude explicite de comparabilité avec les seuils **BV** (grandeur RMS Wm vs BV-PK distincts, unités, bande, fenêtre, axes, catégorie et domaine) ; seuil BV utilisé **seulement si** équivalence démontrée, sinon `COMPARABILITE_BV_NON_ETABLIE` et résultats sans seuil ; `NON_CLASSÉE` si catégorie de local ambiguë. Aucune déclaration de conformité normative.
- **G4** : persistance non destructive, intégrité/idempotence, exports figures/CSV/HTML clairs et code réutilisable ; preuves publiées et relues sur GitHub.
- **G5** : `DERNIER_RAPPORT.md` avec le présent `mission_id` et `status: DONE` **seulement si** tous gates logiciels sont satisfaits ; `IN_PROGRESS` checkpoint ou `BLOCKED` en cas de blocage réel, avec faits vérifiables et SHA/commits. La revue indépendante A-01 de ChatGPT reste distincte.

## Permissions, arrêts et invariants
`target_branch: main`, `allow_commit: true`, `allow_push: true` sans validations répétitives : l'utilisateur a déjà accordé les permissions permanentes pour les opérations logicielles. `allow_flash: false`, `allow_debug: false` car **aucune intervention STM32**. Ne pas toucher ST-LINK, TR2-001/002, microSD, FRAM, COM4/COM5, configuration FS, Windows, service de supervision LIVE, missions précédentes, archives historiques. Ni achat de normes ni contournement de droits/licences. Si les éléments normatifs publics ne suffisent pas à valider numériquement la méthode, publier une implémentation expérimentale avec limitations explicites et statut proportionné, sans inventer de coefficient ou seuil. Préserver les travaux locaux, ne pas reset/rebase/force push. Pas de Work.

Codex inspecte le dépôt réel, choisit les solutions minimales, **exécute** les tâches autorisées, teste, commit/push sur main, vérifie indépendamment la publication côté distant, puis rédige et publie `DERNIER_RAPPORT.md`. ChatGPT vérifie ensuite les preuves et décide de la qualification.
