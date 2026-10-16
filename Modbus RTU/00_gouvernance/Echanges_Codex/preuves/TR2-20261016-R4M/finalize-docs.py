from pathlib import Path
import subprocess as s,datetime,json
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4M-live-fram-reconciliation');ex=root/'Modbus RTU/00_gouvernance/Echanges_Codex';sha=s.check_output(['git','rev-parse','3606942'],cwd=root,text=True).strip()
(ex/'DERNIER_RAPPORT.md').write_text(f'''---
mission_id: TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION
status: BLOCKED
base_ref: main
base_sha: 0e09805f4ade74ed04553f04efb7a881e8bfb84b
initial_head: 11719e3946e0aa0773e8993b841c82760130fcf3
result_sha: {sha}
created_at_utc: {datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')}
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-M — maintenance acquise dans sa portée ; campagne arrêtée sur nouveau STARTED

**BLOCKED, conformément à la condition d'arrêt du contrat.** La maintenance
isolée a réellement retiré l'epoch1 par un vrai RENEW terminé4/0 et refusé
l'ancienne clé1/1/code13 avec résultat24, sans écriture ni acquisition.
Le nouveau START(epoch2,txid1,run2) s'est arrêté sur budget1241us>1000us,
avec nouveau STARTED indéterminé9. Aucun nouveau nettoyage, reprovisionnement
ni essai n'a suivi. **Aucune qualification écritureFRAM+FIFO ou R6**.
Firmware initial restauré exactement2Mio ; FRAM nouvelle conservée intégralement.

[Procédure et restrictions](../../05_Firmware/LIVE_R4M.md) ·
[preuves et manifeste](preuves/TR2-20261016-R4M/README.md) ·
[mesures dérivées des logs](preuves/TR2-20261016-R4M/measurements.json) ·
[audit indépendant du snapshot final](preuves/TR2-20261016-R4M/fram-final-audit.json).
Les logs GDB bruts priment ; les fichiers MODEL-ONLY ne sont pas des relevés cible.

## Identité, sauvegardes et ancien verrou

TR2-001, ST-LINK003500463235510B37333439, UID004A003D/38425018/20333157
vérifiés. Flash2097152 etFRAM262144 sauvegardées deux fois avant flash/reset/writeNVM,
comparées intégralement et égales au final R4. Accès GDB qualifié minimal nécessaire
aux sauvegardes ; seulement transport de lecture/RAM avant vérification des fichiers.
WHO7B/CTRL1=00/FIFO_CTRL4=80(mode0 bypass)/CTRL10=00 observés sans écriture capteur.
Ancienne Flash M016 attempt02, pas un successeur cumulatif qualifié.

Ancien journal : RESERVEDgen1 CRC05cd4bb6, STARTEDgen2 CRC73a97135,
epoch1/tx1/code13/run1/config2/gen2, deux INTENT authentiques sans reçu observé.
**L'effet ancien reste indécidable** ; aucun COMPLETED/28/29 inventé. Snapshot
intégral initial archivé sans perte dans tests/fixtures/r4_started_snapshot.zlib,
SHA c814 ci-dessous ; mission et rapport R4 archivés byte-identiques à0e09805.
Sauvegardes binaires complètes récupérables sous /home/lolo/dev/msm/reports/TR2-r4m/hardware/.

## Modifications et validation logicielle

Code initial de maintenance **bc3e83de050a527cc839b494025c906f5c06246b** :
module bench-only r4m_maintenance, entrée opt-in fermée/WAIT à chaque boot,
journal d'adresses, mesures et garde-fous, test sur snapshot authentique,
auditeur hors ligne inspect_live_fram.py et LIVE_R4M.md. Aucun serveur UART/RTU/SD
n'est initialisé dans ce probe. INTENT128 authentique maintient la borne run1 ;
commit logique journal par média transactionnel existant, aucun effacement brut.
Pont RETIRING autorise uniquement vraiRENEW1->2 ; frontend ouvre après retraite durable.
Le contrôle négatif prouve que ce pont ne doit jamais être exposé à une entrée
produit générique. Reset : CLOSED ; reprises limitées aux SHA explicitement inspectés.
Ce n'est pas une nouvelle politique R0 de recovery automatique.

Après premier échec, code **{sha}** : checkpoints supplémentaires
entre CRC/décodages successifs d'epoch/effet/identité/configurations, aucun contrôle
supprimé ni seuil relevé. Reprise6 indépendante de l'isolation, limitée au SHA exact
du pont et à l'INTENT128 byte-identique à l'archive ; pas de second vidage journal.

Validations effectives :

- Nouveau test maintenance :142budgets de coupure d'isolation,4phasesRENEW,
  anti-rejeu/runfloor, refus sans INTENT, refus double isolation, reprise du pont
  exacte et refus INTENT altéré. Initial PASS1,97s, sanitizer1,16s.
- **Après modification core,137/137 tests hôte PASS56,30s**, dont test1371,92s.
  Réexécution justifiée par le risque de régression ; code cœur a alors changé.
  Sanitizer ASan/UBSan du test maintenance PASS1,38s.
- `cmake --build` hôte/probe ARM et
  `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` PASS.
  Appel préalable `./tr2_validate.sh` refusé126 (fichier nonexécutable), sans test lancé ;
  hôte etcross exécutés séparément, aucune répétition de suite déjà réussie.
- BIN canonique V1 inchangé :6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9.
  Aucun test hôte/cross/sanitizer présenté comme validation matérielle.

## Exécutions physiques distinctes

| Observation | Trigger4 initial, bc3e83d | Trigger6 reprise,3606942 |
|---|---:|---:|
| Départ UTC |08:43:58|08:53:57|
| Image BIN SHA256 |cc2d815b1ca3089d9fb05fb336d5484ae789c70daa20c26a80e9b2fe201bc062|a76be76c890383040bd58de281d1fc1f6c86195363809a39b5df850bd470ad21|
| État terminal |stage20 erreur6 après isolation|stage22 erreur6 pendant START|
| Workmax / limite |1075/1000us|1241/1000us|
| FRAM readmax/write max |132/164us|132/164us|
| Octets écrits, LIVE seulement |52426|314556|
| Plages agrégées / commandes physiques |3/1639|18/9834|
| FIFO XYZ / timestamps |0/0|0/0|
| FIFO running/stopped ; error/stop_error |false/true ;0/0|false/true ;0/0|

Les seuils restent quantum32/work1000us/service2500us/gap5ms/premier-mot20ms expérimental.
DWT/HCLK160MHz ; pas calibration externe ni WCET électrique. Aucun halt GDB pendant
les essais :300s minimum avant les observations terminales. Le premier essai a fini
en9,232s de ticks, le second26,258s. Les délais d'attente hôte ne sont pas des durées RUN.
CS relâché observé via ODR après chaque write ; toutes plages dans[122880,245760),
tous résultats0, aucune saturation du journal d'adresses/transport. Cumul366982octets,
11473commandes sur21plages ; relectures payload/header/root du média conservées.
Comparaison externe complète des zones protégées après chaque exécution et à la fin.

**Premier essai :** isolation atomique publiée (racine gen5), puis dépassement pendant
recovery avant RENEW/START. Snapshot pont c868 sauvegardé puis double lecture identique.
Un diagnostic **lecture seule** (0write, capteur arrêté) a réussi avec workmax824us ;
la cause précise du premier dépassement reste non attribuée. La sortie GDB initiale
s'est terminée en erreur de présentation sur tableau0opérations ; résultats précédents
et snapshot sont acquis, pas une réussite masquée de la commande d'observation.

**Reprise distincte, sans reprovisionnement :** frontend fermé sur snapshotc868,
INTENT et journal vérifiés, vraiRENEW1/65535/code12 terminé4/0 en12,062952s.
Epoch2/previous1/flags0 durable ; ancienne clé refus24 sans écriture ni dispatch.
START2/1/run2 dure6,035546s puis échec : work1241us, safety STOP236us,
maximum observé des appels STOP237us. Ces valeurs ne sont pas un WCET de panne SPI.
Le port contient run2/config2 et0XYZ ; cela **ne prouve pas l'absence d'effet capteur**.

Snapshot final : racine gen11/payload/header/root CRC valides ; slot0 copieA
RESERVEDgen5 CRCb6a943c8, copieB STARTEDgen6 CRCc0cd794b, epoch2/tx1/code13/run2,
effect_state1. Deux INTENTgen3/4 pourrun2, aucun START_OBSERVED durable.
Slot127 conserve vraiRENEW COMPLETEDgen4/4/0. Moteur observé FAULT/indeterminate9.
Hypothèse à vérifier hors écriture : coût cumulé du start/observe réel avant le
prochain checkpoint. Le code et les variables bornent cette piste ; aucun backtrace
au point exact de la panne ne permet d'en faire une cause démontrée.

## Restauration, empreintes et limites

Flash standard avec vérification,114688octets/14pages restaurés depuis la nouvelle
sauvegarde, puis comparaison intégrale2Mio exacte. Aucune écriture FRAM de restauration,
aucun mass erase/Option Bytes/RDP, aucune manipulation physique demandée.
Dernière observation : CommissioningReadOnly en attente, GDB détaché.
FRAM relue après restauration byte-identique au snapshot final f2bd.

| Donnée | SHA256 |
|---|---|
| Flash initiale et finale2Mio |c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26|
| FRAM initiale262144 |c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561|
| Pont après isolation, doublelecture |c868950fe3c60f9a3b23a9ce8d565029ee57d4f06b08cbf6062de9238b2af950|
| FRAM finale262144, avant/aprèsrestauration |f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1|
| Historique[0,122880), avant=après |4e3c2d39f03eaf7cf46d8c987d944548c1f14752d36bafa1a41439078cfb3cf3|
| Tail[245760,262144), avant=après |4fe7b59af6de3b665b67788cc2f99892ab827efae3a467342b3bb4e3bc8e5bfe|
| LIVE finale |3d3d4f7c76e2fe4295f5ab2519dc86a2ffaa279fdb905c7df99d909d4e6556e3|

**Non exécutés après le nouveau STARTED :** RENEW2->3, service60s/FIFO en coexistence,
STOP de commande, APPLY, START3, injection de faute service aprèsCOMPLETED,
reboot/recovery finale mode5. Scripts correspondants préparés ne sont pas des essais.
Aucune répétition pour obtenir PASS. Aucun nouvel essaiSD/HP/COM7/RS485 ; aucun
provider temps capteur calibré, B3/protocole2/RTU électrique/R6 non qualifiés.

## Blocage réel et prochaines actions

La condition explicite du contrat « nouveau STARTED indéterminé : stopper » est
atteinte. **Ne pas rembobiner FRAM ni réutiliser epoch1/run1 ou run2**. Les triggers
actuels refusent le SHA final ; M016 ignore LIVE. Conserver cet état et les snapshots.
Prochaine mission/arbitrage : qualifier hors écriture les bornes START/observe/STOP,
instrumenter chaque étape du port réel, puis décider d'une réconciliation fondée
sur preuve causale authentique ou d'un nouveau contexte de maintenance explicitement
autorisé. Aucune terminalisation rétrospective9->COMPLETED ni budget relâché pour PASS.

## Git, journal et publication

53anciens worktrees/HEAD inchangés,54au total avec celui de mission ; deux stashes
6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87
préservés. Dépôt d'origine propre à82169af7e76797731358852bec83bfb0ceb54ccf.
2596fichiers protégés hash-identiques, dont archives/gels/mission/gouvernance/dettes.
Worktree isolé : /home/lolo/dev/msm/projects/MSM_vibe-R4M-live-fram-reconciliation,
branchecodex/r4m-live-fram-reconciliation, push explicite versrefs/heads/main.

Checkpoints0d1fc0fe6bfd1e33e057021b25bbd55f8d2b7953 et
093cc1c4ccf6f261481bda96464a4ff3895094f1 publiés et relus intégralement depuis GitHub,
manifestes41 puis80preuves contrôlés. Code réellement flashé indiqué ci-dessus ;
les commits documentaires ultérieurs ne changent pas l'image évaluée.
Journal ETAT_COURANT_TR2 actualisé et roadmap factuelle R4-M ajoutée : maintenance
bornée acquise, nouveau verrou/run2, firmware restauré, suite conditionnée au contrat.
Permissions exercées : développement/tests/commit/push normal/flash/debug qualifiés.
Publication finale et relecture distante encore en cours ; statut BLOCKED porte
sur le verrou matériel/persistant, pas sur l'attente de publication. Le SHA final
et l'état Git après publication seront fournis dans la réponse Codex.
''')
p=root/'Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md';t=p.read_text();start=t.index('## R4-M — checkpoint avant maintenance,16/10/2026');t=t[:start]+'''## R4-M — 16/10/2026, campagne arrêtée conformément au contrat

- **OBSERVÉ / PRÉSERVÉ** : UID/STLINK vérifiés ; doubles backups frais Flash2Mio/FRAM262144 conformes R4. Ancien STARTED reste indécidable, archive intégrale c814e9cc…271561 et échanges R4 conservés, historique/tail inchangés.
- **VALIDÉ LOGICIEL** : maintenance isolée CLOSED/RETIRING,142coupures+4phasesRENEW ; checkpoints recovery additionnels après premier dépassement.137tests complets PASS56,30s/sanitizer ciblé1,38s/ARM ; BINcanonique V1 inchangé6a2150af…15a9. Code physique final3606942.
- **OBSERVÉ CIBLE** : isolation unique puis1075us>1000us avant RENEW ; pont c868950f…af950 sauvegardé deux fois. Reprise explicitement liée à ce pont, sans réisolation : vraiRENEW1->2 COMPLETED4/0, ancienneclé refus24/0write. START(epoch2,tx1,run2) échoue1241us, nouveau STARTED9,0XYZ ; STOP réel observé236us, FIFOarrêté. Aucun essai/cleanup supplémentaire après cette nouvelle indétermination.
- **RESTAURÉ / BLOCKED** : Flash initiale exacte2Mio c5bc344d…f5b26, FRAM finale f2bd7a99…8a0c1 conservée sans rembobinage, M016 CommissioningReadOnly enattente/GDBdétaché. STOP de commande/APPLY/coexistence FIFO/reboot final/faute injectée non exécutés ; aucune qualif R6/B3/RTU2.
- **À TRANSMETTRE À CHATGPT** : nouveau verrou impose l'arrêt du contrat ; ne pas réutiliser run2 ni retirer le verrou sans nouveau plan explicite. Qualifier d'abord hors écriture les quanta du port START/observe ; cause exacte1241us non attribuée. Rapport et snapshots sans perte publiés pour contrôle indépendant, aucune décision normative R0 modifiée.
''';p.write_text(t)
p=root/'Modbus RTU/00_gouvernance/ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';t=p.read_text();old=next(line for line in t.splitlines() if line.startswith('| **R4 — Temps réel**'))
new='| **R4 — Temps réel** | **R4-M BLOCKED SUR NOUVEAU STARTED ; MAINTENANCE BORNÉE OBSERVÉE** | Quanta FRAM/CRC/SHA, arrêt indépendant ; ancien verrou archivé, vraiRENEW1->2 et ancienneclé refus24. | R4-M :137tests/ARM/sanitizer ; isolation unique puis reprise du pont sans reprovisionnement. Nouveau START epoch2/run2 indéterminé après1241us>1000us,0XYZ ; arrêt contractuel, Flash initiale restaurée, FRAM/identités conservées. Coexistence FIFO/STOP de commande/APPLY/reboot final nonqualifiés ; R6 fermé. [R4-M](../05_Firmware/LIVE_R4M.md). |';assert old!=new;p.write_text(t.replace(old,new))
print('REPORT BLOCKED; journal and roadmap updated')
