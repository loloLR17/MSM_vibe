# T1000 — Reprise quotidienne

État qualifié le 10 octobre 2026. Pas de développement fonctionnel dans cette mission.

Depuis un nouveau PowerShell :

```powershell
tr2-shell
tr2-codex --version
tr2-git status --short
tr2-validate
tr2-supervision validate
```

Si le terminal prédate l'ajout au PATH, ouvrir un nouveau terminal ou utiliser `& "$env:LOCALAPPDATA\T1000\bin\tr2-codex.cmd"`. Le raccourci « TR2 Debian » ouvre la racine du clone. Depuis Debian :

```bash
source "$HOME/dev/msm/scripts/environment.sh"
cd "$TR2_REPO"
git status --short
git rev-parse HEAD
gh auth status
codex login status
tr2-codex
```

`tr2-codex` se place à la racine, vérifie la présence d'AGENTS.md et du journal, puis lance Codex natif. Le chargement effectif de l'AGENTS.md a été observé via `thread/start` du serveur local, sans démarrer de tour d'inférence. Le journal doit être lu par l'agent selon AGENTS.md. Aucune configuration Codex permanente n'a été modifiée : le défaut natif observé est **read-only / on-request**, et non C3. Si la prochaine mission autorise l'écriture, utiliser explicitement une session adaptée, par exemple `tr2-codex --sandbox workspace-write --ask-for-approval on-request`. Cela ne donne aucune permission Git ou matérielle absente du contrat. Ne pas utiliser un bypass des approbations.

Avant mise à jour du dépôt :

```bash
git status --short
git fetch origin
git log --oneline --left-right HEAD...origin/main
# Seulement si le clone est propre et sans divergence :
git pull --ff-only origin main
```

Ne pas réinitialiser un clone sale. Aucun merge/rebase automatique, force push ou clean destructif. L'identité Git est locale au clone ; les permissions de commit/push dépendent de chaque mission.

Firmware et supervision :

```bash
tr2-validate             # 102 tests et cross-build, SDK Linux chargé par le lanceur
tr2-validate --cross-build-only
tr2-supervision validate # restore, build, tests, résultats hors sources
tr2-supervision run --config /chemin/vers/configuration-validee.json
```

La supervision attend une configuration opératoire réelle. COM7 et les valeurs de l'exemple ne sont pas qualifiés ; CN1/COM3 du ST-LINK n'est pas l'adaptateur RS-485. Sans configuration, le lanceur atteint le service et affiche son usage : ce contrôle ne qualifie pas une liaison physique. Les fichiers SQLite doivent rester hors Git et leurs chemins sont relatifs au fichier de configuration.

Outils ST/GDB depuis PowerShell, sans toucher au MCU :

```powershell
& 'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe' --version
& 'C:\ST\STM32CubeCLT_1.22.0\GNU-tools-for-STM32\bin\arm-none-eabi-gdb.exe' --version
& 'C:\ST\STM32CubeCLT_1.22.0\STLink-gdb-server\bin\ST-LINK_gdbserver.exe' --version
```

La chaîne physique déjà qualifiée utilise le serveur ST et GDB Windows local, compilations WSL. Pour une mission physique autorisée, relire le rapport `RAPPORT_QUALIFICATION_PHYSIQUE.md`, identifier le SN, vérifier les empreintes du nouvel artefact et contrôler la microSD : le firmware courant peut écrire au boot. Ne pas copier un breakpoint par adresse dans un autre ELF. OpenOCD et gdb-multiarch sont disponibles ; leur transport matériel alternatif n'est pas qualifié. Aucun nouveau flash/debug nécessaire pour la migration.

Après fermeture de Debian, `wsl --terminate Debian` puis `tr2-shell` permettent une reprise ; ne terminer la distribution qu'après sauvegarde et arrêt des tâches. Le redémarrage réel a été testé dans cette mission.

GitHub : `gh` est un lanceur natif vers `/usr/bin/gh` qui récupère le jeton depuis GCM/coffre Windows uniquement pour le processus enfant. Aucun jeton n'est exporté dans le shell. Ne pas lancer `gh auth token` dans un journal. En cas d'expiration, authentification interactive avec `/usr/bin/gh auth login`, puis `python3 "$HOME/dev/msm/scripts/gcm-bridge.py" import-gh` : la copie en clair n'est retirée qu'après vérification coffre et identité. Sans coffre accessible, le pont échoue explicitement, sans fallback en clair.
