---
mission_id: TR2-20261029-R1-ARM-DSP-FIFO-BENCH
status: IN_PROGRESS
base_ref: main
base_sha: 8ab7aeccf3a623cefc9fe92a3d88c44c5e6d1ea8
initial_head: c4c0d1820487f3394a98a46960375ede6feb79cd
result_sha: 8c14f1037af2e82d252852cdbfcb184614135216
created_at_utc: 2026-10-29T16:07:17.188444+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R1-ARM — résultats physiques acquis, publication finale en cours

**Critères techniques atteints dans la portée du banc isolé TR2-002 : micro-benchmark
apparié réel, FIFO sans/avec Q15, puis dix secondes avec FIR, sans faute observée et
avec arrêt effectif.** IN_PROGRESS porte uniquement sur la publication/relecture de
ce bilan. Pas de qualification de production, d'endurance ou de coexistence FRAM/RTU.
Les preuves micro du checkpoint962f9028 ont été relues depuis origin/main ; elles
n'ont pas été rejouées. Les preuves FIFO complètent ces acquis.

## Identité, images et préservation

- Contrat publié c4c0d182, base8ab7aecc. Source FIR scientifique inchangée0ad417d6.
  Micro réellement évalué : **0c4b86997829496768bd31bf0fd9bf7b27dbdc0a**.
  Banc FIFO réellement évalué : **8c14f1037af2e82d252852cdbfcb184614135216**.
- Cible relue : TR2-002/NUCLEO-U575ZI-Q, ST-LINK **0032001F3235510C37333439**,
  UID **001B0046 /38425018 /20333157**. TR2-001 n'a reçu aucun flash, debugger,
  reset ni commande START/STOP. L'utilisateur confirme ne pouvoir alimenter les deux
  cartes simultanément ; son arrêt d'alimentation a été accepté pendant le banc.
  Le service Windows est conservé et ses pertes explicites ne qualifient pas une panne
  du firmware principal. Son fonctionnement actuel et sa Flash/FRAM n'ont pas été relus.
- Une sauvegarde initiale Flash2MiB vérifiée, conservée localement : SHA-256
  `dd1a2b44d155abbfc094dacf498d61f973688f83cf6c7685ecb265bc444fb1f1`.
  Préfixe21860B identique au probeB historique `fdb7fa…df829`, vecteurs valides.
  Aucun empilement de sauvegardes ni restauration inutile. Branche de protection
  `archive/tr2/point-retour-pre-spi2-20261029` conservée àa3f5ac33 ; stashes/worktrees préservés.
- Deux flashs standards au total dans cette mission : micro, puis FIFO ; programmation
  normale des pages nécessaires à0x08000000, SN explicite, `-v -rst`, et readbacks exacts.
  Aucun mass erase/OptionBytes/RDP, reset logiciel supplémentaire commandé ou écriture
  persistante métier. Aucun accès SD/SPI2, FRAM/SPI1, B5/RENEW, COM4 ou politique Windows.
- BIN FIFO34192B, SHA-256 **`d30c8bd9b2ad0fc1c5b02982b8384b4866d7760a8e3d2c67c2391bfb5b2f868d`** ;
  ELF **`898fc69da26543b13189b43ecd86df1a510e5df9c6b14743fc8387157cd76c9e`**.
  BIN micro13260B `9ffe22d574053bea653ba21e86dc92344f874ccbd824ba531ee86400cf5507e6`.

## Micro-benchmark acquis

Même image, source synthétique déterministe en RAM, huit cas sans/avec FIR pour des
blocs1/8/60/128,256 blocs par cas, ordre apparié alterné. DWT réel, HCLK configuré
160MHz, interruptions/SysTick actifs, aucune interaction debugger pendant le calcul.
Lecture/barrières28cycles non soustraits. Chaque distribution brute est publiée.

| Sources/bloc | Émission sans FIR moyenne,cycles | Émission Q15 moyenne /P95 /P99 /max,cycles | Bloc Q15 max,µs |
|---:|---:|---:|---:|
| 1 | 182.000 | 9371.385 /9406 /9427 /9427 | 62.669 |
| 8 | 182.288 | 9363.678 /9406 /9434 /9445 | 84.863 |
| 60 | 182.121 | 9364.755 /9414 /9440 /9482 | 543.081 |
| 128 | 182.093 | 9364.397 /9414 /9445 /9492 | 1167.544 |

Pour60sources/bloc, temps instrumenté ramené àl'ODR **nominale**26666,667Hz :
3,616% sans FIR contre24,064% avec, delta20,448points. C'est une déduction depuis
les cycles réellement mesurés, pas une mesure de cadence capteur ou un WCET.
**128sources dépassent la référence work1ms** : cette contrainte reste visible.
Le quantum FIFO opt-in64mots a été choisi en conséquence, sans relever les limites.

## Dérivation FIFO et qualification physique

Le raccordement du kit a été annoncé par l'utilisateur ; tensions et continuité ne
sont pas des mesures réalisées par Codex. La présence du capteur est ensuite établie
par la lecture réelle **WHO_AM_I0x7B** et les transactions validées. SPI3 unique,
PC9–PC12, mode3, HSI16/4=4MHz, pleine échelle logique2/CTRL1_XL **0xAC=±8g** durant
les captures. Ce readback ne qualifie pas la précision métrologique à±8g.

Modifications minimales : constructeur alternatif du mapper DEC8 vers un sink
exclusif conservant index et microsecondes, défaut LiveMetrology inchangé ; limite
de poll configurable seulement àl'arrêt, défaut128 inchangé. Un seul propriétaire
FIFO et les mêmes contrôles tags/parité/compteurs/horloge/transferts. Tout défaut
invalide le DSP et arrête le capteur. Aucun second lecteur ou file archive définitive.

Quatre déclenchements distincts, sans pause GDB dans les fenêtres :

1. WHO et calibration2s, **6577ancres /193brackets**, span1890868µs/149957ticks,
   résidu maximal33µs (borne100µs), recalcul indépendant exact depuis les bruts.
2. Sans FIR2,000649s :52600 XYZ temporels /5260 sorties un-sur-dix.
3. Avec FIR2,001425s :52608 XYZ /5261 sorties.
4. Après succès du couple court seulement, FIR10,000329s :264056 XYZ /26406 sorties.

| Scénario | XYZ acceptés /sorties | Service moyenne /P95 /P99 /max,µs | Work max,µs | Gap max,µs | FIFO pic,mots |
|---|---:|---:|---:|---:|---:|---:|
| Sans FIR,2s | 52,600 / 5,260 | 633.831 / 650.100 / 677.231 / 719.706 | 136.606 | 973.000 | 29 |
| Avec FIR,2s | 52,608 / 5,261 | 1285.016 / 1399.869 / 1438.900 / 1455.119 | 570.800 | 1709.000 | 51 |
| Avec FIR,10s | 264,056 / 26,406 | 1313.839 / 1399.750 / 1400.525 / 1455.269 | 570.575 | 1709.000 | 50 |

Tous les polls bruts sont disponibles :2255/1301/6381 observations respectivement ;
`fifo-comparison.json` donne aussi les distributions complètes work/gap/XYZ par poll.
Le work est le consommateur complet (copie bornée, mapper, validation, FIR/hachage),
le service inclut le contrôle d'horloge et SPI/parser/work. Le gap est quantifié à1µs,
premier zéro conservé. Les max ne sont pas additionnés comme s'ils étaient simultanés.

Occupation du **service mesuré** :71,441% sans FIR,83,531% FIR court,83,833% FIR long.
Occupation du work :10,499% /29,634% /29,739%. L'attente250µs est active ; ces parts
ne sont **pas** une mesure CPU avec idle. Les rafales plus grandes avec FIR amortissent
les contrôles SPI : le delta de service n'isole donc pas àlui seul le coût du FIR.
Les sessions physiques successives ont naturellement des signaux/hashes différents ;
seule la comparaison micro utilise exactement le même jeu source.

Cadences issues des événements observés :26435,072 /26437,937 /26435,570 XYZ/s.
Temps/index réellement fournis par le mapper DEC8, aucun arrondi milliseconde ni
ODR nominale utilisée pour fabriquer les événements.20/20/100contrôles dynamiques,
résidus5/8/11µs sous100µs. Aucun FULL/OVR/watermark signalé, faute de transfert,
tag/compteur/timestamp, refus de log ou overflow de sonde.11sorties WARMUP par
session FIR, zéro clipping/rail ; les rails source sont déjà refusés par le parser.

Préfixes conservés par session :2048mots SPI,1024entrées exactes,128sorties FIR.
**103sorties de chaque session FIR** sont vérifiées exactement hors cible par une
convolution entière indépendante, avec temps causal/indices/WARMUP ; les1024XYZ
sont comparés au préfixe capteur. Le reste repose sur compteurs/hashes/contrôles de
continuité, **pas sur une archive intégrale des XYZ**. Au STOP4/4/8XYZ restent sans
ancre future : ils ne sont ni extrapolés ni émis, aucun tail artificiel.

## Marges, ressources et limites

- Références historiques conservées1000/2500/5000µs. Pire mesure du couple court et
  long :work570,800µs, service1455,269µs, gap1709µs ; marges observées429,200 /
  1044,731 /3291µs. Résultats favorables **dans ce banc instrumenté**, pas une borne
  industrielle. Le quantum64mots n'a même pas été atteint :max51mots.
- STOP court/long190/189/190µs, readbacks conformes. Dernières transactions SPI
  décodées indépendamment depuis la RAM :WHO7B, CTRL1_XL0C/ODRoff,
  FIFO_CTRL480/bypass, CTRL1000/timestampoff, tous transferts complets.
- Empreinte FIFO :text34172B/data20B/BSS731416B, dont gros tableaux de sondes RAM.
  Ce coût de banc n'est pas le besoin RAM du FIR (état744B/coeff214B acquis).
  Marqueur de pile4KiB :profondeur observée816B, plancher3280B intact ; hors startup
  et garde de frame256B, pas une preuve exhaustive de pile. Micro464B/plancher1584B.
- GCC ARM14.2.1, Cortex-M33/Thumb soft ABI, `-O2`, sections éliminées ; MAP, attributs,
  symboles, options build et `.su` publiés. Q15 source inchangé, aucune exploration Q31
  nécessaire face aux marges observées avec quantum borné.
- **FRAM/RTU/UART NON EXÉCUTÉS** : modules non confirmés/accessibles sur ce banc,
  aucun test de coexistence simulé présenté comme physique. SysTick reste actif,
  2001/2001/10000IRQ observées ; aucune preuve nouvelle sur IRQ UART ou Modbus.
  Pas de GO stockageSPI2, endurance1h/8h, production, LIVE/AC1 Windows ou métrologie.

## Validations et incidents conservés

Changement d'interface partagée : validation complète justifiée par le risque de
régression, **150tests hôte et cross-build canonique PASS**, sortie réelle conservée.
Test supplémentaire du quantum8mots sur le propriétaire mock existant PASS, maintien
du défaut128 et refus de mutation en marche ; test DEC8 sink/rollover/µs/défaut PASS.
Deux tests des outils de rejeu/corruption PASS, distincts des essais matériels.
Cross-build de l'image FIFO réelle PASS. Les anciens59rejeux SciPy inchangés et le
micro-benchmark physique réussi ne sont pas relancés.

Erreurs analysées, non masquées : indentation `-Werror`, dépendances du lien corrigées
avant tout flash ; script sans bit exécutable invoqué ensuite avec `bash`. Premier
GDB préflight impossible (`Failed halting target`, timeout45s), contrôle CLI refusé ;
cause non isolée, pas de flash avant UID relu sur demande utilisateur. Pas de diagnostic
certain d'un défaut de câblage àpartir de cet incident. Ancien échec de dump micro
corrigé par capture seule au DONE, sans rejouer le micro. Décodeur RAM STOP corrigé
hors cible (structure10champs, pas11), aucune répétition de transaction matérielle.

## État final, preuves et publication

TR2-002 conserve le firmware FIFO **d30c8b…f868d**, terminal4/run4/error0,
capteur arrêté et vérifié, debugger détaché/serveur fermé. Aucun autorun après reset :
retour au WAIT avant commande capteur. Flash initiale récupérable par backup vérifié,
restauration inutile non effectuée. TR2-001 reste hors expérience, alimentation actuelle
non relue ; service et données persistantes non modifiés. Deux stashes historiques et
worktrees conservés, aucune résolution automatique de divergence.

[Projet et procédure](../../../experiments/r1_dsp_arm_bench/README.md),
[preuves micro](preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/micro/README.md),
[preuves FIFO](preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/fifo/README.md).
100pièces FIFO exactes compressées avec SHA brut/stocké, en plus des38pièces micro ;
originaux dans `/home/lolo/dev/msm/reports/TR2-r1-arm-dsp-fifo-bench/`.
Le journal ETAT_COURANT_TR2.md est actualisé :résultats physiques/marges/arrêt et
limites FRAM/RTU. Suite proposée àChatGPT :revue indépendante A-01, puis mission
séparée de qualification microSD SPI2 sur TR2-002 ; AC1 Windows reste distinct.

État de publication : branche locale `codex/r1-arm-dsp-fifo-bench`, destination
explicite `refs/heads/main`. Commit logiciel8c14f103 : `feat(bench): prepare bounded
single-owner FIFO FIR measurements with exact event times`, déjà poussé normalement
surmain et référence récupérée. Dépôt propre après ce commit ; seuls ce bilan,
le journal et les preuves/README de mission sont ajoutés ensuite. Stashes6251b988 /
f3b829e6 relus inchangés ; worktrees et ancien dépôt principal préservés. Bilan présent
àpublier puis relire intégralement depuis GitHub ; verdict DONE seulement après cette
relecture. Aucun blocage technique restant dans la portée capteur FIFO mesurée.
