# M013-H — préflight HP hors port et verdict

11 octobre 2026. **NO-GO pour la campagne physique. Préflight hors port PRÉPARÉ.** L'environnement HP Windows/Python/pySerial n'est pas accessible depuis Codex. Le Windows réellement accessible est Dell Latitude 7320 (T1000), pas HP ; aucune commande py/python trouvée dans son PATH (cela n'exclut pas une installation ailleurs). Aucun accès distant HP établi. Aucune installation ni modification de politique/configuration effectuée.

## Action minimale future

Après revue de ce rapport par ChatGPT, coller **une fois le bloc ci-dessous dans PowerShell sur le HP** et transmettre toute la sortie. Il cherche Python existant (sans lancer un alias Windows Store), télécharge uniquement deux sources figées/hachées dans un nouveau dossier temporaire, inventorie machine et COM7 par WMI **sans ouvrir le port**, puis teste OFFLINE et mock. Il n'installe rien ; aucune commande --transmit n'est exécutée, aucun changement de politique PowerShell. En cas d'échec réseau, Python absent/incompatible ou pySerial manquant : transmettre l'erreur, pas de retry physique ni installation improvisée. Dossier de preuves créé sans écrasement ; le JSON est enregistré et affiché. Les fichiers temporaires de simulation sont locaux au préflight et supprimés à sa sortie.

```powershell
& {
  $ErrorActionPreference = 'Stop'
  $tr2Python = Get-Command py.exe,python.exe -ErrorAction SilentlyContinue | Where-Object { $_.Source -notlike '*\WindowsApps\*' } | Select-Object -First 1
  if (-not $tr2Python) { throw 'Python introuvable dans PATH : NO-GO ; transmettre cette erreur sans installer ni ouvrir COM7.' }
  $tr2Dir = Join-Path $env:TEMP ('TR2-M013-H-' + [guid]::NewGuid().ToString('N'))
  New-Item -ItemType Directory -Path $tr2Dir | Out-Null
  $tr2Root = 'https://raw.githubusercontent.com/loloLR17/MSM_vibe/ef9e4e64a4a93a5bffdb81daeee2049fc651dbde/Modbus%20RTU/05_Firmware/platform/host/rs485_rx_probe/'
  $tr2Hashes = @{ 'preflight_hp.py' = '2eb5e5475e719ab3f9a2a748ba2e1a4beb7120fe5f51b5f49d52629a0e7ea20b'; 'rtu_campaign.py' = '5efd7e37769f84adfe1a439225814e7ead481a0fdb48614331d8ffe4b649d957' }
  foreach ($tr2Name in $tr2Hashes.Keys) {
    $tr2File = Join-Path $tr2Dir $tr2Name
    Invoke-WebRequest -UseBasicParsing -Uri ($tr2Root + $tr2Name) -OutFile $tr2File
    if ((Get-FileHash -Algorithm SHA256 $tr2File).Hash -ne $tr2Hashes[$tr2Name]) { throw 'SHA256 incorrect : arret sans execution.' }
  }
  Get-CimInstance Win32_ComputerSystem | Select-Object Manufacturer,Model
  Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match '\(COM7\)' } | Select-Object Name,Status
  $tr2Args = @()
  if ($tr2Python.Name -eq 'py.exe') { $tr2Args += '-3' }
  $tr2Args += (Join-Path $tr2Dir 'preflight_hp.py')
  $tr2Output = & $tr2Python.Source @tr2Args
  $tr2Exit = $LASTEXITCODE
  $tr2Output | Set-Content -Encoding UTF8 (Join-Path $tr2Dir 'preflight-result.json')
  $tr2Output
  Write-Output ('Exit=' + $tr2Exit + '; preuves=' + $tr2Dir + '; aucune transmission autorisee')
}
```

Code figé `ef9e4e64a4a93a5bffdb81daeee2049fc651dbde`. Ce bloc PowerShell a été analysé syntaxiquement sous Windows T1000 ; son exécution complète sur HP est **NON VÉRIFIÉE**. Le préflight Python est testé sur Linux (NON Windows) et par doubles de dépendance/plateforme. `environment_ready=true` signifierait uniquement Windows/Python>=3.9/pySerial3.5 et OFFLINE/mock réussis sur le poste où il est lancé ; **campaign_go=false reste toujours**, identité HP à confirmer par sortie constructeur/modèle et utilisateur. Import pySerial sans construction Serial ; fonction d'ouverture réelle interdite pendant les tests CLI. Ne prouve ni pilote/paramètres série opérationnels ni réservation COM7, écho du câble ou topologie exclusive. Une absence d'entrée COM7 dans l'inventaire reste NO-GO. Un alias Python Microsoft Store est exclu ; aucune installation automatique.

## État cible effectivement relevé en H

Image E diagnosticON/observerON/TXON/unité1 exacte49200 octets, SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`, ELF `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`. Trois sessions GDB seules détachées, serveurs terminés ; aucun CubeProgrammer HOTPLUG/reset/flash. IRQ49/66 actives CPU et SWD (ISER1=0x20000/ISER2=4), ISPR/masks0, RXBUSY_RX/count1/erreurs0, TXidle/PG4bas, filevide. Tick2121087→2130079, poll723241806→726307644 ; compteurs trafic inchangés F : RX8/frame1/CRCvalid1/accepted1/TXstarted1/TCcallback1, aucune erreur/drop/failsafe. Snapshot RAM768KiB et baseline bruts hachés conservés. Cet état ponctuel ne garantit pas un état futur : nouvelle baseline GDB avant une campagne autorisée, jamais de halt pendant le trafic.

## Revue de la campagne et suite

Plan G conservé : 12 écritures maximum (10 nominales, 1 mauvais CRC, 1 unité2), nominales après chaque rejet, fenêtres1s puis100ms et100ms initiaux, arrêt avant nouvelle écriture à20s, RXmax256, timeoutwrite1s. Écho/écho+réponse jamais supprimés ; silence uniquement attendu pour deux négatifs, tout autre octet/timeout/partiel/erreur/logdéfaillant→arrêt sans retry/purge. RX tardif vérifié avant requête suivante et après pause finale ; octets arrivant après fermeture ne sont pas observables. Un RX survenu dans la course check→write peut rendre l'échange ambigu et sera jugé sur le brut ; absence générale de réponses tardives non prouvée. Budget20s est une barrière avant écriture, pas garantie temps réel d'OS. Aucun correctif fonctionnel de l'engine requis ; seul --offline explicite ajouté pour lever la lacune CLI de préflight.

**Prérequis avant futur GO** : sortie HP réelle positive avec hashes, COM7 FT232R présent, seul maître HP/une cible unité1 et aucun répondant unité2 confirmés, journal nouveau, revue indépendante ChatGPT et mission d'essai distincte. La commande physique n'est pas rendue exécutable par H ; le modèle réservé du plan G reste conditionnel. Maximum une future campagne, aucun replay de l'instruction E consommée. Acquisition temporelle DE/TC/retourRX reste WAITING instrument identifié ; répétabilité/robustesse générale/production/J1global/E4 non qualifiées. Aucune répétition J1/J2/J3/RE-RST/VIO/SB28/câblage demandée.
