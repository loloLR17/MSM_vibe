# Rapport T1000 - qualification locale du 10 octobre 2026

Statut global : PARTIEL pour l'audit complet du poste (lecture BitLocker bloquee).
Criteres logiciels d'acceptation : VALIDES.
Qualification STM32 physique : NON APPLICABLE a cette mission ; non demontree.

Identifiant de rapport : T1000-20261010-QUAL-LOCAL.
Contrat : demande utilisateur dans cette session ; environnement exclusivement T1000.
Reference documentaire : main, SHA distant observe 8c03929184581a915c06fa488b9983c306dfc910.
allow_commit=false ; allow_push=false ; allow_flash=false ; allow_debug=false.
Aucune branche de modification. Aucun firmware TR2 evalue.
Aucun depot local modifie, aucun clone/pull/commit/push execute.
Etat Git local initial/final : NON APPLICABLE au travail effectue hors depot ; aucun git status sur un depot de travail.
Configuration Codex C3 autorisee pour cette session ; configuration permanente inchangee.

## 1. Configuration observee

| Element | Observation directe | Statut |
|---|---|---|
| PC | Dell Inc., Latitude 7320 | VALIDE |
| Windows | Windows 11 Professionnel 64 bits, 25H2, 10.0.26200.9457 | VALIDE |
| WSL | 3.0.1.0 ; seule distribution listee Debian, Running, version 2, par defaut | VALIDE |
| Noyau Linux | 6.18.40.1-microsoft-standard-WSL2, x86_64 | VALIDE |
| Debian | GNU/Linux 13 trixie, DEBIAN_VERSION_FULL=13.7 | VALIDE |
| Compte de test | <UTILISATEUR>, uid/gid 1000, repertoire personnel /home/<UTILISATEUR> | VALIDE |
| Repertoire initial Windows | C:\Users\<UTILISATEUR> | VALIDE lors de la qualification precedente de cette session |
| Repertoire initial WSL | /mnt/c/Users/<UTILISATEUR> | VALIDE |
| Repertoire des tests | /home/<UTILISATEUR>/t1000-qualification-20261010-YRJQ2U | VALIDE |
| Execution Codex locale | commandes Windows, WSL, installation et builds executes par l'agent | VALIDE |
| BitLocker actuel | acces refuse aux lectures manage-bde et CIM | BLOQUE |

Le probleme historique "Failed to create unified exec process: helper_unknown_error: setup refresh had errors"
n'a pas ete reproduit dans cette session. Cela qualifie l'execution courante, pas toutes les configurations futures.

## 2. Etat initial et versions finales

Sauf .NET et wget, les outils ci-dessous etaient deja installes. Aucun outil operationnel reinstalle.

| Outil | Version finale / chemin | Statut et portee |
|---|---|---|
| Git Debian | 2.47.3, /usr/bin/git ; paquet 1:2.47.3-0+deb13u1 | VALIDE, version et ls-remote |
| CMake | 3.31.6, /usr/bin/cmake ; paquet 3.31.6-2 | VALIDE, generation et build |
| Ninja | 1.12.1, /usr/bin/ninja | VALIDE, build C et C++ |
| GCC / G++ hote | 14.2.0, /usr/bin/gcc et /usr/bin/g++ | VALIDE, compilation et execution |
| GCC / G++ ARM | 14.2.1 20241119, /usr/bin/arm-none-eabi-gcc et g++ ; paquet 15:14.2.rel1-1 | VALIDE, Cortex-M33 |
| Binutils ARM | 2.44 ; paquet 2.44-3+23+b1, /usr/bin/arm-none-eabi-* | VALIDE, liens/readelf/nm/objdump/size/objcopy |
| Newlib | 4.5.0.20241231-1, libnewlib-arm-none-eabi et libnewlib-dev | VALIDE pour headers et liaison minimale avec memcpy |
| OpenOCD | 0.12.0, /usr/bin/openocd ; paquet 0.12.0-3+b2 | VALIDE pour lancement --version ; communication cible INSTALLÉ MAIS NON TESTÉ |
| usbutils | 018, /usr/bin/lsusb ; paquet 1:018-2 | VALIDE pour --version ; enumeration USB BLOQUEE sans bus USB WSL |
| usbipd-win | 5.3.0-54+Branch.master, C:\Program Files\usbipd-win\usbipd.exe | VALIDE pour version/list ; partage USB NON APPLICABLE |
| Node.js Windows | v24.20.0, C:\Program Files\nodejs\node.exe | VALIDE pour lancement |
| npm Windows | 11.19.0, C:\Program Files\nodejs\npm.cmd | VALIDE pour lancement |
| Codex CLI Windows | 0.162.1, C:\Users\<UTILISATEUR>\AppData\Roaming\npm\codex.cmd | VALIDE pour lancement ; session interactive distincte non testee |
| Node Linux natif | absent du PATH Linux | ABSENT ; node.exe Windows accessible par interop |
| Codex Linux natif | aucun lanceur Linux operationnel trouve ; lanceur npm Windows herite echoue | ABSENT ; installation Linux distincte non necessaire aux tests de cette mission |
| SDK .NET Debian | initialement absent ; 10.0.401, /usr/bin/dotnet -> /usr/share/dotnet/dotnet | VALIDE |
| Runtime .NET / ASP.NET Core | 10.0.12 | VALIDE pour inventaire ; runtime .NET execute |
| Workloads .NET additionnels | aucun | NON APPLICABLE au projet console |
| GDB ARM / gdb-multiarch | non trouve dans le PATH et emplacements inspectes | ABSENT |
| STM32CubeIDE / CubeProgrammer / CubeCLT / CubeU5 | aucun trouve dans PATH et emplacements usuels inspectes | ABSENT dans le perimetre d'inspection ; recherche non exhaustive de tout le SSD |

Les executables de build Linux ne sont pas dans le PATH Windows observe : les utiliser via wsl.exe -d Debian.
npm et codex dans le PATH WSL provenaient du montage Windows, pas d'une installation Linux native.

## 3. Documentation et coherence

Lus integralement via le connecteur GitHub, ref main :
- AGENTS.md ;
- 00_Environnement_developpement/T1000/JOURNAL_W1_W2.md ;
- ARCHIVE_JOURNAL_W1_W2_2026-10-10.md ;
- VERSIONS.md ;
- INSTALLATION.md ;
- Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md.

Le complement W2-A a W2-E et l'archive de fin de seance correspondent aux versions observees.
VERSIONS.md et INSTALLATION.md contiennent encore des instantanes historiques "pas installe" :
ils ne decrivent pas l'etat actuel directement mesure.
Debian 13.7 confirme le complement du journal, plutot que le socle historique 13.5.
Node/npm/Codex Windows confirment l'archive de fin de seance.
Aucun de ces documents n'a ete modifie ; aucun journal TR2 actualise.

## 4. Installations effectuees

Methode officielle Microsoft Debian 13 x64 :
https://learn.microsoft.com/en-us/dotnet/core/install/linux-debian

Sources Debian preexistantes conservees :
0000debian.sources : trixie, trixie-updates, trixie-backports et trixie-security, main, HTTPS.
Ajout uniquement du depot Microsoft Debian 13 par son paquet officiel packages-microsoft-prod.
Pas de script dotnet-install, pas de melange de fournisseurs pour les paquets .NET.
libicu76 est la dependance systeme Debian resolue normalement par APT.
Pas de mise a niveau generale, pas de suppression de paquets ; --no-remove applique aux installations.

Commandes executees et terminees avec retour 0 :
1. apt-get install -y --no-remove wget
2. wget -O packages-microsoft-prod.deb https://packages.microsoft.com/config/debian/13/packages-microsoft-prod.deb
3. dpkg-deb --info packages-microsoft-prod.deb
4. sha256sum packages-microsoft-prod.deb
5. dpkg -i /home/<UTILISATEUR>/t1000-qualification-20261010-YRJQ2U/packages-microsoft-prod.deb
6. apt-get update
7. apt-cache policy dotnet-sdk-10.0
8. apt-get -s install dotnet-sdk-10.0
9. apt-get install -y --no-remove dotnet-sdk-10.0

SHA256 du paquet de depot telecharge :
d0c2f69250c6ce0d4c6220b142f999d039a3c560af7f980b943687d106ca8e38
Il s'agit de l'empreinte relevee, pas d'une comparaison a une empreinte independante publiee.

Installations : wget 1.25.0-2 ; packages-microsoft-prod 1.1-debian13 ;
dotnet-sdk-10.0 10.0.401-1 ; libicu76 76.1-4 ;
dotnet-host, dotnet-hostfxr-10.0, dotnet-runtime-10.0, dotnet-runtime-deps-10.0,
dotnet-targeting-pack-10.0, aspnetcore-runtime-10.0, aspnetcore-targeting-pack-10.0,
dotnet-apphost-pack-10.0 : 10.0.12-1.

APT : 250 MB telecharges pour .NET, 678 MB d'espace supplementaire annonce, 10 nouveaux paquets.
wget : 984 kB telecharges, environ 3.9 MB supplementaires.
dpkg --audit final : retour 0, aucune anomalie signalee.

sudo -n true a echoue (mot de passe requis).
wsl.exe -d Debian -u root -- id a confirme un acces root disponible (retour 0).
Les commandes administratives APT/dpkg ont utilise cet acces existant.
Aucun changement sudoers, compte par defaut, politique Windows ou configuration Codex.
Les tests ont ete executes avec <UTILISATEUR>.

## 5. Tests et preuves

Chaque commande de validation dans run-tests.sh et run-dotnet.sh affiche son propre EXIT_CODE.
Les journaux sont fournis avec le rapport ; le code global d'un groupe ne remplace pas ces codes individuels.
Transport robuste : PowerShell -NoProfile -EncodedCommand (UTF-16LE), puis script bash encode base64
transmis a wsl.exe. Ce codage protege dollars/guillemets contre une expansion prematuree.

### Hote C/C++ : VALIDE

Commandes, toutes retour 0 :
- cmake -S host -B host/build -G Ninja -DCMAKE_BUILD_TYPE=Release
- cmake --build host/build --verbose
- ctest --test-dir host/build --output-on-failure
- host/build/host_c
- host/build/host_cpp

Deux programmes reels C et C++, compilation avec -Wall -Wextra -Werror.
CTest : 2/2 reussis ; sorties : T1000 C: 42 et T1000 C++: 42.

### Cortex-M33 et Newlib : VALIDE pour compilation et edition de liens minimales

Commandes retour 0 :
- arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -ffreestanding -fno-builtin -Wall -Wextra -Werror -c arm/minimal.c -o arm/minimal.o
- arm-none-eabi-g++ -mcpu=cortex-m33 -mthumb -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -Werror -c arm/minimal.cpp -o arm/minimal_cpp.o
- arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -nostartfiles -Wl,-e,_start -Wl,-Ttext=0x08000000 arm/minimal.o arm/minimal_cpp.o -lc -lgcc -o arm/minimal.elf
- arm-none-eabi-readelf -h -A arm/minimal.o arm/minimal.elf
- arm-none-eabi-nm arm/minimal.elf
- arm-none-eabi-size arm/minimal.elf
- arm-none-eabi-objcopy -O binary arm/minimal.elf arm/minimal.bin
- arm-none-eabi-objdump -f arm/minimal.o arm/minimal.elf

Objet : ELF32 little endian, REL, ARM EABI5.
ELF : EXEC, ARM EABI5 soft-float ; attribut v8-M.mainline, profil Microcontroller, Thumb.
objdump : elf32-littlearm, architecture armv8-m.main.
nm : symbole memcpy lie, cpp_add et _start presents.
size : text=316, data=0, bss=4 octets.
L'adresse synthetique 0x08000000 sert uniquement au test de liens ; aucune adresse STM32 approuvee.
Pas de startup STM32, linker script de carte, HAL, firmware TR2 ou execution sur cible.
C++ teste sans exceptions/RTTI ; bibliotheque standard C++ embarquee non qualifiee.

### .NET 10 / C# : VALIDE

Commandes retour 0 :
- command -v dotnet
- dotnet --info
- dotnet --list-sdks
- dotnet --list-runtimes
- dotnet new console --name T1000CSharp --output csharp --framework net10.0 --no-restore
- dotnet restore csharp/T1000CSharp.csproj
- dotnet build csharp/T1000CSharp.csproj --configuration Release --no-restore
- dotnet run --project csharp/T1000CSharp.csproj --configuration Release --no-build
- dotnet workload list

RID linux-x64, MSBuild 18.9.11+e34a38d2a.
Build : 0 warning, 0 error.
Execution : T1000 C#: sum=42; runtime=.NET 10.0.12; os=Debian GNU/Linux 13 (trixie)
puis T1000 C# VALIDATION OK. Le programme verifie le calcul et OperatingSystem.IsLinux().
Projet console sans dependance NuGet tierce : restauration validee, telechargement d'un paquet externe non teste.
DOTNET_CLI_HOME et NUGET_PACKAGES confines au repertoire temporaire ; telemetrie des tests desactivee par variables de processus.

### Git, Windows et Codex : VALIDE dans la portee mesuree

Retours individuels 0 :
- git --version
- git ls-remote https://github.com/loloLR17/MSM_vibe.git refs/heads/main
  resultat : 8c03929184581a915c06fa488b9983c306dfc910 refs/heads/main
- wsl.exe --version
- wsl.exe --list --verbose
- usbipd.exe --version
- usbipd.exe list
- node.exe --version
- npm.cmd --version
- codex.cmd --version

Depuis WSL, apres cd /mnt/c/Users/<UTILISATEUR> :
cmd.exe /c codex.cmd --version -> codex-cli 0.162.1, retour 0.
Cela teste l'interop Windows, pas une installation native Linux ni une nouvelle session authentifiee.
La presente mission fournit une preuve directe d'execution autonome Windows/WSL et de builds par l'agent.
Acces GitHub public en lecture valide ; push, authentification pour depots prives et ssh-agent non qualifies.

## 6. Echecs et corrections

- Premier essai de boucle WSL : les variables bash ont ete expansees avant execution.
  Erreurs "--version: command not found" et faux marqueur SUDO_RC=0.
  Ces sorties sont invalidees. Releve refait par transport encode ; codes individuels fiables.
- Essai d'encodage JS btoa : ReferenceError: btoa is not defined.
  Aucune commande locale executee par cet essai ; encodeur explicite utilise ensuite.
- sudo -n true : retour 1, "sudo: a password is required".
  Acces root WSL existant verifie et utilise pour installations autorisees.
- wget initial : retour 127, "wget: command not found".
  dpkg-deb --info consequent : retour 2, archive inexistante ; sha256sum : retour 1.
  wget installe via APT Debian ; telechargement et inspections relances avec succes.
- Derniere commande optionnelle du premier script hote/ARM, "file arm/minimal.o arm/minimal.elf" :
  retour 127, "file: command not found" ; script global retour 1.
  Les compilations/liens precedents ont chacun retour 0.
  Format ensuite confirme par arm-none-eabi-objdump (retour 0) et readelf deja reussi.
  Pas d'installation du logiciel file, non necessaire.
- lsusb et lsusb -t : retour 1 ; lsusb -t affiche "/sys/bus/usb/devices: No such file or directory".
  Pas de bus USB WSL actuellement ; aucune connexion/attache imposee.
- codex herite dans WSL : retour 127, ".../npm/codex: 15: exec: node: not found".
  Lanceur Windows codex.cmd teste explicitement ; PATH et identifiants non modifies.
- cmd.exe lance depuis le repertoire Linux : avertissement UNC, repli sur repertoire Windows par defaut.
  Essai relance depuis /mnt/c/Users/<UTILISATEUR> : retour 0 sans avertissement.
- dotnet new : message "An issue was encountered verifying workloads."
  Retour 0 ; console creee ; restore/build/run reussis ; dotnet workload list ensuite retour 0.
  Aucun workload requis pour ce test ; pas de mise a jour de workloads non necessaire.
- manage-bde.exe -status C: : retour 1,
  "ERREUR : une tentative d'acceder a une ressource requise a ete refusee.
   Verifiez que vous disposez de droits d'administration sur l'ordinateur."
- Lecture CIM Win32_EncryptableVolume : retour 1, Acces refuse, HRESULT 0x80041003.
  BitLocker BLOQUE pour cette session non elevee ; aucune modification effectuee.
- Premiere commande Windows groupee d'inventaire : retour global 1 lie notamment aux outils absents
  dans Get-Command ; les requetes CIM et volume ont bien produit leurs donnees.
  Versions natives relancees ensuite avec codes individuels.

- Generation initiale du rapport via commande encodee trop longue : CreateProcess os error 206,
  Nom de fichier ou extension trop long. Aucune commande demarree ; ecriture reprise par fragments.

## 7. Disque

SSD physique Win32_DiskDrive : PM991a NVMe Samsung 256GB, 256052966400 octets
(256.05 GB decimaux, environ 238.47 GiB).
Volume C : 254886801408 octets (254.89 GB, environ 237.38 GiB).
Libre avant installation : 195728027648 octets (195.73 GB, environ 182.28 GiB).
Libre apres installation/tests : 194681454592 octets (194.68 GB, environ 181.31 GiB).
Variation observee : environ 1.05 GB ; elle peut inclure l'activite generale du poste.

WSL df -B1 / : capacite virtuelle 1081101176832 octets, utilise 3211141120 octets.
La capacite virtuelle (~1 TiB) n'est pas la capacite physique du SSD.
Aucune compaction VHD, suppression de cache APT preexistant ou modification de partition.

## 8. STM32 : outils disponibles et suite

OpenOCD demarre pour --version. Scripts presents :
/usr/share/openocd/scripts/interface/stlink.cfg
/usr/share/openocd/scripts/target/stm32u5x.cfg
Leur presence ne qualifie pas un ST-LINK ni une carte.

usbipd list : peripherique d'entree USB, webcam, Bluetooth, tous Not shared ;
aucun ST-LINK visible, aucune liaison persistee.
Aucun bind/attach, flash, reset/halt ou GDB sur cible.

Complements possibles a choisir pour retrouver les capacites HP :
- STM32CubeProgrammer : GUI/CLI officielle pour programmation et verification STM32.
  https://www.st.com/en/development-tools/stm32cubeprog.html
- STM32CubeCLT : toolchain, client/serveur GDB, CubeProgrammer et SVD ; peut dupliquer GCC ARM.
  https://www.st.com/en/development-tools/stm32cubeclt.html
- STM32CubeIDE : option IDE integree si le workflow HP en depend.
  https://www.st.com/en/development-tools/stm32cubeide.html
- STM32CubeU5 : SDK HAL/LL/CMSIS et exemples pour applications STM32U5.
  https://www.st.com/en/embedded-software/stm32cubeu5.html
- Client GDB ARM : absent ; une voie Debian gdb-multiarch ou ST reste a qualifier si debug futur requis.

Aucun de ces complements installe automatiquement : inventaire exact du HP, version CubeU5 requise,
choix Windows/WSL pour le flash/debug et procedure d'installation ST restent a etablir.
La documentation ST des produits a ete consultee ; aucune procedure d'installation cible n'est pretendue validee.

## 9. Decisions humaines et limites

1. Lecture BitLocker : elevation Windows administrative necessaire pour terminer cet audit.
   Aucun etat de protection/chiffrement actuel ne peut etre deduit de l'ancien journal.
2. Qualification physique : mission distincte avec materiel identifie et autorisations flash/debug.
3. Parite HP : confirmer le workflow ST et les versions a reprendre avant installation complementaire.
4. Codex natif WSL : option future si souhaite ; le workflow Windows pilotant WSL fonctionne deja.

La documentation officielle OpenAI distingue les workflows Windows natifs et WSL :
https://learn.chatgpt.com/docs/windows/windows-sandbox
Competence OpenAI Docs appliquee. Aucune instruction de modification de configuration appliquee.

## 10. Livrables et preservation

Scripts et journaux conserves pour revue dans le repertoire temporaire dedie.
Copie du rapport et des preuves dans le dossier temporaire Windows de meme identifiant.
Aucun nettoyage de fichiers preexistants. Aucun logiciel preexistant supprime.
Les seuls changements persistants systeme sont les paquets autorises, le depot officiel Microsoft
et les index APT actualises. Aucun changement Git global ou identifiants GitHub.
Aucun fichier du depot ou document GitHub modifie, aucun commit/push.
Les protections Windows, la politique PowerShell et le sandbox permanent Codex sont inchanges.

Capacites validees : build/test hote C/C++, cross-compilation/liens minimaux Cortex-M33/Newlib,
console C# .NET 10, GitHub public lecture, execution autonome locale Windows et Debian.
Capacites non demontrees : firmware STM32 complet, flash, debug, execution/validation physique,
developpement C# GUI Windows, bibliotheque C++ embarquee complete, GitHub prive/push.
