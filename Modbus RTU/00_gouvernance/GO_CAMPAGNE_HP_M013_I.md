# M013-I — GO conditionnel et commande unique HP

11 octobre 2026.

**État historique après M013-J : commande I consommée par l’unique campagne rapportée dans J. Ne plus exécuter le bloc ci-dessous, ni renommer/supprimer le journal pour le relancer. Audit MCU concordant ; clôture et limites dans DERNIER_RAPPORT.md et CLOTURE_M013_ET_PROPOSITION_M014.md.**

**GO technique conditionnel pour une seule campagne HP, 12 écritures maximum.** Aucun lancement avant contrôle indépendant du présent rapport par ChatGPT et exclusivité du maître HP assurée. Si plusieurs maîtres/répondants sont présents, si l'exclusivité est incertaine ou si l'état cible a été perturbé depuis la baseline : **NO-GO d'exécution**, ne pas lancer. La topologie exclusive n'est pas déduite des accès Codex et aucun autre équipement n'est supposé présent ou absent.

Observation rapportée par l'utilisateur dans le contrat I, pas mesure Codex directe : HP Laptop15-db1xxx, COM7 disponible, Python3.13.3/pySerial3.5, préflightH environment_ready=true, OFFLINE/mock réussis, fichiers hachés conformes, zéro transmission. Le bloc ci-dessous réutilise exactement le dossier fourni ; pas de préflight, téléchargement ou installation répété. SHA256 du script figé `ef9e4e64a4a93a5bffdb81daeee2049fc651dbde` contrôlé avant toute ouverture ; journal `M013-I-physical.jsonl` créé exclusivement par Python, jamais écrasé.

Avant le lancement futur : aucune autre application/maître actif sur COM7/bus ; seule cible diagnostic unité1 et aucun répondant unité2 pour le cas négatif. Aucun recâblage/contrôle J1/J2/J3/RE-RST/VIO/SB28 redemandé. Aucun CubeProgrammer HOTPLUG, reset/flash/coupure ou halt GDB entre baseline saine et campagne ; si intervention connue, état à actualiser par Codex sous autorisation avant lancement. Aucun debugger/halt pendant les échanges. Exécuter ce bloc **une seule fois sur HP après revue GO ChatGPT**, puis communiquer toute sortie et fichierJSONL même en cas d'échec. Ne pas relancer, renommer ou supprimer le journal pour contourner l'arrêt ; ne pas répéter l'ancien probeE.

```powershell
& {
  $ErrorActionPreference = 'Stop'
  $tr2Dir = 'C:\Users\Lolo\AppData\Local\Temp\TR2-M013-H-696f8b55f09242cfaa38ff397de54256'
  $tr2Script = Join-Path $tr2Dir 'rtu_campaign.py'
  $tr2Log = Join-Path $tr2Dir 'M013-I-physical.jsonl'
  if (-not (Test-Path -LiteralPath $tr2Script -PathType Leaf)) { throw 'Script absent : arret sans ouvrir COM7.' }
  if ((Get-FileHash -Algorithm SHA256 -LiteralPath $tr2Script).Hash -ne '5efd7e37769f84adfe1a439225814e7ead481a0fdb48614331d8ffe4b649d957') { throw 'Script modifie : NO-GO.' }
  if (Test-Path -LiteralPath $tr2Log) { throw 'Journal deja present : ne pas relancer, renommer ou supprimer.' }
  $tr2Python = Get-Command py.exe,python.exe -ErrorAction SilentlyContinue | Where-Object { $_.Source -notlike '*\WindowsApps\*' } | Select-Object -First 1
  if (-not $tr2Python) { throw 'Python indisponible : arret sans ouvrir COM7.' }
  $tr2Args = @()
  if ($tr2Python.Name -eq 'py.exe') { $tr2Args += '-3' }
  $tr2Args += @($tr2Script, '--transmit', '--authorized-mission', 'TR2-20261011-M013-I-HP-CAMPAIGN-GO', '--log', $tr2Log)
  & $tr2Python.Source @tr2Args
  $tr2Exit = $LASTEXITCODE
  Write-Output ('Exit=' + $tr2Exit + '; journal=' + $tr2Log + '; ne pas relancer')
  if (Test-Path -LiteralPath $tr2Log) { Get-Content -LiteralPath $tr2Log }
}
```

Bloc analysé syntaxiquement sur Windows T1000, retour0 ; **non exécuté** par Codex. Il résout le même launcher py.exe/python.exe existant que le préflight H, évite l'aliasStore ; un autre environnement/PATH que celui validé laisserait le lancement non couvert. Le marqueur --authorized-mission identifie I, ne remplace pas la revue humaine. L'ouverture exclusive du journal précède l'ouverture série ; erreur de fichier, hash, script, launcher ou journalpréexistant empêche tout envoi. Journal créé puis échec d'ouverture = essai arrêté sans retry.

Campagne fixe : cas1..4 nominaux, cas5 CRCincorrect `010303E80001047B`, cas6..7 nominaux, cas8 unité2/CRCcorrect `020303E800010449`, cas9..12 nominaux. Nominal : requête `010303E80001047A`, réponse exacte7octets `0103020000B844` (UNKNOWN0/CRC0) ; négatifs : silence1s puis nominalsuivant réussi. COM7/1152008E1, timeoutlecture0/écriture1s, handshake/DTRRTSdésactivés. Fenêtre1s+pause100ms parcas,100msinitiaux ; barrière20s avant nouvelle écriture, RXmax256. Tout écho/écho+réponse/bruit/CRCpartiel/extra/exception/timeoutnominal/RXtardif/overflow/écriturepartielle/I/O/logdéfaillant→arrêt/fermeture sans retry ni purge ; aucun écho supprimé. Journaux UTC/tempsmonotone/chunksbruts/intent/nbécrits/verdict, simulated=false pour branchephysique. Retour0 seulement campagne complète ; retournon0/traceback toujours arrêter et préserver preuves.

Limites : première branche COM7 physique de cet outil encore non testée ; le préflight et les doubles de Serial ne qualifient pas le pilote ni un transitoireDTRRTS/écho à l'ouverture. 20s est une barrière logicielle avant écriture, pas garantieOS. Course RXcheck→write et octets après fermeture restent nonexclus ; aucun succès temporel général déduit. Acquisition DE/TC/dernierstopbit/retourRX WAITING instrument identifié. La campagne ne constitue pas qualification temporelle, endurance, production, J1global ou E4.

## Baseline STM32 et analyse après la future campagne

Image E diagnosticON/observerON/TXON/unité1, 49200octets exacts SHA256dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540. GDB seul : ready1/erreurs0, RXBUSY_RX/count1, IRQ49/66 actives CPU/SWD/ISPR0, PG4bas/TXidle/filevide ; tick2901075→2910233/poll989199439→992322297. Baseline finale brute et JSON dans /home/lolo/dev/msm/reports/TR2-mission013-I-20261011T165146Z, SHA256fadf0042a2a6946586815a6b9c01c192499cf8ed764bf4349784d0fc7a034feb. Compteurs initiaux : RX8/frame1/CRCvalid1/CRCbad0/foreign0/acceptedavailable1/TXattemptstartedTC1, erreurs/drop/failsafe0, UARTIRQ17/TIM6IRQcallback3 ; trafic inchangé F/H.

Après unique campagne : aucune nouvelle émission/reset/flash avant relevé APRÈS sous mission appropriée, préserverJSONL/sortie. Douze cas complets attendent deltaRX96/frame12/CRCvalid11/CRCbad1/foreign1/acceptedavailable10/TXattemptstartedTC10, erreurs/drop/failsafe0 et RXarmé/PG4bas/filevide/NVICsain. Valeurs absolues attendues RX104/frame13/CRCvalid12/CRCbad1/foreign1/acceptedavailable11/TXattemptstartedTC11 sur ce boot ; IRQ n'ont pas de nombre imposé. Si arrêt, comparer préfixe réellement écrit, jamais supposer12. Deltas modulo32bits ; reboot/wrapmultiple inconnu rend la comparaison ambiguë. Snapshot dernièretrame et callbackTC ne prouvent pas tous fronts physiques. Aucune campagne effectuée en I.
