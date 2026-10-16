from pathlib import Path
import json
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4P-physical-coexistence');local=Path('/home/lolo/dev/msm/reports/TR2-r4p');gov=root/'Modbus RTU/00_gouvernance';ex=gov/'Echanges_Codex'
rest=json.loads((local/'hardware/restoration-summary.json').read_text());assert rest['result']=='PASS'
report='''---
mission_id: TR2-20261016-R4P-PHYSICAL-COEXISTENCE
status: IN_PROGRESS
base_ref: main
base_sha: f7366e535b90a4ef79642a3debfa601f35c40b9c
initial_head: 03309c076da7322fb774d41c032112cd663390fd
result_sha: 9f429e513faba26ce89b760361c9cba08057bc24
created_at_utc: 2026-10-16T12:55:58Z
updated_at_utc: 2026-10-16T13:20:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-P — coexistence physique FRAM/FIFO observée et restauration terminée

**Résultat technique : critères de cette mission satisfaits dans le périmètre borné.**
La correction causale R4-D est exercée dans le vrai moteur R2, avec FRAM physique
et FIFO IIS3DWB simultanés : deux START durablement COMPLETED, renouvellement sous
acquisition, RUN60s, STOP/APPLY/START, faute de service post-COMPLETED et recovery
lecture seule réussis. Le premier échec du harness et sa correction sont conservés.
Aucun seuil relevé, aucune seconde isolation, aucun nouveau STARTED indéterminé.
Le statut IN_PROGRESS porte seulement sur la publication/relecture finale à obtenir.
La qualification indépendante ChatGPT et R4/R6 complets restent distincts.

## Référence, permissions et préservation

Départ distant main03309c076da7322fb774d41c032112cd663390fd, contrat READY R4-P,
basef7366e535b90a4ef79642a3debfa601f35c40b9c. AGENTS/protocole/journal/roadmap,
R0/R1/R2/R3/R4/R4M/R4D et preuves relus ; références normatives inchangées.
Travail isolé dans `/home/lolo/dev/msm/projects/MSM_vibe-R4P-physical-coexistence`,
branche `codex/r4p-physical-coexistence`, push normal explicite vers main.
Le main local historique82169af7e76797731358852bec83bfb0ceb54ccf est propre et conservé,
55 anciens worktrees, deux stashes et2830fichiers protégés byte-identiques ; ni reset,
merge/rebase ni écrasement. Contrat/rapport R4-D archivés byte-identiques depuisf736.
Autorisation logicielle/commit/push/flash/debug exercée uniquement dans ce périmètre.
Aucune intervention physique demandée ou simulée ; aucun SD/HP/RS485/RTUélectrique.

ST-LINK003500463235510B37333439, UID004A003D/38425018/20333157 vérifiés dans les logs.
Préflight12:51:28UTC : doubles lectures Flash2097152 et FRAM262144 identiques,
FlashSHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAMSHA f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1.
WHO7B, ODRoff/FIFObypass/TSoff lus avant écriture. Sauvegardes fraîches récupérables
sous `/home/lolo/dev/msm/reports/TR2-r4p/hardware/` avant tout flash/NVM.
Programmation qualifiée SWD8000/SN/adresse0x08000000/write-verify-reset, puis reset
logiciel GDB qualifié ; aucun HOTPLUG, mass erase, OptionBytes/RDP ou boot persistant.

## Code, images et validations logicielles

- Code initial369887b9583ca05d0208c3d18bdd485311bcb151 : capture stricte de l'état
  authentique STARTED2/run2, profil opt-in R4P, test139 et plan de maintenance.
- Code corrigé réellement flashé9f429e513faba26ce89b760361c9cba08057bc24 : saut O(1)
  du scan protégé et continuation explicitement liée au pont exact, précédée d'un
  contrôle readonly physique. Cœur `src/` R4-D inchangé : checkpoints causaux7a6a8b9
  conservés, barrières et admission non affaiblies. Aucun refactoring hors mission.
- Profil `tr2-r4p-coexistence-NON-PRODUCTION`, R4_BENCH/R4M_MAINTENANCE/R4P ON,
  R4D OFF, Debug, SDK Linux STM32CubeU5v1.9.0, GCC ARM14.2.1. HCLK160MHz,
  SPI1/FRAM2,5MHz et SPI3/capteur4MHz vérifiés. Aucun UART/SD/ingress produit actif.
  Les noms de variables r4m_* sont réutilisés ; SHA et scripts identifient R4-P.

| Image | Octets BIN | SHA256 BIN | SHA256 ELF |
|---|---:|---|---|
| Initiale369887b |112760|7d51abafd51c4b19159e84d360ba36d7873bac86d46c983586d958d3ace58283|d61ce4a9ee004cabd5eed867c0b13a9cfd6a46699f0fc6127fd009883afa76d5|
| Corrigée9f429e5 |113120|a5c56e7e4f2d9aeaf09ecf99f07c906975033b3b71b2a2b40b6e6ac1a8534b8f|1ff70554b3d8fa8039ea0c7ecc927449d8d9c7098a985964837e85a508b4f3e5|

Test hôte ciblé139 `r4p_maintenance` : PASS1,93s initial, PASS1,92s après ajout du
contrôle du pont/INTENT altéré. Vrais stores R1/R2 et snapshot authentique,142budgets
de coupure isolation +4phases RENEW ; monotonie epoch/run, anciennes clés, absence
de reçu ancien inventé, CLOSED au reboot et contrôle négatif du pont générique.
ASan/UBSan ciblé : PASS1,12s initial et après correction. Cross-build-only canonique
et lien ARM dédié PASS avant chaque nouvelle image. Warnings newlib syscalls de
link conservés ; aucune erreur masquée. BIN canonique V1 toujours
6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9.

Validation proportionnée : modifications de maintenance/harness avec test ciblé,
sanitizers et cross-build ; les138tests R4-D/full et1561budgets R2 déjà démontrés sur
le cœur inchangé sont réutilisés, pas rejoués. **139tests complets non annoncés.**
Aucun résultat hôte/build/flash assimilé à une preuve physique.

## Isolation unique, premier échec et reprise démontrée sûre

L'ancien STARTED(epoch2,tx1,run2/config2) reste historiquement indécidable ; aucun
COMPLETED rétroactif. Original262144 archivé sans perte en fixture et hors cible.
Capture exacte SHA+sémantique, deux INTENT authentiques128octets, floors epoch2/run2.
Une seule isolation transactionnelle du journal LIVE :52426octets/1639commandes,
payload/header/root, sans modifier epoch/INTENT/config/provenance ni historique/tail.
Frontend CLOSED/RETIRING : seul vrai RENEW peut ouvrir un nouveau contexte.

Premier trigger4 à12:56:41UTC : arrêt work1075us>1000us pendant compare_protected,
avant RENEW et sans nouveau STARTED/XYZ/run réservé. Le statut9 encore en RAM est
l'ancien actif ; le snapshot physique courant a bien un journal vide, pas un nouvel
indéterminé. Pont conservé SHA713a953384f2688f8448989ba706bf0dcef33ace3f177a1ee90fa5eb63713ba0.
Toute écriture arrêtée ; aucune réisolation ni retry du START.

Cause démontrée par breakpoint/pile en lecture seule : failed->checkpoint->
compare_protected, at245760.3840itérations sans checkpoint pour sauter LIVE
avaient consommé le quantum avant la première lecture tail. Appel GDB interrompu
au breakpoint pour conserver la pile, jamais présenté comme une comparaison réussie.
Correction minimale : saut direct122880->245760, mêmes139264octets protégés comparés.
Ce défaut de harness est distinct du défaut causal START/OBSERVE corrigé en R4-D.

Après tests/build, nouvelle image au WAIT, trigger7 ownershipfalse : montage et
comparaison complète139264octets, work342us/service4us, zéro écriture, aucune
deadline. Nouvelle lecture intégrale identique au pont713a ; dépassement ainsi
maîtrisé avant toute reprise NVM. Reset CLOSED puis seul trigger8+token/SHA exact
à13:08:52UTC : reprise du vrai RENEW2->3 **sans seconde isolation**.
Capture_bridge exige journal vide/noactive/nolast/noindeterminate, epoch2/gen3,
previous1/flags0 et INTENT128 authentique. Mode6 désactivé, aucune reprise au boot.
Un firmware générique exposé sur ce pont serait dangereux (test négatif conservé) ;
ce probe fermé n'introduit aucune récupération produit automatique.

## Résultats physiques de la séquence corrigée

Observation terminale après fin autonome, sans halt durant l'acquisition : stage10,
error0, six commandes4/0, aucune deadline, garde refermée après faute attendue.

| Commande réelle | Epoch/txid | Run | Durée admission→terminal |
|---|---|---:|---:|
| RENEW2→3 |2/65535|—|12,029168s|
| START |3/1|3|42,043145s|
| RENEW3→4 sous FIFO |3/65535|3|72,117310s|
| STOP |4/2|3|42,082639s|
| APPLY config3/gen3 |4/3|—|15,050051s|
| START |4/4|4|42,066015s|

RUN entre fin RENEW et début STOP : **60000989us** (60,000989s), acquisition maintenue.
Les durées ci-dessus incluent les barrières/commits synchrones coopératifs ; elles
ne sont ni le temps du port matériel START/STOP, ni une latence de réponse RTU validée.
Ancienne clé2/1 refusée résultat24 sans effet/écriture supplémentaire ; run3 puis4
réservés, aucun réemploi du run2. Epoch finale4/previous3/gen7/flags0, config3/gen3,
dernier START4/4 COMPLETEDgen24/run4/effect_state2, TR2Egen13 observé.

| Mesure cible | Maximum/compte observé | Critère |
|---|---:|---:|
| Travail entre checkpoints |913us|<=1000us (marge87us)|
| Service FIFO |566us|<=2500us|
| Intervalle appels FIFO en capture |822us|<=5000us|
| Lecture FRAM physique |132us|quanta32octets|
| Écriture FRAM physique |164us|quanta32octets|
| FIFO niveau maximal deux runs |25mots|512mots capacité|
| XYZ total deux runs |6454507|compteurs de parsing|
| Timestamps total deux runs |806814|compteurs de parsing|
| Polls FIFO |550976|erreurs FIFO/stop0|
| STOP du port métier, maximum |238us|observé nominal|
| STOP sécurité injectée |235us|observé nominal|
| Détection faute→fin STOP |242us|faute logicielle unique|

Dernier run seul1028814XYZ/128602TS ; totaux deux runs via compteurs r4m_*all.
Aucun FULL/OVR/malformed/discontinuité signalé par le parser/chemin en service,
qui aurait interrompu la séquence. Cela ne démontre pas zéro perte globale ni une
précision/calibration de cadence. Pas d'archivage intégral des échantillons.
`first_data_us=6027487` est relatif à l'admission du premier START, **inclut les
barrières avant effet** ; ce n'est pas un Ton capteur de6s. La garde premier mot20ms
reste expérimentale et distincte du gap continu5ms ; aucune borne Ton physique
indépendante ni provider B3 temps observé/calibré qualifiés dans cette mission.

Mode8 :1310650octets écrits,40975commandes physiques,75plages agrégées, toutes
entièrement LIVE[122880,245760), status0 ; CS fermé40975fois. Avec l'unique isolation :
1363076octets/42614commandes. Deux scans complets protégés,278528octets comparés.
Aucune plage/log refusée ni overflow. Audit des compteurs/plages dans measurements.json.

Faute de service **logicielle** injectée100ms après la fin du second scan, seulement
après dernier START durable COMPLETED : work.error=TR2_ERROR_STORAGE8 attendu,
engine LIVE_FAULT, stopped=true/running=false, aucune écriture après faute et
aucune indétermination. Pas une panne SPI/électrique, pas un WCET faute HAL/IRQ.

## Persistence, recovery et état matériel final

Snapshot cible262144 après campagne :
**a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9**.
Byte-identique à l'oracle hors ligne dérivé des vrais stores ; celui-ci n'a jamais
été écrit sur cible. Audit indépendant `inspect_live_fram.py` : racines/header/payload,
CRC journaux/epoch/TR2E/provenance conformes, racine activegen37. Historique122880
et tail16384octets exacts initiaux ;411octets finaux différents seulement LIVE.
Le snapshot source STARTED reste conservé, pas résolu rétroactivement.

Reset logiciel puis trigger5 readonly : PASS stage10/error0, engine READY,
noactive/noindeterminate, dernier terminal retrouvé/epoch4/run4/config3,
0commandes/0écriture/0XYZ, aucun auto-START ; work342us/service4us.
Lecture FRAM entière identique avant/après reboot. Il s'agit d'un reset logiciel,
pas d'un power-cycle physique ni d'un essai de coupure électrique réel.

Flash initiale restaurée par14pages/114688octets puis **2097152octets relus exacts**,
SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26.
Firmware hérité M016 CommissioningReadOnly en attente, capteur arrêté à la fin de
la campagne, GDB détaché ; pas de firmware LIVE cumulatif laissé par supposition.
FRAM encore intégralement relue après restauration Flash : mêmea13b3401…a2ef9,
**aucun rembobinage FRAM**, réservations epoch/run consommées conservées.
Ni identité persistante historique, norme R0, V1, dette A04 ni stashes modifiés.

## Preuves, publication et suite

[Preuves complètes](preuves/TR2-20261016-R4P/README.md) : logs GDB bruts et commandes,
invocations flash/identités, snapshots FRAM zlib sans perte, audits CRC/SHA,
mesures dérivées, modèle explicitement MODEL-ONLY, tests/builds/provenance/images,
préservation et manifeste SHA256SUMS. Bruts/backups Flash2Mio locaux récupérables
hors Git ; images probes compressées publiées pour contrôle, NON-PRODUCTION.
[Procédure et garde-fous](../../05_Firmware/LIVE_R4P.md).

Commits logiciels369887b et9f429e5 ; checkpoints0daaf36/2929a8b/9394e9b déjà publiés
et relus. Dernier checkpoint9394e9bb4a99a6be4a412fad0abbbcd6786ed1ec relu intégralement
125fichiers/112preuves avant lecture terminale. Rapport final/proofs/journal/roadmap
à publier et relire selon protocole ; aucun statut DONE anticipé.

ETAT_COURANT_TR2 actualisé : coexistence bornée, défaut/correction, réservations,
recovery/restauration et limites ; roadmap R4 actualisée sans fermer R4 global/R6.
Pas de blocage technique restant dans ce périmètre R4-P. À transmettre à ChatGPT :
contrôle indépendant des preuves, puis contrat distinct pour RTU/IRQ/TX, images RAM,
STOP/WCET sous fautes électriques, CPU/pile/provider temps/B3/composition LIVE.
La marge nominale87us ne constitue pas une réserve garantie en toutes conditions.
Aucune qualification industrielle, endurance, R5/R6 ou perte globale zéro annoncée.
'''
(ex/'DERNIER_RAPPORT.md').write_text(report)
p=root/'Modbus RTU/05_Firmware/LIVE_R4P.md'
p.write_text(p.read_text()+'''

## Résultat physique du 16/10/2026 — code9f429e5

Trigger7 readonly PASS,0write/snapshot713a inchangé/work342us. Trigger8 démarre
le vrai RENEW2->3 sans réisolation puis six commandes terminales4/0 : START3/run3,
RENEW3->4 sousFIFO, RUN60,000989s, STOP/APPLYconfig3/START4/run4. Work913us,
service566us/gap822us,6454507XYZ/806814TS sur les deux runs, niveau25, aucune
erreur FIFO/stop signalée ;1310650octets/40975commandes physiques/75plages LIVE,
CSfermés40975fois. Deux scans complets278528octets protégés. Première isolation
séparée52426octets/1639commandes, jamais répétée. L'échec1075us reste archivé.

Faute logicielle de service8 post-COMPLETED : STOP235us, réaction242us,
aucune écriture supplémentaire, pas de nouveau STARTED indéterminé. STOP métier
portmax238us ; durée transactionnelle STOP42,082639s inclut les barrières.
Snapshot cible finala13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9
byte-identique à l'oracle (jamais écrit sur cible), racine37/epoch4/run4/config3.
Reset logiciel/recoveryreadonly PASS,0write/0autostart, snapshot intégral inchangé.
Flash initiale2Mio restaurée exactec5bc344d…f5b26, FRAM finale conservéea13b…a2ef9.

Ces observations satisfont le périmètre borné R4-P ; pas qualification indépendante
ChatGPT/R4globale/R6, ni power-cycle, WCET en faute électrique, providerB3 ou
communicationRTU validés. Premiermot6027487us mesuré depuis admission inclut
commitspré-effet, ce n'est pas un Ton capteur. Voir rapport/preuves de mission.
''')
p=gov/'ETAT_COURANT_TR2.md'
s=p.read_text();s=s.replace('**Actualisation :** 14 octobre 2026,','**Dernière actualisation :** 16 octobre 2026, R4-P coexistence FRAM/FIFO observée dans son périmètre borné ; Flash initiale restaurée et FRAM finale epoch4/run4 conservée (détails en fin de document). **État du second banc rapporté le14/10, non relu depuis :**',1)
s+='''

## R4-P — 16/10/2026, coexistence FRAM/FIFO observée dans le vrai moteur R2

- **OBSERVÉ PHYSIQUE BORNÉ** : code9f429e513faba26ce89b760361c9cba08057bc24, correction R4-D conservée ; vraiRENEW2→3, START3/run3, RENEW3→4 sousFIFO, RUN60,000989s, STOP/APPLYconfig3/START4/run4 tous COMPLETED4/0.6454507XYZ/806814TS, niveau25, pas d'erreur FIFO/stop signalée ; work913us<=1000/service566us<=2500/gap822us<=5000.1310650octets/40975commandes écrits uniquementLIVE, scans complets protégés278528octets.
- **DÉFAUT HARNESSS CORRIGÉ / PREUVES CONSERVÉES** : isolation logique unique52426octets puis premier arrêt1075us avant RENEW, aucun nouveau STARTED. Pile readonly démontre3840itérations de saut LIVE sanscheckpoint ; correction O(1), test139/sanitizer/ARM PASS. Contrôle physique readonly work342us/0write et pont713a inchangé avant reprise explicite sans réisolation. Ancien STARTED2/run2 demeure historiquement indécidable et archivé ; aucune réadmission/reçu inventé, ancienneclé refus24.
- **ARRÊT / RECOVERY OBSERVÉS** : faute logicielle8 après COMPLETED, arrêt235us/réaction242us ; pas faute électrique/WCET. Reset logiciel/recoverylectureseule READY/0write/0autostart, epoch4/previous3, dernier START4/4/run4/config3, racine37. Snapshot complet finala13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9 et CRC audités ; historique/tail exacts.
- **RESTAURATION VÉRIFIÉE** : Flash initiale2Mio exactec5bc344d…f5b26, M016 CommissioningReadOnly WAIT/GDBdétaché ; FRAM finale relue après restauration et conservée sans rembobinage.55anciensworktrees/deuxstashes/2830fichiers protégés préservés, R4-D archivée byte-identique. Aucun accèsSD/HP/RS485/OptionBytes ni interventionphysique.
- **SUITE / LIMITES** : R4-P livrée pour contrôle indépendant ChatGPT ; R4/R6 complets non qualifiés.87us de marge nominale ne bornent pas les fautes électriques ; CPU/pile/IRQ/TX/imagesRAM/providertemps/B3 et compositionLIVE restent à qualifier par contrat distinct. Pas de perte globale zéro ni endurance annoncées ; premiermot mesuré depuis admission inclut les commits, distinct de Ton. [Plan et preuves](../05_Firmware/LIVE_R4P.md), rapport courant et snapshots publiés selon protocole.
'''
s=s.replace('DÉFAUT HARNESSS','DÉFAUT HARNESS');p.write_text(s)
p=gov/'ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';s=p.read_text();old=next(x for x in s.splitlines() if x.startswith('| **R4 —'))
new='| **R4 — Temps réel** | **R4-P COEXISTENCE PHYSIQUE BORNÉE OBSERVÉE ; REVUE INDÉPENDANTE À FAIRE** | Vrai R2/FRAM et FIFO simultanés, RENEW/START/RUN60s/STOP/APPLY/START, faute logicielle puis recoveryreadonly. | Code9f429e5 : work913us/service566us/gap822us,6454507XYZ/806814TS, FIFOmax25/erreurs0 ; unique isolation, ancienSTARTED archivé, epoch4/run4 conservés. Flash initiale exacte restaurée. Défaut du scan corrigé et contrôlereadonly préalable PASS. Pas WCET faute électrique/IRQ/RTU/B3 ; R4 global/R6 non qualifiés. [R4-P](../05_Firmware/LIVE_R4P.md). |'
assert old in s;p.write_text(s.replace(old,new))
print('final documents prepared IN_PROGRESS pending remote publication')
