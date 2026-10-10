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


---

## 6. Complément du 10 octobre 2026 — Installation et qualification réellement exécutées

Mission d'archivage : `T1000-20261010-ARCHIVE-001`. La mission locale précédente `T1000-20261010-QUAL-LOCAL` a réalisé l'installation de .NET et les tests. Cette section complète la procédure historique ; aucune nouvelle installation n'est effectuée pendant l'archivage. Le [rapport détaillé archivé](qualifications/2026-10-10/RAPPORT_T1000.md) est la référence des commandes réellement exécutées, codes de retour, corrections et limites ; le [manifeste](qualifications/2026-10-10/MANIFESTE.md) décrit les preuves conservées et leurs transformations.

### État atteint

Windows 11 Pro 25H2 build 26200.9457, WSL 3.0.1 et Debian 13.7 sous WSL2. Chaîne hôte **VALIDÉE** : GCC/G++ 14.2.0, CMake 3.31.6, Ninja 1.12.1, avec compilation/exécution C/C++ et 2/2 tests CTest. Chaîne Cortex-M33 minimale **VALIDÉE** : GCC ARM 14.2.1, Binutils 2.44, liaison Newlib et inspection ELF. SDK .NET 10.0.401/runtime 10.0.12 **VALIDÉS** par console C# restaurée, compilée et exécutée.

Git 2.47.3 et GitHub public en lecture seule **VALIDÉS**. OpenOCD 0.12.0 et usbipd-win 5.3.0 vérifiés pour leur lancement/inventaire ; Codex CLI Windows 0.162.1 et l'exécution de commandes Windows/Debian par l'agent sont démontrés. SSD physique 256 Go, environ 194,68 Go libres lors des contrôles après tests.

### Méthode .NET employée

[Instructions officielles Microsoft pour Debian 13](https://learn.microsoft.com/en-us/dotnet/core/install/linux-debian). Après audit, le SDK était absent et les autres outils de build opérationnels. `wget` a été ajouté via APT Debian ; le paquet officiel `packages-microsoft-prod.deb` pour Debian 13 a ajouté le dépôt Microsoft, puis `dotnet-sdk-10.0` a été installé avec ses dépendances. Les sources Debian préexistantes ont été conservées ; pas de script dotnet-install ni de mise à niveau générale. Les paquets .NET proviennent du dépôt Microsoft, `libicu76` du dépôt Debian.

Séquence exécutée pendant la qualification source, depuis un répertoire temporaire dédié :

```sh
apt-get install -y --no-remove wget
wget -O packages-microsoft-prod.deb https://packages.microsoft.com/config/debian/13/packages-microsoft-prod.deb
dpkg-deb --info packages-microsoft-prod.deb
sha256sum packages-microsoft-prod.deb
dpkg -i packages-microsoft-prod.deb
apt-get update
apt-cache policy dotnet-sdk-10.0
apt-get -s install dotnet-sdk-10.0
apt-get install -y --no-remove dotnet-sdk-10.0
```

Les opérations APT/dpkg ont utilisé l'accès root WSL déjà disponible via `wsl.exe -d Debian -u root`, après l'échec de `sudo -n true`. Aucun changement sudoers ou compte par défaut. Les tests ont utilisé le compte Linux ordinaire. Pour une reproduction, vérifier d'abord l'état installé et les sources : ne pas réinstaller un outil déjà fonctionnel. L'empreinte du paquet de dépôt téléchargé est dans le rapport ; elle a été relevée, sans comparaison à une empreinte indépendante publiée.

### Reproduction des validations minimales

Les [scripts et journaux archivés](qualifications/2026-10-10/MANIFESTE.md) conservent les programmes et résultats. Copier `run-tests.sh` et `run-dotnet.sh` dans un nouveau répertoire vide hors dépôt/projet, puis les exécuter avec bash dans Debian, avec les dépendances déjà présentes. Les scripts créent leurs propres fichiers de test ; ils ne doivent pas être lancés au milieu de travaux existants.

Le script hôte/ARM historique termine par `file`, absent lors du contrôle : cet échec facultatif 127 est conservé. Les compilations et liens précédents ont chacun réussi ; la vérification complémentaire par `arm-none-eabi-objdump -f` a confirmé ELF32 ARMv8-M et est consignée dans `audit-final.log`. Ne pas assimiler le retour global de ce script à l'échec des builds déjà consignés. Le script C# confine son cache et DOTNET_CLI_HOME au répertoire de test ; aucun workload additionnel n'est requis pour la console.

Sous PowerShell, employer `npm.cmd` et `codex.cmd` selon le relevé Windows, sans assouplir la politique d'exécution. Les outils Linux de build s'utilisent via `wsl.exe -d Debian`. Le lanceur Codex hérité du montage Windows dans le PATH Linux échoue faute de Node Linux ; ce n'est pas une installation native Linux opérationnelle. L'interop explicite `cmd.exe /c codex.cmd --version` fonctionne depuis un répertoire Windows monté dans WSL. Aucune configuration permanente Codex modifiée.

### Suite et limites

- **BLOQUÉ** : lecture BitLocker actuelle, nécessitant des privilèges administrateur Windows. L'ancien état rapporté ne permet pas de conclure sur le chiffrement/protection actuels.
- **INSTALLÉ MAIS NON TESTÉ** : communication ST-LINK. Aucun ST-LINK visible ; aucun bus USB WSL disponible lors des relevés. Ne pas partager/connecter automatiquement de périphérique.
- **NON APPLICABLE** au contrat source : flash et debug physique ; aucune qualification matérielle STM32 réalisée.
- **ABSENT** dans les emplacements inspectés : GDB ARM/gdb-multiarch et compléments STM32Cube. Examiner CubeProgrammer, CubeCLT, CubeIDE et CubeU5 selon le workflow HP, versions et procédure d'installation officielle ; aucune installation automatique décidée ici.

Statut final de qualification source : **PARTIEL** pour l'audit complet, critères logiciels **VALIDÉS**. La publication de cette documentation ne qualifie ni flash, ni debug, ni fonctionnement physique.
