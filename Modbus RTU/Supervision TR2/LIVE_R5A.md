# R5-A — lecture LIVE, observations PC et SQLite/Web

Mission `TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB`, HEAD initial `8f16632`, référence contractuelle `d0a37ef`. Cette tranche est exclusivement logicielle PC : aucun accès COM4, flash/debug, reset, FRAM ou SD. Le dernier état matériel rapporté reste M016 restauré par R4-V ; il n'a pas été relu ici.

## Obstacle démontré sur les captures physiques

Le mapping normatif LIVE hérite des adresses B3 : **3002 = ALARM_FLAGS ; 3003 = SEVERITY_GLOBAL**. Le projecteur STM32 `src/application/live_metrology.c`, lignes 203–204 à la référence initiale, écrit la sévérité au registre 2 et les alarmes au registre 3. Ce code est identique à celui réellement exécuté en R4-V, `8db2f67`.

Les captures authentiques, vérifiées par CRC, confirment l'inversion. Premier résultat du candidat R4-V à `10:36:09.3891128 UTC` : 3002=3, 3003=240, statut=4, flags=1215, séquence=1, RMS global=958 mg. Le PC normatif lit donc une sévérité réservée de 240 et doit refuser cette observation comme mesure qualifiée.

| Capture publique immuable | ADU totales | B3 | Sévérité réservée 240 | B3 sans valeur valide |
|---|---:|---:|---:|---:|
| R4-U live05 | 1632 | 828 | 780 | 48 |
| R4-V baseline | 1655 | 840 | 790 | 50 |
| R4-V paced | 1075 | 549 | 498 | 51 |

Les **2068 B3 concernés sont refusés et conservés en quarantaine**. Les 149 autres décrivent des états sans mesure valide. RMS, crêtes et compteurs restent lisibles structurellement, mais l'intégralité B3 n'est pas conforme. Les résultats FIFO, latence et FRAM antérieurs ne sont ni rejoués ni effacés ; leur portée ne doit pas être étendue à une conformité de tous les champs B3.

Aucun échange automatique des mots côté PC, changement du mapping gelé ou correctif firmware hors périmètre n'est appliqué. Le défaut localisé n'appelle pas un changement de norme. Sa correction exige une mission autorisant le projecteur STM32, un oracle indépendant des offsets et une nouvelle capture réelle alarmes/sévérité/HELD. **R5-A reste BLOCKED sur ce critère**, malgré la tranche PC sûre livrée ci-dessous.

## Audit du socle et intégration additive

| Couche existante | Écart initial | Adaptation |
|---|---|---|
| Transport NModbus / IRegisterTransport | Interfaces lecture et écriture disponibles | LIVE dépend uniquement de l'interface lecture. Décodeur ADU hors ligne avec contrôles FC03, CRC, unité et longueur ; transport physique inchangé. |
| B0Reader / B0SessionFactory | Égalité stricte protocole 1 | Conservés. LiveProfileReader distingue exactement 1, 2 et les versions inconnues. |
| B3VibrationSupervision | Décodage hérité sans validation LIVE | Réutilisé pour les champs et MSW/LSW ; validation LIVE supplémentaire des 48 mots, réservés, états, flags, sévérité et UTC. |
| Fleet / PollingExecutor / commandes V1 | Le polling FAST lit B1/B3/B5 ; commandes sans epoch | Conservés. LiveObservationService est une composition explicite séparée du scheduler V1, sans commande ni B5. |
| Domaine d'observation | Profil, qualité LIVE et session absents | Observation avec source, bus/adresse, identité, versions/capacités, réception PC, segment et continuité. Contexte absent explicitement inconnu. |
| SQLite 6 / b3_archive | Historique sans métadonnées LIVE | Migration 7 ajoutant seulement live_observation et son index ; aucune rétroattribution aux anciennes lignes. |
| API/Web S7 | Contrats V1 et pages existantes | Source LIVE optionnelle, GET /api/v2/live et page intégrée /live.html. Source absente : 404. |
| Service existant | Production V1 par défaut | Mode explicite --live-replay réutilisant Application, Persistence et le host S7, sans usine de connexion série. |

## Sélection stricte et limites de provenance

| Cible | Décision du lecteur |
|---|---|
| Protocole 1 | LegacyV1, B0 seulement ; le décodeur historique reste applicable, aucun support epoch déduit. |
| Version inconnue, longueur incorrecte ou réservés B0 | Refus avant lecture LIVE. |
| Protocole 2 sans bit LIVE16 | Refus par défaut ; opt-in explicite LiveDiagnostic, signalé comme partiel. |
| Protocole 2 / capacités 0x1D ou 0x1F | Profil annoncé LIVE, sans qualification produit implicite. |
| Bit LIVE16 sans les capacités requises ou structures incompatibles | Refus, sans acceptation silencieuse. |

Après B0/21, le lecteur vérifie B4/2 (structure 2, capacité LIVE et réservés), B6/1 (structure 2), et B7/1 (structure 1) seulement pour une cible annonçant LIVE complet. Il lit ensuite B3/48. Le diagnostic partiel ne lit ni B1/B2/B7 ni B5 et n'écrit jamais B4/B5.

Les types, l'ordre MSW/LSW et les unités hérités sont conservés : statut/validité/alarmes/sévérité, UTC, âge et durée en ms, séquence, nombre d'échantillons, RMS/crêtes globales et XYZ en mg, axe dominant, dépassements, latch et compteurs. Les huit mots réservés doivent être nuls. FRESH avec HELD, arrêt ou V=0 est refusé, ainsi qu'une sévérité incohérente.

UTC absente impose timestamp=0 et WindowUtc=null ; le PC ne remplace jamais ce temps par son heure de réception. Une UTC présente est la déclaration B3 qualifiée par son flag, **pas une vérification indépendante par B2**. Temps événement monotone, run, epoch, identité et génération de configuration ne sont pas exposés par B3 : ils restent null. Les headers B4 ne prouvent pas la configuration de la fenêtre ; aucun campaign_id n'est créé.

Le lecteur n'est pas raccordé au scheduler série de production dans R5-A. La composition avec un maître COM4 unique relève de R5-B, après correction du firmware et identification réelle des capacités.

## Continuité, persistance et présentation

Chaque observation reçoit un identifiant PC distinct et est conservée : aucune déduplication par séquence seule. Une répétition est RepeatedSequence, un saut Gap, le plafond FFFFFFFF IndeterminateSaturation. Un recul devient RegressedSequence et ouvre un segment : reboot possible, jamais prouvé par le seul compteur. Reconnexion, redémarrage du service ou changement d'identité ouvrent aussi un segment PC distinct d'un boot STM32.

Une perte de transport est CommunicationLost ; une trame non conforme est Rejected, avec snapshot qualifié absent et registres/décodage normatif conservés en quarantaine. Une identité non lue dans ce cycle reste inconnue, tandis que bus/adresse sont conservés. Une ancienne valeur n'est jamais réintroduite comme nouvelle mesure valide.

La migration 7 est additive et transactionnelle. Colonnes indexables et payload JSON conservent toutes les métadonnées et valeurs inconnues. Les tables historiques, le journal B5 et leurs lignes ne changent pas. Un échec SQLite apparaît comme Failed avec son message ; le cycle suivant peut continuer. Aucun rattrapage automatique ni exactly-once n'est promis, notamment si le processus s'interrompt après commit avant son retour.

La projection est protégée contre les lectures concurrentes. Une réception dépassant le délai PC configurable (5 s par défaut), ou une horloge PC reculée, retire IsCurrent. Cette politique est distincte de la fraîcheur STM32 ; le flag reçu n'est pas une extrapolation future et la période autoritative de la fenêtre n'est pas connue ici. HELD est présenté comme valeur retenue non courante. L'UI expose source, qualité, UTC absente, continuité et état d'archivage ; tous les textes reçus passent par textContent.

## Validations réellement exécutées

Depuis `Modbus RTU/Supervision TR2` :

```bash
dotnet restore TR2.Supervision.sln
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
# Après la dernière correction du chemin des fichiers statiques :
dotnet test tests/TR2.Supervision.Service.Tests/TR2.Supervision.Service.Tests.csproj --no-restore
node tests/live-ui.test.cjs src/TR2.Supervision.Web/wwwroot/live.js
```

SDK 10.0.401 / runtime 10.0.12, Debian WSL2. Build sans avertissement ni erreur. **551 tests complets PASS**, dont les 539 préexistants ; après la dernière modification Web et fixture UTC, **201 tests Service PASS**. Sept scénarios JS sous DOM simulé passent ; aucune qualification par navigateur graphique n'est revendiquée. Les nouveaux tests couvrent captures réelles, CRC/longueur/endianess, profils, qualité, migration depuis 6, SQLite corrompue/indisponible, séquences, reconnect/restart, non-écriture et contrats API/S7.

Échecs conservés : le premier chargement hex ne gérait pas les espaces du captureur, puis trois tests ont révélé l'inversion réelle. L'attendu n'est pas devenu « 240 valide » : les oracles prouvent désormais le refus des 2068 trames, avec des fixtures normatives indépendantes. Le premier démarrage HTTP a renvoyé 404 : wwwroot dépendait du cwd. Correction ciblée de ContentRoot/WebRoot vers AppContext.BaseDirectory ; les fichiers étaient déjà copiés au build. Les anciens tests Service passent.

La seconde fumée HTTP réelle renvoie 200 sur /, /live.html, /live.js, /api/v1/fleet et /api/v2/live. SQLite contient 52 observations dont une quarantainée, indexée V/F=0. Le rejet de la sévérité 240 est visible via API. Sortie du service 0 ; aucun port COM ouvert. Aucun build/test STM32 inchangé n'a été répété.

## Reproduction hors ligne et reprise

Décompresser une capture publique R4-V dans un fichier JSONL externe, puis :

```bash
dotnet run --project src/TR2.Supervision.Service --no-build -- \
  --live-replay /chemin/externe/paced-master.jsonl \
  --database /chemin/externe/base-test-neuve.db --listen http://127.0.0.1:5087
```

Ouvrir /live.html. Le replay accéléré (100 ms par observation) utilise l'heure PC de relecture ; ce n'est pas l'heure de l'acquisition d'origine. B0/B3 proviennent des octets authentiques contrôlés par CRC. Les headers B4/B6 absents de ces ADU sont **simulés**, explicitement étiquetés ; aucun registre de matériel actuellement connecté n'est présumé. À épuisement de la capture, le transport signale une perte, sans fabriquer de nouvelles mesures.

La reprise exige un contrat permettant la correction ciblée du projecteur firmware et la démonstration des offsets sur le fil, sans toucher aux barrières B5 ni perdre les identités FRAM. Après cela : composition série unique R5-B, B1/B2/B7 et capacités complètes, puis R6 bout en bout. Aucun GO industriel ni modification normative n'est donné par cette livraison PC.
