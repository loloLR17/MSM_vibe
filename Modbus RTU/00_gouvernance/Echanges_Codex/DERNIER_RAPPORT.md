---
mission_id: TR2-20261019-LIVE-COM4-LOSS-DIAG
status: IN_PROGRESS
base_ref: main
base_sha: 3a1c2b23af7619ce80ddef06198f22bac8960cdb
initial_head: dfe5539eb1f9e4bbc3d850cab3dc84ac018bc925
result_sha: a71fd097ab6486d528d51c7760be8db64cb5fd8e
created_at_utc: 2026-10-19T14:41:29.015242+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# COM4 rétabli — cause d’audit GDB reproduite, procédure corrigée

**La cause est l’appel de lecture FRAM injecté par GDB dans un callback coopératif chronométré lors de l’audit final précédent.** Le garde-fou2,5ms a correctement arrêté le firmware ; ce dernier recevait encore les octets UART mais ne servait plus Modbus. Ce n’était pas un START engagé. La reproduction ciblée retrouve le même défaut sans modifier la FRAM. Procédure corrigée, un nouveau START/STOP réel réussi et B0/B3 vérifiés **après le dernier audit, sans reset interposé**. Le statut IN_PROGRESS porte sur la publication/relecture finale restante, pas sur un blocage technique.

**État laissé : LIVE c36c971 conservé, cible STOP5/19/HELD, service Windows actif sur5100, seul maître COM4, GDB détaché.** Pour utiliser la démo déjà lancée, ouvrir `OUVRIR_EDGE.cmd` puis `START.cmd` ; ne pas lancer un second service. Attendre le terminal+effet (~40s) ; STOP puis `ARRETER_SERVICE.cmd` pour une fermeture volontaire. Le service readonly peut rester actif lorsque la cible est arrêtée ; aucune acquisition n’est laissée sans surveillance.

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

## 3. Remise en service physique observée

|Commande|Identité|Terminal+effet|
|---|---|---|
|START|5/18, run14|39,8399798s ; COMPLETED4/0 + B3V/F|
|STOP|5/19|39,9153058s ; COMPLETED4/0 + B3arrêté/HELD/nonFRESH|

Aucune répétition de l’ancienSTART expiré, aucune clé5/12..17 rejouée, aucunRENEW. Nouveau couple alloué depuis gardesB0/B3/B5 réelles et ledger durable avant submit ; handofftokenisé et ouvertureCOM4exclusive réussis, aucunforçage. Les limites de pause c36 oscillante et signature future demeurent documentées, aucun mélange de DLL. Les deux services de remise en route utilisent réellement le **lanceur installé LANCER_SERVICE.cmd** et sa nouvelle SQLite/trace vivante ; pas d’archive ou de serviceWSL substitué.

Vrai Edgeheadless : deux capturesRUNNING **80→146points globaux,120→219XYZ**, valeurs et réceptions renouvelées ; API et payloadsSQLite corrélés.305nouvelles observationsV/F, aucunB3rejeté dans le snapshot final. Le nouveau cycle est borné pour vérifier la récupération ; la campagne11min50s déjà acquise n’est pas rejouée, aucune nouvelle endurance/métrologie/qualificationindustrielle annoncée.

Audit final corrigé : stage7/error0, work806/service2044µs sous1000/2500, FIFO/stop0, deadline_missedfalse. FRAM complète **cd1486dedaf7b04902f68bd372e26ee35f96aa43cfe5c8024954c6fe66cd3406**, exacte au modèle du cœurR2 : **journal102/root115/epoch5/run14/config3gen3/STOP5/19** ; préfixe122880 et queue16384octets byte-identiques au préflight. Le modèle est une preuve de métadonnées, pas un effet physique simulé substitué. Pas de perte d’historique ou baisse de compteurs.

Après cet audit14:36:36UTC, **réception B0/B3 réelle à14:37:26.521UTC**, STOP/HELD/seq61, transportConnectedtrue, archivePersisted, aucune réinitialisation entre les deux. isCurrentfalse est attendu pour HELD ; ce n’est pas un CommunicationLost. ImageLIVE et377fichiers applicatifs Windows inchangés ; SAC1/zérorefus corrélé, un seul service WindowsPID19160 et aucundebugger à14:37:31UTC. Port5100/COM4/1152008E1, cible1. Le service reste lancé pour la démo ; ses traces sont dans `data\service-9300-9947.log` et `data\serial-9300-9947.jsonl`.

## 4. Préservation, validations et publication

Les **6336payloadsSQLite initiaux sont conservés exactement**, y compris les276échecs utilisateur ; les anciens journauxserial/service restent byte-identiques. Snapshot final cohérent via l’API native WindowsSQLite backup (connexion source readonly, service laissé actif), **7713observations**, intégritéok ; ne pas traiter une copie brute du WAL actif comme cohérente. [API de sauvegarde Node24](https://nodejs.org/download/release/v24.0.0/docs/api/sqlite.html#sqlitebackupsourceDb-path-options).1377nouvellesobservations à l’instant du snapshot ;722CommunicationLost intentionnels de pause opérateur explicitementsanssnapshot. Les snapshots des logs finaux actifs sont des préfixes datés, pas leur fin définitive.

Validation proportionnée : syntaxePython, refusprocessactif avantGDB, reproductionphysique ciblée de l’ancien audit, deux contrôlespositifs lectureFRAMàfrontière + B0/B3 postdétachement, START/STOP/Edge/SQLite réels. Firmware et.NETinchangés : **aucun ancien test/build/campagne prolongée rejoué**, aucun nouvel exécutable Windows à faire autoriser. Les permissions flash restent disponibles mais non exercées. Debug/reset qualifiés exercés ; aucuneSD/OB/mass erase/écriture arbitraireFRAM/gestephysique/politiqueWindows.

8180fichiers protégés/72autresworktrees/deuxstashes et mainhistorique82169afpropre préservés ; branche auxiliairecodex/live-com4-loss-diag, pushnormal explicitemain. Premier commit **a71fd097** corrige l’outil/procédure. Le présent rapport et les preuves sont publiés en checkpoint puis relus depuisGitHub avant statutDONE ; SHA final et étatGit dans la réponse de clôture, sans auto-référence du commit. Journal ETAT_COURANT_TR2 et roadmap actualisés : cause, erratum, état réel connecté/STOP et règle d’audit corrigée.

[Preuves et manifestes](preuves/TR2-20261019-LIVE-COM4-LOSS/README.md). Preuves locales `/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag/`. Sources PC/firmware physiquement évaluées **c36c9718a75e6b40c45707a482e8db836296cab2** ; result_sha désigne uniquement l’outil corrigé. Suite : contrôle indépendantChatGPT ; signaturefuture et déploiement du correctif de pause restent séparés. La disponibilité est vérifiée aux horodatages fournis, pas promise indéfiniment.
