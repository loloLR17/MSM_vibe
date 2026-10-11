# T1000 — Architecture définitive

Décision du 10 octobre 2026, mission `T1000-20261010-CLOTURE-001`.

La racine `/home/lolo/dev/msm/` est demandée explicitement par l'utilisateur. Les documents et configurations inspectés ne fixent pas le nom exact du sous-dossier du dépôt. La mission autorise alors une décision cohérente : **`projects/MSM_vibe`**, conservant le nom du dépôt et séparant les fonctions.

```text
/home/lolo/dev/msm/
  projects/MSM_vibe/              unique clone Git actif
  tools/STM32CubeU5-v1.9.0/       SDK officiel commun, checkout Git avec HAL/CMSIS
  scripts/                       liens vers les scripts versionnés T1000
  backups/                       archives vérifiées de migration et anciens espaces
  reports/2026-10-10-cloture/     preuves locales, logs, TRX, artefacts, rapports historiques
  cache/                         sorties temporaires communes régénérables
  tmp/                           essais et fichiers temporaires

~/.local/bin/                    points d'entrée Linux (tr2-*, gh, Codex)
~/.local/opt/codex/              installation npm native, version 0.162.1
~/.local/share/tr2/environment.sh  entrée de compatibilité vers scripts/environment.sh
~/.nuget/, ~/.npm/              caches standards des gestionnaires, hors dépôt
~/.codex/                       état et authentification personnels, hors Git

C:\ST\STM32CubeCLT_1.22.0\       outils ST Windows, pilotes et GDB
%LOCALAPPDATA%\T1000\            GCM, MinGit, pont coffre et lanceurs Windows
Documents\T1000-CLOTURE-20261010\ copie du dossier de clôture et sauvegarde
Documents\T1000-TR2-FINAL-20261010\ preuves historiques logicielles/physiques
Documents\T1000-STM32-20261010\    première qualification et installateur officiel conservé
```

Les projets et le SDK sont sur le filesystem Linux. Aucun build Linux ne dépend des documents ou archives Windows. Seul l'accès authentifié Git/gh utilise le coffre Windows, via l'interop WSL ; une panne de ce pont n'empêche pas une compilation hors ligne des sources déjà présentes.

Exception aux sorties communes : les scripts firmware canoniques créent `05_Firmware/build-host-validation` et `build-stm32-p11c`. La supervision conserve ses `bin/obj` standards : son test d'architecture déduit la racine des sources depuis la DLL. Ces sorties sont ignorées par Git et régénérables ; elles ne sont ni des sources ni des preuves uniques. Le placement .NET dans un cache extérieur a été essayé puis abandonné après un échec réel, sans modifier le test.

La migration est un déplacement du clone et du SDK, pas un reclonage : Git, identité locale et correctif non commité conservés. Les empreintes de 3 408 fichiers de travail sont identiques avant/après. `git fsck --full` réussit. Une archive du clone avant migration et un patch exact sont conservés. L'ancien chemin `/home/lolo/projects/MSM_vibe` n'est plus un clone actif ni une dépendance. Les archives historiques restent accessibles, avec leurs anciens chemins considérés comme des instantanés.

Les liens `scripts/` pointent vers `projects/MSM_vibe/00_Environnement_developpement/T1000/scripts`. Les lanceurs Windows `.cmd` sont copiés dans `%LOCALAPPDATA%\T1000\bin`, ajouté au PATH utilisateur ; le PATH système est inchangé. Les commandes Windows Git passent par WSL et ne manipulent pas directement le clone via Git Windows/UNC.

Les anciens espaces ont été réunis dans `backups/historical-workspaces-20261010.tar.gz` : 7 779 fichiers comparés octet par octet à l'archive avant leur retrait. Le clone pré-migration reste dans une archive distincte. L'extraction Windows CubeCLT de 967 Mo est encore verrouillée par l'ancien installateur élevé ; elle est conservée, avec ZIP officiel de contenu identique archivé. Aucune nouvelle UAC ni arrêt forcé n'a été utilisé pour cet élément facultatif.

## Liaison temporaire T1000 vers HP Brouette — SSH (2026-10-11)

Le T1000 reste le poste de développement et de pilotage Codex ; le HP Windows 10 « Brouette » (nom observé `LAPTOP-3F6J7STJ`) héberge la supervision et l'adaptateur USB-RS485 sur COM7. Les deux postes utilisent le Wi-Fi privé. Codex sous WSL emploie le client SSH Windows `/mnt/c/Windows/System32/OpenSSH/ssh.exe` et son alias `tr2-hp` avec une clé dédiée ; aucune installation Codex sur HP n'est nécessaire.

La mission `TR2-20261011-SSH-HP-001` est DONE pour l'accès SSH non interactif, l'inventaire et les diagnostics OFFLINE. Outils versionnés : `00_Environnement_developpement/T1000/ssh-hp/` (`run_hp.py`, `hp-task.ps1`, tests, README). Les scripts imposent des tâches bornées, vérifient l'identité distante, collectent les sorties et codes de retour, appliquent des délais et un verrou coopératif. Consulter le README pour les commandes exactes et le retrait réversible.

**Limites :** aucun accès physique COM7 ni trafic Modbus n'a été exécuté par cette mission ; la qualification de production M014-D demeure BLOCKED. Toute campagne physique future exige un contrat distinct et des garde-fous d'exclusivité du maître, délais et journal. Le compte SSH distant est administrateur ; conserver clés et secrets hors Git et prévoir une réduction des privilèges. Cette infrastructure est provisoire jusqu'à l'arrivée du dock USB. Voir `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` pour les preuves et réserves.
