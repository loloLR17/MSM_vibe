---
mission_id: TR2-20261014-P12H3H-SDMMC1-ROOTCAUSE-REVIEW
status: DONE
base_ref: main
base_sha: 6e9f0a48023288a5ed1e3cf4029ebc1f6c143ccd
initial_head: 274214e3a0b40e06e11e11401b00318c5cd0480c
result_repository: https://github.com/VILLO88/NUCLEOU575
result_sha: 1e3c9183bad023ebf7d234134823d9270a0e8b64
created_at_utc: 2026-10-14T11:50:53.654418+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Revue SDMMC1 : première erreur non isolée, propagation expliquée

**Revue documentaire accomplie. Aucune cause unique du premier CRC n'est démontrée.** Les sources contredisent l'absence générale de pull-ups et l'exécution des transferts MSC directement en IRQ. Elles confirment un chemin de récupération multibloc incomplet et une traduction large des erreurs en « média absent ». Ces constats expliquent une aggravation possible après panne, sans identifier ce qui provoque la première erreur.

Un protocole court de lecture hors MSC est proposé, **non exécuté**. Statut **DONE pour la revue documentaire publiée et relue**, sans qualification physique ni clôture de la cause du premier CRC.

## Sources et limites de cette revue

HEAD initial main `274214e3a0b40e06e11e11401b00318c5cd0480c`. Gouvernance inchangée depuis `6e9f0a48023288a5ed1e3cf4029ebc1f6c143ccd`, journal intégral et rapports des trois missions relus. Les hypothèses de Claude proviennent de leur résumé dans le contrat ; son mémoire intégral n'est pas fourni et aucune observation matérielle ne lui est attribuée.

Source [VILLO88/NUCLEOU575](https://github.com/VILLO88/NUCLEOU575/tree/1e3c9183bad023ebf7d234134823d9270a0e8b64) propre, épinglée à `1e3c9183bad023ebf7d234134823d9270a0e8b64`. Vingt fichiers inspectés : [catalogue, lignes et empreintes](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/source-review-provenance.json). `result_sha` désigne ce code externe analysé statiquement, **aucun code nouvellement exécuté**.

Schéma Eagle officiel Adafruit au commit `959792478854acec21fb862650427ffaa32b07f1`, [extraction des pièces et nets](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/adafruit-netlist-review.json). [ST MB1549-C05](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf), feuilles 5/7, et [UM2861 Rev10](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf), tables 19/20 et alimentation, consultés par accès web au PDF texte. Téléchargement local ST en timeout, outils curl/pdftoppm absents : aucune visualisation locale prétendue. Révision physique des cartes et composants réellement montés non vérifiés. Les sources indispensables restent accessibles.

## 1. Pull-ups, vitesse GPIO, alimentation et routage

[MSP L122–141](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Core/Src/stm32u5xx_hal_msp.c#L122) configure CMD/PD2 et D0–D3/PC8–11 en **AF12, push-pull, PULLUP, HIGH**. CLK/PC12 conserve AF12/HIGH mais sans pull-up interne. HIGH vaut 2 dans OSPEEDR, **pas VERY_HIGH, qui vaut 3**. Ces valeurs sont des faits de code ; PUPDR/OSPEEDR n'ont pas été relus ici sur le MCU. Le `.ioc` affecte les broches mais ne contient pas toutes ces personnalisations : le C compilé prévaut, une régénération seule ne garantit pas leur conservation.

Le [schéma Adafruit, pièces L4461–4468 et nets L4590–4710](https://github.com/adafruit/Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB/blob/959792478854acec21fb862650427ffaa32b07f1/Adafruit%20SDIO%20microSD.sch#L4461) indique deux réseaux de résistances **47 kΩ** vers 3V3 : R2/G3 sur D0, R2/G2 sur D1, R1/G1 sur D2, R1/G2 sur D3, R1/G3 sur CMD, R1/G4 sur CLK. Le CLK a donc aussi un pull-up externe. C1 **10 µF** et C2 **100 nF** relient 3V3 à GND. Alimentation et logique directes 3,3 V, sans régulateur ni adaptation 5 V.

**Le schéma tranche la conception : les pull-ups ne sont pas absents.** Il ne prouve pas les résistances réellement montées, les soudures ou les formes d'onde. Aucun ajout de résistances à l'aveugle. Un multimètre peut vérifier une anomalie ciblée de continuité ou de tension DC ; il ne qualifie pas les fronts à 12 MHz. L'augmentation HIGH→VERY_HIGH n'est pas une correction démontrée et peut accroître les oscillations sur le montage.

ST confirme D0/PC8→CN12-2 via SB19, D1/PC9→CN12-1 via SB22, D2/PC10→CN11-1, D3/PC11→CN11-2, CLK/PC12→CN11-3, CMD/PD2→CN11-4. SB19/SB22 fermés sont déjà rapportés par l'utilisateur. Le manuel signale une dérivation morpho/Zio et un risque de qualité du signal ; CN8 vide ne supprime pas la piste. Les fils 10 cm ne suppriment pas ces stubs ni les contacts. Pas de nouvelle soudure proposée automatiquement.

POWER_BOARD montre le rail 3V3 et la sélection d'alimentation JP6, ainsi qu'une sélection VDD distincte. Le schéma ne prouve ni le choix actuel des jumpers, ni la tension au socket, le retour GND ou une chute pendant transfert. **Retirer deux câbles ne mesure pas VDD carte à 0 V.** Pour un futur départ contrôlé, vérifier la décharge au breakout ; des mesures dynamiques ne deviennent nécessaires qu'après une hypothèse ciblée.

## 2. Initialisation et horloges

[main L99–137](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Core/Src/main.c#L99) initialise SD, demande 4 bits, attend TRANSFER, pose `sd_ready=1`, puis initialise TinyUSB. **MSC ne démarre pas avant SD.** La fonction HAL PCD `MX_USB_OTG_FS_PCD_Init` n'est pas appelée.

[HAL InitCard L471–515](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c#L471) impose l'identification en 1 bit, à ≤400 kHz, et une attente d'au moins 74 cycles. ClockDiv2 dans main ne signifie donc pas une identification entière à 12 MHz. HAL_Init contrôle SSR/largeur/état puis remet ErrorCode à zéro (L400–453). ACMD6 change ensuite la largeur carte et la HAL reconfigure CLKCR.

Le commentaire « fallback 1 bit » n'est **pas une procédure de repli vérifiée** : après erreur 4 bits, aucun retour explicite et aucune validation de concordance carte/contrôleur. Certains chemins peuvent accepter le changement carte puis échouer globalement. Cependant, les essais acquis ont HAL_OK, SSR largeur2 et CLKCR4002 : le repli n'explique pas leur mode 4 bits démontré.

SDMMC et USB utilisent CLK48/HSI48 dans MSP L101–104. Registres historiques concordants : CPU160 MHz, kernel48 MHz, SD48/(2×2)=12 MHz calculés. Le partage de HSI48 ne prouve pas un conflit d'horloge. Jitter et signaux électriques restent non mesurés.

## 3. Contexte réel des transferts et base de temps

[TinyUSB usbd L170–180](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Middlewares/TinyUSB/src/device/usbd.c#L170) déclare MSC avec `xfer_isr=NULL`. La tâche appelée depuis main distribue les événements ; [MSC L847–859](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Middlewares/TinyUSB/src/class/msc/msc_device.c#L847) appelle le callback HAL. **Le transfert SD MSC s'exécute dans main, pas directement dans l'IRQ USB.** Les anciens appels de fonction depuis GDB sont un contexte différent, perturbateur potentiel.

La HAL utilise le polling FIFO, sans DMA/IT dans ce chemin. MSP active NVIC SDMMC1 à priorité0 ; cela ne prouve pas une activité IRQ SD, les masques de transfert polling n'étant pas ceux des API IT/DMA. TinyUSB active l'IRQ OTG sans régler sa priorité ; celle du reset est attendue, mais non relue ici. La priorité0 du MSP PCD ne constitue pas une preuve d'exécution de ce MSP, désactivé dans main. Une préemption USB du polling reste plausible ; **RX_OVERRUN0x20 n'est pas DATA_CRC_FAIL0x2**.

La base HAL est **TIM17, priorité7**, pas SysTick : timebase TIM L79–97 et callback main L430–437 incrémentent le tick. Un SysTick vide n'est donc pas un défaut démontré. Les callbacks SD dans main permettent la préemption TIM17 ; un blocage de timeout dû à un long callback SD en IRQ0 n'est pas étayé. OSAL masque l'IRQ USB autour de la copie de queue (L183–188), pas pendant tout le callback SD.

FatFs/diskio sont embarqués mais aucune opération `f_mount/f_open/f_read/f_write` n'est appelée dans Core. MSC accède directement à HAL_SD. Les waits CMD13 de diskio ne sont pas actifs dans ces essais. RAW du volume PC et NoMedia sont distincts ; aucun double montage FatFs/PC démontré.

## 4. Récupération et « No Media »

[HAL_ReadBlocks L742–819](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c#L742) s'arrête sur erreur ou DATAEND. **CMD12 multibloc n'est envoyé que si DATAEND est présent** (L775). Une sortie CRC sans DATAEND efface les flags et remet HAL READY/ContextNONE sans CMD12. Le timeout logiciel sort même avant désactivation de CMDTRANS. Le chemin write est analogue. Ceci est compatible avec la carte DATA5 observée alors que HAL est READY : **état logiciel READY ≠ carte TRANSFER**.

[HAL_SD_GetCardState L2818–2832](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c#L2818) envoie CMD13 via SD_SendStatus, ajoute les erreurs au handle et retourne zéro local si la réponse n'a pas été obtenue. Ce n'est pas une observation passive : commande, réponse et flags peuvent changer.

`msc_disk.c` L92/119 retourne -1 après erreur HAL, sans recovery. Après HAL_OK, les boucles L98/L125 peuvent interroger CMD13 jusqu'à 100000 fois ; c'est un compteur, pas un budget en millisecondes. Un statut ultérieur réussi peut masquer un épisode de statut raté, tout en laissant ErrorCode non nul. Les requêtes ultérieures du PC peuvent accumuler les erreurs de commande après le premier échec de données.

[TEST_UNIT_READY L35–44](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Core/Src/msc_disk.c#L35) annonce ASC3A pour tout état non TRANSFER. [TinyUSB MSC L873–877](https://github.com/VILLO88/NUCLEOU575/blob/1e3c9183bad023ebf7d234134823d9270a0e8b64/Middlewares/TinyUSB/src/class/msc/msc_device.c#L873) traduit aussi read-1 en medium_not_present. **NoMedia peut donc être une conséquence d'I/O refusée, pas une absence physique.** Capacité retournée depuis le handle et `sd_ready=1` ne prouvent pas la disponibilité. `ejected=false` est observé dans les preuves. Le callback ignore offset et tronque bufsize/512 ; fragile hors requêtes alignées, mais aucune attribution des CRC à ce point dans les chunks8192 alignés testés.

## 5. Chronologie et hypothèses

| Étape acquise | Faits et limite de conclusion |
|---|---|
| PoC20cm |Init/4bits conformes ; CRC0x2 observés en Debug et Release. Deux jeux128KiB finalement exacts, puis NoMedia récurrent. Premier événement non figé avant cleanup.|
| Après erreurs |Carte DATA5/R1B00, HAL READY, erreurs de commande accumulées. Recovery incomplète compatible ; ordre exact non enregistré.|
| Comparatif10cm |NoMedia avant GDB, reset logiciel unique sans récupération, aucun cycle. Pas de mesure causale de longueur.|
| COLDSTART10cm |Coupure complète déclarée, NoMedia avant premier GDB, Error1007 ensuite. Ni VDD0V ni première commande fautive prouvés.|
| PC natif antérieur |1GiB et ancien jeuB128KiB intègres ; média Online. Fonctionnement borné sur ce chemin, pas authenticité/capacité entière/qualification TR2.|

0x1007=1+2+4+1000 ; 0x2005=1+4+2000 ; 0x3005=1+4+1000+2000. **L'OR des bits n'enregistre aucun ordre.** RESP1 peut être celui d'une commande ultérieure. LL `SDMMC_GetCmdResp1` L1355–1439 efface des flags et peut produire CMD_CRC_FAIL si RESPCMD ne correspond pas : le bit1 n'est pas automatiquement la preuve d'un CRC électrique de commande.

| Hypothèse | Niveau, arguments pour et contre |
|---|---|
| Recovery multibloc aggravant NoMedia |**Fortement étayée** par code et DATA5/READY ; n'explique pas la première erreur.|
| Signaux, stubs, contacts, retour GND |**Plausible prioritaire** : CRC et avertissement ST ; succès intermittents et absence de mesure ne permettent pas de choisir une cause électrique.|
| Rail SD, décharge et état initial |**Plausible** : VDD0 non mesuré ; initialisation et succès PC ne prouvent pas le rail du breakout.|
| Polling/préemption USB |**Plausible** : FIFO et IRQ actifs ; callbacks main, absence de RXOVR observé et erreurs en contexte GDB limitent l'attribution.|
| Vitesse GPIO erronée |**Faible comme défaut établi** : HIGH existe, VERY_HIGH non exigé par les preuves ; fronts non mesurés.|
| Pull-ups absents, identification à12MHz, MSC avant SD |**Contredites dans le code/conception** ; population réelle et comportement dynamique restent non vérifiés.|
| Repli après échec4bits |**Fragilité réelle**, sans explication des essais à largeur4bits confirmée.|
| Carte globalement défectueuse ou fausse |**Peu soutenue pour les zones vérifiées** ; reste du média/authenticité non qualifiés.|
| FatFs concurrent ou transferts SD en IRQ MSC |**Contredites pour le chemin inspecté**.|
| Debugger cause exclusive |**Faible** : NoMedia précède le premier GDB COLDSTART ; perturbation possible dans les autres essais.|

## 6. Un seul protocole futur, non exécuté

[Protocole détaillé](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/PROTOCOLE_LECTURE_PROPOSE.md) : lecture seule hors MSC, **1 bit puis 4 bits à12MHz**, mêmes GPIO/HAL/profil et zone historique connue LBA506304. Une lecture512 puis8192 seulement si les phases précédentes sont conformes. SSR et CMD13 préalables identifiés comme commandes actives. Premier record RAM avant effacement des flags, aucun CMD13 après erreur, aucun halt pendant polling.

Chaque bras a un départ électrique contrôlé. Échec1bit : arrêt total. Échec4bits après succès1bit : sensibilité4bits renforcée, mais le débit polling quadruple aussi ; D1–D3 seuls ne sont pas isolés. Succès des deux bras : lectures hors MSC bornées, sans démonstration qu'USB est la cause. Insertion depuis le lecteur PC et mesure VDD0, si retenue pour valider un départ froid, relèvent de l'utilisateur dans une future mission. Aucun geste demandé maintenant.

L'instrumentation nécessitera une copie HAL/LL séparée, des captures RAM avant cleanup, sans changer les conditions ni ajouter de printf dans le polling. **Aucun probe développé/compilé ici.** Pas de correction recovery, testUSB immédiat, 20×64MiB ou endurance. ChatGPT doit contrôler cette revue puis publier le contrat physique approprié.

## 7. Preuves PC antérieures et état conservé

Le [rapport PC antérieur](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/qualification-PC-anterieure/RAPPORT_QUALIFICATION_CARTE_PC.md) et ses preuves compactes sont copiés **hors ligne** ; manifest local27fichiers vérifié, aucun lecteur interrogé ici. Séquentiel1GiB exact, 12,472Mo/s write et66,683Mo/s read ; write64KiB max1077,795ms/P99140,115ms. Random4KiB sans WRITE_THROUGH avec flush final :2311readIOPS/554,76writeIOPS, zéro différence sur8MiB ; avec WRITE_THROUGH :7,58writeIOPS. Aucun résultat défavorable supprimé. Ce sont des essais antérieurs avec autre hôte/méthode, **pas certificationA1, capacité totale31,9Go, VDD froid ou preuve d'absence de CRC sur Nucleo**.

**Aucun flash, GDB, halt, reset, accès au lecteur PC, écriture SD ou modification firmware dans cette mission.** Documents seulement : inspection de sources, netlist, empreintes, liens et diff ; aucun build ou ancien test rejoué. Carte dans lecteur PC selon dernier utilisateur, état actuel non relu. Image Nucleo Release historique2a2fe744… non modifiée ; état matériel actuel non présumé. Principal/FRAM/512Go/Modbus/protections non accédés.

Dépôt original propre à82169af7e76797731358852bec83bfb0ceb54ccf et deux stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2/f3b829e6188f491920edf74a5f9ed0f559d08b87 préservés. Worktree `MSM_vibe-SDMMC1-rootcause-review`, branche `codex/p12h3h-sdmmc1-rootcause-review`, basé sur main courant, sans merge/rebase/reset destructif. COLDSTART archivé byte-identique depuis6e9f0a48023288a5ed1e3cf4029ebc1f6c143ccd. Mission active/gouvernance/gels/roadmap/detteA04 intacts. Journal actualisé avec les constats, hypothèses et protocole **NON EXÉCUTÉ**. Commit/pushmain autorisés pour documents ; flash/debugfalse respectés.

Publication normale sur `main` du commit `d107a4307c4f83900633bc41835fc550c563b29b` (`P12 H3h SDMMC1: document root cause review, prior PC evidence and bounded read protocol`) réussie. Référence récupérée de GitHub ; **19 fichiers relus intégralement et byte-identiques**, **14 entrées du manifest vérifiées** : [preuve distante](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/remote-readback-d107a430.json). Archive et fichiers protégés inchangés, source tiers propre, dépôt original et stashes préservés. Finalisation documentaire DONE uniquement, sans nouvelle opération matérielle. Le worktree sera propre après commit ; dernière publication également à vérifier, SHA distant communiqué dans la réponse finale sans auto-référence. DONE signifie revue accomplie ; contrôle indépendant ChatGPT et futur contrat physique restent distincts.
