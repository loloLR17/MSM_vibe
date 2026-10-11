---
mission_id: TR2-20261011-M014-A-SUPERVISION-DIAGNOSTIC
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 36e0055cf481e29529c546564143705b8abc58d9
mission_sha: 65d7f6f11d1a8d10a4b1d182616a25865eda65d5
result_sha: bcc69c9d07aa02f756f9e2db0d67f62613d1951e
created_at_utc: 2026-10-11T17:28:17.072526+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# M014-A — voie diagnostic .NET isolée, logiciel validé hors port

**Critères logiciels satisfaits : build réussi,502/502 tests, paquet Windows préparé. BLOCKED provisoire uniquement pour attente de publication/relecture distante.** Aucune ouverture COM7, émission RS485, installation/exécution HP, flash/debug/reset ou répétition de campagne M013-I. UNKNOWN0 ne signifie ni READY ni production/bidirectionnel général qualifiés.

## Références et inspection

Main initial propre `36e0055cf481e29529c546564143705b8abc58d9`, stashes005/002 conservés. Fetch/pull --ff-only vers contrat `65d7f6f11d1a8d10a4b1d182616a25865eda65d5` ; seul contrat modifié par ce fast-forward. Base_sha/created_at absents du contrat : base_sha=null, pas de date contractuelle inventée. Mission active ChatGPT inchangée, autorisation commit/push main et interdiction flash/debug respectées. AGENTS/protocole, journal intégral réel, rapport J et proposition M014 consultés, architecture/contraintes des gels supervision et composants transport/protocole/runtime/configuration/CLI/IHM/tests inspectés.

**Lecture du code** : B1Reader demande20 registres, B0SessionFactory requiert vraie identification/version/DeviceId. Le firmware diagnostic ne fournit normalement que FC03/1000/1 UNKNOWN0 ; runtime production exige d'autres autorités. Audit dans `00_gouvernance/DIAGNOSTIC_SUPERVISION_M014_A.md`. Aucun changement de gels/V1, firmware, lecteur B1/B0, transport partagé ni projet/dépendance de solution.

## Modifications et garanties

`Supervision TR2/src/TR2.Supervision.Service/DiagnosticConsoleApplication.cs` ajouté ; dispatch6 lignes dans SupervisionConsoleApplication avant chargement RuntimeConfiguration/composition. Réutilisation factory NModbusSerialBusConnectionFactory et ModbusSerialConnectionSettings existantes. Aucune composition runtime/Web/SQLite/session, découverte B0, polling/B5 ou écriture dans ce chemin. Chemin `--config` inchangé.

`--diagnostic` OFFLINE défaut sans port/fichier ; lecture seulement par opt-in `--read-once`, port/baud/parité/timeout explicites et nouveau `--log`. Unité1/8bits/stop1/FC03/1000/1 fixes ; inconnus/doublons/mélange production rejetés. Retry0 conservé par politique existante, une connexion/un appel lecture, fermeture sur sortie/erreur, pas reconnexion. Journal CreateNew/exclusif, intention persistée avant ouverture et lecture, résultats UTC PC/registres ou erreur/stade/catégorie/code ; pas capture brute série inventée. Paramètres et attentes hex clairement identifiés.

Succès seulement longueur1/valeur0 ; UNKNOWN0, production non évaluée, DeviceId indisponible. Timeout série borné100..5000ms ; deadline de l'attente lecture4×timeout (4000ms profilHP) puis disposal, sans nouvelle opération. Faute tardive observée. Cancellation ne retire pas une transmission déjà potentielle ; retour diagnostic1 sur cancellation, runtime production inchangé. Borne applicative n'est pas garantie temps réel sur driver open/dispose bloqué.

Tests ajoutés dans `tests/TR2.Supervision.Service.Tests/DiagnosticConsoleApplicationTests.cs` et `tests/TR2.Transport.Tests/NModbusDiagnosticFrameTests.cs`. README/prérequis PC actualisés, audit/procédure HP documentés. Archives mission/rapport J exactes depuis HEAD initial, immuables, sans réécriture des preuves historiques.

## Validation réellement effectuée sur T1000

SDK.NET10.0.401/runtime10.0.12 Debian WSL ; commandes depuis `Modbus RTU/Supervision TR2` :

- `dotnet restore TR2.Supervision.sln` : réussi.
- `dotnet build TR2.Supervision.sln --no-restore` : réussi,0warning/0erreur.
- Tests CLI ciblés initiaux21/21, puis6 cas configuration ajoutés couverts par suite complète ; framing ciblé6/6 puis9/9 après ajout taille/unité/fonction incorrectes.
- `dotnet test TR2.Supervision.sln --no-build --logger trx --results-directory <preuves>/final-full-tests` : **502/502,0échec/0ignoré** ; Architecture1, Domain17, Protocol45, Transport33, Application221, Persistence31, Service154. Première suite499/499 conservée ; répétition finale justifiée par trois nouveaux cas framing.36 nouveaux cas,466 anciens conservés sans affaiblissement.
- CLI OFFLINE réel : `dotnet run --project src/TR2.Supervision.Service --no-build -- --diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000`, retour0, plan exact et OFFLINE ; texte COM7 sans ouverture.
- `dotnet publish src/TR2.Supervision.Service/TR2.Supervision.Service.csproj -c Release -r win-x64 --self-contained true -p:PublishSingleFile=false -p:PublishTrimmed=false -o <preuves>/hp-win-x64` : réussi. Aucun EXE Windows lancé.
- Analyse syntaxique des deux blocs PowerShell préparés par Parser::ParseFile Windows : réussie, pas exécution/installation ni changement ExecutionPolicy.
- Diff/index/périmètre inspectés, diff --check réussi ; source évaluée conservée/recomparée aux octets du commit `bcc69c9d07aa02f756f9e2db0d67f62613d1951e`.

Réponse exacte/NModbus requête010303E80001047A et réponse0103020000B844 vérifiées **sur flux mémoire**, ainsi que CRC erroné, quantité/unit/function incorrectes, troncature/silence et exceptions04/06, chaque fois une seule écriture de requête sur le faux flux. Ce nombre d'écritures mémoire n'est pas une émission RS485. CLI testé avec factory simulée : aucun appel d'ouverture hors activation, un seul read, getter d'écriture interdit, fermeture, erreur port occupé simulée/timeout/exception/size/value, journal existant conservé, cancellation et lecture bloquée bornée. CRC délégué à NModbus testé à sa couche réelle ; traduction des erreurs transport existante conservée/testée.

Aucun échec build/test observé ni test masqué. Deux recherches initiales de chemins candidats inexistants (nom test d'architecture et emplacement prérequis) ont été résolues par inventaire réel, sans création de substitut. Validation .NET complète proportionnée au changement CLI/risquePC ; pas de test C/cross-build/flash/debug nécessaire ni autorisé, firmware inchangé.

## Paquet et HP : préparation seulement

Paquet temporaire autonome win-x64 avec runtime.NET/ASP.NET,370 fichiers applicatifs113708544octets plus manifeste embarqué. ZIP50 086 986octets SHA256 `51e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0`. EXE162816octets SHA256 `13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872`. Artefact local horsGit : `<preuves>/TR2-M014-A-win-x64.zip` ; non téléversé comme binaire GitHub, non transféré auHP. Procédure exacte de copie/vérification/extraction nouveau dossier et reproduction publish dans l'audit. Pas de SDK/runtime système/service Windows/politique/configuration à installer pour ce paquet temporaire ; ce choix ne gèle pas packaging produit/versionOS minimale.

Préflight préparé : identité HP/OSx64, inventaire.NET facultatif car paquet autonome, hashes par fichier, GetPortNames/PnP sans ouverture, lancement OFFLINE seulement **à effectuer sous prochaine mission**. Une commande future de lecture unique est documentée avec NO-GO M014-A, journal nouveau fixé par futur contrat, arrêt sansretry après toute issue. Aucun GO transmission actuel.

**NON VÉRIFIÉ** : accès/installation/lancement WindowsHP, compatibilitéOS réelle, branchSerialPort/NModbus surFT232R/COM7, disponibilitéexclusive/driver/fermeture sous panne/lecturephysique. Inventaire ne prouve pas port libre. Pas accèsHPdirect, DellWindows n'est pas HP ; dock attendu nonbloquant, aucune nouvelle connexion remote supposée. Pas de DeviceId/READY fictif, pas de changement firmware pour contourner B0/B1.

## Preuves, limites et suite

Preuves horsGit : `/home/lolo/dev/msm/reports/TR2-mission014-A-20261011T171855Z` (remplace `<preuves>` ci-dessus) : restore/build/targeted/full/final-full/publish/OFFLINE logs, TRX/test-summary.json, evaluated-source.json, git-stashes.txt, hp-procedure-syntax.json, package-summary.json, manifeste embarqué et ZIP. Preuves/backups/stashes antérieurs conservés, dossier J retrouvé, pas diagnostics/builds physiques M013 répétés.

Journal ETAT_COURANT_TR2 actualisé : voie diagnostique préparée, validationPC réelle, paquet/préflightHPnonexécutés, responsabilitésCRC et réservesdriver/capturebrute/deadline ; prochaine décisionChatGPT = revue indépendante puis contrat séparé HP horsport et éventuelle lecture unique avec état cible/baseline et collecteaprès séparément autorisés. Acquis et campagneI/J restent historiques ; aucun nouvel état santé MCU observé M014-A. RéservesM013-J inchangées : frontsDE/TC/stopbit/t1.5/t3.5/retourRX, robustesse/endurance/reset/watchdogIRQ/J1global/production/E4. Aucun acquis physique redemandé.

Permissions exercées : développement/doc/build/tests horsmatériel, commitlocal et publication normale main prévue ; flash/debugfalse, aucune émission/COM7/MCU/USB/OptionBytes/effacement/configurationpermanente. Commit code/audit `bcc69c9d07aa02f756f9e2db0d67f62613d1951e`, message `Add isolated offline-first M014-A .NET diagnostic probe and HP preparation`. Rapport/journal/archives complètent cette tranche validée. Vérification distante à consigner après push ; statut provisoire n'indique pas un blocage logiciel/HP à résoudre avant publication. SHA final et étatGit après publication seront donnés dans la réponse Codex.
