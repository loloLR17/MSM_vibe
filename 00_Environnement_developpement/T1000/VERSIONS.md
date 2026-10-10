# T1000 — Inventaire des versions

Instantané consolidé le 10 octobre 2026 à partir des informations rapportées par l'utilisateur et des sorties APT de la conversation « Préparer installation PC ». Ces valeurs ne décrivent pas automatiquement l'état actuel d'un autre poste.

Voir [JOURNAL_W1_W2.md](JOURNAL_W1_W2.md) pour les faits et leur provenance, et [INSTALLATION.md](INSTALLATION.md) pour la procédure.

## Socle rapporté

| Composant | Version / état | Nature de la preuve |
| --- | --- | --- |
| Windows | Windows 11 Pro 25H2, build 26200.9457 | Rapporté par l'utilisateur |
| WSL | 3.0.1 | Rapporté par l'utilisateur |
| VirtualMachinePlatform | Activée | Rapporté par l'utilisateur |
| Debian | 13.5 Trixie, WSL2 | Rapporté par l'utilisateur |
| Architecture APT | amd64 | Sortie apt-config dump |
| Utilisateur Linux | lolo | Rapporté par l'utilisateur |
| Répertoire personnel | /home/lolo | Rapporté par l'utilisateur |

La version du noyau WSL et les versions de SDK ou d'outils matériels ne sont pas renseignées faute de preuve.

## Outils APT : disponibles, pas installés

Commande source : `apt-cache policy git cmake ninja-build gcc-arm-none-eabi`, après `sudo apt update` réussi.

| Outil / paquet | Version amont candidate | Version Debian candidate complète | État installé observé |
| --- | --- | --- | --- |
| Git / git | 2.47.3 | 1:2.47.3-0+deb13u1 | (none) |
| CMake / cmake | 3.31.6 | 3.31.6-2 | (none) |
| Ninja / ninja-build | 1.12.1 | 1.12.1-1 | (none) |
| GCC ARM / gcc-arm-none-eabi | 14.2.rel1 | 15:14.2.rel1-1 | (none) |

Les préfixes `1:` et `15:` sont les epochs Debian, conservés dans les versions complètes. Les quatre candidats viennent de `trixie/main` à priorité 500.

CMake **4.3.4-1~bpo13+1** est également disponible via `trixie-backports/main` à priorité 100 ; il n'est pas le candidat sélectionné. Aucun outil ne doit être présenté comme installé sur la base de sa seule version candidate.

## Référence des dépôts et état temporaire

- Fichier de sources : `/etc/apt/sources.list.d/0000debian.sources`.
- Debian : `https://deb.debian.org/debian`, suites `trixie trixie-updates trixie-backports`.
- Sécurité : `https://security.debian.org/debian-security`, suite `trixie-security`.
- Composant : `main`.
- Keyring déclaré : `/usr/share/keyrings/debian-archive-keyring.pgp`.
- Dernière sortie fournie : `apt update` réussi, 41 paquets upgradables. Aucune mise à niveau démontrée.
- BitLocker rapporté : 93.3 % de chiffrement en cours, protection désactivée, aucun protecteur ; état final non vérifié.

Cet inventaire est un relevé, pas un verrouillage des dépendances : les candidats APT peuvent changer. Après installation, ajouter une entrée datée avec les versions effectivement installées et conserver la distinction entre ancien instantané et nouveau relevé. La chaîne de développement n'est pas encore qualifiée.


---

## 10 octobre 2026 — Versions effectivement contrôlées lors de la qualification locale

Mission d'archivage : `T1000-20261010-ARCHIVE-001`. Source : [rapport détaillé archivé](qualifications/2026-10-10/RAPPORT_T1000.md), [journaux et empreintes](qualifications/2026-10-10/MANIFESTE.md). Ce relevé complète les instantanés historiques ; il ne transforme pas les anciennes versions candidates en observations d'installation.

| Composant | Version contrôlée / paquet | État et portée |
| --- | --- | --- |
| Windows | 11 Pro 25H2, build 26200.9457, x64 | **VALIDÉ** |
| WSL / Debian | WSL 3.0.1 ; Debian 13.7 trixie sous WSL2, x86_64 | **VALIDÉ** |
| Noyau WSL | 6.18.40.1-microsoft-standard-WSL2 | **VALIDÉ** |
| Git | 2.47.3 ; 1:2.47.3-0+deb13u1 | **VALIDÉ**, version et GitHub public lecture |
| CMake | 3.31.6 ; 3.31.6-2 | **VALIDÉ**, génération/build |
| Ninja | 1.12.1 ; 1.12.1-1 | **VALIDÉ**, build C/C++ |
| GCC/G++ hôte | 14.2.0 | **VALIDÉ**, compilation, exécution, 2/2 CTest |
| GCC/G++ ARM | 14.2.1 20241119 ; gcc-arm-none-eabi 15:14.2.rel1-1 | **VALIDÉ**, Cortex-M33 minimal C/C++ |
| Binutils ARM | 2.44 ; 2.44-3+23+b1 | **VALIDÉ**, liaison et inspection ELF |
| Newlib | 4.5.0.20241231-1 | **VALIDÉ**, headers et liaison minimale avec memcpy |
| SDK .NET | 10.0.401 ; 10.0.401-1 | **VALIDÉ**, console C# restaurée/compilée/exécutée |
| Runtime .NET / ASP.NET Core | 10.0.12 ; 10.0.12-1 | **VALIDÉ**, inventaire ; runtime .NET exécuté |
| OpenOCD | 0.12.0 ; 0.12.0-3+b2 | **VALIDÉ** pour lancement ; cible **INSTALLÉ MAIS NON TESTÉ** |
| usbutils | 018 ; 1:018-2 | **VALIDÉ** pour version ; bus USB WSL **BLOQUÉ** |
| usbipd-win Windows | 5.3.0-54+Branch.master | **VALIDÉ**, version/list ; partage **NON APPLICABLE** |
| Node.js / npm Windows | 24.20.0 / 11.19.0 | **VALIDÉ**, lancement |
| Codex CLI Windows | 0.162.1 | **VALIDÉ**, lancement ; exécution locale Windows/WSL par l'agent démontrée |
| wget / dépôt Microsoft | 1.25.0-2 / packages-microsoft-prod 1.1-debian13 | Installés lors de la qualification source |
| libicu76 | 76.1-4 | Dépendance Debian de .NET, installée lors de la qualification source |
| GDB ARM et compléments STM32 | Non trouvés dans PATH/emplacements inspectés | **ABSENT** dans ce périmètre ; à examiner |

SSD physique : 256052966400 octets (environ 256 Go). Espace libre mesuré après tests : 194681454592 octets ; relevé Windows légèrement ultérieur : 194678382592 octets, soit environ **194,68 Go** dans les deux cas. Capacité virtuelle WSL distincte.

BitLocker actuel : **BLOQUÉ**, non vérifié faute de privilèges administrateur. ST-LINK, flash et debug physique non qualifiés. Aucun workload .NET additionnel requis pour la console. Node/Codex natifs Linux absents du PATH opérationnel ; usage Windows pilotant WSL validé. La qualification reste **PARTIELLE** pour l'audit complet et valide les capacités logicielles indiquées, sans validation matérielle STM32.
