from pathlib import Path
import json,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');p=Path('/home/lolo/dev/msm/reports/TR2-r4t');g=r/'Modbus RTU/00_gouvernance';e=g/'Echanges_Codex';v=json.loads((p/'physical-summary.json').read_text());rest=json.loads((p/'hardware/restoration-summary.json').read_text());assert v['result']=='PASS' and rest['result']=='PASS'
now=datetime.datetime.now(datetime.timezone.utc).isoformat()
report=f'''---
mission_id: TR2-20261016-R4T-MONOTONIC-JOURNAL-RECOVERY-B3
status: IN_PROGRESS
base_ref: main
base_sha: 1799b97db7d1c5dc0bc3892501569fdcd7a4e0b9
initial_head: d17c5457e35fd99269bdc0312fb5c48770ff0572
result_sha: 79ba06a156d43ca337da405b750b03134de5e72b
created_at_utc: {now}
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-T — retraite monotone et B3 LIVE physique réussies dans leur portée bornée

**Critères techniques applicables satisfaits ; publication finale en cours de
relecture distante.** Le vrai RENEW4→5 conserve la monotonie du journal, puis
le vrai START(run6) et STOP produisent leurs reçus durables4/0. Acquisition réelle,
provider observé, métrologie N4096 et B3 avec V/F actifs sous charge FRAM/RTU RAM.
**M016 restauré et relu intégralement ; aucune identité FRAM rembobinée.**
Ce bilan ne qualifie ni un firmware produit LIVE complet, ni R4global/R6.

## Référence, inspection et préflight

Départ distant `d17c5457e35fd99269bdc0312fb5c48770ff0572`, mission R4-T READY et
base contractuelle `1799b97db7d1c5dc0bc3892501569fdcd7a4e0b9` vérifiées dans
l'historique. AGENTS/gouvernance/V2, journal intégral, roadmap, R0/B5 V1.1,
R1/R2 et R4-M/D/P/R/S consultés. Archives R4-S déjà présentes préservées, sans
doublon. Étude locale86d0f4b inspectée et conservée isolée ; sa maintenance avec
pont vide n'a pas été intégrée. Seuls helpers/capture sémantique relus ont servi
de point de départ, avec la nouvelle retraite atomique décrite ci-dessous.

TR2-001 NUCLEO-U575ZI-Q, ST-LINK `003500463235510B37333439`/V3J15M7,
UID `004A003D/38425018/20333157`, T1000/WSL2 et SDK Linux STM32CubeU5v1.9.0.
Préflight22:00:20UTC : capteur arrêté/WHO7B ; Flash entière2Mio c5bc…f5b26 et
FRAM26214414fb…2aa9 exactes aux backups authentiques R4-S réutilisés. Lecture
GDB qualifiée, aucun HOTPLUG. Aucun diagnostic R4-S dix secondes répété.

## Solution transactionnelle et preuves sur copies

Le journal `LiveCommandStore.generation` est le maximum des générations des
records sélectionnés ; le root compte les publications de l'enveloppe, tandis
que configuration, epoch et effet ont leurs propres générations. Ces domaines
ne sont pas interchangeables. Le vieux mécanisme de pont vide ferait26→0→4.
La nouvelle procédure ne publie **jamais** ce pont :

1. SHA complet14fb, root39/payload exact, cinq slots/records, STARTED26/4-5/run5,
   INTENT15/config3/gen3, epoch4/previous3/gen7/flags0 contrôlés avant staging.
2. Retirer uniquement les224octets du slot3 dans la candidate RAM, sans commit
   séparé. Conserver les autres reçus et INTENT. Le frontend de maintenance fermé
   retire l'actif de son état volatile, conserve la génération26 réelle, puis
   appelle le **vrai submit R2** RENEW4/65535 : RESERVED27 est publié dans la même
   enveloppe atomique que la retraite. Aucun record de compteur artificiel.
3. Vrais steps R2 : STARTED28, transition epoch5 liée à29, COMPLETED30/4/0.
   Root39→43 ; runfloor5 et INTENT128 exacts. STARTED5 reste historiquement
   indécidable/archivé, aucun COMPLETED/28/29 fabriqué pour lui.
4. Ouverture de la qualification seulement après RENEW réel terminé et flags0 ;
   les cinq anciennes clés refusées24 sans write/effet dans le modèle. Sur cible,
   contrôle négatif explicite de l'ancienne clé4/5 :24/0write.

Cœur R2, formats, norme R0/V1 et anciennes gardes R4-M/P **inchangés**. Les seuls
privilèges de retraite sont dans le probe de maintenance de ce contrat, sans
entrée produit/UART, chaque boot CLOSED/WAIT ; token consommé avant toute tentative.
Un échec invalide store/frontend et arrête le capteur, sans réisolation implicite.
Un pont RESERVED27 n'est jamais exposé à un firmware LIVE générique.

**Nouveau test ciblé PASS22,86s** :568budgets de coupure sur quatre barrières
(admission/STARTED/activation/terminalisation), payload par strides et chaque octet
header/root ; reboot, monotonie, INTENT/floors/protégés, SHA altérés, anti-rejeu,
prochainrun6/STOP sur vrais stores. **O2/ASan/UBSan ciblés PASS13,41s.**
Les états interrompus restent fermés : RESERVED RENEW peut être terminalisé28
par le vrai moteur, STARTED RENEW sans transition reste9 ; ils ne garantissent
pas une poursuite automatique et nécessitent inspection/plan explicite. Ces
résultats de RENEW ne résolvent jamais rétroactivement STARTED5.

## Essais physiques distincts et correction du harness

**Image01/code65afffc** : refus réel R2 5/7 avant le premier commit. Prédicat
I/O du port non raccordé au harness ;0octet/0commande physique écrite, FRAM
entière14fb inchangée. Le contrôle a correctement refusé, sans nouvelle identité.
La comparaison au modèle de succès échoue dans le journal original, conservé.

Correction minimale : `owner.acquisition.budget=admission` avant la retraite,
réemploi du prédicat existant ownership/work/réserveFIFO106, jamais true forcé.
Contre-test supplémentaire **PASS0,05s** : refus⇒0write/start, token consommé,
frontend indisponible et reboot STARTED5/journal26 exact. Helper transactionnel
inchangé : ses568coupures et sanitizers acquis n'ont pas été rejoués.

**Image02/code79ba06a**, après vérification de la FRAM inchangée : maintenance
physique PASS22:06:09UTC,4commits/209704octets uniquement LIVE. Snapshot entier
byte-identique au modèle RENEW, SHA
`f035749d0f0881277971824df5a338500c5636063c131f056648a4a682f1e312`.
Journal30/root43/epoch5/runfloor5, ancienneclé24 ; checkpoint publié avant la suite.

## Composition physique FIFO / R2 / calcul / B3 / RTU RAM

Après contrôle du snapshot entier, second token/trigger distinct ; aucune nouvelle
isolation, aucun reset/reflash entre maintenance et acquisition. Calibration du
nouveau run sur brackets réelles, puis START5/1/run6 et STOP5/2 terminés4/0.
Configuration3/génération3, axes7, pleineéchelle0, N4096 ; autorité vraie issue du
propriétaire R2/FIFO, pas l'identité volatile diagnostique de R4-S.

| Observation | Résultat mesuré |
|---|---|
| FIFO réelle | **3918509 XYZ / 489814 TS**, niveau max66, erreurs FIFO/STOP0 |
| Datation émise |3918504XYZ,5 en attente d'ancre ; pas de remplissage fictif |
| Temps événement observé |147,320665s entre premier et dernier XYZ datés |
| Boucle explicite après START terminé |10,000766s, distincte des longs commits R2 sous FIFO |
| Fenêtres calculées |**74 N4096** ; première rapprochée des4096bruts indépendants |
| RTU loopback RAM |**13548 ADU cohérents/CRC**,11989 V/F actifs,809 HELD observés |
| Work / service / gap FIFO max |**915 / 1964 / 2203µs**, bornes1000/2500/5000 conservées |
| Marges nominales observées |85 / 536 / 2797µs ; pas de WCET garanti |
| Suivi compteur/DWT |1465contrôles, résidu max13µs, seuil100µs conservé |
| Écritures physiques mission |629112octets/19668appels,36plages,12commits, tous LIVE |
| Arrêt sûr final |234µs observés ; STOP métier prouvé par effet/reçu/COMPLETED et HELD |

Paire de service maximale1961µs : poll1396 incluant consommateur394, dont
datation77/métrologie239 ; RTU562, horloge/publication0 dans cet appel. Coûts
imbriqués et maxima indépendants ne doivent pas être additionnés.
Lecture FRAM max130µs/write163µs. Les timings séparés du wrapper STOP nominal,
bien que stockés par le firmware, n'ont pas été exportés ; pas de valeur inventée.
Les quanta de toute la campagne, y compris STOP, sont couverts par le maximum
work915µs et l'absence de dépassement, sans qualification de panne électrique.

Oracle indépendant sur les **premiers4096 XYZ bruts physiques** : RMS
global/X/Y/Z **959/952/102/38mg**, crêtes **993/985/147/79mg**, durée154ms,
concordance exacte avec la première fenêtre embarquée. Aucune amplitude étalon
externe ni vérification intégrale des74fenêtres par leurs bruts annoncée.

ADU conservés et CRC revérifié hors cible : dernier frais statut4/flags1215
(`V=1,F=1`, cohérence configuration, fenêtre complète, UTCabsente), âge1195ms,
séquence74. Après STOP : statut1/flags5819, HELD1/F0, âge10123ms, séquence74
retenue. UTCflag0 et timestamp0, aucun temps civil fabriqué. Les134? valeurs ne
sont pas nécessaires : les13548cohérences sont vérifiées sur captures RAM par
le harness ; les deux ADU exacts sont publiés pour contrôle indépendant.
Le contexte run/config est également observé dans l'owner/métrologie et les
records R2 ; aucun registre B3 réservé n'est réaffecté pour le transporter.

## Validation, artefacts et limites

Nouveau test maintenance et contre-test admission, ARM dédié/canonique exécutés.
Les141tests R4-S et leurs oracles sur cœur inchangé sont **réutilisés**, sans
suite143complète annoncée (143tests enregistrés après les deux ajouts).
Commande canonique : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0
bash ./tr2_validate.sh --cross-build-only` PASS. Le premier lancement direct
avait échoué sur le bit exécutable absent ; aucun chmod/config persistante.
Validation ciblée justifiée : changements dans harness/maintenance opt-in,
aucune modification des sources du cœur. BIN V1 canonique byte-identique
`85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5`.

Code réellement exécuté `79ba06a156d43ca337da405b750b03134de5e72b`, GCC ARM
14.2.1, SDK1.9.0, profil CUMULATIVE+R4/R4M/R4P/R4R/R4S/R4T explicite
**NON-PRODUCTION**, FIFO SPI3 4MHz, FRAM SPI1 2,5MHz, CPU160MHz.
Image02 BIN140624octets SHA
`8c743c35a0d313185535c80d0437a7c7f02309539fcf8025a5ba1e5f610445be`, ELF SHA
`7ec3e16d44825815fdcbcb6eb3e10d55498092e0776f86ae4d627520528ca1e6`.
BIN/ELF exacts des deux images, profils, logs et source SHA dans les preuves.
La compilation sans travail après commit confirme les mêmes sources que l'image.

N8192/16384/32768 **non exécutés** : le harness/contrôle RTU et le tampon oracle
sont bornés à4096, BSS observée660568octets ; étendre les bruts à32768 ajouterait
172032octets, au-delà de la SRAM786432 avant pile/heap. La couverture et la marge
mémoire requièrent un autre plan de collecte ; ne pas élargir artificiellement
les critères ou répéter une campagne équivalente. Aucun R4global/R6, UART/RS485
électrique/IRQ/TX, PC/SQLite/Web/R5, dérive SI/thermique, rollover/endurance ni
WCET en faute HAL/IRQ/CPU/pile qualifiés. Pas de perte globale zéro affirmée.

## État final et conservation

FRAM finale262144octets **identique au modèle des métadonnées**, jamais écrit
sur cible : SHA `371a9d16c6285d0150608751ccfdd5217ff303d42d8e5446d66a8130342e6634`.
Journal38/root51, epoch5/previous4/gen9/flags0, START5/1/run6 gen34 et STOP5/2
run6 gen38 terminal4/0, TR2Egen21/phase3, configuration3/gen3. Toutes CRC/racines
contrôlées hors ligne ; historique[0,122880) et tail[245760,262144) byte-identiques.
L'ancien run5 et sa clé4/5 ne sont jamais réutilisés.

**Restauration vérifiée22:10:24UTC le16/10/2026** :18pages normales147456octets,
relecture complète **Flash2097152octets exacte** SHA
`c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.
FRAM complète relue après reset/restauration, identique au snapshot final371a…6634,
aucun rollback. M016 CommissioningReadOnly WAIT, capteur arrêté, GDB détaché.
Ce reset vers M016 n'est pas un essai de reboot du produit LIVE ni un power-cycle.
Aucun SD/HP/câblage/COM/RS485, mass erase, OptionBytes/RDP ou boot persistant.

Journal actualisé (état courant remplacé par R4-T, acquis et réserves), roadmap
factuelle actualisée ; normes, gouvernance, dettes A-04 et archives intactes.
6497fichiers protégés,60autres worktrees, ancienmain82169af propre, deux stashes
6251b988…/f3b829e6… et étude locale86d0f4b préservés (`preservation-final.json`).
Aucun reset Git/clean/rebase/merge/force push ni intégration aveugle de l'étude.

## Publication et prochaine étape

Commits logiciels65afffc puis79ba06a, checkpoint physique657cd94, push normaux
sur `main`. Dernier checkpoint relu depuis GitHub :
`657cd94e36caf4c9b454def91a46ef1ca1749f61`,101fichiers/88preuves comparés.
Ce bilan final est IN_PROGRESS uniquement jusqu'à sa relecture distante ; les
critères techniques bornés ci-dessus sont PASS, aucun blocage technique restant
pour cette portée. Après relecture, publier DONE et sa preuve, puis communiquer
le SHA distant final. Contrôle indépendant et éventuelle qualification par
ChatGPT distincts ; aucun tag créé par Codex.

[Plan R4-T](../../05_Firmware/LIVE_R4T.md), [preuves et manifeste](preuves/TR2-20261016-R4T/README.md),
[mesures/oracle/ADU](preuves/TR2-20261016-R4T/physical-summary.json),
[restauration](preuves/TR2-20261016-R4T/hardware/restoration-summary.json).
Backups complets conservés hors Git : `/home/lolo/dev/msm/reports/TR2-r4t/hardware/`.
Suite recommandée : revue indépendante R4-T, puis qualification distincte des
fenêtres supérieures avec collecte adaptée, intégration produit/IRQ/RS485 et
chaîne R5/R6, sans répéter les acquis inchangés et sans réappliquer cette retraite.
'''
report=report.replace('Les134? valeurs ne\nsont pas nécessaires : les13548cohérences','Les13548cohérences')
(e/'DERNIER_RAPPORT.md').write_text(report)
f=r/'Modbus RTU/05_Firmware/LIVE_R4T.md';f.write_text(f.read_text()+'''

## Résultat physique final — 16/10/2026 UTC (17/10 local)

- Image02/code79ba06a : RENEW4→5 réel, journal26→30/root39→43, snapshot f035…e312 exact au modèle ; ancienneclé4/5 refusée24/0write. Puis START5/1/run6 et STOP5/2 COMPLETED4/0 réels.
- Composition réelle :3918509XYZ/489814TS,74fenêtres N4096,13548RTU RAM cohérents dont11989V/F ; oracle indépendant première4096exact (RMS959/952/102/38mg, crêtes993/985/147/79mg). Temps entre événements datés147,320665s ; boucle explicite après START terminé10,000766s, commits R2 sousFIFO distincts.
- Maxwork915/service1964/gap2203µs, bornes1000/2500/5000 inchangées ; FIFOmax66, erreurs0.1465contrôles horloge/résidumax13µs/seuil100. Arrêt sûr234µs ; timer STOP nominal distinct non exporté, aucune valeur ajoutée a posteriori.
-12commits/629112octets/19668appels physiques uniquement LIVE,36plages auditées. FRAM finale entière371a9d16c6285d0150608751ccfdd5217ff303d42d8e5446d66a8130342e6634 exacte au modèle de métadonnées ; journal38/root51/epoch5/run6/config3, historique/tail intacts.
- M016 restauré22:10:24UTC :18pages normales, Flash2Mio exacte c5bc…f5b26 ; FRAM finale relue après restauration et inchangée, aucune identité rembobinée. Capteur arrêté/GDBdétaché. Pas de reboot LIVE produit ni power-cycle, RS485 électrique, R4global/R6 ou endurance qualifiés.
- N supérieurs non couverts par ce harness/oracle4096 ; BSS660568 et collecte32768naïve+172032 dépasseraient786432SRAM avant pile. Prévoir collecte adaptée dans une tranche distincte. Rapport final et preuves pour contrôle indépendant ChatGPT.
''')
f=g/'ETAT_COURANT_TR2.md';t=f.read_text();a=t.index('**Dernière actualisation :**');b=t.index('**État du second banc',a);t=t[:a]+'''**Dernière actualisation :** 16 octobre2026UTC /17octobre local, R4-T : retraite monotone physique PASS et composition R2/FIFO/B3/RTU RAM PASS borné N4096.3918509XYZ/74fenêtres/13548RTU ; work915/service1964/gap2203µs. M016 restauré à22:10:24UTC, Flash2Mio exacte c5bc…f5b26 ; FRAM finale371a9d16…6634/journal38/root51/epoch5/run6/config3 conservée sans rollback. Contrôle indépendant ChatGPT à réaliser ; R4global/R6 ouverts. '''+t[b:];a=t.index('- **Cible principale vérifiée');b=t.index('\n- **Code A04',a);t=t[:a]+'''- **Cible principale vérifiée le16/10/2026UTC (R4-T) :** TR2-001 / NUCLEO-U575ZI-Q, SN003500463235510B37333439, UID004A003D/38425018/20333157. À22:10:24UTC, M016 CommissioningReadOnly WAIT restauré, Flash2097152octets exacte SHA c5bc344d…f5b26 ; capteur arrêté/GDBdétaché. FRAM262144 finale371a9d16…6634 relue après restauration, historique/tail exacts ; epoch5/run6/config3, journal38/root51, dernier STOP5/2 COMPLETED4/0. Ancien STARTED4/5/run5 archivé indécidable, pas de reçu inventé. Aucun firmware produit LIVE complet laissé sur carte ; les états précédents ci-dessous sont historiques.'''+t[b:];t+='''

## R4-T — résultat final du16/10/2026UTC (17/10 local)

- **OBSERVÉ PHYSIQUE PASS BORNÉ** : code79ba06a, retraite slot3+vrai RENEW atomique sans pont vide, journal26→30/epoch4→5/runfloor5 ; suite START6/STOP réels,3918509XYZ/489814TS/74fenêtres4096/13548RTU RAM cohérents dont11989V/F. Oracle première4096exact ; UTCabsente, STOP/HELD observés. Work915/service1964/gap2203µs, FIFOmax66/erreurs0,1465contrôles horloge/résidumax13µs.
- **VALIDÉ LOGICIEL / LIMITES** :568coupures/contre-tests/sanitizers nouveaux PASS ;141tests R4-S sur cœur inchangé réutilisés, pas de suite143complète annoncée. Premier refus5/7 sans write préservé, raccordement budget I/O corrigé sans affaiblissement. N supérieurs non couverts par collecte4096/SRAM ; pas de qualification R4global/R6, IRQ/RS485, WCET faute, dérive SI/thermique ou endurance.
- **RESTAURÉ / PRÉSERVÉ** : FlashM0162Mio exacte c5bc…f5b26 ; FRAM371a9d16…6634 entière exacte au modèle, journal38/root51/epoch5/run6/config3,12commits/629112octets uniquement LIVE. Historique/tail,6497fichiers protégés,60autres worktrees/stashes intacts. Restauration22:10:24UTC, capteur arrêté/GDBdétaché, aucun rollback/SD/HP. Prochaine étape : contrôle indépendant ChatGPT puis couverture produit/collecte N supérieurs/RS485/R5/R6 distincte ; ne pas réappliquer la retraite.
''';f.write_text(t)
f=g/'ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';lines=f.read_text().splitlines()
for i,l in enumerate(lines):
 if l.startswith('| **R2 —'):lines[i]='| **R2 — Moteur live** | **LOGICIEL VALIDÉ ; R4-T COMPOSITION PHYSIQUE BORNÉE PASS** | Retraite exacte monotone, vrai RENEW4→5, START/STOP run6 sous FIFO/B3/RTU RAM. | Journal26→30→38/root39→51, INTENT/floors/protégés conservés ; pas de politique recovery produit automatique ni IRQ/RS485 électrique qualifiés. [R4-T](../05_Firmware/LIVE_R4T.md). |'
 if l.startswith('| **R3 —'):lines[i]='| **R3 — FIFO/calcul/B3** | **R4-T B3 PHYSIQUE N4096 PASS BORNÉ** |3918509XYZ/74fenêtres,13548RTU RAM cohérents dont11989V/F, oracle première4096exact, STOP/HELD/UTCabsente. | Contexte R2 run6/config3/gen3 observé ; N supérieurs/collecte adaptée et produit complet à qualifier. Contrôle indépendant ChatGPT en attente. [R4-T](../05_Firmware/LIVE_R4T.md). |'
 if l.startswith('| **R4 —'):lines[i]='| **R4 — Temps réel** | **R4-T COMPOSITION BORNÉE PASS ; R4GLOBAL OUVERT** | Maxwork915/service1964/gap2203µs sous1000/2500/5000 ; écritures réelles FRAM/FIFO/calcul/RTU RAM. | FIFOmax66/erreurs0,629112octets LIVE/12commits ; M016 restauré exact, FRAM371a…6634/epoch5/run6 préservée. CPU/pile/IRQ/RS485/fautes/endurance et R6 non qualifiés. [R4-T](../05_Firmware/LIVE_R4T.md). |'
f.write_text('\n'.join(lines)+'\n');print('FINAL DOCS WRITTEN',now)
