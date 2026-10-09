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
