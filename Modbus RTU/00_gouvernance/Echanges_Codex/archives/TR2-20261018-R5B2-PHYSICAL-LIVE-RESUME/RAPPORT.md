---
mission_id: TR2-20261018-R5B2-PHYSICAL-LIVE-RESUME
status: DONE
base_ref: main
base_sha: 9187f4b324647b8eb5b007886151f0bc665c4947
initial_head: dc47ee0e20dbbdd6047c517571c841eb5502d674
result_sha: 7bf84b127ce3c473e340004b8d34a650594c45ed
created_at_utc: 2026-10-18T17:13:53.044327+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R5-B2 — démonstration physique réussie, M016 restauré

**DONE dans la portée diagnostique bornée : critères techniques satisfaits, bilan et preuves publiés puis relus intégralement depuis GitHub.** Le vrai service Windows reçoit les mesures physiques COM4, les archive dans SQLite et les sert à l'API et au Web. Edge headless a rendu une mesure valide et fraîche. Aucun blocage technique actif ; aucune qualification R6, produit ou industrielle.

## Versions, modifications et validations réutilisées

HEAD initial `dc47ee0e20dbbdd6047c517571c841eb5502d674`, base contractuelle `9187f4b324647b8eb5b007886151f0bc665c4947`. Firmware et service réellement exécutés : **`dc773733a98d78016e1d0f8a341b155e1851e2c7`**, inchangés sur main. Contrôleur natif versionné avant essai : **`7bf84b127ce3c473e340004b8d34a650594c45ed`**, `docs(r5b2): checkpoint approved local service smoke and bounded physical controller`.

Aucune correction du service, du parseur, du profil ou du firmware n'a été nécessaire. Le lecteur strict accepte les vrais B0/B4/B6/B3 du probe avec l'opt-in `LiveDiagnostic`. Aucun header simulé, assouplissement du validateur ou writer caché. Ajouts limités au contrôleur de campagne, collecteur HTTP local, orchestration et preuves. Le contrôleur reprend les échanges qualifiés R5-AFIX, expose une commande par processus, attend le terminal durable et ferme son port avant la phase suivante. Compilation et selftest CRC sans transmission réussis, puis gardes et fermeture vérifiées physiquement.

Les **561 tests .NET, sept scénarios JS et builds ARM R5-B sont réutilisés, sans répétition**, car leurs entrées sont inchangées. Les 372 fichiers de la publication Windows locale ont été comparés intégralement au manifeste évalué. BIN du probe : 147640 octets, SHA `2cf491cc8cfa8da76fe97512b3fb6471a753b3ac89ef9b169bf05745075a27ee` ; ELF `4750312506e1ff726240cf180e707d4945e2e43cb28b5a8f62af24677004ccb7`. Image réutilisée, flash et readback exacts.

Le fait utilisateur sur le démarrage local Windows est désormais complété par une reproduction Codex sur COM99 : HTTP 200, CommunicationLost, snapshot null, archivage Persisted et sortie 0. Aucun changement de politique, signature ou protection. Une première collecte HTTP a temporisé ; le helper suivant explicite `Proxy=null` pour la boucle locale et réussit, sans modifier le service. Les deux smokes et leurs données restent conservés séparément des acquisitions COM4.

[Note R5-B2](../../Supervision%20TR2/LIVE_R5B2.md), [preuves et manifestes](preuves/TR2-20261018-R5B2/README.md) : 126 artefacts BUNDLE. Originaux : `/home/lolo/dev/msm/reports/TR2-r5b2/`. Service local : `C:\Users\lolo\AppData\Local\MSM\TR2-r5b\` ; sorties Windows dans le répertoire frère `TR2-r5b2-evidence`.

## Résultats physiques réellement observés

Préflight frais le 18/10 à 17:01:40 UTC : UID `004A003D/38425018/20333157`, ST-LINK `003500463235510B37333439`, WHO=7B et capteur arrêté. Flash M016 complète c5bc…f5b26 et FRAM complète 4e7fab…84bd conformes. COM4 FTDI 0403:6001/BG04KSSWA retrouvé ; COM3 ST-LINK exclu. Les nouvelles identités ne sont utilisées qu'après la garde exacte journal70/root83/run10/STOP5/11.

| Critère | Observation réelle |
|---|---|
| Maître unique | Ouverture concurrente COM4 refusée pendant la lecture initiale, sans émission du contre-test. Processus attendus et ports fermés entre phases. |
| Commandes R2 | Une START5/12/run11 puis une STOP5/13, terminaux4/0 ; aucun retry, collision, RENEW ou retraite. Quatre FC10 du contrôleur seulement, zéro du service. |
| RS-485 | 824 ADU du service, dont206 B3 :64 initiales,88 courantes,54 HELD ;1033 ADU du maître explicite. Total1857 réponses UART, CRC exacts. |
| SQLite7 | 528 observations :234 COM4 (64 initiales,88 courantes,28 pertes logiques,54 HELD) et294 COM99 sans mesure. integrity_check=ok, identifiants uniques, répétitions conservées. |
| Corrélation | Tous les champs B3 brut→service→SQLite comparés ;141 observations API reliées par observationId. Exemple complet versionné. |
| Web réel | HTML/JS HTTP200 ; DOM Edge headless rendu avec COM4, LiveDiagnostic, Persisted, valeurs en mg et UTC absente. Aucun navigateur humain ni screenshot revendiqué. |
| Variation | Neuf RMS globaux courants réellement observés :1046,1047,1079,1080,1082,1083,1085,1086,1092 mg. Aucune vibration provoquée ou valeur fabriquée. |
| Perte/reprise | Fermeture logique du handle entre5 et8s :28 CommunicationLost, snapshots null, IsCurrent=false ; retour courant et second segment PC. |
| HELD | 54 observations après STOP, FRESH=0 et IsCurrent=false, également servies à l'API. |
| Acquisition MCU | 2301494 XYZ,287687 timestamps, FIFOmax70 ; erreurs FIFO, stop, UART et drops=0. |
| Budgets | work810≤1000µs ; service2029≤2500µs ; gap FIFO2330≤5000µs. Limites inchangées. |
| Oracle4096 indépendant | RMS global/X/Y/Z=1088/82/1082/79mg ; peaks1173/151/1165/160mg, exacts aux sorties MCU. |
| Terminaux côté maître | START39,869131s ; STOP39,857972s. Ce n'est pas un arrêt industriel rapide. |

`analyze.py` reconstruit les ADU depuis les chunks série réels sous NModbus : CRC, unité, FC03, longueur, offsets3002/3003, réservés et UTC contrôlés. Tous les champs décodés sont comparés au payload SQLite et aux réponses API. Le fichier `correlated-example.json` relie un même observationId à son ADU, au service, à SQLite et à l'API ; le DOM est relié à une autre observation réelle par son ReceivedAt exact, sa séquence et ses valeurs.

Exemple rendu par Edge : réception PC17:06:48.7987826UTC, séquence18/âge445ms, RMS1047/84/1042/61mg, peaks1121/135/1114/129mg, V/F/HELD=true/true/false. UTC fenêtre nulle. Les run/epoch/configuration et temps événement absents du fil restent inconnus ; ils ne sont pas extrapolés depuis le contrôleur. La déconnexion est logique avec fermeture du port, pas un débranchement électrique. Le segment PC ne prouve pas un reboot STM32.

## Restauration et préservation

**M016 restauré le18/10/2026 à17:09:24UTC**, par19 pages normales/155648 octets. Flash entière **2097152 octets exacte**, SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.

FRAM finale **262144 octets**, SHA **`2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84`**, exacte au modèle R2 et relue après restauration, **sans rollback**.419408 octets écrits/8 commits dans LIVE uniquement. Historique[0,122880) et tail[245760,262144) identiques. Journal70→78/root83→91, epoch5, run10→11, config3gen3, STOP5/13 et effet phase3 conservés. **Les clés5/12–5/13 sont désormais consommées.**

Après restauration : WHO7B, CTRL1=00, FIFOCTRL4=80, TSCTRL=00 ; ODR off, FIFO bypass, timestamps off. M016 CommissioningReadOnly WAIT, GDB détaché et COM fermé. Inventaire des processus ciblés vide à17:10:40UTC.8165 fichiers protégés,66 autres worktrees et deux stashes inchangés ; dépôt historique main82169af propre. Aucun accès SD/HP, changement de câblage, mass erase, Option Bytes/RDP, rollback ou changement de politique Windows/sandbox/Codex.

## Limites et suite

Démonstration courte N4096, **NON-PRODUCTION**, profil partiel : B0 protocole2/cap12 sans LIVE16 ; B1/B2/B7 non composés ; B6 UNAVAILABLE préparé par le serveur, seul son header2 est lu par le service. UTC et identités de fenêtre absentes du fil. Aucune qualification d'endurance, calibration, CEM, robustesse électrique, WCET en faute, STOP industriel ou produit LIVE complet. Les logs ASP.NET `Hosting environment: Production` décrivent uniquement l'environnement hôte par défaut.

Prochaine étape : contrôle indépendant ChatGPT puis contrat R6/couverture produit et traitement de la latence STOP. Ne pas répéter les validations acquises sur code inchangé, ni relancer la campagne5/12–5/13. Une nouvelle mission doit dériver ses gardes et identités admissibles de l'état frais issu de2b501a.

Journal et roadmap actualisés factuellement : déblocage Windows désormais observé par Codex, chaîne physique, budgets, FRAM, restauration, limites et suite. Aucun jalon J4/J5 qualifié unilatéralement, aucune dette A04 ou archive historique modifiée. Permissions commit/push/flash/debug exercées. Worktree `codex/r5b2-physical-resume`, checkpoint7bf84b1 publié ; aucun merge/rebase/reset/force push.

**Publication et relecture acquises :** bilan `dc547080f1403fe7cfc053013eda7d3d6949fae1`, push normal puis récupération de main depuis GitHub à2026-10-18T17:17:28.211572+00:00. Comparaison intégrale de133 fichiers, contrôle de128 empreintes et126 artefacts BUNDLE,1857 ADU CRC exacts, SQLite528 lignes/234 COM4, exemple API et DOM réels, Flash2Mio et FRAM complète restaurées vérifiées. [Preuve de relecture](preuves/TR2-20261018-R5B2/readback-dc54708.json). Ce résultat autorise le statut DONE de livraison R5-B2 ; contrôle indépendant ChatGPT et qualifications ultérieures restent distincts.

La mise à jour DONE et les archives sont publiées ensuite par un push normal, puis relues à leur tour. Le SHA final est fourni dans la réponse Codex, sans référence autoréférente dans ce commit. Aucun nouvel essai physique, build ou test acquis répété pendant la clôture.
