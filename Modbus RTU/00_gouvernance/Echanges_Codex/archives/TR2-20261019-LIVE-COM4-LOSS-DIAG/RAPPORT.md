---
mission_id: TR2-20261019-LIVE-COM4-LOSS-DIAG
status: DONE
base_ref: main
base_sha: 3a1c2b23af7619ce80ddef06198f22bac8960cdb
initial_head: dfe5539eb1f9e4bbc3d850cab3dc84ac018bc925
result_sha: c56d32a2b8608d7938f1bd020df6993131732281
created_at_utc: 2026-10-19T14:41:29.015242+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# COM4 et redémarrage HELD rétablis — causes démontrées, protections conservées

**Deux défauts distincts ont été traités.** Le silence COM4 provenait du READ FRAM injecté par GDB dans un callback chronométré : garde2,5ms correctement déclenchée, runtime arrêté. Une seconde limite de c36c971 refusait START depuis un STOP/HELD valide ; elle a été constatée avant clôture, corrigée, testée puis qualifiée avec un paquet Windows complet réellement accepté sous SAC actif. La disponibilité a été vérifiée après le dernier audit, sans reset interposé. Livraison DONE après publication615206e7a1387fdb158df93cd70355aca1b94e07 et relecture distante intégrale :254livrables exacts/239preuves et snapshot13623lignes intègre. Contrôle indépendant ChatGPT distinct de cette clôture.

**État laissé : firmware STM32 c36c971 conservé, cible STOP5/21/HELD ; service Windows corrigé actif sur5100, seul maître COM4 ; GDB détaché.** Les lanceurs habituels dans `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\` redirigent vers la publication complète qualifiée `TR2-LIVE-COM4-HELD-20261019`, sans substitution de DLL. Ouvrir `OUVRIR_EDGE.cmd` puis `START.cmd` ; ne pas lancer un second service. STOP reste à attendre jusqu’au terminal+effet (~40s), puis `ARRETER_SERVICE.cmd` ferme le service. Aucun run laissé sans surveillance.

## 1. Preuves initiales avant reset

HEAD main réel dfe5539e, contrat et gouvernance vérifiés ; nouveau worktree isolé `MSM_vibe-LIVE-COM4-loss-diag`, branche `codex/live-com4-loss-diag`. Main historique82169af, autres worktrees et stashes préservés. AGENTS/protocole/gouvernance inchangés depuis la mission précédente ; journal relu intégralement. Archives antérieures jamais réécrites.

- Windows14:26:49UTC : aucun service/debugger restant ; FTDI BG04KSSW reconnu surCOM4, COM3STLINK distinct, SAC1. Les sept fichiers data initiaux sont copiés et hashés **avant toute reprise**. La base était fermée/sans WAL : intégritéok,6336observations, dont les6060du bilan antérieur et276CommunicationLost utilisateur.
- Journal service utilisateur :276observations du14:17:51.479 au14:25:26.242UTC, **aucun snapshot reçu**. Trace serial-5512-26404 :260émissions, toutes FC03B0 `0103000000158405`, zéroRX et zéroFC10 dans ce fichier. Ledger17 inchangé. L’échec opérateur14:19 dèsB0/sansFC10 est un constat utilisateur/contrat ; aucun fichier brut opérateur de cet essai n’a été trouvé et il n’est pas inventé. Les deux écrituresB5 n’auraient eu lieu qu’après B0/B3/B5 et persistance du ledger.
- Première session GDB : **aucun reset ni appel de fonction cible**. Identité SN003500463235510B37333439/UID004A003D/38425018/20333157 ; préfixe Flash147024octets exact à LIVE9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981. PC dans HAL_Delay de la boucle terminale, r4u.error6, engineLIVE_FAULT, **max_service846318µs/deadline_missed**, limite2500µs. FIFO arrêtée, aucunactif, dernierSTOP5/17 complet/journal94/root107/run13/config3.
- IRQ activées, UART sans erreurHAL ; RX contient les octets des requêtesB0. La file de réception déborde ensuite faute de consommation :2547événements perdus au relevé initial, conséquence cohérente de la boucle arrêtée. Aucun HardFault actif observé. Registres UART interprétés via `g_serial.uart.Instance` réel0x46002400 ; le dump initial annexe0x40013800 n’est pas une preuve de l’état de cette UART.
- Après préservation de cette faute, lecture FRAM fraîche complète : **e18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15**, exacte au dernier STOP connu ; aucune admission/écriture persistante/reset/flash avant ces constats.

## 2. Diagnostic différentiel et correction

L’audit précédent appelait `stm32_fram_storage_read(&fram,0,baseline,262144)` à l’adresse d’arrêt GDB quelconque. Les traces antérieures montrent un service interrompu ; ce READ non coopératif prend environ846ms à2,5MHz. À la continuation, `tr2_cooperative_checkpoint` inclut ce temps dans la durée du callback, retire l’autorité via `live_runtime_step`, arrête le capteur puis quitte la boucle de service pour HAL_Delay. Les IRQ restent actives ; plus de traitement Modbus foreground.

Un premier reset logiciel justifié, après sauvegarde, rétablit immédiatement le profilB0 et B3 arrêté dans le **même paquet Windows/firmware**, sans changement matériel. Reproduction unique contrôlée : capteurarrêté, servicefermé, breakpointu_service/work.servicing=true, mêmeREAD262144octets, détachement ; **max_service846632µs, erreur6/deadline_missed/LIVE_FAULT** reproduits, FRAMentièreinchangée. Un second reset sort de cette faute provoquée. **Deux resets, zéroflash**, aucun rollbackFRAM ni restaurationM016.

**Correction minimale d’outillage, pas des garde-fous :** [capture_stopped_fram.py](../../Supervision%20TR2/operator-demo/tools/capture_stopped_fram.py) et [procédure](../../Supervision%20TR2/operator-demo/COM4_DIAGNOSTIC.md), codea71fd097. Refus d’un observateur/debugger actif avant toute connexion ; ELF et Flash c36 exacts, UID et dernierSTOP correspondant au JSONL, aucun FIFO/actif/faute ; breakpoint au prochain `live_runtime_step` **hors scope** (depth0, servicingfalse, main_busyfalse), puis READ/dump. Aucun compteur/flag/deadline effacé ou modifié, aucune tolérance augmentée. La normale entrée coopérative ouvre ensuite son propre scope. Lecture sûre avant commandes : FRAMe18e exacte, B0/B3 rétablis après détachement. Test de garde réel : service actif refusé avant toute connexion GDB, aucun.gdb/.bin créé. Dernier audit complet également réussi et suivi d’une vraie lecture bus, sans reset.

**Erratum du bilan précédent :** les deux cycles et la tenue11min50s restent des observations réelles. En revanche, j’avais omis le contrôleB0 après la dernière lectureFRAM GDB ; l’affirmation de disponibilité du firmware après cet audit était erronée. Cette mission corrige ce constat sans réécrire l’archive ni affaiblir les protections. La cause exacte actuelle est fortement étayée par état initial, ancienne commande, durée caractéristique et reproduction contrôlée ; aucune mesure électrique d’alimentation/câble n’est prétendue. Le retour bidirectionnel sans intervention matérielle et le RXinitial réfutent une rupture de liaison nécessaire pour expliquer cet incident.

## 3. Première reprise physique sous c36c971 — état intermédiaire

|Commande|Identité|Terminal+effet|
|---|---|---|
|START|5/18, run14|39,8399798s ; COMPLETED4/0 + B3V/F|
|STOP|5/19|39,9153058s ; COMPLETED4/0 + B3arrêté/HELD/nonFRESH|

Aucune répétition de l’ancienSTART expiré, aucune clé5/12..17 rejouée, aucunRENEW. Nouveau couple alloué depuis gardesB0/B3/B5 réelles et ledger durable avant submit ; handofftokenisé et ouvertureCOM4exclusive réussis, aucunforçage. Les limites de pause c36 oscillante et signature future demeurent documentées, aucun mélange de DLL. Les deux services de remise en route utilisent réellement le **lanceur installé LANCER_SERVICE.cmd** et sa nouvelle SQLite/trace vivante ; pas d’archive ou de serviceWSL substitué.

Vrai Edgeheadless : deux capturesRUNNING **80→146points globaux,120→219XYZ**, valeurs et réceptions renouvelées ; API et payloadsSQLite corrélés.305nouvelles observationsV/F, aucunB3rejeté dans le snapshot final. Le nouveau cycle est borné pour vérifier la récupération ; la campagne11min50s déjà acquise n’est pas rejouée, aucune nouvelle endurance/métrologie/qualificationindustrielle annoncée.

Premier audit de remise en service : stage7/error0, work806/service2044µs sous1000/2500, FIFO/stop0, deadline_missedfalse. FRAM complète **cd1486dedaf7b04902f68bd372e26ee35f96aa43cfe5c8024954c6fe66cd3406**, exacte au modèle du cœurR2 : **journal102/root115/epoch5/run14/config3gen3/STOP5/19** ; préfixe122880 et queue16384octets byte-identiques au préflight. Le modèle est une preuve de métadonnées, pas un effet physique simulé substitué. Pas de perte d’historique ou baisse de compteurs.

Après cet audit14:36:36UTC, **réception B0/B3 réelle à14:37:26.521UTC**, STOP/HELD/seq61, transportConnectedtrue, archivePersisted, aucune réinitialisation entre les deux. isCurrentfalse est attendu pour HELD ; ce n’est pas un CommunicationLost. ImageLIVE et377fichiers applicatifs Windows inchangés ; SAC1/zérorefus corrélé, un seul service WindowsPID19160 et aucundebugger à14:37:31UTC. Port5100/COM4/1152008E1, cible1. Ce service intermédiaire a ensuite été fermé pour la mise à niveau ; ses traces restent dans `data\service-9300-9947.log` et `data\serial-9300-9947.jsonl`.

## 4. Refus START depuis HELD identifié et corrigé avant clôture

Un contrôle normal supplémentaire, sans forçage, lit B0/B3/B5 mais **n’émet aucunFC10** : le garde c36 exigeait V/F=0 au lieu de distinguer une valeur HELD conservée d’une acquisition active. Il refuse donc le vrai B3STOP1/flags5819. Ledger19 inchangé. Ce refus sémantique n’est **pas** le timeoutB0 initial, dont la cause reste l’audit GDB. Les deux cycles antérieurs réussis comportaient un reboot entre eux et ne démontraient pas la reprise directe depuis HELD ; cette limite est désormais explicitée.

Correctif **d67741f2**, `CanStart` : statutREADY1, bitacquisitionarrêtée4096 positif, FRESHabsent, valeur soitabsente soit explicitementHELD. Les étatsRUNNING/FAULT/absenceSTOP/valeurvalidenonHELD restent refusés ; gardesB0/B5/ledger/exclusivité inchangées. Le fixtureboot du test porte désormais les vrais flags4097, pas un zéro incomplet. Régression de handoff rouge2échecs/2succès avant correction, puis **248testsService PASS/0skip**, dont7cas de garde et les handoffsboot/HELD avec incertitude d’écriture. Aucun test supprimé/affaibli. Cette validation a été relancée car le codeService a effectivement changé ;350autrestests.NET et le firmware/ARM inchangés ne sont pas rejoués.

### Publication Windows réellement contrôlée

Publication complète377fichiers dans **`C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-COM4-HELD-20261019\app\`**, Service.dll SHA **63f3ddc1a91d99599a719bc6a013aa52810a9813a9d08c40233ce45b29cb85fa**. Compilation faite sur b031d01a+patchsource conservé, contenu compilé identique au commitd67741f2 ; la provenance distingue cette construction de l’horodatage du commit ultérieur. Aucun rebuild destiné seulement à changer les métadonnées des octets déjà acceptés.

Premier smoke COM99 : serviceexit0/zéroblocageCI/SAC1, mais le délaiHTTP25s expire au premier chargement ; première observation journalisée ~35s après lancement (pas une mesure exclusive du démarrageWeb, cause fine de latence non isolée). Échec conservé et **non présenté PASS**. Le contrôleur reçoit un paramètre d’attente de démarrage60s (1..120), sans changer les critèresHTTP200/exit0/SAC1/zéroCI/octetsinchangés. Confirmation avec **les mêmes377fichiers**, quatreHTTP200/exit0/SAC1/aucunblocagecorrélé à14:49:29UTC. Outilc56d32a2. Aucun COM4 sollicité par ce smoke. La raison interne de l’acceptation Windows n’est pas établie ; aucune acceptation des prochaines compilations ou signaturedurable n’est revendiquée. L’ancien rebuild21a440… refusé n’est jamais copié dans un paquet accepté.

Le service c36 est fermé proprement sur B3arrêté. **12060observations** de sa base fermée sont copiées exactement dans les données de la nouvelle publication, avec ledger19 et journaux ; l’ancien paquet/basedonnées restent conservés. Originaux des cinq lanceurs sauvegardés dans `TR2-LIVE-COM4-HELD-20261019\legacy-c36-launchers\` avec empreintes. Les anciens.cmd ne font qu’appeler les nouveaux.cmd, **après preuve de confiance au propre emplacement du nouveau paquet**. Aucun ancien chemin/DLL/processus n’a servi à éluder un refus. Pas de mélange d’assemblies ; les377anciensfichiersapp restent identiques.

### Qualification physique du paquet corrigé

- Départ depuis le vrai B3HELD hérité deSTOP5/19, **aucun reset entre les deux cycles** : START5/20/run15 terminal4/0+effetV/F en**39,8293935s**. Les compteurs runtime montrent2starts dans le mêmeboot au dernier audit.
- Edgeheadless réel : **234→298points globaux,351→447XYZ**, observations courantes/renouvelées, données API exactes àSQLite ;359observationsV/F de cette version dans le snapshot final. Pas de simulation substituée au bus.
- **STOP5/21 via le lanceur habituel installé**, redirection native comprise : terminal4/0+arrêtHELD/nonFRESH en**39,8382777s**, retour0. `ARRETER_SERVICE.cmd` ferme ensuite le service avec retour0.
- Dernier audit sécurisé14:55:28UTC : stage7/error0, work806/service2044µs sous1000/2500, deadline_missedfalse, FIFO/stop0 ; **FRAM97f43a53316248d705fbf86d9efe2ad8fa93ab2d37c2af937cfcc068e69455f3**, intégralement exacte au modèleR2, préfixe122880/queue16384 inchangés. **Journal110/root123/epoch5/run15/config3gen3/STOP5/21** ; aucune baisse/reprise de compteur.
- Après ce dernier audit et **sans reset**, B0/B3 réels reçus à**2026-10-19T14:56:07.1128262+00:00** : transportConnectedtrue, STOP/HELD, archivePersisted, isCurrentfalse attendu pour HELD. Service relancé par le lanceur habituel ; Windows14:56:12UTC : **SAC1/zéroblocagecorrélé/377nouveauxfichiers exacts/unservicePID13868/aucundebugger**. Logs actifs dans la nouvelle publication `data\service-12977-21261.log` / `serial-12977-21261.jsonl`.

L’acquittement de pause stable publié8a54 fait partie de cette publication complète ; il est désormais utilisé dans les handoffs physiques du nouveau paquet. L’ancien c36 conserve son défaut de pause et de gardeHELD, mais il n’est plus le paquet exécuté par les lanceurs habituels. Le firmware STM32 demeure **c36c971/9bbe…0981**, sans reflash. La tenue11min50s historique n’a pas été répétée ; ces cycles vérifient les deux réparations dans une portée bornée, pas une endurance ou un arrêtindustriel rapide.

## 5. Préservation, validations et publication

**6336payloads utilisateur initiaux conservés exactement**, y compris276CommunicationLost ; anciens journauxserial/service byte-identiques. L’ancien paquet est gelé avec12060observations, toutes retrouvées sans modification dans le snapshot de la nouvelle base. **Snapshot final13623observations**, intégritéok, via [APIbackupSQLiteWindows](https://nodejs.org/download/release/v24.0.0/docs/api/sqlite.html#sqlitebackupsourceDb-path-options) sur connexion source readonly, service laissé actif ; pas de copie brute incohérente duWAL. Les préfixes de logs actifs sont des instantanés datés, pas leur fin définitive. `summary.json` décrit la première reprise ; **`summary-final.json` décrit l’état consolidé**.

Validation proportionnée : reproductioncausale unique, auditcorrigé refusprocessactif/contrôlesstricts/lecturespositives + B0/B3aprèsdetach, deuxcycles pour deuxversionsPCdistinctes, gardeHELDrouge/vert248tests, publicationWindows/contrôleCI/HTTP, SQLite/Edge réels et modèles de métadonnées. Firmwareinchangé : aucun ancien cross-build/testC/campagneendurance répété. **Zéroflash, deuxresets qualifiés nécessaires uniquement à l’étude causale**, aucunautrereset pour rendreSTARTHELDpossible, aucuneSD/OB/mass erase/rollbackFRAM/gestephysique/politiqueWindows.

8180fichiers protégés/72autresworktrees/deuxstashes et mainhistorique82169afpropre préservés. Branchecodex/live-com4-loss-diag, pushnormal explicitemain. Commitsa71fd097(outillageGDB),b031d01a(checkpointdiagnostic),d67741f2(gardeHELD),c56d32a2(attentecontrôleWindows). Le checkpointb031d01a a été relu intégralement :147livrables/137preuves etSQLite7713lignes intègre ; consolidation **615206e7a1387fdb158df93cd70355aca1b94e07** relue : **254livrables byte-identiques/239preuves brutes et stockées, SQLite13623intègre**. Worktree propre après ce push, anciens worktrees/stashes/main conservés. Cette clôture est poussée puis relue également ; son SHA sera donné dans la réponse finale. SHA final et étatGit dans la réponse, sans auto-référence du commit. Journal/roadmap/opérateur actualisés : cause/erratum, gardeHELD, paquet effectivementexécuté et étatSTOP/serviceactif.

[Preuves et manifestes](preuves/TR2-20261019-LIVE-COM4-LOSS/README.md), [reprise](../../Supervision%20TR2/operator-demo/REPRISE.md), [diagnostic](../../Supervision%20TR2/operator-demo/COM4_DIAGNOSTIC.md). Preuves locales `/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag/`. **Sources physiques : STM32c36c971 ; PCcontenud67741f2** (provenancebuildb031+patch), outils a71/c56. Suite : contrôle indépendantChatGPT ; signaturedurable et futurespublications restent chantier distinct. Disponibilité vérifiée aux horodatages fournis, pas promise indéfiniment.

Contrôle complémentaire avant clôture : B0/B3 reçus à**2026-10-19T15:04:20.9126637UTC**, toujours transportConnectedtrue/STOP/HELD après le dernier audit, sans reset supplémentaire. État technique conservé ; aucune acquisition supplémentaire lancée pour la publication.
