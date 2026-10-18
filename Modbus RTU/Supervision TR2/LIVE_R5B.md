# R5-B — composition LIVE directe, checkpoint bloqué au déploiement Windows

Mission `TR2-20261017-R5B-LIVE-PHYSICAL-TO-WEB-DEMO`, HEAD initial `868a80dcba90f794aa694cc470fca6a421292a2e`, base contractuelle `ae24fa3c5a2773f9379b021f3457513d844545ef`.

**BLOCKED : le démonstrateur physique n'a pas été exécuté.** Le code de composition est testé logiciel, mais Windows refuse son chargement. Aucun PASS physique, aucun GO R5/R6/production. R5-A et R5-AFIX conservent leurs rapports et preuves historiques.

## Composition et portée réellement livrées

Code évalué `dc773733a98d78016e1d0f8a341b155e1851e2c7` : même application `TR2.Supervision.Service`, nouvelles options `--live-direct`, transport existant NModbus/SerialPort, application `LiveObservationService`, migration SQLite7 et host Web S7 réutilisés. Le mode direct exige `--allow-diagnostic` ; il applique le lecteur B0/B4/B6/B3 réel et le décodeur strict existant. Il n'utilise jamais `CapturedLiveTransport`, ne simule aucun header et ne modifie pas les champs manquants. Si le vrai firmware refuse ces lectures, il faudra qualifier le refus puis envisager le parcours B0/B3 borné prévu au contrat, pas inventer des headers.

Le transport direct n'expose que `IRegisterTransport`, sérialise les accès, ouvre un seul propriétaire et ferme sa connexion à l'arrêt, sur échec ou sur déconnexion logique explicite. La trace optionnelle sous NModbus conserve les octets réellement écrits/lus, par chunks, sans reconstitution silencieuse des trames. Les anciennes compositions V1 gardent leur chemin sans trace. Les tests prouvent une ouverture pour vingt lectures concurrentes, zéro accès au writer, fermeture/reconnexion et absence de retry dans la même observation. L'exclusivité matérielle COM4 reste à démontrer.

La page et l'API n'obtiennent aucun command sink : aucune écriture B5/FC10 cachée, POST commandes non exposé dans cette composition. Le fichier de contrôle contenant exactement `disconnected` ferme réellement le handle au prochain cycle ; absence du fichier reconnecte. Il s'agit d'une déconnexion **logique opérateur**, pas d'un débranchement USB/RS485 prouvé. Lors d'une perte, aucune ancienne mesure ne devient nouvelle observation valide. Run, epoch, configuration de fenêtre et temps UTC restent inconnus lorsqu'absents du fil.

Le probe distinct `TR2_STM32_R5B_LIVE` reprend le runtime R4-U, le pacing250µs et le projecteur corrigé R5-AFIX. Il exige FRAM4e7fab…84bd/journal70/root83/run10/STOP5/11, prépare uniquement START5/12/run11 et STOP5/13, sans retraite ni RENEW. Il reste au WAIT avant trigger. Le maintien UART après STOP est porté de2s à20s pour laisser observer HELD après fermeture du maître de commandes ; les recettes historiques gardent2s. Budgets1000/2500/5000µs inchangés. Ce nouveau probe est **compilé, jamais flashé ni exécuté**.

## Validations acquises, sans répétition après reprise

| Niveau | Résultat conservé |
|---|---|
| .NET Linux SDK10.0.401/net10.0 | build0warning/0error ; 561tests/0échec/0skip, dont211Service et35Transport ; huit nouveaux cas directs |
| Compatibilité | anciens tests S0–S7/V1, migrations, vraies captures corrigées R5AFIX et anciennes captures240 négatives passent dans la suite |
| JS | sept scénarios sous DOM simulé PASS, pas navigateur graphique |
| Publication Windows | `dotnet publish -r win-x64 --self-contained true -m:1` réussi ; manifestes des fichiers, pas runtime installé globalement |
| STM32 canonique | `bash ./tr2_validate.sh --cross-build-only` PASS ; SDK U5v1.9.0 ; BIN85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5 inchangé |
| Probe R5B | ARM PASS, BIN147640octets `2cf491cc8cfa8da76fe97512b3fb6471a753b3ac89ef9b169bf05745075a27ee`, ELF `4750312506e1ff726240cf180e707d4945e2e43cb28b5a8f62af24677004ccb7` |
| HTTP réel Linux, port série volontairement absent | /, /live.html, /live.js, /api/v1/fleet et /api/v2/live→200 ; POST/api/v1/commands→404 ; retour service0 |
| SQLite du smoke | 54observations CommunicationLost, tous snapshots/run/epoch nuls, integrity_check=ok ; aucune mesure physique |
| Modèle hôte R2 | sur copie FRAM initiale, START5/12/run11 puisSTOP5/13 prédisent journal78/root91 et SHA2b501a…7dd84 ; modèle de métadonnées seulement, jamais à charger sur cible |

Les146tests firmware et sanitizers R5-AFIX sur cœur inchangé sont réutilisés, non rejoués. Le nouveau changement firmware est limité au harness : cross-build canonique et dédié proportionnés. Les tests PC complets sont justifiés par le point d'entrée et l'usine de transport partagés.

Échecs conservés : premier build MSBuild parallèle termine MSB4166 (nœuds enfants quittés ; fichiers diagnostics indiqués devenus absents), build `-m:1` réussi ; lancement direct du script shell refusé car non exécutable, invocation `bash` réussie sans chmod ; première préparation du modèle hôte avait changé par erreur la garde initiale70 en78, assertion refusée sans accès matériel, source corrigée et exécution réussie. Ces échecs ne sont pas masqués, aucun contrôle affaibli. Les warnings newlib syscalls des builds ARM sont présents dans les logs ; pas assimilés à un build .NET sans warning.

## Blocage Windows observé

Une seule invocation du service Windows, port **COM99** volontairement inexistant pour smoke sans cible, échoue avant ouverture série :

`Could not load ... TR2.Protocol.dll ... contrôle d'application ... (0x800711C7)`.

Lecture seule du journal `Microsoft-Windows-CodeIntegrity/Operational` : événements3033 et3077, policy `{0283ac0f-fff1-49ae-ada1-8a933130cad6}`, exigences de signature Enterprise non satisfaites. Événements3089 associés conservés. Ce sont des faits de déploiement ; ils ne prouvent aucun défaut du capteur ou du Modbus. Aucun déplacement du binaire pour éluder le contrôle, changement de politique/signature/règle d'approbation ni désactivation de protection n'a été tenté. **Une voie de déploiement approuvée, avec signature/autorisation adaptée par le responsable du poste, doit être définie avant reprise Windows.** Les permissions de mission ne lèvent pas cette restriction.

Une hypothétique passerelle série alternative n'est ni qualifiée ni présentée comme disponible. Aucun mapping `/dev/ttyS4` versCOM4 n'est déduit de son seul nom. Le lancement sous Linux valide uniquement HTTP/SQLite en CommunicationLost, pas l'accès à l'adaptateur Windows.

## État matériel prouvé, sans nouvelle mutation

Préflight17/10/2026 11:56:42UTC : STLINK003500463235510B37333439, UID004A003D/38425018/20333157, Flash complète2097152octets SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`, FRAM complète262144octets SHA `4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd`. Identiques au dernier état R5AFIX, M016 CommissioningReadOnly ; WHO7B/ODRoff/FIFObypass/TSoff observés par lectures de registres. GDB qualifié seulement, sans HOTPLUG, reset ni écriture persistante ; staging RAM utilisé pour les sauvegardes. Historique/tail et identités inchangés ; aucun rollback FRAM.

Inventaire17/10 11:57:40UTC : COM4FTDI0403:6001/BG04KSSWA, COM3STLINK exclu, aucun processus TR2/BenchMaster/GDB/Programmer dans le filtre d'inventaire. L'inventaire ne prouve pas tous les propriétaires possibles de handles : aucune exclusivité physique COM4 n'est revendiquée. Aucun COM4 ouvert par R5B ; aucune émission RS485, commande START/STOP, flash, reset, SD/HP/OptionBytes. Aucune restauration nécessaire puisque M016 n'a pas été remplacé. Les clefs5/12–5/13 et run11 sont **préparées, non consommées par cette mission**. Depuis l'interruption, aucun nouvel accès cible : ces observations restent datées du17/10, pas télémétrie actuelle.

## Reproduction logicielle et reprise physique

Sources et builds restent sous Linux natif. Depuis `Modbus RTU/Supervision TR2` :

```bash
dotnet restore TR2.Supervision.sln
dotnet build TR2.Supervision.sln --no-restore -m:1
dotnet test TR2.Supervision.sln --no-build -m:1
node tests/live-ui.test.cjs src/TR2.Supervision.Web/wwwroot/live.js
dotnet publish src/TR2.Supervision.Service/TR2.Supervision.Service.csproj   -r win-x64 --self-contained true -m:1 -o /chemin/externe/windows
```

Ne pas refaire ces validations sur code inchangé : logs/TRX/images acquis. Les commandes physiques suivantes sont **prévues, non exécutées**. Après déploiement Windows approuvé et nouveaux contrôles matériels, lancer le même service Windows avec chemins locaux/UNC effectivement approuvés :

```text
TR2.Supervision.Service.exe --live-direct COM4 --allow-diagnostic --database <base-test.db> --listen http://127.0.0.1:5089 --trace <trace-neuve.jsonl> --seconds 30 --unit 1 --connection-control <fichier-controle>
```

Trace neuve obligatoire ; durée1..600s et unité1..247 ;1152008E1 fixe ; serveur loopback. Arrêt au terme ouCtrl+C, libération du handle. Ne pas lancer tant que le firmware M016 actuel ne fournit pas le profil LIVE prévu. Ne jamais ouvrir simultanément BenchMaster et le service. L'imageR5B ne démarre pas spontanément l'acquisition.

Ordre restant :

1. Résoudre le déploiement Windows avec l'administrateur/responsable du poste, vérifier lancement autorisé et HTTP sans cible ; ne pas désactiver les protections implicitement.
2. Recontrôler mission/main et état matériel frais, Flash/FRAM/WHO et port/adaptateur exclusif ; réutiliser les sauvegardes si toujours identiques. Ne présumer aucune identité libre depuis le seul présent rapport.
3. Finaliser le contrôleur de campagne R5B séparant les phases COM4 sans chevauchement : lecture directe au repos ; maître explicite STARTadmissible puis fermeture ; service direct RUNNING avec réception/API/SQLite et déconnexion logique/reconnexion ; fermeture avant STOP explicite puis service HELD durant les20s prévues. Les anciennes recettes R5AFIX5/10–5/11 ne sont pas réutilisables. Pas de nouveau protocole de commandes dans l'UI.
4. Flash qualifié du probe correspondant au code/hash relus, readback et trigger borné ; budgets FIFO/FRAM/STOP inchangés, aucun GDBpause en acquisition. Capturer octets réels, vérifier CRC et corrélation SQLite/API/rendu Web, deux valeurs distinctes seulement si observées ; pas de métadonnées manquantes inventées.
5. STOP sûr, audit FRAM entière/historique/tail et nouveaux reçus, restauration M016 par pages nécessaires avec contrôle intégral2Mio, capteuroff/COMfermé/GDBdétaché ; rapport et relecture distante.

Preuves versionnées : [R5B](../00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5B/README.md), manifeste SHA256SUMS/BUNDLE. Originaux externes `/home/lolo/dev/msm/reports/TR2-r5b/`. Aucun essai physique d'acquisition R5B à répéter puisqu'aucun n'a été lancé. Ce checkpoint ne vaut ni clôture du démonstrateur ni qualification indépendante ChatGPT.
