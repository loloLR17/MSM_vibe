---
mission_id: TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF
status: BLOCKED
base_ref: main
base_sha: ec5f295678fbf1d60470069ea2f55aa548b7fc11
initial_head: 7575a051092714c3296476327fb4ace8b069cfe0
result_sha: c2c1437b080a44589836fff37b30c706adc4c1cc
created_at_utc: 2026-10-31T10:10:05.842480+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5 confort — calculs préliminaires réels livrés, porte P0 bloquée

**Le traitement autonome réalisable est achevé :** six heures originales
retraitées en1–80Hz non pondéré, accélération et vitesse physique intégrée
séparées, deux scénarios de cadence, fenêtres60/120s, contrôles indépendants,
SQLite secondaire rempli et page horsligne. **Le KPI confort pondéré demandé
n'est pas livré : P0 reste bloquée par les définitions normatives indisponibles.**
L'utilisateur a répondu «Aucun accès disponible actuellement». Aucun achat de
norme ni substitution d'une formule non vérifiée n'a été effectué.

**BLOCKED surP0 après exécution et publication des travaux réalisables.**
Le lot **c2c1437b080a44589836fff37b30c706adc4c1cc** a été récupéré depuisGitHub
et relu intégralement : **36fichiers/1232919octets byte-exact** (results-remote.json).
Il ne reste pas un test physique relançable dans ce contrat : la définition
normative fondamentale manque. Aucun essai physique ni accèsSTM32/SD/COM effectué.

[Page locale non spécialiste](preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/index.html),
[synthèse POC](preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/SYNTHESE_POC.md),
[preuves/CSV/figure](preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/README.md),
[méthode/reproduction](../../../experiments/r5_comfort_analysis/README.md),
[ficheP0/sources précises](../../../experiments/r5_comfort_analysis/P0_METHOD.md).
HTML consultable horsligne après téléchargement des dossiers ; surGitHub,
PNG/Markdown sont directement lisibles, aucun site hébergé n'est présumé.

## P0 — ce qui a été vérifié, ce qui manque

Consultations primaires effectuées : OBP ISO20283-5:2016 (règle des renvois
non datés/dernièreédition et définitions), aperçuSIST10pages/aperçuiTeh5pages ;
OBP ISO2631-2:2026 et aperçu autorisé2003de9pages ; ficheISO8041-1:2017 ;
BVNR467 partieF juillet2026 officiel. Recherche locale de textes complémentaires
sous/home/lolo/dev/msm sans source disponible ; recherches web complémentaires
n'ont pas donné les définitions requises depuis une source primaire autorisée.

Les aperçusISO2631 ne donnent pas l'annexeA Wm ; ISO20283§7/8.1/équivalence et
les tolérances/oracles8041 restent inaccessibles dans ces aperçus. L'édition2003
n'est pas substituée à2026, ni une intégration physique confondue avec
l'équivalent de présentation ISO. Les copies web non officielles n'ont pas
servi de source à une implémentation normative. Aucune reproduction des textes
normatifs complets ou table complète n'est publiée.

Écart de repérage documentaire : le texteBV situe lePCmachine continuellement
occupé enmer en**section3table4** du chapitre6, passection2table4 citée au contrat.
Constat signalé sans réécriture du contrat, sans repère affiché et sans présumer
que l'occupation effective/la pose répondent à cette classe de local.

**Wm non implémenté, grandeur équivalente ISO non implémentée, noms/repères/
classes BV désactivés.** La porte d'import/refonte préliminaire refuse ces
prétentions non vérifiées. Aucun pourcentage de minutes n'est appelé conformité
ou assimilé aux pourcentages de locaux du référentiel.

Pour lever P0 : accès licite à ISO20283-5:2016 §7,§8.1/équation et annexeA ;
ISO2631-2:2026 annexeA (ou autreédition explicitement justifiée) ; spécifications
Wm/points de référence et tolérancesISO8041 pertinentes. Ensuite seulement :
implémentation versionnée, oracles indépendants, contrôle unités/équivalence,
règle de direction/durée et éventuelle correspondance locale vérifiée. Pas de
validation intermédiaire réclamée pour les opérations logicielles autorisées.

## Résultats réellement calculés — aucune pondération de confort

Grandeur : **maximum des RMS directionnels XYZcapteur**, différent du vecteur
technique également exporté. HypothèseMCU ; unités nominales non étalonnées.

| Indicateur non pondéré | FenêtreMCU | Fenêtres FULL | P50 | P95 | Maximum |
|---|---:|---:|---:|---:|---:|
| Accélération Butterworth1–80Hz (mg) |60s|365|31.137|36.789|51.410|
| Accélération Butterworth1–80Hz (mg) |120s|182|31.436|36.556|50.309|
| Vitesse physique intégrée approchée (mm/s) |60s|365|1.424|2.197|2.990|
| Vitesse physique intégrée approchée (mm/s) |120s|182|1.430|2.169|2.641|

L'écart-type population, les trois axes, la normevectorielle technique, la crête,
les couvertures/qualités et les variantesQPC sont exportés. L'écart-type de ces
fenêtres n'est pas une incertitude métrologique. Des valeurs plus élevées
indiquent une plus forte vibration filtrée selon ce calcul, **pas une gêne
humaine, un défaut de machine ou un dépassement acceptable/inacceptable**.
Aucune comparaison de ces mm/s physiques aux repères de vitesse pondérée BV.

Source utilisée : XYZ archivés **avant** passe-bandePC2–25Hz, pas conversion
des13.36mg publiés. Le filtre causal1–80Hz est Butterworthprototypeordre4/total8,
float64SOS, gain−3.0103dB aux deuxbornes, rollofffini/retard non compensé.
La vitesse résulte deB(s)/s, unzéroàl'origine annulé avant transformation
bilinéaire, réalisant une intégrationtrapézoïdale stable. Relationdiscrète :
`V/A=9.80665/(j·2fs·tan(πf/fs))`, distincte de l'intégrationidéale1/(j2πf).
Biais théorique à80Hz≈−0.30% ; formule/réponse documentées, aucune conversion
constante d'un RMSmg général vers mm/s ni pondération baptiséeWm.

Les11warmupsFIR et10s après chaque début/reprise sont exclus ; gaps, clipping
ou NaN invalident et réinitialisent. FULL exige toute la durée et autant de
sortiesvalides que d'entréesobservées. Par définition/scénario :367fenêtres60s
(365FULL) et184fenêtres120s(182FULL), autres partielles exclues des quantiles.
Pas de valeur zéro de substitution. Ces exclusions sont **nouvelles pour cette
méthode**, sans retoucher les résultats2–25Hz de la mission précédente.

## Sensibilité à la cadence et limites physiques

MCU :2661.4620215Hz ; hypothèseQPC moyenne :2679.5162205Hz. Les deux reprennent
**les mêmes échantillons et fenêtres60/120sMCU**, sans les requalifier en duréesSI.
Le témoinQPC initial, sonSHA et la provenanceR5 acceptée sont vérifiés/réutilisés,
sans refaire l'ancienne campagne. Écartdebase≈0.678% non résolu physiquement.

Sur les minutesFULL, différencesQPC/MCU du maximum RMSdirectionnel :
- accélération : médiane−0.3815%, plage[−0.8726%;−0.0693%] ;
- vitesse intégrée : médiane−0.6323%, plage[−0.7216%;−0.3144%].
Ces scénarios quantifient une sensibilité de calcul, **pas le budget d'incertitude
total**, qui reste non chiffrable pour sensibilité/pose/bruit/orientation/horloge.

FIR107Q15 : SHA binaire0e528c6d…cc1b4 identique au header ; somme32768/DCgain1,
gaincalculéà80Hz≈0.99995955(−0.000351dB), délai53natifs≈1.991msMCU,
réjectionfinie≈−77.73dB au pointfs_sortie−80Hz. Réponse calculée aux1/2/5/10/20/
40/80Hz dans les deuxbases ; erreur d'arrondi bornée0.122mgnominal/axe, sans
hypothèse d'erreurblanche. Pas de qualification complète de repliement/filtrage
sensoriel. La ficheST DS12569rev8 décrit des caractéristiques nominales ;
**aucune réponse physique du montage à1Hz ni niveau de bruit mesuré indépendamment**
n'en découle. DC1g et projectionlente synthétique testés ; changement d'orientation
et composantes gravitaires variables ne sont pas magiquement identifiables.

Le lieu est annoncé plancherPCmachine/double-face mince ; XYZcapteur, correspondance
navire inconnue, aucun étalonnage de pose/sensibilité. UTC_UNK2026-10-31T10:08:19.359021+00:00N : pas d'arrivée,
manœuvre ou régime attribué. R3 reste CLOSED_WITH_RESERVATIONS, digestproducteurRAM
perdu lors de la coupure ; cette analyse ne requalifie ni la chaîne industrielle
producteur→SD, ni8h/V1, ni une expositionA(8), un grade naval ou une structure.

## Vérifications exécutées et conservation des acquis

- **11tests ciblésPASS** : transfertcomplexe gain/phase contre prototypeButterworth
 analytique indépendant, sin1/2/5/10/20/40/80Hz, horsbande, DC1g/inclinaison0.1Hz,
 bruit/impulsion/multiton, clipping/gaps/bords/lots/durée/absence, maxdirection≠norme,
 refuscomparabilité non qualifiée et SQLite/idempotence/cohortes/schéma1/P0.
 Pas d'oracleWm prétendu ; les testsWm requis restent impossibles tant queP0 manque.
- **12contre-calculs réelsPASS**,3plages×2bases×2unités : FFTalgébrique vsSOS temps,
 erreurrelative max3.893e-11 souslimite1e-6 fixée ; contrôle supplémentaire
 vitessephysiquefréquentielle idéale vsapproxtrapèzes, écartmax0.0582% souslimite1%.
 Les trois plages sont sélectionnées sur les minutes de vitesse faible/médiane/forte,
 coordonnéesexactes dansmanifest etreal_crosschecks.json, sans attributionmachine.
- **Intégrité complète renouvelée pour cette nouvelle analyse** : lecteurR3 strict
 COMPLETE/CRC/index/session/footer/qualité etSHA entier avant/après identique.
 Source `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`,
 671088640B dont351312816Butiles,750669records/58552136XYZ, rails0/clips0,
 session8444550407367521, firmwarec3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e.
 SHAentier91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5 ;
 SHAutile5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41.
- Calcul réel entier **43.18s / picRSS274232KiB≈267.8MiB**, lots4096records.
 Aucun tableau58552136objetsPython ni lecture de préallocationcommeéchantillons.
 Dépendances/environnementprivéR5 réutilisés. Unecommandeauxiliaire a utilisé lePython
 système sansNumPy puis a été relancée avec le venv ; pas d'installationglobale.
- Les14tests de l'ancien moteur **inchangé** ne sont pas relancés ; pas debuildSTM32/
 .NET puisqu'aucun code de ces composants n'est modifié. Figure inspectée visuellement,
 liensHTML locaux/JSON/2204lignes et flags normatifs désactivés vérifiés.

## Catalogue secondaire, requêtes et signatures

Base finale locale :
`/home/lolo/dev/msm/reports/TR2-R5-comfort-20261031/comfort-secondary-v2.sqlite`,
626688B, SHA256 **471ca360f35ef5e72fb1c510ce8e6118c31f4a04de0e88e367e2dcad86407a84**.

**Schéma2 secondaire isolé**, aucune migration de v1/live ;1campagne/4définitions/
4analyses/**2204fenêtres**/24références. Définitions : unité/bande/SOS/editionNONE,
statutaxes, qualité/UTC, échelle, horloge, support/site/orientation, version ;
identitéd'analyse=campagneSHA+définitionSHA+codeSHA. Les imports normatifs non
vérifiés sont refusés. Transactions/index/clés/FK/FULL ; aucune insertionXYZbrute.

Deux imports réels : mêmeidentité/comptes, secondidempotent ; intégritéok/FK0.
Quatre requêtes exécutées :8séries unité/base/fenêtre ; pirevitesse60sMCU à18540s,
2.99016mm/s ;184lignesaxes120s dontpartielles signalées ;1statutéditionNONE pour4méthodes.

Passerelle uniquementread-only versv1 :49134métriques/1analyse et indicateurs
raw_DC/band_2_25 toujoursprésents ; SHAe436de69e55e2126c029acc5afea9cb058b0a6fc6ce857e41bc70a26693646c5
inchangé avant/aprèslecture. Anciennes accélérations2–25/PSD/spectrogrammes/raies
conservées etliées dansla page ; aucune modification de DBWindows ni vieuxexports.

La signature refuse les comparaisons d'équivalence intercampagnes si méthode,
version, filtre, bande/unité, fenêtre/horloge, firmware/échelle, lieu/support/axes/
qualité/UTC diffèrent ou si les paramètres critiques ne sont pas qualifiés.
Elle refuse donc actuellement mêmeune fausse équivalence de montage «inconnu».
Les deuxhypothèses de ce mêmefichier restent des scénarios de sensibilité explicites.

## Gates, publication et reprise

| Gate | État réel |
|---|---|
|G0/P0|BLOCKED : Wm/équivalence/tolérances et oracle normatif indisponibles|
|G1|Intégrité et testsnonpondérés démontrés ; testsWm nonexécutables|
|G2|Sixheures préliminairesnonpondérées calculées/contrôlées ; indicateurprincipalpondéré absent|
|G3|PASS pourcohortepréliminaire isolée, requêtes etréimport réel|
|G4|Page/figure/synthèse explicites, diagnosticancien intact ; aucunseuil affiché|
|G5|PASS : code/preuves publiés et36fichiers relus byte-exact surc2c1437b ; cette clôtureBLOCKED est également publiée/vérifiée|

Preuves≈1.03MB : code/tests/méthode/P0/manifeste/CSV/PNG/HTML/sourcesetSHA ;
pas .TR2/DB/SDbrute dansGit. LOCAL_ARTIFACTS.json donne chemins/empreintes réels.
Numériqueévalué45e70a789feb70ed3ed753c13f542252af3bf26e, checkpoint publié et
8fichiers relus byte-exact depuisorigin/main. Code de persistance/page ajouté
et effectivement exercé ensuite, publié dansc2c1437b. Les calculs anciens n'ont
pas été relancés pour cette publication ; résultatnumérique45e70a78 etcodecomplet
c2c1437b distingués explicitement.

WorktreeisoléMSM_vibe-R5-comfort sur7575a051 ; dépôthistorique82169af7,
worktreesR5antérieurs, archives déjà clôturées et stashes6251b988/f3b829e6 préservés.
Aucune résolution de divergence/merge/rebase/reset/clean/forcepush, aucun accès
STM32/SD/COM/serviceWindows ; dernierétatphysiquedéclaréoff/SDauPC nonrevérifié.
Permissions exercées : logicielPC/commits/pushnormaux versmain uniquement.
JournalETAT_COURANT_TR2 actualisé pour acquis/blocage/reprise ; gouvernance et
spécifications non modifiées. Deuxpushnormaux réussis :
-45e70a78 : `R5 comfort: checkpoint P0 source limits and tested unweighted fallback`,
 8fichiers récupérés/relus byte-exact ;
-c2c1437b : `R5 comfort: publish real unweighted six-hour results and isolated schema2 catalogue`,
 36fichiers récupérés/relus byte-exact, dontcode/tests/page/CSV/preuves/rapport/journal/mission.
La dernière miseàjourBLOCKED est ensuite commitée/poussée/vérifiée ; sonSHA et
l'étatGitpropre sont fournis dans la réponse finale, sans référence circulaire.

**Point de reprise :** worktreeMSM_vibe-R5-comfort, codepubliéc2c1437b,
sourceE6N5inchangée, résultatslocauxTR2-R5-comfort-20261031/real-unweighted-v1,
basecomfort-secondary-v2.sqlite/SHAci-dessus, anciencataloguev1intact.
Commencer par revue indépendanteChatGPT des limites etpreliminaires, puis obtenir
un accèslicite auxclauses précisesP0_METHOD.md. Unefoiscelui-cidisponible,
implémenter/versionnerWm etl'équivalencevérifiée, testercontreoracle ettraiter
la même archive ; pas de nouvelle acquisition ni répétition de la tranche nonpondérée.
Aucune conclusiondeconfort acceptable/inacceptable n'est disponible actuellement.
