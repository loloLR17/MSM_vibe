# T1000 — Validation depuis l'architecture finale

Mission `T1000-20261010-CLOTURE-001`. Sources : main initial `75eba6b431725b858db3ac5380884934026a1475`, correctif test autorisé et documentation/outillage de mission. Aucune source de production modifiée. Les commits de clôture et preuves distantes sont consignés dans DERNIER_RAPPORT et le bilan final.

| Contrôle réel | Résultat |
|---|---|
| Sauvegarde puis déplacement | Archive vérifiée, empreintes de 3 408 fichiers identiques, git fsck réussi |
| Firmware `tr2-validate` depuis clone final | 102/102 CTest réussis, host build et cross-build ARM réussis |
| Supervision `tr2-supervision validate` | restore/build réussis, 0 erreur/0 warning, 466/466 tests, 0 ignoré |
| Reprise après `wsl --terminate Debian` | systemd running, binfmt actif, aucun service failed, interop Windows préservée |
| Tests après redémarrage | 102/102 CTest et 466/466 .NET réussis à nouveau |
| Git/gh | compte loloLR17, droits repo pull/push/admin par API, fetch/ls-remote authentifiés, helper local vers coffre |
| Codex natif | 0.162.1, login ChatGPT, doctor 20 OK/1 idle/0 fail ; thread/start confirme cwd et instructionSources AGENTS.md |
| PowerShell | .cmd vers WSL, version Codex, login, Git racine et aide supervision réussis |
| Supervision sans configuration | atteint réellement le service et affiche son usage ; code d'usage attendu, aucune communication physique |
| SDK | CubeU5 v1.9.0, HEAD d88042df24f16799957c24f00c2b234c9e306188, HAL/CMSIS nécessaires présents |
| Outils ST | CubeProgrammer 2.23.0, serveur GDB 7.14.0, GDB ARM Windows 15.2.90 ; lancement version réussi |

Comptes de tests .NET : Domain 17, Architecture 1, Protocol 45, Transport 24, Application 221, Persistence 31, Service 127 = 466. Tous réussis, aucun supprimé, désactivé ou affaibli.

Versions Linux : Git 2.47.3, GCC/G++ 14.2.0, GCC ARM 14.2.1, binutils 2.44, Newlib 4.5.0.20241231-1, CMake 3.31.6, Ninja 1.12.1, gdb-multiarch 16.3, OpenOCD 0.12.0, .NET SDK 10.0.401/runtime 10.0.12, Node 20.19.2/npm 9.2.0, gh 2.46.0. CubeCLT Windows 1.22.0 utilise une toolchain GCC distincte 14.3.1 : build évalué ici avec celle de Debian.

Empreintes du nouveau cross-build :

```text
BIN 5bb8d47bf370c99596b8e782611375f1ef086b121e11652618274925f376c0f1
ELF 1d401f483ab523033cc60de5935346b4c37723e52b00c270b67b74ad314ac484
MAP 4d0b149f8ecdc978d40b823796b102082298ca3f7c56158f982664c21ac03590
```

BIN/ELF identiques aux artefacts de la mission physique ; MAP différente à cause des chemins. BIN 74 412 octets, text 74 300/data 112/bss 69 776. Warnings Newlib/nosys conservés dans le log, sans erreur de build. La qualification physique précédente reste valide pour ce binaire exact ; aucun flash/debug répété ici.

Preuves complètes hors Git : `dev/msm/reports/2026-10-10-cloture` et copie Documents/T1000-CLOTURE-20261010. Logs firmware-validation, dotnet-validation, restart-validation, fichiers TRX des sept suites avant/après redémarrage, manifests, réponses expurgées de contrôle Codex, archives et rapports historiques. Aucun jeton, clé, binaire, sauvegarde Flash ou cache ajouté au dépôt public.

Les tests logiciels ne constituent pas une validation fonctionnelle du TR2. Le service supervision n'est pas qualifié en exploitation série réelle ; la chaîne de poste est prête pour une mission fonctionnelle distincte avec configuration matérielle autorisée.

Publication réelle : correctif de test `dc6dac73bbbdd19ecf921f84808065f816a5bc9c`, puis ensemble T1000 `61b1217fd8d0a337fdbc07445c056061a7e93c1c` poussé normalement sur main. 39 fichiers relus intégralement depuis la référence récupérée de GitHub, comparés aux fichiers locaux ; rapport et journal relus aussi via API. Git propre. Le premier rejet GH007 a été résolu avec identité noreply locale et sauvegarde des commits non publiés, sans affaiblir la protection ni force push. La mise à jour finale du rapport sera également vérifiée ; son SHA est donné dans le bilan final.
