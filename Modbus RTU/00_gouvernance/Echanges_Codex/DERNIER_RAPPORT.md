---
mission_id: TR2-20261019-WINDOWS-TRUST-LIVE-RESUME
status: IN_PROGRESS
base_ref: main
base_sha: 0d0c395dc0cf5f68193d3bade6df8fcdc75ad092
initial_head: cb34b3f1dd19cc04be0235cd568f61fb343e850b
result_sha: 8a54a289dae2dad312cf2245c0db03fc76cc67ef
created_at_utc: 2026-10-19T13:41:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Checkpoint — premier cycle LIVE physique acquis, second run à poursuivre

L'utilisateur autorise explicitement la qualification limitée au paquet **c36c971**, accepté réellement par Windows, et reporte la signature durable à un chantier distinct. Aucune protection Windows modifiée/contournée. Le correctif logiciel de pause `8a54a28` est publié et testé mais sa nouvelle DLL reste refusée : il n'est **pas** utilisé pour les essais physiques. Le SHA result_sha désigne les développements publiés ; PC et firmware physiquement évalués proviennent de **c36c9718a75e6b40c45707a482e8db836296cab2**.

## Acquis observés

- Politique effective SAC active (état1), GUID0283ac0f-fff1-49ae-ada1-8a933130cad6, pas de certificat CodeSigning disponible ni de droit administrateur. Raison exacte de l'acceptation ancienne R5-B2 non démontrée. Paquet c36c971 inchangé : smoke Windows COM99, quatre HTTP200, aucun refus corrélé,377 fichiers identiques. Une recompilation Service.dll SHA21a440e3…a2d17 reste bloquée ; aucune promesse d'auto-allow futur.
- Préflight frais13:34UTC : UID004A003D/38425018/20333157 et ST-LINK003500463235510B37333439 ; Flash M016c5bc…f5b26 et FRAM2b501a…7dd84 exactes aux sauvegardes vérifiées ; capteur arrêté ; COM4 FTDI BG04KSSW libre. Aucun travail local historique ou stash supprimé.
- Image LIVE147024octets SHA256 **9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981**, flash standard qualifié puis readback exact. Boot autonome stage7/error0, STOP, journal78/root91. Aucun START au boot.
- Vrai service Windows `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\`, COM4 1152008E1, nouvelle SQLite `data\live.db`, HTTP5100, `--seconds 0`.
- START5/14 : terminal4/0 et effet B3 V/F en39,8497082s ; STOP5/15 : terminal4/0 + arrêté/HELD/nonFRESH en39,8574011s. L'outil exige token acquitté après fermeture du poller puis ouverture COM exclusive avant toute écriture. Pas de commande forcée/retry automatique ; pause connue oscillante mais handle effectivement fermé. Chaque ledger est persisté avant submit.
- Edge headless réel, pas revue visuelle humaine : captures RUNNING avec102→168 points globaux et153→252 XYZ ; overview courant, API/history et SQLite vivants ; captures HELD non courantes et CSV réels conservés. Valeurs mg renouvelées, pas d'inférence UTC fenêtre/run/config absents du fil.
- Audit arrêté après STOP :2 555 901XYZ/319488TS, erreursFIFO/stop0 ; maxwork806µs/service1980µs/gap2220µs sous budgets1000/2500/5000 ; journal86/root99/epoch5/run12/config3, STOP5/15. FRAM SHA87722e50f3e78b9556c1ecfb0ea53cf35f3b520ea760b4f2ec4960ef7a40ad87 ;219octets modifiés exclusivement dans la zone LIVE ; préfixe historique122880 et queue16384 inchangés.
- Correction acquittement stable8a54a28 :239 tests Service réussis ;350 autres tests, cross-build et construction image inchangée réutilisés, sans répétition. Publication rebuild Windows refusée conservée séparément ; aucun mélange de DLL.

## Reprise immédiate

Le premier service a été fermé proprement après STOP. Vérification du reboot autonome arrêté puis **second run prolongé surveillé** à exécuter ; aucun résultat d'endurance encore annoncé. Ne pas rejouer5/14–5/15 ; allocation depuis B5 et ledger réel. Pas de GDB pendant FIFO. Laisser LIVE installé si sain, STOP final et audit avant clôture.

Preuves externes : `/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/`. Copie publiée et manifeste dans [preuves/TR2-20261019-WINDOWS-TRUST-LIVE](preuves/TR2-20261019-WINDOWS-TRUST-LIVE/README.md). Rapport intermédiaire, pas qualification industrielle ni validation du déploiement futur. Autorisations exercées : développement/test, commit/push, flash standard/debug ; aucun SD/OB/mass erase/rollback FRAM. État Git et relecture distante seront précisés lors de la clôture.
