# M014-A — diagnostic .NET isolé, préparation hors port

11 octobre 2026. **VALIDÉ logiciel sur T1000 ; PRÉPARÉ pour HP ; NON TESTÉ sur HP.**
Aucune transmission ni ouverture COM7 autorisée dans cette mission. La campagne
M013-I est consommée et ne doit jamais être relancée.

## Audit et frontière conservée

| Composant réel | Constat / décision M014-A |
|---|---|
| `TR2.Transport/NModbusSerialBusConnectionFactory.cs` | Factory existante réutilisée sans modification, paramètres explicites `ModbusSerialConnectionSettings`, SerialPort et NModbus 3.0.83. |
| `NModbusSafetyPolicy.cs`, `NModbusRegisterClient.cs` | Retries=0, attente retry=0, SlaveBusyUsesRetryCount=true ; politique existante conservée. CRC/framing délégués à NModbus. |
| `TR2.Protocol/B0SessionFactory.cs` | Identification B0 et version compatibles requises pour une session production ; cible diagnostic sans DeviceId exploitable. Aucune session construite ici. |
| `B1Reader.cs`, `B1SystemState.cs` | Lecture normale B1 de 20 registres ; firmware diagnostic ne fournit normalement que FC03/1000/1 UNKNOWN=0. Lecteur standard inchangé. |
| `PhysicalSupervisionRuntimeFactory.cs` | Compose polling, priorité B5 et host ; entièrement contourné par la branche diagnostic avant chargement de configuration. |
| `RuntimeConfiguration.cs`, `SupervisionConsoleApplication.cs` | `--config` reste le chemin production avec SQLite, bus, endpoints, politiques et Web optionnel. Aucune configuration production acceptée par la branche diagnostic. |
| `TR2.Supervision.Web` et gel S7 | IHM via projections/runtime unique, aucun accès direct au port. Aucun endpoint HTTP ni nouvelle autorité IHM ajouté. |
| Solution et tests existants | Aucun projet/dépendance ajouté, gels S0–S7 et V1 inchangés ; tests d'architecture, runtime, B5 et Web conservés. |

Choix minimal de **SUPERVISION_POLICY de banc** : ajout de
`DiagnosticConsoleApplication` dans le composition root existant et dispatch
CLI avant toute composition production. Une deuxième pile RTU ou une fausse
identification ne sont pas nécessaires. Ce chemin sert une cible diagnostic
isolée ; il n'intègre pas celle-ci à la flotte de production.

## Interface et garanties testées

`--diagnostic` seul retourne OFFLINE, sans ouvrir de port ni créer de fichier.
Il peut aussi recevoir le plan série explicite ci-dessous, toujours hors port.
L'exécutable sans argument conserve le rejet d'usage existant, sans trafic.
Seul `--diagnostic --read-once` avec tous les paramètres et un nouveau journal
autorise techniquement le chemin série ; l'autorisation opérationnelle doit
provenir d'une mission physique suivante, pas du flag CLI.

Paramètres configurables : `--port`, `--baud` (1..1000000), `--parity`
(`None|Odd|Even|Mark|Space`, nom exact), `--timeout-ms` (100..5000).
8 bits et un stop bit, unité1, FC03/1000/quantité1 restent fixes pour cette tranche.
Options inconnues, doublons, mélanges `--config`/diagnostic, valeur numérique
de parité, activation sans journal et configuration incomplète sont rejetés
avant ouverture.

Le journal JSONL est créé avec CreateNew et FileShare.None, intention persistée
avant ouverture puis avant lecture. Un fichier existant empêche toute ouverture.
Une seule connexion et un seul appel de lecture, aucune interface d'écriture,
boucle de polling, découverte B0, B5, reconnexion ou retry. Disposal à la sortie,
y compris après erreur. L'ouverture exclusive réelle dépend du driver Windows ;
son comportement et les autres maîtres physiques restent à vérifier sur HP.

Timeout série de lecture/écriture explicite ; deadline de l'attente applicative
de lecture à quatre fois ce timeout (4000 ms pour 1000 ms). Timeout/cancellation
provoquent la fermeture de la ressource, observent une éventuelle faute tardive
et ne lancent aucune nouvelle opération. Ce mécanisme couvre notamment une tâche
de lecture bloquée ; il ne constitue pas une garantie temps réel sur la durée
d'ouverture/fermeture d'un driver défaillant. Une cancellation ne retire pas une
requête qui pourrait déjà être émise. Ne pas relancer après une erreur.

Succès seulement pour un tableau de longueur1 contenant0. Sortie **UNKNOWN(0),
production non évaluée, DeviceId indisponible** ; jamais READY ni qualification
physique. Codes : 0 succès diagnostic/hors port, 1 erreur ou cancellation
diagnostic, 2 usage/configuration. La cancellation production reste inchangée.

Le journal contient paramètres, timestamps UTC PC, intention, registres décodés,
résultat ou erreur/stade/catégorie/code Modbus. `expectedRequestHex` et
`expectedResponseHex` sont des attentes, **pas une capture brute**. La frontière
transport existante fournit des registres, pas les chunks série ; pas de trace
wire inventée. Le CRC normal/incorrect et les exceptions sont testés directement
avec NModbus sur `IStreamResource` mémoire, sans port ni MCU.

## Validation T1000 réellement obtenue

SDK .NET10.0.401, runtime10.0.12 Debian WSL. Restore/build de la solution réussis,
zéro avertissement/erreur. Suite finale **502/502**, dont27 nouveaux tests CLI
et9 nouveaux tests framing NModbus. Tests : absence d'ouverture sans activation,
rejet d'options, paramètres exacts, lecture unique et non-écriture, UNKNOWN,
taille/valeur incorrectes, CRC erroné, unité/fonction incorrectes, troncature,
exceptions04/06 sans retransmission, timeout, port occupé simulé, journal préexistant,
cancellation et tâche bloquée avec fermeture. Aucun test ancien retiré/affaibli.

OFFLINE CLI réel exécuté sous Linux : COM7 est un texte de plan, aucun port ouvert.
Publication `win-x64` Release autonome réussie ; aucun exécutable Windows lancé.
Pas de validation firmware/cross-build nécessaire : firmware, interfaces
partagées et transport de production inchangés, risque PC vérifié par la suite
.NET complète.

Preuves : `/home/lolo/dev/msm/reports/TR2-mission014-A-20261011T171855Z/` :
logs restore/build/tests/publish/OFFLINE, TRX, package-summary et manifeste.
Le paquet et la procédure HP ci-dessous restent une préparation, pas une installation
ni une qualification Windows/.NET/driver physique.

## Installation HP préparée, à effectuer sous mission suivante

HP Laptop15-db1xxx = seul maître ; Dell Latitude7320/T1000 = développement.
Le dock Dell à venir n'est pas nécessaire. Aucune installation HP effectuée ici.

Paquet temporaire autonome **win-x64**, runtime .NET/ASP.NET embarqué,
sans installation SDK/runtime système, service Windows, registre ou politique
PowerShell. Ce choix est limité au diagnostic M014 ; packaging produit/version
Windows minimale restent à qualifier séparément.

- T1000 : `/home/lolo/dev/msm/reports/TR2-mission014-A-20261011T171855Z/TR2-M014-A-win-x64.zip`.
- Accessible depuis Windows Dell : `\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-mission014-A-20261011T171855Z\TR2-M014-A-win-x64.zip`.
- ZIP50086986 octets, SHA256 `51e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0`.
- 370 fichiers applicatifs,113708544 octets décompressés, plus manifeste JSON.
- EXE162816 octets SHA256 `13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872`.

Préparation de transfert : copier le ZIP sur HP par le moyen logiciel disponible,
vérifier ce hash et extraire dans un **nouveau dossier local** sans écraser une
installation existante, par exemple `$env:LOCALAPPDATA\TR2\M014-A`. Ne pas prendre
la présence du ZIP local Dell pour une publication binaire GitHub ou un transfert
HP déjà effectué. Le manifeste embarqué permet de vérifier chaque fichier avant
exécution. L'ensemble des DLL/runtime doit accompagner l'EXE.

Reproduction du paquet depuis le code publié, sur T1000 :

```bash
cd '/home/lolo/dev/msm/projects/MSM_vibe/Modbus RTU/Supervision TR2'
dotnet publish src/TR2.Supervision.Service/TR2.Supervision.Service.csproj \
  -c Release -r win-x64 --self-contained true \
  -p:PublishSingleFile=false -p:PublishTrimmed=false -o '<nouveau-dossier>'
```

## Préflight HP non intrusif préparé (non exécuté)

Depuis le nouveau dossier extrait, bloc PowerShell interactif ; aucun `.ps1`
ni changement/bypass d'ExecutionPolicy. Inventaire seulement, **pas d'ouverture
COM7 pour en tester l'occupation** : celle-ci reste NON VÉRIFIÉE.

```powershell
$ErrorActionPreference = 'Stop'
Get-CimInstance Win32_ComputerSystem | Select-Object Manufacturer, Model
[Environment]::OSVersion.VersionString
if (-not [Environment]::Is64BitOperatingSystem) { throw 'Windows x64 requis pour ce paquet' }
$dotnet = Get-Command dotnet -ErrorAction SilentlyContinue
if ($dotnet) { & $dotnet.Source --list-runtimes } else { 'dotnet systeme absent : paquet autonome' }
$manifest = Get-Content -Raw '.\M014-A-package-manifest.json' | ConvertFrom-Json
foreach ($file in $manifest.files) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $file.path).Hash.ToLowerInvariant() -ne $file.sha256) {
        throw ('Hash incorrect : ' + $file.path)
    }
}
[System.IO.Ports.SerialPort]::GetPortNames()
Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match 'COM7|FT232' } | Select-Object Name, Status
& '.\TR2.Supervision.Service.exe' --diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000
if ($LASTEXITCODE -ne 0) { throw 'Preflight OFFLINE echoue' }
```

Conserver sortie/hashes, confirmer le modèle HP et le convertisseur attendu.
Ce préflight ne prouve ni disponibilité exclusive, ni santé MCU actuelle,
ni voie série .NET fonctionnelle. Aucun geste électrique acquis à répéter.
Si le paquet ne démarre pas sur HP, conserver l'erreur et diagnostiquer le logiciel
sans transmission ; ne pas choisir un autre COM ni démarrer `--config`.

## Lecture future unique, NO-GO dans M014-A

Uniquement après revue ChatGPT et **nouveau contrat** définissant état cible sain,
baseline, seul maître HP, port/configuration et maximum **une requête**. La capture
après sera séparément autorisée ; aucun debugger pendant l'échange, pas de CLI
HOTPLUG/reset/flash. Commande préparée, **jamais exécutée dans M014-A** :

```powershell
& '.\TR2.Supervision.Service.exe' --diagnostic --read-once --port COM7 --baud 115200 --parity Even --timeout-ms 1000 --log '.\future-mission-read-once.jsonl'
```

La prochaine mission fixera le nom unique du journal avant lancement ; ne pas
effacer/renommer un journal pour répéter. Stop et transmettre code de retour,
sortie et JSONL même si erreur/timeout ; aucun retry, polling ou campagne I.
Attente : requête `010303E80001047A`, réponse `0103020000B844`, registre0.
NModbus n'expose pas la trame brute au CLI : prévoir une preuve brute distincte
si la prochaine mission la requiert.

Restent NON VÉRIFIÉS : installation/exécution HP, compatibilité OS, branche série
.NET réelle, driver/ouverture exclusive/fermeture sous panne et lecture physique
unique. Les réserves M013-J demeurent : qualification temporelle DE/TC/stopbit/
retourRX, endurance/reset/watchdogIRQ, production/J1global/E4. UNKNOWN ne les lève pas.
