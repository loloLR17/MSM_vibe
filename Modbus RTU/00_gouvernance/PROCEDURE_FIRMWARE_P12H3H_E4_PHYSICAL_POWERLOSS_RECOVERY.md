# P12 / H3h-E4 — Procédure de qualification physique power-loss / recovery

## Statut et baseline

PROCÉDURE PRÉPARÉE — NON EXÉCUTÉE.
Aucun résultat physique n'est consigné comme obtenu.

Baseline firmware : `111ca804c08cd126abc0ae10ff2a923ce7554ebe`.
Ce document et son commit documentaire ne modifient pas cette baseline firmware.
Avant exécution, vérifier Git et l'absence de changement firmware depuis cette
baseline ; si le firmware diffère, réviser la procédure avant qualification.

## Sources et portée

Sources : `platform/stm32/main.c` (E4MarkerRead/Write, E4PrepareFresh,
E4CutPowerPoint, E4Write/Sync, Sdmmc2_Bringup), adaptateur
`stm32_sdmmc_bulk_media.c`, `campaign_data_store_bulk.c`,
`campaign_bulk_metadata.c`, `campaign_data_store.h`, `result.h`,
`tr2_validate.sh`, `tr2_build_stm32.sh`, conception D4-A/D4-C/E2 et validation E3.
Les chemins firmware sont relatifs à `Modbus RTU/05_Firmware`.

Qualification bornée du backend D5-C au-dessus d'E1, aux deux frontières
transactionnelles E4. Exclusions : atomicité secteur, coupure au milieu d'un
transfert SD, pire stall, débit du DataStore de production, taille de buffer de
production, acquisition continue, intégration SystemRuntime et capacité
utilisable de configuration. E4 ne clôture pas ces sujets d'intégration.

## A — Préconditions et autorisations

Depuis la racine :

```bash
git pull --ff-only origin main
git status
git rev-parse HEAD
git diff 111ca804c08cd126abc0ae10ff2a923ce7554ebe -- 'Modbus RTU/05_Firmware' tr2_validate.sh
```

Le dernier diff doit être vide. Consigner HEAD documentaire et baseline firmware.
Ne pas utiliser un ancien artefact non suivi comme preuve du build attendu.

Matériel : NUCLEO-U575ZI-Q, STM32U575, ST-LINK et microSD sacrificielle,
SDMMC2 1 bit / 10 MHz, montage documenté H3h-B/B2 et qualifié E1/E3.
La qualification H3d3-B consigne STLINK-V3E, SN
`0032001F3235510C37333439`, Device ID `0x482`, programmation SWD à
`0x08000000` et debug GDB. Ce numéro est historique ; il ne désigne pas le programmateur du poste actuel.

Données humaines fournies pour cette mission : cible NUCLEO-U575ZI-Q /
STM32U575, ST-LINK `003500463235510B37333439`, STM32CubeCLT Windows
`D:\ST\STM32CubeCLT_1.22.0`. Les invocations ci-dessous ont été déclarées
effectivement utilisées et qualifiées par l’utilisateur lors du bring-up.
Cette provenance humaine est distincte des vérifications statiques du repository.
Recontrôler les identités réellement connectées avant chaque flash.

Autorisations futures : build et collecte logiciels ; flash standard et debug
selon AGENTS.md et la qualification des chaînes. Cette procédure ne constitue
ni une autorisation de flash/debug ni un résultat physique E4.
Une mission pourra autoriser leurs répétitions après établissement des chaînes.
Chaque manipulation physique reste humaine.

Autoriser explicitement les écritures sur carte sacrificielle :

- E1, exécuté avant E4 à chaque boot : blocs 2048..2051 ;
- E4 : blocs 4096..5119 ; metadata 8192 octets au début de cette fenêtre ;
- marqueur séparé : bloc 5120 ; capacité requise : au moins 5121 blocs de 512.

Aucun filesystem n'est utilisé. Aucun effacement global n'est demandé.
Le marqueur contient magic `0x45344632`, état et complément d'état.
États : FRESH=1, PAYLOAD_CUT=2, METADATA_CUT=3, DONE=4.
Campagnes : E411=`0xE411`, E412=`0xE412`, records de 16 octets.
Un marqueur absent/invalide déclenche le nettoyage metadata puis FRESH.
Un marqueur valide d'un essai précédent n'est pas automatiquement effacé.

Départ admissible : génération fraîche connue, E411/E412 EMPTY après préparation.
Pour un essai déjà commencé ou DONE préexistant : arrêter ; une nouvelle
initialisation sacrificielle exige une procédure et une autorisation explicites.
Ne pas inventer une écriture GDB pour forcer le départ.

## B — Build et flash

Build existant, sans opération matérielle :

```bash
STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 ./tr2_validate.sh --cross-build-only
sha256sum 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.elf' 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin' 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.map'
```

Exiger code 0 et CROSS-BUILD VALIDATED ; conserver log, date, HEAD et empreintes.
BIN à programmer : `Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin`.
ELF de symboles : fichier `.elf` du même build. Adresse documentée : 0x08000000.

Adresse confirmée par `platform/stm32/linker/STM32U575ZITXQ_FLASH.ld`
(FLASH ORIGIN 0x08000000) et par la section `.isr_vector` de l’ELF inspecté.
Le script de build produit bien les noms BIN/ELF ci-dessus.

Depuis PowerShell Windows, commande de flash qualifiée fournie par l’utilisateur :

```powershell
& "D:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" `
  -c port=SWD sn=003500463235510B37333439 freq=8000 `
  -w "C:\Users\Lolo\Desktop\MSM_Vibe\MSM_vibe\MSM_vibe\Modbus RTU\05_Firmware\build-stm32-p11c\tr2_stm32_p11c.bin" 0x08000000 `
  -v -rst
```

Avant flash : identifier cible/programmeur, contrôler BIN/ELF et adresse,
exclure mass erase/Option Bytes/protections/configuration persistante.
Succès : code de sortie 0 et vérification explicite. La qualification historique
fournie rapporte `Download verified successfully`, `MCU Reset`,
`Software reset is performed` ; ces textes ne sont pas des résultats E4 nouveaux.
La commande lance un reset : le premier cut peut déjà être atteint avant
connexion GDB. Sa boucle permanente permet encore de l’observer avant coupure.
Un reset/reflash ne remplace jamais une coupure physique.

Serveur qualifié, depuis PowerShell Windows :

```powershell
& "D:\ST\STM32CubeCLT_1.22.0\STLink-gdb-server\bin\ST-LINK_gdbserver.exe" `
  -d -i "003500463235510B37333439" --frequency 8000 `
  -cp "D:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin"
```

Client qualifié, depuis WSL dans `Modbus RTU/05_Firmware` :

```bash
gdb-multiarch build-stm32-p11c/tr2_stm32_p11c.elf
```

Puis dans GDB :

```gdb
target extended-remote 172.17.0.1:61234
```

L’adresse réseau est celle de la procédure humaine qualifiée sur ce poste.
Si elle n’est plus joignable, arrêter et diagnostiquer ; aucune alternative
n’est prescrite. Après une coupure, relancer si nécessaire ces mêmes outils
et reconnecter sans reflasher, sans `load` et sans reset supplémentaire.

## C — Observation automatisable

Codex pilote lui-même les outils et collecte leurs sorties ; l'utilisateur ne
recopie pas des commandes GDB. Ne pas utiliser `load`, `set variable` ou une
écriture de mémoire pour fabriquer un état. Utiliser des breakpoints matériels
pour éviter la modification logicielle de la Flash.

Le build existant ne demande ni Debug ni `-g`. Ne pas supposer que `r401`,
`r402`, `marker` ou les lignes source sont accessibles dans son ELF.
Les symboles globaux peuvent être lus sans types DWARF avec les expressions
suivantes, une fois la connexion autorisée établie :

```gdb
p/u *(unsigned int *)&tr2_sdmmc2_e4_phase
p/u *(unsigned int *)&tr2_sdmmc2_e4_cut_point_reached
p/u *(unsigned int *)&tr2_sdmmc2_e4_last_result
p/u *(unsigned int *)&tr2_sdmmc2_e4_recovery_status
p/u *(unsigned long long *)&tr2_sdmmc2_e4_recovered_prefix_bytes
p/u *(unsigned int *)&tr2_sdmmc2_stage
p/x *(unsigned int *)&tr2_sdmmc2_error_code
```

TR2_OK=0 ; recovery VALID=0, EMPTY=1, CORRUPTED=2, UNAVAILABLE=3,
UNSUPPORTED=4. UINT32_MAX est une sentinelle non exécutée.

Ces lectures standard GDB constituent la capture minimale ; ajouter `p/x $pc`
et `bt` pour situer l’arrêt et le chemin appelant. Si le CPU tourne, l’interrompre
avec Ctrl-C dans GDB puis capturer. Au cut, il est déjà dans une boucle permanente.
Consigner les sorties brutes et leur horodatage ; `continue` reprend l’exécution
après une observation intermédiaire autorisée. Aucun `monitor reset` supplémentaire
n’est prescrit. Ces commandes E4 sont préparées par inspection statique ; leur
lecture effective sur cible reste à constater pendant la future qualification.

### Vérification ELF hors cible effectuée lors de la préparation

`arm-none-eabi-nm -S` confirme les globals suivants dans l’ELF disponible :

| Symbole | Adresse | Taille |
|---|---|---|
| tr2_sdmmc2_e4_phase | 0x2000398c | 4 |
| tr2_sdmmc2_e4_cut_point_reached | 0x20003990 | 4 |
| tr2_sdmmc2_e4_last_result | 0x20000028 | 4 |
| tr2_sdmmc2_e4_recovery_status | 0x2000002c | 4 |
| tr2_sdmmc2_e4_recovered_prefix_bytes | 0x20003998 | 8 |
| tr2_sdmmc2_stage | 0x20000260 | 4 |
| tr2_sdmmc2_error_code | 0x20000224 | 4 |

Ils sont présents en SRAM, avec des tailles compatibles avec les lectures
ci-dessus. Leur observabilité symbolique est établie hors cible ; la connexion
et les valeurs matérielles ne sont pas vérifiées ici.
ELF SHA-256 : `935da9136b2bafea88462d1f6bcc6a2b3aa2ae48631daa2e74c56cdab7ac795e`.
BIN SHA-256 : `70253181e697fe9b37159b2a1b0e33fc834f2fc0e96d822a50878428d5701a2e`.
La conversion ELF vers BIN par `arm-none-eabi-objcopy -O binary` et comparaison
`cmp` est identique. Ces artefacts non suivis ne prouvent pas leur rattachement
au HEAD : refaire le build traçable en B avant utilisation.

Contrôles à répéter depuis le répertoire firmware après le build de qualification :

```bash
arm-none-eabi-nm -S build-stm32-p11c/tr2_stm32_p11c.elf
arm-none-eabi-readelf -S build-stm32-p11c/tr2_stm32_p11c.elf
arm-none-eabi-objdump -d --disassemble=E4CutPowerPoint build-stm32-p11c/tr2_stm32_p11c.elf
arm-none-eabi-objdump -d --disassemble=Sdmmc2_Bringup build-stm32-p11c/tr2_stm32_p11c.elf
```

Le désassemblage de cet ELF situe les points matériels suivants :

| Point | Adresse vérifiée dans cet ELF uniquement |
|---|---|
| Boucle cut, après désactivation IRQ et publication phase/drapeau | 0x0800105e |
| Phase 3 après diagnostic E411, avant init E412 | 0x08001e04 |
| Completion après écriture DONE et stage 38 | 0x08002010 |
| Boot DONE après stage 38, sans écriture de marqueur | 0x08001fd8 |

Si et seulement si l’ELF fraîchement produit correspond à cette empreinte,
les commandes standard `hbreak *0x08001e04`, `hbreak *0x08002010` et
`hbreak *0x08001fd8` permettent d’observer les chemins correspondants.
Armer seulement les points nécessaires et retirer les breakpoints devenus
inutiles avec `delete <numéro affiché par GDB>`. Un breakpoint ne reste pas
nécessairement actif à travers une perte d’alimentation ; les captures stables
et l’historique de l’essai restent nécessaires. Ne pas promettre de retenir
un boot déjà exécuté.

Codex devra inspecter le désassemblage de l'ELF fraîchement produit, déterminer
et consigner les adresses de breakpoints matériels correspondant exactement à :

1. la boucle de E4CutPowerPoint, après phase/drapeau et désactivation IRQ ;
2. phase 3, après stockage du diagnostic E411 et AVANT l'initialisation active E412 ;
3. après stage 38 sur le chemin METADATA_CUT ;
4. après stage 38 sur le chemin DONE.
Ces points sont définis par le code, pas par des adresses d'un ancien binaire.
Les lignes de référence de la baseline sont respectivement 345, 1207, 1288 et
1278. Ne pas utiliser un breakpoint de ligne sans vérifier son mapping.
Sans mapping certain, arrêter INCONCLUSIVE avant une qualification.

Au point 2, les globals valent phase=3, recovery_status=VALID et prefix=16.
L'entrée dans cette branche établit E412 EMPTY par sa garde réelle.
Pour distinguer METADATA_CUT et DONE, utiliser leurs chemins de contrôle distincts.
Le marqueur SD n'est pas un global RAM et aucune adresse RAM de ce marqueur
n'est prescrite. Enregistrer le chemin et, si des locals sont effectivement
accessibles, leur valeur ; ne jamais prétendre avoir lu un marqueur SD sur la
seule base d'une variable phase.

## D — CUT 1 READY

Après départ contrôlé, continuer jusqu'à la boucle de cut payload.
Exiger simultanément :

- phase=2 ; cut_point_reached=1 ;
- last_result=0 (préparation du scénario réussie) ;
- error_code SDMMC=0 ;
- PC dans la boucle E4CutPowerPoint, atteinte depuis E4Sync ;
- aucun stage d'erreur ; stage=16, dernier succès E1, pas 33 ;
- historique du départ frais et du build conservé.

À ce point, recovery_status peut encore être UINT32_MAX et prefix=0 : ils ne
sont PAS une preuve du recovery futur. last_result=0 ne désigne pas le retour
du checkpoint injecté : celui-ci n'est pas encore revenu.

Alors seulement annoncer CUT 1 READY et archiver la capture horodatée.
Demander à l'utilisateur de couper toutes les sources alimentant MCU ET microSD,
y compris les chemins USB/ST-LINK susceptibles de maintenir l'alimentation.
Attendre son retour confirmant l'action ; préciser le montage et les sources
réelles avant l'essai. Demander ensuite la remise sous tension.

## E — Recovery CUT 1

Après reconnexion, ne pas reflasher ni effacer la SD. Le boot peut avancer
spontanément jusqu'au cut 2 ; le blocage E4 maintient ce point observable.
Deux méthodes d'observation :

- arrêter au point 2 préarmé si la chaîne permet de conserver/réinstaller ce
  breakpoint avant progression ; observer phase=3, VALID/16 et garde E412 EMPTY ;
- si le boot a déjà atteint la boucle phase 4, lire les globals recovery_status=0
  et prefix=16 : ces valeurs ont été posées en phase 3 et ne sont pas écrasées
  avant ce cut. Cela prouve le recovery CUT 1 sans recopier des locals invisibles.

PASS CUT 1 exige capture préalable CUT 1 READY, confirmation humaine de vraie
coupure/remise sous tension, puis recovery E411 VALID/16.
Si phase 4 est déjà atteinte, le scénario metadata a été exécuté automatiquement
par le firmware ; Codex ne demande la deuxième coupure qu'après PASS CUT 1.
Il est impossible de promettre un contrôle humain/logiciel entre les scénarios
sans une chaîne capable de retenir le boot au point 2. Ne pas modifier le
firmware pour simuler ce contrôle dans cette procédure.

## F — CUT 2 READY

Exiger PASS CUT 1 déjà établi et capture courante :

- phase=4 ; cut_point_reached=1 ; last_result=0 ; error_code=0 ;
- recovery_status=0 et recovered_prefix_bytes=16 (diagnostic E411) ;
- PC dans la boucle E4CutPowerPoint, atteinte depuis E4Write après écriture
  du descriptor candidat E412 ; stage=16, pas 36.

Le marqueur METADATA_CUT a été écrit avant le checkpoint ; il ne suffit pas
à lui seul. La phase 4 intervient avant sync/readback de publication mais après
retour réussi de l'écriture SD, qui attend déjà l'état TRANSFER.
Alors annoncer CUT 2 READY, collecter la capture, demander seulement la coupure
physique MCU/microSD, attendre confirmation, puis demander la remise sous tension.

## G — Recovery final et DONE

Après deuxième coupure, utiliser le point 3 ou une capture stable après retour
normal de Sdmmc2_Bringup. Exiger :

- phase=6 ; stage=38 ; cut_point_reached=0 dans ce nouveau boot ;
- last_result=0 ; recovery_status=VALID/0 ; recovered_prefix_bytes=16 OU 32 ;
- aucune erreur SDMMC et observation de la branche METADATA_CUT vers DONE.

Le garde E411 VALID/16 est obligatoire avant cette branche ; le diagnostic global
final concerne E412, pas E411. Conserver les preuves CUT 1 séparément.
16 signifie ancienne autorité E412 récupérable ; 32 signifie candidate valide
récupérable. Les deux sont admissibles ; aucune valeur ne prouve seule la coupure.
Le marqueur DONE est écrit/synchronisé avant phase 6/stage 38.

## H — Reboot supplémentaire en DONE

Après capture finale, demander une nouvelle coupure/remise sous tension physique
pour contrôler un nouveau boot. Conserver la SD et le même firmware.
Armer/observer le chemin DONE au point 4 si le boot peut être retenu à temps.
Sinon, corréler le DONE écrit au boot précédent, le reboot attesté et la capture
stable aux gardes du code mappé : ce chemin est alors déduit, pas directement
observé par breakpoint. Consigner explicitement cette différence de preuve. Attendre phase=6, stage=38, cut_point_reached=0,
last_result=0, recovery_status=0, prefix=16 ou 32 identique au boot précédent.
Les gardes imposent de nouveau E411 VALID/16 et E412 VALID/16 ou 32.
Le chemin DONE ne crée aucune campagne et n'appelle pas E4MarkerWrite.
Cette absence d'écriture E4 est établie par le chemin observé et sa revue ; E1
réécrit néanmoins 2048..2051 au boot. Ne pas annoncer absence globale d'I/O SD.
Si nécessaire, un breakpoint matériel sur E4MarkerWrite peut confirmer l'absence
sur ce boot, sous réserve du nombre de comparateurs disponibles.

## I — Verdict et captures

PASS : toutes les étapes D à H observées et corrélées au même essai/firmware,
avec les deux coupures physiques attestées. Le reboot de contrôle est distinct.
FAIL : erreur effectivement observée, préfixe/statut incohérent, stage d'erreur,
retour inattendu du checkpoint injecté (stage 33/36, last_result à conserver),
ou DONE relançant un scénario/réécrivant le marqueur.
INCONCLUSIVE : cut non observé avant coupure, identité/artefact incertain,
observation manquante, alimentation microSD non réellement coupée, mapping debug
incertain, essai préexistant ou interruption de préparation sans preuve suffisante.
Une préparation interrompue n'est pas automatiquement une corruption démontrée.
Ne jamais transformer un recovery conforme en preuve rétrospective du cut.

| Étape | Date/heure | HEAD/empreintes | Phase/stage/drapeau | last_result | status/prefix | PC/chemin | Action humaine attestée | Verdict |
|---|---|---|---|---|---|---|---|---|
| Départ | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |
| CUT 1 READY | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |
| Recovery CUT 1 | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |
| CUT 2 READY | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |
| Recovery / DONE | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |
| Reboot DONE | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | À renseigner | NON EXÉCUTÉ |

Ajouter versions des outils, identités cible/ST-LINK/carte, mode SDMMC, commandes
exactes réellement exécutées, sorties brutes et confirmation de chaque action.
Ne pas remplir les résultats attendus comme observations obtenues.

## Automatisation et obstacles

Codex : Git, build, contrôle et empreintes des artefacts, flash standard après
qualification/autorisation, serveur ST-LINK, connexion GDB, halt/breakpoints,
reset logiciel si couvert, lectures, diagnostics répétés et collecte des preuves.
Il ne faut pas reset le CPU au cut point ni effectuer `load` dans le debug.
Après perte d'alimentation, reconnecter selon la procédure qualifiée ; ne pas
inventer de reconnexion sous reset sans autorisation.

Humain : montage/connexion réels, choix/insertion de carte sacrificielle,
coupures/remises sous tension et confirmation de ces actions. Les permissions
sandbox ou opérations sensibles peuvent également nécessiter une approbation.

Préalables avant exécution : vérifier la disponibilité des commandes qualifiées
et le mapping de l’ELF fraîchement produit ; départ frais établi sans effacement improvisé ;
alimentation MCU/microSD réellement contrôlée ; contrat autorisant flash/debug
et écritures sacrificielles. La qualification physique ne commence pas tant que
ces préalables ne sont pas satisfaits.
