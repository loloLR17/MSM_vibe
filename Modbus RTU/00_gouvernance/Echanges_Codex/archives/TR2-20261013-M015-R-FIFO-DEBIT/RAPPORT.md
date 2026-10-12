---
mission_id: TR2-20261013-M015-R-FIFO-DEBIT
status: DONE
base_ref: main
base_sha: 7f30bc548938d197f52202454392a4f90c6602ec
initial_head: fbcab9c8e99e43be5c5ebb3f008f1270a52c7d26
result_sha: 76b4a17577171c0009ada7e0c67e387286d282dc
created_at_utc: 2026-10-12T23:05:22Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-R — budget temporel établi sans nouvel essai

## Décision

L'analyse A suffit : **le SPI2MHz possède une marge théorique, mais la cadence du service complet Q est insuffisante**. Un segment128mots/896octets doit être traité en environ4,27ms au débit nominal ; les19intervalles DWT observés demandent4,99ms. Le coût supplémentaire au payload est1,357ms, contre0,635ms de budget disponible. Il manque environ0,722ms par segment. La limitation opérationnelle est établie ; elle n'est pas attribuée exclusivement au CPU ou au parsing.

**Aucun essai conditionnel B**, flash, reset, debug ou acquisition dans R. Cette décision suit la consigne d'arrêt dès qu'une décision technique est possible. Répétabilité et512XYZ Q conservées ;4096 reste BLOCKED, cause O/P non établie. Statut **DONE pour le diagnostic court**, après publication et relecture distante au commit `cf99999aae123162307869ecb66533a7765ec50b`. Cela ne transforme pas4096 en succès.

## Débit requis et disponible

| Origine | Débit payload | Portée |
|---|---:|---|
| ODR configurée80000/3Hz,7octets parXYZ et unTS pour8XYZ |210000octets/s|Calcul nominal |
| Timestamps Q,285TS/6870ticks,8XYZ entreTS |208349octets/s|Estimation avec tick nominal12,5µs |
| Bilan FIFO avant overflow :2432mots drainés +359mots accumulés sur94,780ms |206130octets/s environ|Estimation d'arrivées basée sur niveaux et DWT |
| SPI2MHz idéal |250000octets/s|Limite fil, sans overhead |
| SPI2MHz, trames commande1+payload896 |249721octets/s|Limite idéale avec commande FIFO |
| Payload HAL Q |environ246721octets/s|Mesure DWT, phase payload seule |
| Service complet Q,19intervalles DWT |179615octets/s|Mesure DWT entre débuts commande FIFO |

Le210ko/s nominal inclut les tags et timestamps :26666,667XYZ/s ×7×9/8. Le calcul timestamp donne26457XYZ/s sur les octets reçus, **pas une ODR calibrée indépendante** : INTERNAL_FREQ_FINE/63h n'a pas été relevé dans Q, donc sa correction de résolution n'est pas disponible. Le bilan FIFO donne29447mots/s, soit26175XYZ/s sous le ratio8:1 ; lectures asynchrones/quantification/clocks non calibrées limitent sa précision. Ces deux estimations restent au-dessus du service mesuré.

[Datasheet ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), §4.3/9.28/9.39 : format FIFO/tag, intercalage et résolution nominale timestamp ; correction INTERNAL_FREQ_FINE à distinguer. Avec l'hypothèse de dimensionnement ODR+2%, le payload requis serait214200octets/s, toujours sous le plafond SPI idéal mais avec moins de marge. La tolérance datasheet n'est pas une mesure du module.

## Budget DWT, mêmes19intervalles avant overflow

| Coût moyen parsegment | Durée | Nature de la preuve |
|---|---:|---|
| Payload FIFO HAL |3,631625ms|Mesure begin/end avant fermeture |
| Commande FIFO HAL |0,020286ms|Mesure |
| Deux lectures statut, phases HAL |0,098773ms|Mesure cumulée |
| Trois lectures configuration, phases HAL |0,181563ms|Mesure cumulée |
| Résidu entre appels/snapshots |1,056188ms|Durée mesurée par différence, fonctions non isolées |
| **Total cycle** |**4,988434ms**|Mesure DWT, min4,96925/max5,000625 |

La somme est vérifiée. Les trois lectures configuration sont07/4octets,0F/10octets et19/1octet ; deux statuts3A/2octets, et le parsing128mots se placent entre drains. Le résidu inclut les fins HAL après snapshot, GPIO/CS, copie du payload dans le journal, vérifications et parsing/préparation des appels. **Aucune durée de parsing seul ni de memcpy seul n'est mesurée.** L'enveloppe entre fin snapshot payload et début commande du statut suivant vaut0,623076ms en moyenne ; elle inclut copie/parsing/fermeture/préparation, pas seulement calcul CPU. Pas de HAL_Delay1ms dans la portion saturée : les niveaux disponibles restent>=128 et le délai de fifo_window_run ne s'exécute que lorsqu'ils sont insuffisants.

Le181,1ko/s Q provenait de ticks milliseconde arrondis et des débuts de traces. Le179,6ko/s R utilise les débuts commande DWT exacts sur la même acquisition : refinement de frontière/quantification, aucune nouvelle mesure cible. La croissance148→507mots avant la dernière vidange confirme le déficit ; trace151 raw00EA/niveau512 déclenche ensuite le refus Q inchangé. Journaux/buffers non épuisés, HAL sans erreur ; R ne requalifie pas le préfixe rejeté.

## Horloges et prochain choix

Source `src/platform.c:103` et suivantes :MSI16MHz, PLLabsente, AHB/APB3÷1, SPI3 sourcePCLK3, prescaler÷8. Dans les328snapshots Q, CFG1=20070007 et MBRbits30:28=2, cohérents avec÷8 selon CubeU5v1.9.0. `bench_core_hz=16000000` dans la capture Q. SCK2MHz calculé, cohérent avec3,63ms pour896octets contre3,584ms de temps fil ; **aucune mesure indépendante SCK/oscillateur**.

CPU et SPI partagent cette chaîne : changer seulement MSI ferait varier les deux, donc pas de prétention CPU-only. Le diviseur SPI est réglable séparément ;÷4 à CPU16 donnerait4MHz calculés. À overhead constant, ce serait un cycle estimé≈3,15ms, mais les capacités HAL au débit supérieur ne sont pas qualifiées. Ce scénario est une **proposition pour un prochain contrat**, pas un correctif validé ni une acquisition R.

Prochaine décision : conserver tous les contrôles et qualifier un changement de vitesse SPI seul, ou instrumenter séparément copie/parsing/fermetures si une optimisation logicielle est privilégiée. Exiger une cadence complète sous4,27ms avec marge ; ne pas choisir un test CPU-only sur une horloge partagée sans compensation/vérification explicite du SPI. Aucun chantier DMA/refactoring engagé. Les chiffres disponibles permettent ce choix sans consommer de nouveaux essais dans R.

## Vérification, préservation et publication

Analyse hors ligne effective `analysis.py` : décode les binaires Q, compare les164traces auJSON existant, reconstruit2560mots byte-identiques au buffer officiel, vérifie HAL/validité, partitions DWT et19intervalles. CSV et summary.json reproductibles. Aucun firmware modifié : tests/builds Q conservés sans répétition ; aucun nouveau cross-build utile pour des documents et calculs hors cible. Firmware évalué historiquement76b4a17/BINd76a6104a2adc19f77e3b9849e32b9d52312dfd2840eb01f76eb4749cf687b09, source analysée mainfbcab9c.

AGENTS/protocole/journal inchangés depuisQ contrôlés sur Git ; contexte normatif et journal intégral précédemment consultés réutilisés, étatQ courant relu. Archives contrat/rapportQ copiées byte-exactes depuis7f30bc5. WorktreeR isolé, dépôt principal82169af et tous les worktrees/stashes préservés. `allow_commit/allow_push` exercés sur main ; `allow_flash/allow_debug` non exercés.

XLoff/FIFObypass/TSoff, UIDsecond, faults0/detach sont vérifiés **dans les preuves finales Q conservées**, pas comme une nouvelle observation matérielle R. Aucun accès primaire/M014-H/FRAM/SD/Modbus/HP, aucune manipulation. État matériel actuel depuisQ NON REVÉRIFIÉ ; aucune affirmation nouvelle de santé cible.

Journal/roadmap actualisés pour ce budget et le choix conditionnel non exécuté ; M015 IN PROGRESS/4096BLOCKED, acquis512 conservé. [Preuves R](preuves/TR2-20261013-M015-R/README.md), originaux `/home/lolo/dev/msm/reports/TR2-mission015-R-20261012T230522Z`, [corpus Q](preuves/TR2-20261013-M015-Q/). Rapport et calculs publiés par push normal au commit `cf99999aae123162307869ecb66533a7765ec50b` (« M015-R: establish FIFO throughput budget from existing DWT evidence ») ; 13 livrables intégralement relus depuis origin/main et byte-identiques. Preuve publication-readback.json. Worktree propre après publication ; dépôt principal et stashes préservés. Le présent complément clôt la mission et sera lui-même relu après push. SHA final fourni après contrôle ; pas de réussite4096 ni de perte zéro annoncée.
