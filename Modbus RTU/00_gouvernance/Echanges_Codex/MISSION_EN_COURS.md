---
mission_id: TR2-20261019-LIVE-COM4-LOSS-DIAG
status: READY
base_ref: main
base_sha: 3a1c2b23af7619ce80ddef06198f22bac8960cdb
created_at_utc: 2026-10-19T14:30:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Diagnostic prioritaire : TR2 LIVE muet sur COM4 après démonstration réussie

## Constat utilisateur et urgence

L'utilisateur teste la démonstration réelle installée dans `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019`, paquet Windows accepté **c36c971**, firmware LIVE laissé installé. Après les deux cycles physiques réussis de la mission précédente, l'utilisateur a lancé `START.cmd` à 2026-10-19 14:19 UTC : `poller_paused_handle_released`, émission **FC03 lecture B0** `0103000000158405`, puis `Modbus read timed out on bus 'operator'`, code retour 1, **aucun FC10 émis** dans la trace. `/api/v2/live/overview` observé vers 14:21 UTC : dernier `observation` `CommunicationLost`, `snapshot:null`, `identity:null`, `transportConnected:false`, `isCurrent:false`, `archiveState:Persisted`. Dernière valeur HELD connue à 13:55:45.758 UTC, identifiée TR2-001 ; elle est périmée, pas courante. Logs `service-*.log` montrent échecs de lecture répétitifs ~1,7 s jusqu'à 14:20:45 UTC. Le transport ne répond plus **avant la commande**, il ne faut ni inventer une acquisition, ni tenter un START/STOP en aveugle.

## Référence et acquis à protéger

Mission `TR2-20261019-WINDOWS-TRUST-LIVE-RESUME` **DONE dans sa portée**, archivée dans `archives/TR2-20261019-WINDOWS-TRUST-LIVE-RESUME/`; rapport/préuves/commits sur `main`, code et firmware physiquement exécutés **c36c971**, image LIVE SHA `9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981`. Dernier état connu après essais : capteur arrêté, FRAM SHA `e18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15`, journal94/root107/epoch5/run13/STOP5/17 ; transactions5/12..5/17 consommées ; COM/GDB libérés. **Ce sont des constats datés, à recontrôler.**

Les problèmes de confiance Windows persistent pour les **nouveaux binaires**, notamment le correctif `8a54a28` non déployé. **Ne jamais substituer ou mélanger des DLL** dans le paquet accepté. Conserver `data\transactions.json`, `data\live.db`, journaux, toutes les preuves, stashes, worktrees et `main` local divergent. Ne pas lancer `--live-archive` ni service WSL comme substitut de preuve COM4.

## Mission Codex

1. Vérifier `main`, `AGENTS.md`, gouvernance ChatGPT, protocole échanges, rapport de la mission précédente, `operator-demo/README.md`, `REPRISE.md`, code réel Windows c36 et firmware installé. Ne pas répéter les campagnes réussies sans raison, ni nettoyer des worktrees.
2. **Diagnostic prioritairement en lecture seule** : collecter les logs `service-*.log`, `serial-*.jsonl`, Windows Event Logs/ports/processus, ledger, chronologie 13:55–14:22 UTC, et comparer les dernières réponses B0/B3/CRC au premier timeout. Distinguer service actif/COM ouvert/handle libéré de TR2 répondant réellement sur bus. Inspecter USB/FTDI COM4, alimentation/reset et état MCU/sensor via ST-LINK/GDB selon la procédure non destructive qualifiée, uniquement hors FIFO active. Identifier pourquoi un TR2 autrefois joignable cesse de répondre entre deux essais. Ne pas confondre perte physique, émission RS-485, modem, moteur Modbus et état firmware.
3. Séparer les hypothèses testables : alimentation/câble, port COM incorrect/occupé, transceiver/RS-485, firmware arrêté/exception/watchdog, race de handoff, état série MCU, événement système Windows. Procéder par vérifications ciblées et proportionnées. Si geste physique (rebrancher, réarmer alimentation) indispensable, demander **un seul geste précis** après avoir documenté la justification ; ne pas exiger de validation logicielle répétitive.
4. **Préserver l'état durable et les identités** : aucune nouvelle commande B5 avant preuve d'échange fiable et garde courante; pas de retry de START précédent en aveugle. Avant flash/reset, contrôler cible/Flash/FRAM/état transactions et récupération; aucune restauration FRAM ni réutilisation 5/12–5/17, aucune opération destructrice inhabituelle. `allow_flash:true` et `allow_debug:true` disponibles pour une correction avérée et contrôlée, mais pas de flash/refonte de convenance ni restauration M016 arbitraire.
5. Après résolution démontrée : valider rétablissement B0/B3, reprise de l'API `transportConnected`, observations réelles et qualité; START/STOP par la procédure opérateur si garde sûre, terminal + effet, données renouvelées dans Edge, arrêt final explicite. En cas de cause côté firmware ou service, corriger minimalement et tester. Toute version Windows recompliée devra satisfaire Code Integrity par un moyen autorisé et un smoke sans cible; ne pas copier un binaire refusé dans le paquet fonctionnel ni contourner SAC/WDAC.
6. Documenter précisément **la cause prouvée ou les hypothèses restantes**, l'état matériel final, statut COM4/Windows, traces horodatées, tests/effets et marche à suivre pour la démo. Compiler/tester uniquement ce qui change, publier code et preuves sur `main`, `DERNIER_RAPPORT.md` et vérifier distant. Si la remise en service demeure impossible : `BLOCKED` et checkpoint reprenable, sans faux succès ni perte des données. Le lot esthétique et le nettoyage Git sont hors mission.

## Permissions et garde-fous

`allow_commit:true`, `allow_push:true`, `allow_flash:true`, `allow_debug:true`, cible NUCLEO-U575ZI-Q, Flash/FRAM/identité/Option Bytes/microSD préservées, récupérabilité et absence de risque matériel non maîtrisé selon `AGENTS.md`. Ne pas intervenir physiquement à la place de l'utilisateur, ne pas désactiver les contrôles Windows. Priorité à un diagnostic reproductible permettant une démonstration LIVE fiable demain, pas à un reset qui effacerait la cause avant observation.
