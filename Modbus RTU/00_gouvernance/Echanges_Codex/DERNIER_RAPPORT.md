---
mission_id: TR2-20261011-M014-D-SUPERVISION-INTEGRATION
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 6b5df7050cbf63e6c1daf0ec793455a63f8ec81a
mission_sha: 364ce73ddcb3f961a08568fa02ffbb4ba43104a5
result_sha: ccc20526f478ab47c097528a0af33d760f33f22b
created_at_utc: 2026-10-11T18:36:51Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-D — tranches PC validées, production physique bloquée

**Logiciel PC livré et testé : contrôle borné B0/B1 et gestion correcte de perte/
reprise de bus ; 524/524 tests.NET,110/110 tests firmware et cross-build réussis.**
BLOCKED pour la composition de production réelle et les lectures périodiques/
reprise physiques. **M014 n'est pas DONE.** Aucun quota épuisé annoncé : arrêt sur
véritable dépendance structurante après progression des travaux indépendants,
pas par économie de validation. Aucun nouvel essai HP/COM7/RS485, flash/reset/debug.
Diagnostic qualifié E conservé, commandes C et I consommées, ne pas relancer.

## Référence, périmètre et provenance

Main initial6b5df70 propre, stashes historiques005/002 conservés. Fetch/pull
--ff-only sûr vers364ce73, seul contrat changé ; AGENTS/protocole inchangés.
Mission/version/permissions réelles lues, journal intégral consulté, roadmap,
rapport C DONE, gels pertinents S4/S5, sources/tests production/binding/runtime/
blocs/NModbus/polling/recovery inspectés. Base_sha absent : null, aucune valeur
inventée. Archive C MISSION/RAPPORT exacte depuis HEAD initial, immuable ; mission
active ChatGPT inchangée. Tranches cohérentes, pas de refactoring opportuniste.

Code logiciel évalué et paquet D : ccc20526f478ab47c097528a0af33d760f33f22b,
commit `Add bounded B0 B1 system check and preserve unavailable telemetry across bus recovery`.
Dernière extension d'assertions de reprise fraîche validée séparément3/3 sera
committée avec ce rapport ; code applicatif/paquet reste celui deccc2052.

M014-C acquis : utilisateur UNKNOWN0/code0/passedtrue le11/10/2026 à18:15:29UTC,
deltasMCU +8octets/+1trameCRC/+1acceptation/+1TXTC sans erreur. Journal HP intégral
non fourni : son SHA annoncéAFDAB5517A13ABED1E9735A4240F0E6B53C5DC110F9CB717BB50410F698A2272
ne prouve pas contenu/chronologie/hash recalculé. Réserve maintenue, aucune nouvelle
observation MCU en D ; santé C reste historique explicitement identifiée.

## Audit et matrice livrée

[INTEGRATION_SUPERVISION_M014_D.md](../INTEGRATION_SUPERVISION_M014_D.md) contient
matrice développé/simulé/compilé/physique, dépendances, critères et reprise détaillée.
Fait de lecture du code : image diagnostic E publie normalement FC03/1000/1 seulement,
B0/B1complet indisponibles→04. ModbusSystemServer sait lire21/20 registres depuis
vrai runtime, ProductionApplication possède services/dispatcher, mais binding faible
STM32 refuse NOT_AVAILABLE avant préparation. FRAM transactionnelle exige recoveryVALID,
EMPTY refusé ; CampaignDataStoreBulk/SDMMC existent mais composition carte durable/
allocation/provisioning/identité ne sont pas fournis par le port faible.

S0–S7 et polling sont développés avec tests host ; FAST litB1/B3/B5, MEDIUM B2/B7,
SLOW B4/B6, découverte B0. Deux blocs ne suffisent pas à lancer toute cette boucle.
Le fichier roadmap reste une référence initiale, pas une preuve opérationnelle
courante ni un gel silencieusement modifié.

## Modifications logicielles

- `SerialBusRecoveryCoordinator` : avant oublier sessions et fermer driver, marque
  indisponibles tous les appareils identifiés du bus. Valeurs/timestamps conservés,
  même si fermeture échoue. Reconnexion ne rafraîchit pas les données.
- `PhysicalPollingWorkRunner` : notification compatible seulement pour B0 réellement
  Compatible ; une version incompatible n'émet plus ce signal.
- `SystemBlockConsoleApplication` ajouté et dispatch `--system-check` avant runtime.
  OFFLINE défaut sans ouverture/journal ; paramètres validés via parseur diagnostic
  réutilisé (visibilité internal seulement, comportement C préservé). Version supportée
  explicitement fournie ; FC03/0/21 puis1000/20 seulement après B0 compatible.
  Taille exacte, identité réelle lue, UNKNOWN/autre état décodé sans READY fabriqué.
  Une connexion, maximum2requêtes, retry0, aucun polling/écriture/B5/reconnexion.
  Journal CreateNew/exclusif/intention durable avant I/O, UTC/registers/result/failure,
  code0 seulement après fermeture. Deadline d'attente4×timeout par lecture ; driver
  open/dispose non garanti temps réel, cancellation n'annule pas une émission passée.
- Tests service ajoutés/renforcés : activation/OFFLINE/exclusivité journal/cancellation,
  compatibilité/taille/timeout/exception/I/O/fermeture, perte bus2appareils, fermeture
  échouée, reprise même/autre identité/version incompatible et nouvelle valeur fraîche.
  Tests Transport vrai NModbus flux mémoire : requêtes exactes0/21 et1000/20,
  CRC B1 erroné refusé sans retry. Ce sont des écritures mémoire, pas RS485.
- Test firmware `test_modbus_system_server.c` renforcé : serveurRTU/runtimehôte réel,
  B0/B1 complets répétés, CRC/données/actualisation, sourceB1retirée→04 puis retour.
  Identité12345678/services sont fixtures hôte explicites, jamais provisioning carte.
- README et document intégration/HP/reprise ajoutés, journal actualisé, archives C.
  Aucun source firmware production, mappingV1, gel, configuration permanente changé.

## Validations réellement exécutées et justification

Preuves horsGit `/home/lolo/dev/msm/reports/TR2-mission014-D-20261011T182756Z`.

- Ciblage initial recovery/workrunner : `dotnet test ...Service.Tests.csproj --filter
  'FullyQualifiedName~PhysicalPollingFailureRecoveryTests|FullyQualifiedName~PhysicalPollingWorkRunnerTests'`
  +loggerTRX :7/7 réussis, mocks/runtimePC/SQLite, zéro matériel.
- `dotnet build 'Modbus RTU/Supervision TR2/TR2.Supervision.sln' --no-restore` :
  réussi,0warning/0erreur.
- Ciblage SystemBlockConsoleApplicationTests :16/16 réussis, dont attente bloquée
  bornée400ms sur timeout100, pas simulation déclarée physique.
- `dotnet test 'Modbus RTU/Supervision TR2/TR2.Supervision.sln' --logger trx
  --results-directory <preuves>/full-tests` : **524/524,0échec/ignoré**. Suites :
  Architecture1,Domain17,Protocol45,Transport35,Application221,Persistence31,Service174.
  TotalTRX recalculé ;22cas supplémentaires par rapport A502, tests non affaiblis.
- Après extension des seules assertions reprise (B0 seul indisponible puis vraie
  lecture fraîche, autre identité ne récupère pas ancienne donnée), ciblage
  PhysicalPollingReconnectRecoveryTests : **3/3 réussis**. Suite complète pas
  répétée inutilement : code production inchangé depuis la suite524, cas concernés
  revalidés sur dernière version de test. Aucune donnée attendue artificiellement changée.
- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` :
  **110/110 hôte et cross-build réussis**, firmware par défaut compilé seulement.
  Validation complète choisie pour preuve integrationserveur partagée B0/B1 ;
  avertissements newlib `_close/_lseek/_read/_write` préexistants conservés.
  Pas compilation assimilée à exécution cible, BIN défaut non flashé.
- CLI Linux réel `dotnet run --project ...Service --no-build -- --system-check
  --protocol-version 1 --port COM7 --baud 115200 --parity Even --timeout-ms 1000` :
  retour0/OFFLINE, plan2lectures, zéro ouverture/transmission/journal. COM7 est du texte,
  pas un accès Dell au port HP.
- `dotnet publish ...Service.csproj -c Release -r win-x64 --self-contained true
  -p:PublishSingleFile=false -p:PublishTrimmed=false -o <preuves>/hp-win-x64` : réussi.
  ZIP/manifeste créé, CRCarchive et370hashes/octets ZIP/disque vérifiés ; sources commitccc2052.
  Snippet PowerShell OFFLINE préparé analysé Parser::ParseFile sur Dell : syntaxeOK,
  snippet non exécuté.
- Diff/whitespace/index/périmètre contrôlés ; pas secret ni logs volumineux dansGit.
  Recherches candidates de chemins absents résolues par rg, aucun fichier inventé ;
  aucun échec build/test observé/masqué, aucun test supprimé/désactivé/affaibli.

## Paquet et essais HP : prêts logiciel, NO-GO physique

ZIP `TR2-M014-D-win-x64.zip`50094222octets,
SHA256 `ecd7fa606a16258afcb087b388fd7a35382a5bccad8e0b34c9f87aff25a35fc2`.
EXE SHA256 `9b5d247be3f86ef606352f3f8a9faa74cacd5cae9a39cb314f6e93edf62bf6d6` ;
manifesteD SHA256 `a3d60750443517ab3238de3c759ef3f04c6dee322ee2d96bb9e21c20dc72577c`.
Publish autonome .NET/ASP.NET, pas nouveau SDK système requis. Artefact local horsGit,
non transféré/exécuté HP ni hébergé comme binaire GitHub. Paquet A intact.

Document D prépare OFFLINE nouveau paquet après revue ChatGPT et contrôles hashes/
PnP/exclusivité ; contrôle physique futur maximum2lectures seulement après vraie
cibleB0/B1. Pas de commande émission prête à coller avant GO, pas marqueur C réutilisé.
Plan périodique/perte/reprise doit fixer durée/nombre/groupes/seuils/arrêt avant
intervention HP ; loop --config existant est continu, pas campagne bornée prétendue.
Aucun GDB pendant trafic, utilisateur seul exécutant physique, pas accès distant HP.

## Véritable blocage, limites et reprise

L'activation de production exige source/politique d'identité stable/unique/persistante,
adresse/version, géométrieFRAM/imageVALID/provisioning, backendSD/allocation durable,
metadata configuration et séparation des écritures du harness. Ces décisions ne
sont pas documentées/provisionnées pour cette carte. AGENTS §8 : décision structurante
non prévue à escalader ; §9 : allow_flash n'autorise pas format/écriture arbitraire
non volatile. Ne pas combler avec DeviceId42, UID réduit, READY ou backendFRAM historique.
Les autres travaux indépendants ci-dessus ont été réalisés malgré ce blocage.

Aucun nouveau flash : pas image production sûre à déployer ; préserver le diagnostic
qualifié plutôt que flasher le harness par défaut. BIN E49200/hashdc6ac75f… et
ELF4c496149… rehashés sur disque, readback C identique conservé, sauvegardes antérieures
intactes (`preserved-diagnostic.json`). Aucune nécessité de nouveau GDB/reset pour
cet audit logiciel ; l'état cible récent C est une observation historique, pas D.

Point de reprise concret dans document D : code/paquet/testspreuves conservés,
autorités carte à décider/fournir puis tests négatifs EMPTY/médias/identité,
composition exclusive, sauvegarde/restauration contrôlée et flash standard qualifié ;
après seulement, revueChatGPT et essaiHPB0/B1borné, puis polling/perte/reprise cadrés.
Aucun code inachevé volontairement laissé ; pas de travail utilisateur incorporé.
Les prochains commits de composition doivent rester sous contrat réel/AGENTS,
pas une autorisation matérielle héritée de C.

M014/J2 opérationnel, production, qualificationtimingDE/TC/stopbit/t1.5/t3.5/retourRX,
robustesse/endurance/reset/watchdog/J1global/E4 demeurent ouverts. Le blocage n'infirme
pas l'acquis diagnosticC, ne qualifie pas un défaut électrique nouveau. Aucun acquis
physique redemandé. Rapport/journal prêts pour revue indépendante ChatGPT et décisions.

## Git et publication

Permissions exercées : développement/tests/build/publish/commits/push normalmain.
Flash/debugtrue non exercés ; zéro nouveau COM7/RS485/flash/reset/GDB/HOTPLUG/
OptionBytes/masserase/USB/média/câblage/service distant/configuration Codex/Windows.
Deux stashes, backups et preuves historiques préservés. Sources ccc2052 et complément
validé seront poussés normalement avec ce rapport BLOCKED et journal/reprise,
puis relus intégralement depuis référenceGitHub. SHA final après contrôle distant
fourni dans réponse Codex ; BLOCKED ne deviendra pas DONE sans preuve opérationnelle
physique et décisions de composition levées.
