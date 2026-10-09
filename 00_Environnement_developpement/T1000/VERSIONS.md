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
