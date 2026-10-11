# M014-B — transfert du paquet et préflight HP OFFLINE

11 octobre 2026. **Paquet vérifié sur T1000 ; transfert et préflight HP préparés,
non effectués par Codex.** Cette procédure ne transmet aucune requête Modbus.
La lecture physique future M014-C nécessite une mission distincte après revue
ChatGPT. La campagne M013-I est consommée, aucun nouveau lancement.

## Paquet existant conservé

Le paquet M014-A est intact : ZIP vérifié, les370 fichiers du manifeste sont
identiques dans le ZIP et le dossier de publication, sans fichier supplémentaire
sauf le manifeste. EXE PE x64, runtimes .NET et ASP.NET10.0.12 embarqués.
Aucun rebuild n'est nécessaire, aucun SDK/runtime système à installer sur HP.

| Élément | Valeur vérifiée |
|---|---|
| Code du paquet | `bcc69c9d07aa02f756f9e2db0d67f62613d1951e` |
| ZIP | `TR2-M014-A-win-x64.zip`,50086986 octets |
| SHA256 ZIP | `51e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0` |
| Fichiers applicatifs | 370,113708544 octets décompressés |
| SHA256 manifeste embarqué | `eeaf70a35f81a9d9a5102cef011dc54bb4e9eae2b774e75af75689b1dc6db7a3` |
| SHA256 EXE | `13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872` |

Source Linux T1000 :
`/home/lolo/dev/msm/reports/TR2-mission014-A-20261011T171855Z/TR2-M014-A-win-x64.zip`.
Source accessible dans l'Explorateur **du Dell**, via son WSL local :

```text
\\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-mission014-A-20261011T171855Z\TR2-M014-A-win-x64.zip
```

Ce chemin ne désigne pas un partage accessible sur HP. Le ZIP n'est pas hébergé
sur GitHub et n'a pas été copié sur HP. La procédure publiée sur GitHub et le
paquet binaire local sont deux livrables distincts.

## Transfert manuel vérifiable

L'utilisateur réalise le transfert de fichier. Méthode simple : si un support
amovible utilisable sur les deux PC est déjà disponible, copier **le ZIP entier**
depuis le chemin Dell ci-dessus sur ce support, puis dans un dossier local HP,
par exemple Téléchargements. Copier, sans déplacer/supprimer l'original.
La présence d'un support, d'un port compatible ou d'une lettre de lecteur n'est
pas supposée ; aucun achat, nouveau dock ou montage du banc n'est demandé.

Si aucun support compatible n'est disponible, transférer le même fichier avec
un canal de fichiers déjà disponible et choisi par l'utilisateur ; aucune
connexion réseau, service cloud, compte ou accès distant n'est configuré ici.
Le contrôle SHA256 ci-dessous est identique quel que soit le canal. En l'absence
de moyen de transfert disponible, seule cette action utilisateur reste en attente ;
ne pas essayer de télécharger le ZIP depuis une URL GitHub inexistante.

Ne pas copier seulement l'EXE. Ne pas modifier le câblage ou les réglages du TR2
pour transférer un fichier entre les PC. Le dock T1000 attendu n'est pas un
prérequis de la préparation logicielle.

## Préflight à exécuter sur HP, uniquement hors port

Ouvrir **Windows PowerShell5.1 64bits** sur HP. Coller le bloc interactif ci-dessous,
pas exécuter un fichier `.ps1`. Aucun changement d'ExecutionPolicy, de registre,
de service Windows ni installation SDK/runtime. À l'invite, saisir le chemin
complet du ZIP **local HP**, sans guillemets autour du texte saisi.

Le bloc vérifie le modèle HP, Windows/x64, taille/hash ZIP et manifeste, extrait
dans un nouveau dossier local, vérifie chaque fichier et inventorie COM7 sans
l'ouvrir. Une seule invocation de l'application, avec arguments OFFLINE fixes.
Les erreurs arrêtent la procédure ; elles n'autorisent pas une transmission.

```powershell
& {
    $ErrorActionPreference = 'Stop'
    if ($PSVersionTable.PSVersion.Major -ne 5 -or -not [Environment]::Is64BitProcess) {
        throw 'Utiliser Windows PowerShell 5.1 64 bits, sans changer la configuration Windows'
    }
    $computer = Get-CimInstance Win32_ComputerSystem
    $os = Get-CimInstance Win32_OperatingSystem
    $computer | Select-Object Manufacturer, Model
    $os | Select-Object Caption, Version, OSArchitecture
    if ($computer.Model -notmatch '15-db1' -or -not [Environment]::Is64BitOperatingSystem) {
        throw 'HP Laptop 15-db1xxx / Windows x64 attendu ; arret, ne pas utiliser le Dell'
    }
    $zip = (Get-Item -LiteralPath (Read-Host 'Chemin complet du ZIP local HP')).FullName
    $zipHash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
    if ((Get-Item -LiteralPath $zip).Length -ne 50086986 -or
        $zipHash -ne '51e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0') {
        throw 'ZIP incorrect : conserver le fichier et arreter avant extraction/execution'
    }
    $suffix = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ') + '-' + [Guid]::NewGuid().ToString('N')
    $destination = Join-Path $env:LOCALAPPDATA ('TR2\M014-B-' + $suffix)
    New-Item -ItemType Directory -Path $destination -ErrorAction Stop | Out-Null
    $transcript = Join-Path $destination 'M014-B-preflight.txt'
    Start-Transcript -LiteralPath $transcript -NoClobber | Out-Null
    try {
        'MODE: M014-B OFFLINE, zero transmission'
        $computer | Select-Object Manufacturer, Model
        $os | Select-Object Caption, Version, OSArchitecture
        'ZIP SHA256: ' + $zipHash
        'Dossier HP: ' + $destination
        Expand-Archive -LiteralPath $zip -DestinationPath $destination
        $manifestPath = Join-Path $destination 'M014-A-package-manifest.json'
        $manifestHash = (Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($manifestHash -ne 'eeaf70a35f81a9d9a5102cef011dc54bb4e9eae2b774e75af75689b1dc6db7a3') {
            throw 'Manifeste incorrect'
        }
        $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
        if (@($manifest.files).Count -ne 370) { throw 'Nombre de fichiers incorrect' }
        foreach ($file in $manifest.files) {
            $path = Join-Path $destination $file.path
            if ((Get-Item -LiteralPath $path).Length -ne $file.bytes -or
                (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $file.sha256) {
                throw ('Fichier incorrect : ' + $file.path)
            }
        }
        'Manifeste et 370 fichiers : OK'
        $dotnet = Get-Command dotnet -ErrorAction SilentlyContinue
        if ($dotnet) { & $dotnet.Source --list-runtimes } else { 'dotnet systeme absent : runtime embarque' }
        $ports = @([System.IO.Ports.SerialPort]::GetPortNames())
        'Ports inventories, sans ouverture : ' + ($ports -join ', ')
        if ($ports -notcontains 'COM7') { throw 'COM7 absent : ne pas choisir un autre port' }
        $portDevices = @(Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match '\(COM7\)' })
        $portDevices | Select-Object Name, Status, Manufacturer, PNPDeviceID
        if ($portDevices.Count -ne 1 -or $portDevices[0].Status -ne 'OK') {
            throw 'Inventaire PnP COM7 absent, ambigu ou en erreur'
        }
        $exe = Join-Path $destination 'TR2.Supervision.Service.exe'
        $output = @(& $exe --diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000 2>&1)
        $exitCode = $LASTEXITCODE
        $output | ForEach-Object { $_.ToString() }
        if ($exitCode -ne 0 -or ($output -join "`n") -notmatch 'OFFLINE: no port opened, no transmission, no log created\.') {
            throw ('Diagnostic OFFLINE echoue, code=' + $exitCode)
        }
        'PREFLIGHT OFFLINE OK ; NO-GO TRANSMISSION tant que M014-C non autorisee'
        'Conserver ce dossier et transmettre la sortie complete pour revue ChatGPT'
    }
    finally { Stop-Transcript | Out-Null }
}
```

Le modèle, l'architecture et le ZIP sont contrôlés avant toute exécution.
Le dossier suffixé est neuf ; aucune installation/préflight antérieur n'est écrasé.
Une sortie en erreur laisse les fichiers de preuve en place. Une erreur avant
création du transcript doit être transmise avec le message PowerShell affiché.
Le transcript reste local HP ; il peut contenir nom de machine/utilisateur/chemins,
ne pas l'ajouter directement au dépôt public. Transmettre à ChatGPT le résultat
hors port et, si nécessaire, une copie expurgée des informations personnelles.

Présence PnP/COM7 ne prouve ni port libre, ni identité électrique du câble,
ni fonctionnement du driver avec .NET. L'utilisateur confirme que COM7 correspond
au convertisseur FT232R connu, sans changer les acquis électriques. Aucun test
`SerialPort.Open`, aucun essai Python, aucun `--read-once`/`--config` dans ce bloc.
OFFLINE ne fabrique pas de réponse UNKNOWN : il affiche une **attente** de banc,
pas un résultat de lecture physique.

## Conditions du futur M014-C

**NO-GO transmission actuel**, même si le préflight OFFLINE réussit.

| Condition à établir avant l'essai unique | État M014-B |
|---|---|
| Revue indépendante ChatGPT du rapport/procédure B | À réaliser après publication |
| Contrat distinct M014-C : port, paramètres, maximum1 lecture, arrêt sans retry, journal exclusif | Nécessaire, aucune autorisation héritée |
| ZIP/hash et370 fichiers conformes sur HP, sortie OFFLINE/code0 et modèle Windowsx64 attendus | Préparé, NON VÉRIFIÉ sur HP |
| Convertisseur FT232R identifié COM7 et aucun programme maître actif | Déclaration utilisateur future ; PnP seul ne démontre pas exclusivité |
| État cible diagnostic unité1 sain et baseline MCU récente sous permissions de mission adaptées | NON OBSERVÉ en B ; ne pas réutiliser J comme preuve actuelle |
| Aucun GDB pendant trafic, aucun CubeProgrammer HOTPLUG/reset/flash perturbateur avant lecture | Garde-fou à conserver |
| Preuves après sous autorisation distincte, code retour et journal PC conservés même après erreur | À cadrer par M014-C |

Le futur essai est FC03/1000/1, COM7/1152008E1/unité1, UNKNOWN0 attendu.
Il ne lance ni polling production, ni découverte B0, ni écriture/B5.
Le futur contrat fixera le nom de journal non existant et les actions avant/après ;
aucune commande de transmission prête à coller n'est fournie par B.
Après timeout/erreur/issue ambiguë : conserver preuves, arrêter sans nouvel essai.

L'application fournit des registres décodés via NModbus et des attentes hex,
pas une capture brute du fil. La prochaine mission doit en tenir compte pour
son critère de preuve. UNKNOWN0 ne signifie pas READY, DeviceId reste inconnu.
Qualification temporelle DE/TC/stopbit/retourRX, endurance/reset/watchdogIRQ,
production/J1global/E4 restent ouvertes. Aucun contrôle électrique acquis à répéter.

## Vérification de cette préparation

Sur T1000 : ZIP/hash/CRC d'archive et370 fichiers vérifiés, structure PE x64 et
runtimeconfig autonome vérifiés ; code M014-A conservé. Le bloc PowerShell a été analysé par Parser::ParseFile sous Windows PowerShell
du Dell : aucune erreur de syntaxe ; options de transcript et type SerialPort
confirmés par métadonnées, sans exécuter le bloc ni inventorier/ouvrir COM7.
Deux tests .NET existants d'absence d'ouverture sans activation ont réussi sous
Linux (2/2, sans build). Les502 tests de M014-A sont des résultats conservés,
pas une suite complète relancée ni une qualification HP. Preuves B : `/home/lolo/dev/msm/reports/TR2-mission014-B-20261011T173305Z`.

## Suite publiée M014-C

Le contrat M014-C-END-TO-END étend maintenant la préparation à un seul essai
HP après revue ChatGPT, puis analyse indépendante HP/MCU dans la même mission.
ZIP conforme et OFFLINE/code0 sont désormais rapportés par l'utilisateur ;
les autres contrôles HP restent à établir. Suivre [la procédure C](ESSAI_HP_DOTNET_M014_C.md),
sans rejouer B ni la campagne Python I. Les états B ci-dessus sont historiques.

L'essai C est désormais consommé : succès rapporté18:15:29UTC et deltasMCU
conformes. Ne pas réexécuter le blocC ; voir le rapport final C avec réserve
journalHP nonlocal et limites de qualification.
