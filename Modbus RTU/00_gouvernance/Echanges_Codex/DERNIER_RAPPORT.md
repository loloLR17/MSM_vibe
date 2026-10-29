---
mission_id: TR2-20261029-R1-DSP-DECIM10-HOST
status: DONE
base_ref: main
base_sha: a94fa2a87de7d93220e3e18f2dbd2f41c412bb83
initial_head: 545b96e1b8bec628219b0dd91c46fc51328d76cc
result_sha: 0ad417d621c0023a2c326383040a5d4e73016329
created_at_utc: 2026-10-29T12:34:45.701841+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R1-DSP-A — FIR XYZ /10 livré, validé en logiciel et relu sur GitHub

La brique portable **FIR 107 taps / décimation 10**, avec conservation du DC, passe les portes hôte et cross-compilation de cette mission. **149 tests C**, 59 rejeux C comparés à SciPy et linkage ARM réussis. Les trois témoins float64 externes sont reproduits. Les portes logicielles et la relecture distante de cette tranche sont satisfaites ; statut `DONE` limité à ce périmètre.

Ce résultat ne qualifie ni CPU/FIFO sur carte, ni stockage, ni AC1, ni la chaîne industrielle. **Aucun flash/debug, accès STM32 ou intervention Windows/COM4** : ces opérations sont exclues du contrat et n’ont pas été utilisées. La mission AC1 archivée reste BLOCKED en attente externe ; aucun nouveau travail AC1 n’est entrepris.

## Références, Git et modifications

Mission récupérée depuis GitHub à **545b96e1b8bec628219b0dd91c46fc51328d76cc**. `AGENTS.md` relu avec son ajout Windows du29/10 ; celui-ci n’étend pas le périmètre de cette tranche. Gouvernance/protocole/journal et les trois références R0 /10 sont consultés. Le §0 prioritaire du plan D10 est appliqué ; l’ancienne cascade2kHz A225+B69 demeure historique, sans dégel des spécifications V1.

Dépôt historique propre à82169af ; worktree de publication propre àfec0d55e au déclenchement, puis branche locale isolée `codex/r1-dsp-decim10-host` basée sur la nouvelle référence. Aucun reset/merge/rebase/nettoyage. Les deux modifications préexistantes de l’ancien worktree AC1 et les deux stashes6251b988…/f3b829e6… sont conservés. Le rapport AC1 courant était byte-identique à son archive avant remplacement par le présent rapport ; aucune archive n’a été réécrite.

La branche distante `archive/tr2/point-retour-pre-spi2-20261029` est vérifiée à **a3f5ac33a728a5afd028192654c17e5b04ee8d7b**, inchangée.

Commit logiciel **0ad417d621c0023a2c326383040a5d4e73016329** — `feat(dsp): add causal exact-DC Q15 XYZ decimator with independent host qualification`, poussé normalement surmain. Le diff est limité à :

- `05_Firmware/include/tr2/domain/acquisition/fir_decimator10.h`, son implémentation et son header de coefficients généré ;
- CMake hôte : source portable, test de lifecycle et outil de replay ; aucun profil produit modifié ;
- `tests/dsp/` : génération déterministe, oracles SciPy, comparaison C float32/Q15/Q31, contrôle de dérive et projet de linkage ARM isolé ;
- [note technique R1-DSP](../../05_Firmware/R1_DSP_DECIM10.md), preuves et présent rapport/journal.

## Architecture et sémantique vérifiées

Audit ciblé : XYZ source `int16`, callback unique `FifoLiveConsume`, temps/index reconstruits par `fifo_event_time_consume`, puis appel direct au calcul LIVE. Aucun second lecteur FIFO n’est ajouté. La future intégration devra dispatcher les mêmes XYZ validés dans le foreground ; elle n’est pas faite sous ce contrat.

Le composant est sans allocation dynamique ni I/O. Il garde107XYZ en anneau et calcule321MAC seulement aux index relatifs0,10,20… d’un segment explicitement identifié. État/phase conservés entre chunks arbitraires ; `ceil(N/10)` sorties, aucune queue après STOP. Les11premières sorties portent WARMUP pour le zéro initial du FIR. Un nouveau segment remet l’état à zéro, avec ID strictement croissant.

Rupture d’index, entrée invalide, disponibilité temporelle changée ou timestamp non croissant : fermeture explicite avant consommation du fautif ; aucun trou interpolé ni reprise automatique. Le batch donne les comptes de préfixe exacts et arrête avant consommation si la capacité de sortie manque. Les fautes FIFO/tag/anchor devront être relayées par le futur propriétaire source, sans prétendre les détecter ici depuis un simple XYZ.

Le timestamp de sortie est **l’extrémité causale fournie**, pas l’instant du centre du filtre ni un UTC inventé. Le délai de groupe est53sources, soit1,9875ms à la cadence nominale seulement. L’autorité fournit des microsecondes déjà déroulées ; un rollover32bits brut est refusé, des temps64bits continus le traversent correctement. La fréquence rationnelle externe est conservée et divisée par10, avec provenance nominale/mesurée explicite ; le composant ne mesure pas l’ODR.

B3, AC1, B4/B5, FRAM, stockage et services PC sont inchangés. La sortie little-endian de l’outil hôte est un format de **preuve**, pas une nouvelle sérialisation normative d’archive.

## Arithmétiques : choix et marges observés

Coefficients générés depuis le `firwin`107/Kaiser7,85726 prescrit. Q15 arrondi nearest-even, puis correction du tap central de−5unités pour rendre la somme exactement32768 ; symétrie conservée. Ce choix versionné diffère du Q15 simplement arrondi du tableau R0, et sa réponse complète est donc recalculée et testée.

| Grandeur numérique | Résultat Q15 retenu |
|---|---:|
| Gain DC après transitoire | Exactement1 ; constantes±1 et rails conservées |
| Somme des valeurs absolues des coefficients entiers |47468|
| Borne absolue de tout accumulateur et somme partielle |1555431424 < INT32_MAX|
| Erreur maximale de gain0–700Hz |0,002766dB|
| Pic stopband observé, grille inclusive262145points |−67,468dB|
| Borne continue, coefficients Q15 exactDC |−66,538dB|
| Borne tonale ajoutant arrondis entrée/sortie, A=12000comptes |−63,412dB|
| Écart maximal Q15/float64 observé |25comptes, cas adversarial ; MIX3comptes |
| Tolérance générale fixée avant les rejeux |27comptes, dérivée de la normeL1 d’erreur des coefficients et des arrondis |

L’arrondi signé nearest-even est implémenté sans décalage signé dépendant de l’implémentation, puis clamp explicite int16. La borne d’accumulateur couvre **tous les historiques int16**, pas seulement les sinusoïdes. Q31 donne0compte d’écart aux jeux float64 testés, float32 au plus1compte. Les alternatives sont de vrais kernels C de test, pas des seules simulations de coefficients.

Le choix Q15 est le plus petit en coefficients (214B contre428B), satisfait les tolérances numériques et évite les accumulateurs64bits. **Aucune vitesse relative Q15/Q31 n’est prétendue** : Q31 utilise aussi une instruction entière `smlal`. L’ABI actuelle soft-float fait utiliser au float32 des routines logicielles. Un choix de rendement définitif attend les cycles DWT du lot physique.

Deux qualités sont séparées : codes source aux rails (axes et compteur deXYZ) et sortie FIR écrêtée (axes et compteur de sortiesXYZ). L’échelon32760→−32760 génère6sorties écrêtées **sans source aux rails**, sans wrap. Les compteurs diagnostiques saturent avec flag explicite ; les index/comptes structurants ne peuvent pas boucler.

## Tests exécutés et portée

**Validation complète appropriée au nouveau cœur portable :** `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` : **149/149 PASS et cross-build canonique PASS**, nouveaux et anciens tests. Le testR1 a ensuite été enrichi pour vérifier les timestamps exacts à5taux rationnels et revalidé seul, normalement et sousASan/UBSan. Le cœur n’a pas changé après la suite complète ; pas de répétition inutile de toutes les validations.

**Oracle indépendant :** Python NumPy2.3.5/SciPy1.17.0, `upfirdn` puis troncature causale et nearest-even/clamp. **59rejeux C** ; Q15/Q31 exacts à leur convolution quantifiée indépendante, float32 dans1compte. Les métadonnées qualité des modes candidats restent explicitement celles de l’état Q15 de référence ; seuls leurs XYZ numériques sont comparés séparément.

D10-00..D10-11 couvrent cette couche : constantes/impulsion et axes, tons sinus/cosinus2/5/25/100/333⅓/666⅔/700Hz, fréquences hors bande et repliements affichés, grille et borne continue, toutes les découpes prescrites/pattern mixte, multitonDC, plein format aléatoire/adversarial, rails/ringing, STOP et segments courts/restart/perte/invalidité, horloges et rollover. Les sorties Q15 sont byte-identiques entre partitions. Le2Hz reçoit8s nominales, plus de12périodes. D10-12 (coexistence physique) etD10-13 (fichiers/stockage/coupure) sont explicitement différés.

Les trois témoins float64 publiés sont **reproduits exactement**, chacun160000sources→16000XYZ/96000octets :

- DC : `7a90f77cc609e017be51c46ae4d9cac3d8476bdf2abe396742da5c504341a828` ;
- impulsion : `27f14a2cb1ff2d01bc9df168a1cabb756f1419b1ad78903a72aa416b28e14995` ;
- MIX : `56696f8d06264bcb24152ec8b705aa209056309b194c1853c9c48dbb5e4e093d`.

Aucune égalité bit à bit Q15/float64 n’est exigée ou inventée. Le rejet continu des coefficients utilise la grille plus le majorant de Lipschitz centré et une marge numérique ; ce n’est pas une certification formelle des opérationsIEEE ni une preuve du filtre analogique capteur. Le plancher de quantification doit être distingué d’un rejet relatif universel à très petite amplitude. La bande de sortie700–1333Hz n’est pas garantie :1600Hz se replie vers1066⅔Hz, hors critère−60dB.

**ASan/UBSan :** testR1 PASS, puis9rejeux RANDOM/STEP/ADVERSARIAL×3arithmétiques PASS. Aucun échec de compilation ou de résultat numérique n’est masqué. Environnement Python : `venv` initialement incomplet fauteensurepip ; installation dans le nouvel environnement via le pip existant, sans modification de Python système. Aucun critère n’est affaibli pour obtenir les témoins.

## Cross-build ARM, SRAM et limites CPU

GCC14.2.1, `-mcpu=cortex-m33 -mthumb`, export des commandes/attributs/désassemblages. Le projet isolé est optimiséO2, sans modification du profil produit ; ABIsoft observée, pas d’argumentsVFP. Q15 utilise`mla`, Q31`smlal`, float32`__aeabi_i2f/__aeabi_fmul/__addsf3`. Ni activationFPU ni conversion en pourcentageCPU présumées.

| ELF uniquement linké | text | data | bss |
|---|---:|---:|---:|
| Baseline |16|0|2568|
| Q15 |2008|0|3384|
| Q15+alternatives |4000|0|3416|

Le BSS inclut la réserve heap/pile2560B du linker réutilisé. Delta Q15 :1992Btext et816Bbss, dont **744B d’état**,32B d’entrée et40B de sortie. Coefficients214B enFlash. `.su` donne80B pour le corpspush et104B pourprocess ; leur imbrication représente184B **hors caller/libc/IRQ**, sans high-water physique. Aucun bufferarchive/SD n’est alloué et aucune RAM libre garantie n’est annoncée.

Les avertissements newlib de stubs système non implémentés du canonique sont conservés dans le log ; aucun échec de compilation/linkage et aucune suppression de contrôle. Ils ne constituent pas une exécution de ces appels sur cible.

Ces ELF de linkage **n’ont pas de table de vecteurs ni de startup appareil** ; ils n’ont jamais été flashés/exécutés. Le canonique cumulatif compile le nouveau module en archive puis le retire au linkage, car il n’est pas raccordé au runtime : BIN inchangé **85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5**, text172652/data480/bss602240. Ce profil n’est pas l’image AC1 historiquement installée ; ne pas mélanger leurs budgets mémoire.

**856000MAC/s reste un calcul idéal**, pas une mesure STM32. Les cycles moyens/P95/P99/max, le WCET, les maximaFIFO/work/service/gap, les IRQ, la pile réelle et les marges sous FRAM/RTU seront mesurés dans une mission R1-ARM physique distincte et explicitement autorisée. Aucun DWT n’est lu ici.

## État final et suite

Aucun accèsWindows/COM4/STM32 : leur état opérationnel n’est pas relu ni transformé en validation fraîche. Aucun changement de service, DLL, lanceur, SQLite, firmware, FRAM, microSD, identité ou OptionBytes par cette mission. Les travaux AC1 et ses preuves restent archivés et non qualifiés ; les autorisations de cette tranche ne permettent pas de les reprendre.

La brique est prête **pour qualification ARM physique suivante**, pas pour une annonce d’enregistrement8h ni un branchementSPI2. Cette future intégration devra conserver un lecteurFIFO unique, le dispatch temporel existant et les budgets coopératifs, avec comparaison appariée sans/avecFIR. Ne pas flasher les ELF de linkage fournis comme preuves.

Journal `ETAT_COURANT_TR2.md` actualisé pour les faits logiciels et les limites ; gouvernance, contrat, normes, archives et branche de retour inchangés. [Commandes, matrice D10, logs, manifestes et artefacts hachés](preuves/TR2-20261029-R1-DSP-DECIM10-HOST/README.md). Preuves locales `/home/lolo/dev/msm/reports/TR2-r1-dsp-decim10-host/`.

Le SHA du commit final contenant le rapport est fourni dans la réponse finale après contrôle distant. `DONE` concerne uniquement cette tranche hôte/cross-compilation ; la revue indépendante ChatGPT et la qualification matérielle restent distinctes.


## Publication finale et état Git

Code **0ad417d621c0023a2c326383040a5d4e73016329**, puis bilan/preuves **45fb24d835aeac12d501b956bee8a75c94c4f1bd** publiés par push normal surmain. À **12:36:05UTC le29/10/2026**, référence récupérée de GitHub : **64fichiers relus intégralement et byte-identiques**, **45artefacts** décompressés avec SHA256 bruts/stockés vérifiés, trois témoins numériques recalculés depuis les octets distants,149tests et59rejeux contrôlés dans leurs sorties conservées. [Trace de relecture](preuves/TR2-20261029-R1-DSP-DECIM10-HOST/remote-readback.json).

Worktree de publication propre/HEAD égal au distant au bilan ; ancienmain propre82169af, deux modifications préexistantes du worktreeAC1 et deux stashes intacts, branche de retoura3f5ac33 inchangée. Cette mise à jour consigneDONE ; sa publication finale est à son tour contrôlée avant réponse. Aucun test ou build supplémentaire après obtention des preuves, aucune opération matérielle ou Windows.
