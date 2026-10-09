# T1000 — Procédure reproductible de préparation

Date de rédaction : 10 octobre 2026. Périmètre : préparation du poste Windows/WSL et vérification des dépôts APT avant installation des outils.

Cette procédure décrit les étapes à reproduire. Les résultats effectivement rapportés sont dans [JOURNAL_W1_W2.md](JOURNAL_W1_W2.md) ; les versions observées et candidates sont dans [VERSIONS.md](VERSIONS.md). Les commandes ci-dessous sont une procédure à exécuter sur le poste cible, pas des commandes exécutées par Codex pendant la mission documentaire.

## 1. Préparer et vérifier le socle Windows / WSL

Socle de référence rapporté par l'utilisateur : Windows 11 Pro 25H2, build 26200.9457 ; WSL 3.0.1 ; fonctionnalité VirtualMachinePlatform activée ; Debian 13.5 Trixie sous WSL2.

Sur un nouveau poste, préparer Windows, activer VirtualMachinePlatform et installer WSL ainsi que Debian selon leur procédure d'installation officielle. Consigner les versions réellement obtenues : les numéros ci-dessus décrivent le poste de référence, sans garantir leur disponibilité future. Les commandes initiales d'installation Windows/WSL et les éventuels redémarrages ne sont pas conservés dans les preuves disponibles ; cette partie n'est donc pas une reconstruction exacte de la session.

Dans PowerShell, vérifier le socle WSL :

```powershell
wsl --version
wsl --list --verbose
Get-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform
```

La dernière commande nécessite une session administrateur. Vérifier que Debian utilise la version 2 de WSL et que VirtualMachinePlatform est activée. Relever l'édition, la version et le build Windows dans les informations système.

Créer l'utilisateur Linux `lolo` lors de la préparation Debian et ouvrir sa session. Dans Debian, vérifier :

```sh
whoami
printf '%s\n' "$HOME"
cat /etc/os-release
cat /etc/debian_version
```

Le poste de référence utilise `lolo` et `/home/lolo`. Conserver le travail Linux dans son répertoire personnel. Ne pas enregistrer les mots de passe dans le dépôt.

## 2. Vérifier les sources APT réelles

Dans Debian :

```sh
ls -la /etc/apt /etc/apt/sources.list.d
apt-config dump
cat /etc/apt/sources.list.d/0000debian.sources
```

Le fichier observé est `0000debian.sources`, au format deb822. Ne pas conclure à l'absence de dépôts parce que `/etc/apt/sources.list` ou `debian.sources` ne produit aucune sortie.

Contenu de référence :

```text
Types: deb
URIs: https://deb.debian.org/debian
Suites: trixie trixie-updates trixie-backports
Components: main
Signed-By: /usr/share/keyrings/debian-archive-keyring.pgp

Types: deb
URIs: https://security.debian.org/debian-security
Suites: trixie-security
Components: main
Signed-By: /usr/share/keyrings/debian-archive-keyring.pgp
```

Comparer le fichier réel à ce contenu. Sur une nouvelle installation, si une adaptation est nécessaire, examiner d'abord tous les fichiers de sources afin d'éviter les entrées en double ; préserver les configurations préexistantes et vérifier la présence du keyring indiqué. Ne pas renommer ou écraser automatiquement une source existante.

## 3. Actualiser les index et relever les candidats

```sh
sudo apt update
apt list --upgradable
apt-cache policy git cmake ninja-build gcc-arm-none-eabi
```

`apt update` actualise les index ; il n'installe pas les outils et ne met pas à niveau les 41 paquets signalés lors de la session de référence. Le nombre de paquets upgradables et les candidats peuvent évoluer.

Consigner les résultats réels dans le journal et l'inventaire. Lors de la session rapportée, les quatre outils ont tous `Installed: (none)`. Les versions candidates sont Git 2.47.3, CMake 3.31.6, Ninja 1.12.1 et GCC ARM 14.2.rel1 ; [VERSIONS.md](VERSIONS.md) conserve les versions complètes des paquets Debian.

## 4. Point d'arrêt et suite

La préparation documentée s'arrête après la vérification APT. L'installation des outils, la mise à niveau des paquets, le clonage de travail dans Debian, les builds et la qualification de la chaîne matérielle restent à effectuer et à consigner dans une mission ultérieure. Aucune installation de ces quatre outils n'est démontrée ici.

L'état BitLocker rapporté est un instantané : chiffrement à 93.3 % en cours, protection désactivée, aucun protecteur. Il ne constitue pas un état final ni une configuration à reproduire. Vérifier séparément son état final et la gestion des protecteurs sans publier de clé de récupération. Cette procédure ne modifie pas BitLocker.

## 5. Critères de contrôle

- Versions Windows, WSL et Debian relevées sur le poste cible.
- Debian exécutée sous WSL2 ; utilisateur et répertoire personnel vérifiés.
- Sources APT et keyring examinés, sans doublon introduit.
- `apt update` terminé sans erreur ; candidats et état installé relevés.
- Journal distinguant observations, procédure prévue et étapes restant à faire.

Ces contrôles portent sur la préparation du poste. Ils ne prouvent ni compilation, ni flash, ni debug, ni validation physique.
