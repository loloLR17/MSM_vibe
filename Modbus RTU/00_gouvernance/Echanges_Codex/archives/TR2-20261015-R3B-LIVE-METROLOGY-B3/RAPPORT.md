---
mission_id: TR2-20261015-R3B-LIVE-METROLOGY-B3
status: DONE
base_ref: main
base_sha: fa2af5606ff7ae939a14a77c8d593da5f38d7e14
initial_head: 5a50214d2382b53bc8f1798028154674c39f9637
result_sha: 1c5368fd56bc52195d5f44dc37f38f7edf848900
created_at_utc: 2026-10-15T22:04:32Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-B — fenêtres, métrologie B3 et coût de coexistence

**Livraison logicielle R3-B publiée et relue sur main ; limites explicites ci-dessous.** Calcul B3
entier, qualité/âge, alarmes/latch et raccordement causal au propriétaire FIFO
R3-A sont testés. **Production V1 conservée ; aucun profil2, B3 physique, accès
FRAM LIVE ou qualification temporelle n'est annoncé.** Contrôle indépendant
ChatGPT à effectuer ; R4 est nécessaire avant composition opérationnelle.

## Références et Git

Mission READY récupérée sur main5a50214, basefa2af56 contrôlée, AGENTS/protocole,
gouvernance §10 T1000, journal intégral, roadmap, norme/mapping/tests R0,
LIVE_R1/R2/R3A et mission/rapport R3-A consultés. Mission/rapport R3-A archivés
intégralement depuisfa2af56, sans remplacer une archive. Aucune modification du
contrat ChatGPT ni des gels/dettes/archives préexistantes :2334 empreintes
protégées vérifiées. Le contrat omet created_at_utc ; aucune date contractuelle
n'est inventée, la date ci-dessus est celle de rédaction du rapport.

Dépôt historique main82169af propre mais divergent conservé, aucun pull dangereux,
merge/rebase/reset/clean. Worktree isolé
`/home/lolo/dev/msm/projects/MSM_vibe-R3B-live-metrology`, branche locale
`codex/r3b-live-metrology`, destination main. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2
etf3b829e6188f491920edf74a5f9ed0f559d08b87 inchangés, autres worktrees préservés.

- `e80e252` — R3-B: add causal LIVE windows, integer B3 metrology and strict quality alarms.
- `1c5368fd56bc52195d5f44dc37f38f7edf848900` — R3-B: normalize extracted statistics file ending.

Tests/builds dynamiques sur le contenu du premier ; le second retire seulement
une ligne blanche finale (signalée par diff --check), aucun token exécutable ni
corps de fonction ne change. Contrôle statique de ce dernier, sans validation
physique ni répétition de tests pour une fin de fichier. Les commits suivants
concernent uniquement preuves/rapport/état/roadmap.

## Résultats et portée

`LIVE_R3B.md` contient la correspondance R0 détaillée, politique de frontières,
matrice conservé/mutualisé/remplacé/différé et dépendances RTU2.

- Service `LiveMetrology` constant en mémoire, run/config_id/génération copiés,
  indices XYZ exacts, quatre N et P normatifs. Première fenêtre immédiate, suivantes
  au premier XYZ dont temps observé atteint début précédent+P, sans chevauchement
  ni rattrapage inventé. XYZ entre fenêtres toujours drainés et contrôlés.
- Accumulateur vérifié et conversion brute V1 extraits sans changer leurs corps ;
  calculateur isqrt et encodage B3 existants réutilisés, parser/driverST inchangés.
  mg arrondi hérité le plus proche, demi-mg éloigné de zéro ; RMS/crêtes R0 entiers,
  axes désactivés0 avant sommes/global/alarmes ; saturation même non sélectionnée
  rejette conformément au garde-fou physique R3-A/V1.
- B3 mémoire48registres complet : statut/validité/fraîcheur/âge à2P inclus,
  métadonnées non redatées, UTC indisponible/overflow sans arrêter numérique,
  séquence/compteurs saturés, sévérité/dominance et réservés0. Aucun CRC B3 inventé.
- Alarmes strictement>seuil, hystérésis à plancher0, hold causal annulé si fenêtre
  valide manquante/invalidité/STOP/péremption ; latch et compteurs par montée de
  l'union. ACK seulement après disparition prouvée par fenêtre valide fraîche.
  STOP retient la dernière valeur qualifiée, F0/HELD/STOPPED ; erreur retire V/F,
  perte d'autorité retire aussi bit7, sans écraser nombres et métadonnées.
- Binding réel R3-A→service, START après readback, STOP et fautes propagés.
  Notification de perte d'autorité R2 ajoutée sans changer journal/format/codes ;
  panne FRAM invalide B3 et arrête indépendamment de l'écriture du succès.

Le provider **sample_time** observé/calibré est obligatoire et sans défaut permissif.
La grille rationnelle des tests est un modèle, pas un temps capteur réel. Pas de
provider STM32 inventé depuis le temps de défilement SPI ni d'UTC qualifiée par
supposition. Ce raccordement et les budgets R4 restent à prouver.

## Validations réellement exécutées

[Preuves et manifeste](preuves/TR2-20261015-R3B/README.md).

- **133/133 tests hôte PASS**,33,00s au dernier cycle. Validation complète requise
  car accumulateur/conversion partagés et notification moteur touchés. Full
  `STM32CUBE_U5_ROOT=… bash ./tr2_validate.sh`, puis rebuild/ctest finaux et ARM
  `--cross-build-only` justifiés par correction de l'annulation hold à fenêtre
  manquante ; aucune validation physique historique rejouée.
- **448 oracles Python indépendants** :4FS×7masques×4N×4formes, constants/AC/zéro/
  proche saturation ; math.isqrt et rationnels indépendants du C/ST. Aucun attendu
  ajusté à partir du résultat C. N-1 sans résultat, N exact, frontières périodiques,
  invariance aux découpages1..128, arrêt/reprise/erreurs, temps32→64 et >7jours
  simulés, âge2P/2P+1, UTC invalide/overflow/epoch0, compteurs saturés.
- **>4M XYZ supplémentaires** : vrai owner R3-A et vraie métrologie sur modèle SPI,
  timestamps capteur modulo32, fenêtres périodiques puis faute FIFO et invalidation.
  Vraie intégration moteur/store R2→owner→métrologie, APPLY refusé sans I/O en
  acquisition et panne FRAM→arrêt/autorité retirée. Tests R3-A >4M/FS conservés.
- **ASan/UBSan PASS**, trois suites ciblées,4,08s. Pas d'exécution HAL physique.
- **Cross-build cumulatif ARM PASS**,GCC14.2.1/SDK1.9.0 ; lien forcé séparé de
  métrologie/binding STM32 réussi. Ni cette image ni le canonique flashés.

Erreurs conservées : première instrumentation inclut un main de fixture imbriqué
(redefinition) ; wrapper de suite extrait sans changer les assertions. Après
séparation des temps d'événement et de lecture, un test synthétique revenait à0
après un run : corrigé pour conserver la monotonie, pas affaiblir le contrôle.
Les séquences de clock restent indépendantes : une lecture actuelle ne doit pas
faire rejeter artificiellement un échantillon antérieur en queue.

**Écart de méthode à transmettre :** règles/formules R0 lues avant développement,
mais générateur d'oracles matérialisé après première rédaction du service, avant
sa compilation, contrairement à l'ordre demandé « avant développement ». Cela ne
change pas l'indépendance des attendus ; la revue ChatGPT doit connaître cet écart.

## FRAM/FIFO : mesures, déduction et recommandation R4

Instrumentation des vraies phases R2 sur transport mémoire :**313916octets lus /
52426écrits par phase** ; START/STOP/RENEW1255664lus/209704écrits ; APPLY ajoute
une transaction,1569580lus/262130écrits. CPU hôte nominal≈6–14ms selon phase dans
le relevé initial ; sorties finales également conservées, pas une borne de WCET
STM32 ni une latence électrique. START phase2 arme avant persistance, phase3
encore active ; STOP phases0/1 et tout RENEW LIVE exposent aussi la coexistence.

Lecture main.c :FRAM SPI1 SYSCLK160MHz/prescaler64 ⇒2,5MHz calculés. Les seuls
366342octets utiles d'une phase nécessiteraient **>=1,1722944s** de bus sérialisé,
hors entêtes/CPU. FIFO512 à26667XYZ/s +1TS/8XYZ ⇒30000,375words/s :**17,06645ms**
nominal depuis vide,7,06658ms au niveau300. Déduction conditionnelle au clock réel,
pas mesure sur carte : phase monolithique incompatible, budget absent reste refusé.

Stratégie R4 concrète documentée : conserver formats atomiques, un seul owner ;
quanta transport et CRC/hash avec service FIFO entre quanta, aucune réentrance
FRAM/dispatchB5 dans callback, réception STOP prioritaire et arrêt de faute
indépendant. Point de départ32octets=102,4µs utiles à2,5MHz, **à mesurer avec tous
les surcoûts**, admission selon niveau/gap/coût pire ; pas un budget autorisé.
Alternative journal delta serait structurante si cette voie échoue ; aucune
migration/format ni architecture imposés silencieusement ici.

## Architecture, métriques et ressources

[Inventaire414unités/interfaces/tests](preuves/TR2-20261015-R3B/module-inventory.json)
actualisé depuis R3-A avec CMake/glob/symboles :89chemins source ont des symboles
dans le canonique (incluant vendor/générés identifiés). Non lié n'est pas mort.
[Préparation187champs de projection](preuves/TR2-20261015-R3B/projection-readiness.json).

|Référence code Git|C/C++ propriétaire|Tests source|Documentation|
|---|---:|---:|---:|
|Avant R1 b6bae880|30415|45422|140951|
|R2 3f3ca1c|31768|46516|141714|
|R3-A 0c6f002|31999|46620|142059|
|R3-B 1c5368f|32341|47329|142428|

Méthode script R3-A inchangé :blobs Git, lignes physiques avec blancs/commentaires,
hors vendor/générés/build ; code inclut candidats historiques/expériences, tests
séparés, JSON/CSV/logs données hors LOC. **+342code/+709tests/+369docs** versus
R3-A code ; docs incluent entrées mission/archives/rapports intermédiaires. Diff
propre depuis5a50214 :1939ajouts/89retraits Git,23fichiers incluant JSON450lignes,
archives/outils/tests ; pas1939lignes de production. Les rapports ultérieurs ne
sont pas inclus dans le commit code mesuré. Aucun retrait spéculatif.

|ARM-O0|Octets/sections réelles|
|---|---|
|LiveMetrology|720|
|LiveFifoAcquisition|1096 (inclut owner, +8 sur R3-A)|
|LiveCommandEngine|488 (+8 pour notification)|
|AcquisitionWindow|224, type existant|
|Canonique V1 text/data/bss|170916/480/602232|
|ELF de lien forcé distinct text/data/bss|179288/480/602232|

Les nouveaux objets ne sont pas instanciés dans le canonique. `.su` :sample120,
complete312, projection288, consume152octets de cadres statiques, **pas** une
borne complète de pile/IRQ ni de charge. Pas de taille FLASH avantR1 remesurée.
BIN canonique SHA256
`990b2f27fe5c8c27c9c57d596e78fe9d5ce223ef2d9a5c1fe38c3643f8ffba96` ;
ELF `bfd2006d46713505847ba8e101872c1200bd7d4dcab55fe02c2fd82d2df8afc7`.
Lien forcé **NON DESTINÉ AU FLASH** SHA256
`1ab6f8e4560da29ed68249d21f394c1341712fd9f9c89380a83c431ca60a8bb0`.

## Limites et état laissé

Projections B1/B6/B7/B0/B4/B5 préparées champ/par champ et dépendances explicitées,
pas présentées comme raccordées. Agrégateur autorités/READY/fautes R0, inventaire
unknown4/schema2, événements B7 durables, effets hérités2..11, routagePDU/RTU29,
capacité LIVE et provider temps capteur manquent à la composition complète. B0
V1 masque bit4 et B6 V1 refuse santé4 : élargir seuls ces points serait mensonger.
ACK local B3 testé, commande6 successeur non dispatchée ; SELFTEST5 sansSD à
raccorder, REFRESH7 ne devra jamais rajeunir le snapshot ; commande11 garde les
compteurs R0. R4/R5/R6 restent nécessaires ; pas de validation bout-en-bout ni
métrologie/calibration/endurance physique extrapolée des simulations.

**Zéro opération matérielle dans R3-B** : ni inventaire live du banc, ni connexion
cible/debug/reset/flash, ni FRAM/SD/RS485/OptionBytes. L'état hérité est conservé
sans le prétendre relu/restauré. T1000 seul banc futur, HP retiré. Autorisations
commit/push exercées ; flash/debug non exercées. Temps capteur et coexistence
non qualifiés ne justifient pas de flasher une composition incomplète.

Journal/roadmap actualisés ; gels/dettes et mission ChatGPT intacts. Push normal
réussi sur main au commit **`faf0f9456e25cd298ed5a1a147655bef861a59b5`** —
`R3-B: publish metrology evidence, FIFO FRAM analysis and provisional report`.
Après récupération depuis GitHub, tous les livrables modifiés ont été relus
intégralement et comparés octet à octet ;44fichiers du manifeste,2334fichiers
protégés, sources évaluées et stashes vérifiés. Reçu détaillé dans
[remote-readback.json](preuves/TR2-20261015-R3B/remote-readback.json).
Worktree propre à ce point ; dépôt principal et autres worktrees préservés.
Cette mise à jour DONE clôt la livraison logicielle, avec l'écart méthodologique
signalé pour revue ; elle ne clôt ni R4 ni une qualification physique ou un
contrôle indépendant ChatGPT. Le SHA de cette dernière publication et son
contrôle distant sont fournis dans la réponse finale, sans SHA autoréférent.
