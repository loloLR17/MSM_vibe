# M014-C-END-TO-END — une lecture .NET sur HP

11 octobre 2026. **ESSAI CONSOMMÉ : succès unique rapporté le11/10/2026 à18:15:29UTC,
corroboré par deltas STM32. NE PAS EXÉCUTER OU RELANCER LE BLOC CI-DESSOUS.** Cette procédure
remplace pour C les conditions futures de B. Le contrat étendu conserve la même
mission jusqu'à réception du résultat HP et analyse MCU. La campagne Python I
est consommée : ne pas la relancer. Aucun nouveau contrôle électrique demandé.

## Conditions et commande unique

Instructions historiques de préparation (consommées, aucun nouveau GO).
Après **revue explicite de ce bloc par ChatGPT**, ouvrir Windows PowerShell5.1
64bits sur le HP Laptop15-db1xxx et coller le bloc entier, une seule fois.
Indiquer le dossier du paquet M014-A déjà extrait (celui dont l'OFFLINE a réussi),
pas un ZIP ni le chemin EXE. Aucun transfert/extraction ou installation à refaire.
Le ZIP conforme et l'OFFLINE/code0 sont des faits rapportés par l'utilisateur.
Les contrôles complémentaires EXE/manifeste370/PnP restent à effectuer ici.

Le bloc inventorie COM7 sans l'ouvrir ; présence ne signifie pas port libre.
VID0403/PID6001 identifie la famille FTDI, pas à lui seul le convertisseur physique :
l'opérateur confirme le FT232R connu, l'exclusivité et l'absence de perturbation.
L'inventaire de processus candidats est non exhaustif. Aucun arrêt de processus,
sondage Open, Python, polling, découverte, écriture ou changement Windows.

Le dossier fixe `%LOCALAPPDATA%\TR2\M014-C-END-TO-END` doit être absent.
Sa création et le marqueur CreateNew durable interdisent la relance accidentelle,
y compris après erreur. **Ne jamais supprimer/renommer ce dossier pour retenter.**
Si un prérequis échoue avant toute invocation, conserver la sortie et demander
l'analyse, sans transmettre. Aucun GDB pendant l'essai ; aucun reset/flash/HOTPLUG
entre la baseline et cet essai. Si ces conditions ont changé : NO-GO, signaler.

```powershell
& {
    $ErrorActionPreference = 'Stop'
    if ($PSVersionTable.PSVersion.Major -ne 5 -or -not [Environment]::Is64BitProcess) { throw 'Windows PowerShell 5.1 x64 requis' }
    $computer = Get-CimInstance Win32_ComputerSystem
    if ($computer.Manufacturer -notmatch 'HP|Hewlett' -or $computer.Model -notmatch '15-db1' -or -not [Environment]::Is64BitOperatingSystem) { throw 'HP Laptop 15-db1xxx x64 requis ; ne pas utiliser le Dell' }
    $package = (Get-Item -LiteralPath (Read-Host 'Dossier HP deja extrait du paquet M014-A')).FullName
    $manifestPath = Join-Path $package 'M014-A-package-manifest.json'
    if ((Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant() -ne 'eeaf70a35f81a9d9a5102cef011dc54bb4e9eae2b774e75af75689b1dc6db7a3') { throw 'Manifeste incorrect' }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    if (@($manifest.files).Count -ne 370) { throw '370 fichiers attendus' }
    foreach ($file in $manifest.files) {
        $path = Join-Path $package $file.path
        if ((Get-Item -LiteralPath $path).Length -ne $file.bytes -or (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $file.sha256) { throw ('Fichier incorrect : ' + $file.path) }
    }
    $exe = Join-Path $package 'TR2.Supervision.Service.exe'
    $exeHash = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($exeHash -ne '13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872') { throw 'EXE incorrect' }
    $ports = @([System.IO.Ports.SerialPort]::GetPortNames())
    $devices = @(Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match '\(COM7\)' })
    $devices | Select-Object Name, Status, Manufacturer, PNPDeviceID | Format-List
    if ($ports -notcontains 'COM7' -or $devices.Count -ne 1 -or $devices[0].Status -ne 'OK' -or $devices[0].PNPDeviceID -notmatch 'VID_0403[+&]PID_6001') { throw 'COM7 FTDI attendu absent, ambigu ou incorrect' }
    $masters = @(Get-CimInstance Win32_Process | Where-Object { $_.Name -match '^(python(w)?|dotnet|TR2\.Supervision\.Service|modpoll|mbpoll|qmodmaster|modbuspoll|Modbus Poll)\.exe$' })
    $masters | Select-Object Name, ProcessId | Format-Table
    if ($masters.Count -ne 0) { throw 'Processus maitre potentiel actif ; arret sans ouverture de port' }
    'COM7 present ; disponibilite exclusive NON PROUVEE par inventaire.'
    if ((Read-Host 'Apres revue ChatGPT de cette commande, confirmer HP seul maitre, FT232R connu sur COM7, autres logiciels serie fermes, aucun GDB/reset/flash depuis baseline ; saisir M014-C-UNIQUE') -cne 'M014-C-UNIQUE') { throw 'Conditions non confirmees ; aucune transmission' }
    $root = Join-Path $env:LOCALAPPDATA 'TR2\M014-C-END-TO-END'
    if (Test-Path -LiteralPath $root) { throw 'Dossier mission deja present : conserver les preuves ; aucune relance' }
    New-Item -ItemType Directory -Path $root -ErrorAction Stop | Out-Null
    $log = Join-Path $root 'M014-C-read-once.jsonl'
    $transcript = Join-Path $root 'M014-C-console.txt'
    Start-Transcript -LiteralPath $transcript -NoClobber | Out-Null
    try {
        $computer | Select-Object Manufacturer, Model
        'EXE SHA256: ' + $exeHash
        'Manifeste et 370 fichiers: OK'
        $devices | Select-Object Name, Status, Manufacturer, PNPDeviceID | Format-List
        'HP seul maitre confirme par operateur ; aucun processus candidat trouve.'
        'Baseline MCU : M014-C 20261011T180341Z ; essai unique, sans retry.'
        $marker = [System.IO.File]::Open((Join-Path $root 'ATTEMPT-CONSUMED.txt'), [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
        try {
            $bytes = [Text.Encoding]::UTF8.GetBytes('M014-C attempt consumed before invocation; UTC=' + [DateTime]::UtcNow.ToString('o'))
            $marker.Write($bytes, 0, $bytes.Length)
            $marker.Flush($true)
        } finally { $marker.Dispose() }
        $probeExit = $null
        try {
            & $exe --diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000 --read-once --log $log
            $probeExit = $LASTEXITCODE
        } finally {
            'Diagnostic exit code: ' + $probeExit
            if (Test-Path -LiteralPath $log) {
                Get-Content -LiteralPath $log
                Get-FileHash -LiteralPath $log -Algorithm SHA256 | Format-List
            }
            'Conserver tout le dossier : ' + $root
            'ESSAI CONSOMME : ne jamais relancer, meme apres erreur ou resultat ambigu.'
        }
        if ($probeExit -ne 0) { throw 'Diagnostic echoue ou incomplet : arret sans nouvel essai' }
        $records = @(Get-Content -LiteralPath $log | ForEach-Object { $_ | ConvertFrom-Json })
        $results = @($records | Where-Object { $_.event -eq 'result' })
        if ($results.Count -ne 1 -or $results[0].passed -ne $true) { throw 'Journal ambigu : conserver sans relance' }
        'Lecture PC reussie ; qualification end-to-end encore en attente analyse MCU independante.'
    } finally { Stop-Transcript | Out-Null }
}
```

## Preuves à retourner, succès et arrêt

Conserver le dossier complet, même si l'application échoue : journal JSONL,
transcript console et marqueur. Retourner à ChatGPT/Codex la sortie complète,
le code retour, le JSONL exact et son SHA256 affiché. Le transcript peut contenir
chemins/identifiants locaux : ne pas le publier brut dans le dépôt public.
Le fichier JSONL exact est nécessaire au recalcul indépendant du hash ; un résumé
ou une capture seule ne prouve pas sa chronologie. Ne pas inventer une capture fil.

Une seule invocation : COM7,115200,8bits,Even,One,unité1,FC03/1000/1,timeout série
1000ms,Retries0,deadline d'attente4000ms. Journal CLI CreateNew/FileShare.None ;
port fermé par disposal. Driver open/dispose n'a pas de garantie temps réel dure.
Requête attendue `010303E80001047A`, réponse attendue `0103020000B844` ; ces chaînes
sont des attentes, le journal PC contient des registres décodés via NModbus.
CRC est contrôlé par NModbus, testé sur flux mémoire en A.

Succès PC : code0, un start/un read_intent/un decoded_response registers[0]/un
result passedtrue/valueUNKNOWN(0), pas failure, connexion fermée. Toute timeout,
erreur/exception/fermeture ou sortie ambiguë : arrêt, aucun retry/reconnexion.
Code0 seul ne clôture pas la qualification end-to-end.

## Baseline STM32 et analyse après résultat

Baseline réelle du 11octobre, dossier preuves T1000
`/home/lolo/dev/msm/reports/TR2-mission014-C-20261011T180341Z`,
`baseline-final.bin`992octets SHA256
`28d7bff75da0eb1ed7c7fc108c69da3a17d50b744712a5b21341a909d07d0cfb`.
Flash identique à E, IRQ49/66 actives, ready1/errors0, PG4bas, MCU repris.
Détaché après trois sessions GDB seules ; aucun reset/flash/CLI.

| Compteur | Avant C | Delta attendu si lecture nominale | Après attendu |
|---|---:|---:|---:|
| rx_callback / bytes_polled |104|8|112|
| frame_count |13|1|14|
| crc_valid |12|1|13|
| accepted_request / available_status |11|1|12|
| tx_attempt / tx_started / tx_callback |11|1|12|
| crc_bad / foreign_unit |1|0|1|
| error_callback / errors_polled / events_dropped / tx_fail_safe |0|0|0|

Après réception du résultat HP, Codex relit image/observer/transport hors trafic,
compare modulo2^32 à cette baseline et analyse le journal exact ; complète le
**même rapport C**, sans nouvelle mission ni transmission. Après un échec,
les deltas sont des faits à analyser, pas un résultat attendu à imposer.
Sans résultat HP/lecture après, statut BLOCKED ; pas DONE anticipé.
UNKNOWN0 qualifie seulement ce diagnostic borné, pas READY/DeviceId/B0/B1complet,
production, temporisation DE/TC/stopbit/t1.5/t3.5/retourRX, robustesse/watchdog,
endurance/J1global/E4 ni communication bidirectionnelle générale.

## Résultat final C — 11 octobre 2026

Utilisateur : UNKNOWN0/code0/passedtrue, un essai, aucune répétition,18:15:29UTC.
SHA journal annoncé AFDAB5517A13ABED1E9735A4240F0E6B53C5DC110F9CB717BB50410F698A2272 ;
fichier non fourni à Codex, contenu/hash/chronologie non vérifiés indépendamment.
GDB seul après : RX112/frames14/CRCvalid13/acceptedavailableTXTC12, erreurs0 ;
17deltas attendus conformes, image E exacte, IRQ/RX/polling sains. Voir le
[rapport final](Echanges_Codex/DERNIER_RAPPORT.md). Réserve documentaire HP
et limites temporelles/robustesse/production maintenues. Aucune nouvelle émission.
