# T1000 — Journal factuel W1 / W2

Date de consolidation : 10 octobre 2026. W1 et W2 désignent les étapes de préparation Windows/WSL puis Debian/APT ; aucune durée de deux semaines n'est démontrée.

Sources : observations explicitement rapportées par l'utilisateur dans la mission et sorties terminal de la conversation « Préparer installation PC » (identifiant `6ac7ef3b-3e3c-83ed-abdc-c920f3fa94de`). Codex n'a pas réexécuté ces commandes sur le poste cible. La procédure est dans [INSTALLATION.md](INSTALLATION.md), l'inventaire dans [VERSIONS.md](VERSIONS.md).

## W1 — Windows et WSL : état rapporté

| Élément | Observation rapportée par l'utilisateur |
| --- | --- |
| Système | Windows 11 Pro 25H2, build 26200.9457 |
| WSL | 3.0.1 |
| Fonctionnalité | VirtualMachinePlatform activée |
| Distribution | Debian 13.5 Trixie, WSL2 |
| Compte Linux | `lolo` |
| Répertoire personnel | `/home/lolo` |
| BitLocker | Chiffrement à 93.3 % en cours ; protection désactivée ; aucun protecteur |

Ces observations ne sont pas une vérification directe de Codex. L'heure exacte de la mesure BitLocker et son état ultérieur ne sont pas disponibles. Aucun achèvement du chiffrement ou rétablissement de protection n'est affirmé.

## W2 — Diagnostic des sources APT

1. Une première lecture de `/etc/apt/sources.list` et `/etc/apt/sources.list.d/debian.sources` n'a produit aucune sortie. Le premier `apt-cache policy git cmake ninja-build gcc-arm-none-eabi` a indiqué `Unable to locate package` pour les quatre noms.
2. L'inspection de `/etc/apt`, de `sources.list.d` et d'`apt-config dump` a identifié le fichier réel `/etc/apt/sources.list.d/0000debian.sources` (331 octets) et l'architecture APT `amd64`.
3. La lecture de ce fichier a montré les dépôts HTTPS Debian `trixie`, `trixie-updates`, `trixie-backports` et sécurité `trixie-security`, composant `main`, avec `Signed-By: /usr/share/keyrings/debian-archive-keyring.pgp`. Le contenu intégral est reproduit dans la procédure.
4. `sudo apt update` a réussi : 17.4 MB récupérés en 3 s selon la sortie, et **41 paquets upgradables** signalés.
5. Le second `apt-cache policy` a trouvé les candidats suivants :

| Paquet | Installed | Candidate |
| --- | --- | --- |
| git | (none) | 1:2.47.3-0+deb13u1 |
| cmake | (none) | 3.31.6-2 |
| ninja-build | (none) | 1.12.1-1 |
| gcc-arm-none-eabi | (none) | 15:14.2.rel1-1 |

CMake 4.3.4-1~bpo13+1 apparaît dans les backports avec priorité 100 ; le candidat reste 3.31.6-2 à priorité 500. Aucun choix explicite des backports n'est démontré. Aucune modification des sources APT n'est démontrée entre leur inspection et l'actualisation des index.

## État de préparation

- **OBSERVÉ dans les sorties utilisateur** : sources APT identifiées, actualisation réussie et versions candidates disponibles.
- **PRÉPARÉ** : procédure documentaire de reproduction de cette préparation.
- **NON VÉRIFIÉ / À FAIRE** : installation des quatre outils, mise à niveau des 41 paquets, compilation, tests hôte, cross-build, flash, debug et qualification physique sur ce nouveau poste.

« Rien installé encore » concerne les quatre outils inspectés, tous à `Installed: (none)` ; Debian et WSL sont déjà présents. La publication des documents GitHub ne constitue pas une installation sur Debian.

## Mission documentaire et dérogation ponctuelle

Identifiant documentaire de suivi : `T1000-20261010-DOC-001`. Contrat : messages explicites de l'utilisateur dans cette conversation Codex.

```yaml
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
```

Périmètre strict : création de `INSTALLATION.md`, `JOURNAL_W1_W2.md` et `VERSIONS.md` sous `00_Environnement_developpement/T1000/`.

HEAD distant initial contrôlé : `19c39d6a7a9c3d8fef4330a64be427563ed108cb`. L'arbre GitHub complet ne contenait ni ces fichiers, ni instruction AGENTS.md supplémentaire dans leurs répertoires. Le fichier racine AGENTS.md et le protocole V2-B3 ont été consultés. La mission TR2 historique présente dans MISSION_EN_COURS.md ne constitue pas le contrat de cette mission T1000.

**Dérogation explicitement validée par l'utilisateur le 10 octobre 2026** : exemption ponctuelle à l'exigence de présence du journal `Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md`, absent de la référence contrôlée (arbre complet et réponse GitHub 404). Cette dérogation vaut exclusivement pour cette mission documentaire T1000. Aucun journal TR2 n'est créé ou actualisé ; aucun firmware, AGENTS.md ou document de gouvernance TR2 n'est modifié.

Validation retenue : relecture documentaire, contrôle des trois ajouts et comparaison des contenus après publication. Aucune compilation n'est nécessaire pour ces seuls documents. La preuve de publication, le SHA final et la relecture distante sont à fournir dans le compte rendu final Codex ; ce journal ne prétend pas connaître le SHA du commit qui le contient.

Aucun mot de passe, token, clé de récupération BitLocker ou secret n'est conservé. La prochaine étape opérationnelle reste l'installation des outils dans une mission distincte, avec relevé de leurs versions réellement installées.

## 10 octobre 2026 — Complément W2-A à W2-E (observations transmises)

W2-A : Debian passée de 13.5 à 13.7 ; `apt update` et `apt list --upgradable` ont indiqué `All packages are up to date`.

W2-B : compte rendu de l'agent local : `git` 1:2.47.3-0+deb13u1, `cmake` 3.31.6-2, `ninja-build` 1.12.1-1, `build-essential` 12.12, `pkg-config` 1.8.1-4, tous `install ok installed`. Vérification exécutables Git 2.47.3, CMake 3.31.6, Ninja 1.12.1, GCC hôte 14.2.0, pkg-config 1.8.1. Réserve : erreur systemd lors de la configuration OpenSSH, ssh-agent non testé.

W2-C : sorties utilisateur confirmant `gcc-arm-none-eabi` 15:14.2.rel1-1 (GCC 14.2.1), `binutils-arm-none-eabi` 2.44-3+23+b1, `libnewlib-arm-none-eabi` et `libnewlib-dev` 4.5.0.20241231-1, tous installés. Compilation d'un objet C avec `-mcpu=cortex-m33 -mthumb -ffreestanding -Wall -Wextra -Werror` réussie ; `readelf` : ELF32, REL, ARM, EABI5. **VALIDÉ : compilation objet ARM minimale** ; édition de liens, flash et exécution sur cible non testés.

W2-D : `openocd` 0.12.0-3+b2 (exécutable 0.12.0) et `usbutils` 1:018-2 (lsusb 018), tous deux `install ok installed`. **VALIDÉ : installation des utilitaires** ; communication ST-Link non testée.

W2-E : Windows `usbipd-win` 5.3.0 installé via winget. Exécutable présent dans `C:\Program Files\usbipd-win\`, `usbipd list` fonctionnel, PATH système déjà correct ; après nouvelle session PowerShell, `usbipd --version` fonctionne sans modification. **VALIDÉ : outil Windows**, aucun périphérique partagé et aucun ST-Link branché lors du relevé.

W2-F : SDK .NET non installé ou vérifié. Ni VS Code ni Codex CLI ne sont attestés installés sur le T1000 : `Get-StartApps` ne trouve pas Codex, `Get-Command codex,code` ne renvoie rien. Ne pas confondre des sessions Codex/Work distantes avec une installation locale.

Source de ce complément : sorties utilisateur de la conversation ChatGPT et rapport d'agent expressément identifié. Ce complément ne remplace pas les observations historiques précédentes.


---

## 10 octobre 2026 — Qualification locale exécutée et archivage

Mission de qualification : `T1000-20261010-QUAL-LOCAL` ; mission documentaire : `T1000-20261010-ARCHIVE-001`. Les résultats ci-dessous proviennent de commandes exécutées directement par l'agent local, puis confrontées aux scripts et journaux conservés. Référence détaillée : [rapport détaillé archivé](qualifications/2026-10-10/RAPPORT_T1000.md) et [manifeste des preuves](qualifications/2026-10-10/MANIFESTE.md). Les observations historiques ci-dessus restent intactes ; elles décrivent des étapes antérieures.

- **VALIDÉ** : Dell Latitude 7320, Windows 11 Pro 25H2 build 26200.9457, WSL 3.0.1 et Debian 13.7 sous WSL2 (x86_64).
- **VALIDÉ** : Git 2.47.3 et accès GitHub public en lecture seule par `git ls-remote` ; CMake 3.31.6, Ninja 1.12.1, GCC/G++ hôte 14.2.0. Build C/C++ et 2/2 tests CTest réussis, exécutions attendues avec retour 0.
- **VALIDÉ** pour compilation et liaison minimales : GCC ARM 14.2.1, Binutils 2.44, Newlib 4.5.0.20241231-1 ; objets C/C++ Cortex-M33 et ELF32 ARM EABI5/v8-M.mainline avec `memcpy` lié. Aucune qualification d'un firmware STM32 complet ni d'une bibliothèque standard C++ embarquée.
- **VALIDÉ** : SDK .NET 10.0.401 installé dans Debian via le dépôt APT Microsoft officiel pour Debian 13 ; runtime .NET 10.0.12. Console C# restaurée, compilée sans erreur/avertissement et exécutée avec résultat attendu. Les outils déjà opérationnels n'ont pas été réinstallés.
- **VALIDÉ** pour lancement/inventaire : OpenOCD 0.12.0, usbutils 018, usbipd-win 5.3.0, Node.js Windows 24.20.0, npm 11.19.0 et Codex CLI Windows 0.162.1. Commandes Windows et WSL réellement exécutées par Codex dans la session C3 autorisée ; configuration permanente inchangée. Le lanceur Codex Windows hérité dans WSL échoue faute de Node Linux ; l'interop explicite Windows fonctionne.
- **VALIDÉ** pour mesure : SSD physique Samsung 256 Go, environ 194,68 Go libres après installation/tests. La capacité virtuelle WSL d'environ 1 To n'est pas celle du SSD.
- **BLOQUÉ** : état BitLocker actuel non vérifié, lectures refusées faute de privilèges administrateur Windows. L'ancien instantané à 93,3 % ne constitue pas son état actuel.
- **INSTALLÉ MAIS NON TESTÉ** : communication ST-LINK avec les utilitaires disponibles ; aucun ST-LINK visible et aucun bus USB WSL disponible lors du contrôle. Flash et debug physique non qualifiés ; **NON APPLICABLES** au contrat de cette qualification.
- **ABSENT** dans les emplacements inspectés : GDB ARM/gdb-multiarch et compléments STM32Cube ; recherche non exhaustive. CubeProgrammer, CubeCLT, CubeIDE et CubeU5 restent à examiner selon le workflow HP et les versions requises, sans installation automatique.

Statut de qualification : **PARTIEL** pour l'audit complet ; critères logiciels **VALIDÉS**, qualification physique STM32 non démontrée. Les erreurs facultatives et limites sont conservées dans l'archive : `file` absent après les builds réussis, contrôle complémentaire par `objdump`, avertissement workloads .NET sans échec de la console, absence de bus USB WSL.

Archivage : rapport original local retrouvé ; copie publique expurgée des noms de compte/hôte, six journaux et deux scripts inspectés, empreintes originales/publiques dans le manifeste. Aucun binaire ou cache conservé dans Git. Aucune nouvelle installation ou qualification matérielle pendant cette mission documentaire. Autorisations : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`, limitées au périmètre T1000. Aucun firmware, fichier de gouvernance ni journal TR2 modifié.
