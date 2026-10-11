---
mission_id: TR2-20261011-SSH-HP-001
status: DONE
base_ref: main
base_sha: 78093692f2a2b0b6680d657ce1eb6a87ad2d472e
initial_head: f1e830136bf50abafbaad5ae8bdbc53765839017
mission_sha: a1b24bc4632e0d33bbbec2b26913c321c21fe4b3
result_sha: 52f6ab83911536f10f089c1a654862248189c860
created_at_utc: 2026-10-11T21:09:33Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# SSH-HP-001 — SSH et diagnostic OFFLINE HP effectivement exécutés

Critères SSH-HP-001 remplis ; publication normale et relecture distante V2
effectuées. Statut DONE limité à cette infrastructure et aux tests OFFLINE. **Aucune ouverture COM7 ni émission Modbus**.
M014-D demeure BLOCKED pour production ; cette mission ne résout pas ses autorités
carte, ne qualifie pas la communication physique et ne rejoue aucun jeton C/I.

## Références et gouvernance

Dépôt initial main propre à f1e8301, deux stashes005/002 conservés. Fetch puis
pull --ff-only sûr vers a1b24bc, sans merge/rebase/reset. Base7809369 présente et
ancêtre de la mission vérifiée. AGENTS, contrat actif, protocoleV2, journal entier
et rapportD archivé lus ; fichiers concernés/outils existants inspectés avant
création. Pas d'orchestrationSSH équivalente trouvée. ArchivesD déjà créées par
ChatGPT vérifiées octet pour octet contre mission/rapport du HEADinitialf1e8301 ;
pas de réécriture. Mission active inchangée, freezes/V1/firmware/.NET inchangés.

Code réellement évalué : `52f6ab83911536f10f089c1a654862248189c860`, commit
`Add bounded Windows SSH orchestration and verified HP offline diagnostics`.
Le programme autonome A utilisé sur HP conserve sa provenance sourcebcc69c9d,
ce SHA d'orchestration n'est pas présenté comme celui d'un nouveau binaire .NET.

## Observations directes depuis Codex

- ClientSSH Linux : aliastr2-hp non configuré. ClientWindows existant sous
  `/mnt/c/Windows/System32/OpenSSH/ssh.exe` : alias versHP/user configurés,
  BatchMode/clé dédiée, commande `ssh ... tr2-hp hostname` retour0 :
  **LAPTOP-3F6J7STJ**. Aucun fichier clé privée lu/affiché/copiedansGit.
- Probe distant : cwd `C:\Users\Lolo`, PowerShell5.1.19041.6456, compte
  **administrateur**. Connexion SSH non interactive réellement démontrée,
  aucun nouveau droit/service/pare-feu/profil/politique ajouté.
- Inventaire HP en lecture seule : COM4/COM5/COM7, PnP **USB Serial Port(COM7)**,
  StatusOK/FTSER2K. Aucun processus candidat TR2/dotnet/python/putty/terminal/modbus
  détecté au relevé. **Pas preuve COM7 libre/exclusif**, pas inventaire des handles.
- `dotnet` absent du PATH observé, pas preuve absence globale. Paquet autonome
  retrouvé à `C:\Users\Lolo\AppData\Local\TR2\M014-B-20261011-195242` :
  EXE162816octets +manifesteA62485octets. Pas de solution/projet trouvé dans la
  recherche bornée Desktop/Documents/Downloads/LocalAppDataTR2 profondeur4 ;
  aucune absence globale revendiquée, erreurs d'accès de recherche non conclusives.
- EXEA SHA256 `13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872`,
  manifesteA SHA256 `eeaf70a35f81a9d9a5102cef011dc54bb4e9eae2b774e75af75689b1dc6db7a3`.
  **370fichiers tailles/SHA réellement vérifiés sur HP**, manifeste fixé/haché.
  Aucun transfert nécessaire/effectué, paquetA et ses données conservés.
- OFFLINE lancé automatiquement surHP viaSSH, retour0, stderrvide :
  `OFFLINE: no port opened, no transmission, no log created.` Paramètres affichés
  COM7/115200/8/Even/One/unité1/FC03/1000/1/retry0, aucun `--read-once`.
  Cette ligne et le code/hash vérifiés établissent le scénario logiciel sans
  ouverture ; pas une capture du bus ni une validation du driver physique.
- ServiceSSH présent/running/startmanual au relevé. Config explicitement lue :
  AuthorizedKeysFile et MatchGroupadministrators. `sshd -T` global indique
  PubkeyAuthenticationyes/PasswordAuthenticationyes et AuthorizedKeysFile utilisateur.
  ClausesMatch/pare-feu/rangesréseaux effectifs non audités exhaustivement.
  Risque admin/motdepasse signalé ; réduction ultérieure proposée, rien imposé.

## Livraison et garde-fous

`00_Environnement_developpement/T1000/ssh-hp/` contient `run_hp.py`,
`hp-task.ps1`, `test_run_hp.py`, README avec usage/retrait/limites.

- Bibliothèque standard Python seulement ; tâches fixes Probe/Inventory/Offline/
  FailureProbe. ClientWindows/alias fixes, BatchMode/StrictHostKeyCheckingyes,
  pas de saisie motdepasse/acceptation aveugle cléhôte. IdentitéHP vérifiée avant
  inventaire/exécution. ArgumentsPowerShell échappés, aucun argv arbitraire.
- BootstrapEncodedCommand court +script transmisstdin : évite limite Windows,
  pas changement de politique. Logsstdout/stderr binaires +resultJSON dans un
  dossierlocal neuf exclusif. Aucun script/service/agent installé surHP.
- OFFLINE seulement : EXE/manifesteAouD approuvés puis370fichiers vérifiés,
  chemin du manifeste contenu dans dossier ; processus candidat→refus ; mutex
  global nonbloquant libéré après traitement. Pas mode émission/bouclepolling.
- Timeoutconnexion10s/keepalive5s×2/commande45s/processOFFLINE15s. Code distant
  préservé ;255SSH/124timeoutlocal/127clientabsent/2usage. Aucun retryautomatique.
  DéconnexionSSH ne garantit pas annulation distante ; stateunknown explicite.
  KillWindows surtimeout prévu, branche panneprocess testée par mocklocal seulement,
  pas driverphysique qualifié. Course/modification concurrente de fichiers et
  maîtres externes non coopérants restent limites du banc provisoire.

## Tests et corrections réelles

Preuves horsGit dans `/home/lolo/dev/msm/reports/TR2-SSH-HP-001-*`.

| Vérification réellement effectuée | Résultat / preuve |
|---|---|
| hostnameSSH Windows |0, HP attendu ; commande initiale sortieoutil |
| Probe final |0, `probe-2/stdout.log`/result.json |
| Inventaire final +politique globaleSSH |0, `inventory-final/stdout.log`/result.json |
| FailureProbe distant volontaire |**17**, `failure/`, code propagé sansmasquage |
| HashEXE incorrect distant |**1**, `rejected-hash/`, refusavantlancement |
| OFFLINE HP +370hashes |**0**, `offline-2/`, sortiehorsport réelle |
| Mutex détenu par autre sessionSSH |holder0, tentative**1** avantlancement, `lock-test/` |
| TestsPython orchestration |**6/6**, `unit-tests.log` |

Commande tests : `python3 -m unittest discover -s
00_Environnement_developpement/T1000/ssh-hp -v`. Cas : SSHfixe/strict/bootstrapcourt,
quoting/tâchesrefusées, code17/logsbruts/dossierdéjàexistant, timeoutpartiel sansretry,
clientabsent/panneSSH avec étatdistantinconnu, restrictionsOFFLINE/verrou/deadline.
TestsréelsPS/HP complémentaires aux mocks : branchesProbe/Inventory/Offline,
refushash et contentionmutex effectivement exécutées. Aucun test supprimé/affaibli.

Erreurs conservées : première versionEncodedCommand contenanttoutscript refusée
parWindows («ligne de commande trop longue», code1, inventory/etprobe/).
Corrigéeparstdin/bootstrapcourt. Inventaire2 sérialisait excessivement métadonnées
PowerShell de Get-Content : conversionstrings explicite ajoutée ; inventairefinal
compact vérifié. PremierOFFLINE refusait les sous-dossiers légitimes wwwroot du
manifeste : code1/`offline/`, aucunlancement ; contrôle corrigéparchemincanonique
contenudanspackage, prochain essai **OFFLINE seulement** réussi. Ces erreurs ne
sont pas dissimulées, aucune transmissionModbus n'a été répétée.

Validation proportionnée : testsPython + exécutionsPowerShell réelles et inspection
Gitdiff/index ; pas de buildfirmware/.NET nécessaire pour des scripts d'orchestration.
RésultatsD524/524 et110/110 conservés, **non relancés** ni assimilés aux sixnouveauxtests.
Diffwhitespace/périmètre vérifiés ; logsvolumineux/credentials horsGit.

## Limites, retrait et prochaine étape

AccèsSSH validé ne vaut pas qualificationphysique. COM7/exclusivité universelle,
performance/timing/driver souspanne et transfertSCP d'un nouveau paquet non qualifiés.
Pas campagne COM7 autorisée spécifiquement ici, aucune commande physique exécutée.
PasGDB/flash/reset/CLIcible/OptionBytes/masserase/écritureModbus/gestephysique.
Le verrou protège coopérants ; campagnefuture requiert contratspécifique/revue,
borne/timeouts/journalnouveau et assurancemaîtreexclusif/cible, aucun GDBpendanttrafic.
Aucun replayM014-C/M013-I, aucun fauxREADY/DeviceId/B0/provisioning ajouté.

README propose retrait par arrêtusage uniquement (outilnoninstalléHP) ; conservation
preuves. Modifieralias/clé/serviceexistant reste décisionséparée, pas suppression
silencieuse. Réductionfuture : comptestandard limité, authentificationclé et portée
réseauprivé contrôlée, aprèsdécision. AucunsecondCodexHP installé/exécuté.

Journal actualisé pour accèsHP direct désormaisobservé, OFFLINE réel, limites et
suite ; historiques disantHPinaccessible restent datés. À transmettre àChatGPT :
revoirrapportSSH, décider éventuelle réductionprivilèges/campagnefuture souscontrat,
poursuivre décisionsproductionM014-D (toujoursBLOCKED). JournalHPoriginalC noncollecté :
réserve sur soncontenu/hashintégral maintenue, pas conclusiondepuisSHAseul.

## Git et publication

Commitcode52f6ab8 ci-dessus, commit/pushmain autorisés ; flash/debugfalse respectés.
Git initialpropre, travauxhorspérimètre noninclus, deuxstashes/backupspréservés.
Publication normale explicite HEAD:refs/heads/main réussie au SHA
`666254208c5760c53b0ee5c3fed06e7156c289a9`, commit
`Publish SSH HP qualification evidence and retained Modbus limits`. Fetch puis
relecture UTF8 intégrale des11 fichiers livrés/gouvernance/archivesD depuis cette
référence GitHub et comparaison octet pour octet avec les fichiers locaux :
tous conformes (`TR2-SSH-HP-001-remote-reread-1.json` horsGit). Main propre,
HEAD=origin/main, deux stashes conservés. Cette mise à jour DONE sera également
poussée et relue ; son SHA final est fourni dans la réponse Codex sansautoréférence.
La mission SSH est DONE ; M014-D demeure BLOCKED, aucun acquis physique nouveau.
