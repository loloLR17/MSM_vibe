# T1000 — Audit et corrections de clôture

Mission `T1000-20261010-CLOTURE-001`, 10 octobre 2026. **VALIDÉ AVEC RÉSERVES complémentaires**, chaîne fonctionnelle prête. Les contrôles administrateur facultatifs ne bloquent pas la qualification TR2, selon décision explicite de l'utilisateur.

| Écart initial | Correction / traitement réel |
|---|---|
| Clone actif sous projects sans architecture finale validée | Déplacement vers dev/msm/projects/MSM_vibe après sauvegarde vérifiée et recherche documentaire |
| SDK et lanceur pointant sur anciens emplacements | SDK déplacé dans tools ; environnement et lanceurs corrigés, tests depuis les chemins définitifs |
| Journal obligatoire absent, historique HP inaccessible | Escalade, autorisation humaine explicite, nouvel état reconstruit et identifié ; aucun historique inventé |
| Test time_history_store utilisant une structure indéterminée | Correctif d'une ligne déjà autorisé conservé, revalidé ; assertions et production inchangées |
| systemd-binfmt failed sous WSL 3.0.1 | Drop-in ciblé enregistrant Python sans flush global ; service actif et système running après redémarrage |
| Jeton gh dans hosts.yml en clair | GCM officiel et MinGit de support ; transfert au coffre Windows, API vérifiée, retrait de oauth_token ; helper Git local |
| Sorties firmware non ignorées | Deux exclusions précises dans .gitignore, aucun code ou test masqué |
| Copies temporaires et scripts dispersés | Sauvegarde des preuves, archivage compressé et nettoyage ciblé après validation |
| Installation CubeCLT extraite et ZIP redondants | Empreinte de l'exécutable du ZIP identique à celle de l'extraction ; ZIP archivé ; extraction régénérable encore verrouillée, suppression différée sans UAC |

Aucun composant fonctionnel n'est réinstallé. MinGit est ajouté uniquement car GCM Windows exige git.exe, absent auparavant ; il ne devient pas le Git du clone Linux. Versions officielles téléchargées et SHA256 confrontés aux digests des releases GitHub : GCM 3.0.1, `bc51abd9e613a856f909b5ea9244b5a265a4fc8a85383e2982d76034d80cfe3f` ; MinGit 2.56.0.2, `da35e72aa21c005a5a0d298cfbae110bc1609a815730ea0dde84b01a1b3cd3be`. GCM exécutable signé Microsoft, Authenticode valide.

Windows 11 Pro 25H2, build 26200.9457 ; les champs historiques ProductName/Get-ComputerInfo indiquent Windows 10, mais le Caption OS et le build identifient Windows 11. Debian 13.7, WSL 3.0.1, noyau 6.18.40.1. Une seule installation CubeCLT sous C:\ST. Pilotes ST-LINK 2.1.0.0, périphériques présents OK ; catalogues ST avec signature valide. Le booléen IsSigned fourni par CIM ne remplace pas la vérification de catalogue. Aucun changement de pilotes nécessaire.

La correction binfmt est réversible dans `/etc/systemd/system/systemd-binfmt.service.d/t1000-wsl-selective.conf` : ExecStart explicite du seul fichier distro Python et suppression d'ExecStop global. Aucun préfixe d'ignorance des erreurs n'est utilisé. WSL protège WSLInterop ; le flush global échouait. Le registre Python et WSLInterop restent actifs. `wsl --update` et `--web-download` indiquent que la version stable est déjà installée ; 3.0.2 est une préversion, non installée. Réexaminer le drop-in après une mise à jour stable corrigeant ce défaut ou l'installation d'un nouveau format binfmt.d.

Codex Windows et Linux restent 0.162.1. Le natif charge l'AGENTS.md du clone final, est authentifié ChatGPT et ses contrôles réseau passent. Son défaut lu est read-only/on-request ; la configuration C3 de la session opérateur Windows ne devient pas une configuration Linux permanente. Les paramètres Codex et de sécurité ne sont pas affaiblis.

Anomalies de validation corrigées : une première syntaxe de propriété MSBuild a échoué avant restore ; l'option officielle artifacts-path a ensuite compilé mais provoqué le défaut de localisation du test d'architecture. Retour aux emplacements standards sans modification de test ; 466/466 réussis. Le premier client de contrôle Codex RPC a expiré à cause de son lecteur stdout tamponné ; lecture par file thread corrigée, thread/start réussi sans tour modèle. Les logs d'échecs et résultats TRX sont conservés hors Git.

Réserves : contrôles sécurité Windows détaillés et décision BitLocker consignés séparément dans le dossier local privé, sans nouvelle UAC après l'instruction utilisateur. Pas de qualification RS-485/microSD/E4 fonctionnelle, de debug source ligne par ligne ou de transport GDB WSL. Ces points n'empêchent pas le cycle de développement qualifié et ne sont pas déclarés réussis. L'original du journal HP reste introuvable dans les sources accessibles ; un rapprochement pourra être fait s'il réapparaît.

Références : [WSL systemd](https://github.com/microsoft/WSL/blob/master/doc/docs/technical-documentation/systemd.md), [défaut WSL 3.0.1](https://github.com/microsoft/WSL/issues/41783), [coffres GCM](https://github.com/git-ecosystem/git-credential-manager/blob/main/docs/credstores.md), [AGENTS.md OpenAI](https://learn.chatgpt.com/docs/agent-configuration/agents-md).
