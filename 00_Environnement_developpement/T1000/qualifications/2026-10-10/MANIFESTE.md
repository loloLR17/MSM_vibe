# T1000 — Manifeste de qualification du 10 octobre 2026

Mission d'archivage : `T1000-20261010-ARCHIVE-001`. Contrat : demande explicite de l'utilisateur dans la session locale ; branche `main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`.

HEAD initial d'archivage : `8c03929184581a915c06fa488b9983c306dfc910`. Qualification source : `T1000-20261010-QUAL-LOCAL`, même référence documentaire initiale. Le statut de qualification reste **PARTIEL** pour l'audit complet, avec critères logiciels **VALIDÉS** et qualification physique STM32 non démontrée. La publication documentaire ne change pas ce statut.

## Provenance et transformations

Les neuf fichiers proviennent du dossier local `$env:LOCALAPPDATA\Temp\T1000-qualification-20261010-YRJQ2U\`, retrouvé et inspecté. Rapport et huit scripts/journaux conservés ; aucun binaire, cache, fichier de build ou paquet d'installation publié. Les originaux locaux sont préservés.

Les copies publiques occultent les chemins de compte Windows/Linux, le nom du compte Linux et le nom d'hôte WSL avec les marqueurs `<UTILISATEUR>` et `<HOTE_WSL>`. Les chemins système, versions, commandes, résultats, erreurs et codes de retour sont conservés. Les substitutions sont littérales ; aucune suppression de ligne ou correction technique du rapport original. Les empreintes originales et publiques diffèrent quand une transformation a eu lieu : une copie expurgée n'est pas présentée comme identique à l'original.

`supplemental-tests.log` contient des octets non UTF-8 dans l'avertissement français de CMD. Lors de la conversion en texte UTF-8 public, ces octets sont remplacés par U+FFFD ; cette perte d'encodage est déclarée, les commandes, versions et codes de retour ASCII restent lisibles. Les fins de ligne et éventuels BOM sont conservés pour les fichiers valides UTF-8.

`windows-final.log` est déjà un relevé textuel historique produit avec retrait des NUL de la sortie WSL et écriture UTF-8 PowerShell. L'empreinte originale ci-dessous porte sur ce fichier conservé, pas sur une sortie terminal brute reconstituée.

## Empreintes SHA-256

Les noms sont relatifs au présent dossier ; la provenance de chaque original est le dossier source décrit ci-dessus.

| Fichier | Octets original / archive | SHA-256 original local | SHA-256 copie publique | Transformation |
| --- | ---: | --- | --- | --- |
| [RAPPORT_T1000.md](RAPPORT_T1000.md) | 18268 / 18358 | `e6355b4f280f25924386f63017af868121a9745f631a957ab8ac27295349d845` | `e286e303d45a66b9a8d4c865d46f125b6ac3924093b3f30a8caa9218a15afa9a` | occultation compte/chemin/hote |
| [audit-final.log](audit-final.log) | 3557 / 3557 | `247befdb7dd78dfde261a5b70f8420a13f44724a36f1c9d7ad87b4ddd87a5277` | `247befdb7dd78dfde261a5b70f8420a13f44724a36f1c9d7ad87b4ddd87a5277` | aucune : copie identique octet pour octet |
| [dotnet-tests.log](dotnet-tests.log) | 2594 / 2621 | `a6c5731016c13367997d9a45131ffd0632913ccb937819e47c6634bb75c6541e` | `b8b3488d414834128d99f3c9ab152f538ebab49901c204021760e45dc58d6373` | occultation compte/chemin/hote |
| [environment-final.log](environment-final.log) | 4943 / 4972 | `501d8b15079cf62ab5cb99a82c186fa6bb1d77c77f2416029370cd0b258072d6` | `f4bc3fe613e634a23691b1d8b335823e9853657888527ad0226e09583a623d96` | occultation compte/chemin/hote |
| [host-arm-tests.log](host-arm-tests.log) | 6437 / 6491 | `4fab7f7778e112711bdd21936e20e66e45072825f5f0711c23cc40beed1183c2` | `76bef861eeb89ced2130789c83e143c9f6305b5af538f50b3a26cc6e06d20bb4` | occultation compte/chemin/hote |
| [run-dotnet.sh](run-dotnet.sh) | 1134 / 1134 | `e21cc033db05ab72a38f546b653a2b0c0f5010c9c683577ed2b39815e5b4b90b` | `e21cc033db05ab72a38f546b653a2b0c0f5010c9c683577ed2b39815e5b4b90b` | aucune : copie identique octet pour octet |
| [run-tests.sh](run-tests.sh) | 2195 / 2195 | `3ebd17addb89be9084e1eae18ba2f70d53f2b0aad532f9ce60834b2957b7eb07` | `3ebd17addb89be9084e1eae18ba2f70d53f2b0aad532f9ce60834b2957b7eb07` | aucune : copie identique octet pour octet |
| [supplemental-tests.log](supplemental-tests.log) | 1037 / 1064 | `24e5e0c5e18bae7284af1b90a3a83042c7d6469eb097e52065de4f8896518d1e` | `4bb3251253ad0d0b274f413054131547650e06b77b4281b9364678ef0827b472` | octets non UTF-8 remplaces par U+FFFD (avertissement CMD historique); occultation compte/chemin/hote |
| [windows-final.log](windows-final.log) | 1313 / 1313 | `aacb1fb6edb0d0b7b4f8ace749882497dde1cda8cb798b1016ef1c3a25746893` | `aacb1fb6edb0d0b7b4f8ace749882497dde1cda8cb798b1016ef1c3a25746893` | aucune : copie identique octet pour octet |

Le manifeste est une métadonnée créée pendant l'archivage ; il ne contient pas sa propre empreinte. Pour contrôler les neuf preuves publiques depuis ce dossier :

```sh
sha256sum RAPPORT_T1000.md audit-final.log dotnet-tests.log environment-final.log host-arm-tests.log run-dotnet.sh run-tests.sh supplemental-tests.log windows-final.log
```

## Cohérence et limites des preuves

- C/C++ hôte : `host-arm-tests.log` consigne compilation, exécutions et 2/2 tests CTest réussis, chacun avec retour 0.
- Cortex-M33 : compilations C et C++, liaison Newlib, ELF ARM EABI5/v8-M.mainline et symbole `memcpy` observés. L'échec final de `file` (127) est conservé ; `audit-final.log` consigne la vérification de format par `objdump` (0). Le script historique est conservé sans correction et peut donc reproduire cet échec facultatif si `file` reste absent.
- C# : `dotnet-tests.log` consigne SDK 10.0.401, runtimes 10.0.12, restauration, build sans erreur et exécution réussie. Avertissement de workloads conservé ; `supplemental-tests.log` consigne ensuite `dotnet workload list` avec retour 0.
- Git : accès public en lecture à `MSM_vibe/main` et SHA consignés dans `audit-final.log`. Cela ne démontre pas un accès GitHub privé ni un push lors de la qualification source.
- Disque : le rapport donne 194681454592 octets libres après tests ; `windows-final.log`, relevé légèrement ultérieur, donne 194678382592 octets. Les deux correspondent à environ 194,68 Go ; aucune égalité exacte des deux mesures n'est prétendue. Capacité physique SSD : 256052966400 octets d'après le rapport ; la capacité virtuelle WSL n'est pas celle du SSD.
- BitLocker : erreurs administratives décrites dans le rapport ; aucune sortie brute dédiée n'est disponible parmi les neuf fichiers. Les commandes d'installation APT et le second essai Codex depuis un chemin Windows sont également décrits dans le rapport et la session source, sans journal brut distinct conservé ici. Aucune preuve manquante reconstituée.
- Le rapport est la synthèse originale expurgée, pas une capture brute de toutes les commandes. `environment-final.log` confirme l'état Debian et les paquets installés ; `windows-final.log` confirme WSL et les utilitaires Windows.
- OpenOCD, usbutils et usbipd : lancement/version vérifiés ; communication ST-LINK **INSTALLÉ MAIS NON TESTÉ**, énumération USB WSL **BLOQUÉE** sans bus, flash/debug **NON APPLICABLES** au contrat source. GDB ARM et compléments STM32 non trouvés dans les emplacements inspectés : **ABSENTS** dans ce périmètre, recherche non exhaustive.

## Reproductibilité et validation d'archivage

Les deux scripts génèrent eux-mêmes les sources minimales. Pour une nouvelle qualification, les copier dans un répertoire de travail vide hors de tout projet et exécuter `bash run-tests.sh` puis `bash run-dotnet.sh` avec les dépendances déjà installées. Ces commandes ne sont pas relancées pendant la mission d'archivage ; aucun résultat nouveau n'est revendiqué. Le script C# confine DOTNET_CLI_HOME et NUGET_PACKAGES au répertoire de travail.

Validation documentaire : lecture des sources, confrontation rapport/journaux, contrôle des transformations et empreintes, conservation intégrale des préfixes historiques des trois documents de suivi, contrôle des liens locaux, syntaxe des scripts et diff limité au périmètre autorisé. Publication soumise au contrôle de non-divergence de main ; SHA du commit et preuve de relecture distante fournis dans le rapport final de session. Aucun document de gouvernance TR2 ni journal TR2 modifié.
