# MSM TR2 — R0-MÉTIER — R1-ARM : budgets FIFO, CPU, RAM et intégration du FIR /10

**Statut : revue indépendante documentaire des sources de `main` et protocole de mesure proposé — ni benchmark ARM nouveau, ni essai physique, ni qualification.** 29/10/2026. Auteur : ChatGPT. HEAD contrôlé avant rédaction : `e9cfd49a11a3206a21f8d2450b4b9c8b597857c6`.

**But :** déterminer où raccorder le FIR de 107 coefficients, les ressources qu'il doit partager avec FIFO/FRAM/RTU et les preuves mesurables nécessaires **avant intégration microSD SPI2**. La sortie **≈2 666,7 jeux XYZ/s** est le candidat R1 privilégié après discussion métier, non une révision tacite du cahier V1-05 gelé à 2 000 Hz. [Comparatif et oracles FIR /10](R0_METIER_20261029_COMPARATIF_2667HZ_MODBUS_RTU.md) ; [plan d'essais déterministes](R0_METIER_20261029_PLAN_ESSAIS_DETERMINISTES_DSP.md).

## 1. Observations factuelles dans le code et les résultats historiques

| Périmètre observé | Source directe sur `main` | Conséquence |
|---|---|---|
| Un seul propriétaire de FIFO | `../05_Firmware/platform/stm32/live_fifo_acquisition.c` ; `stm32_live_acquisition.c` | `FifoLiveConsume` accepte un callback, le parseur conserve tags, erreurs et index ; **éviter absolument un deuxième lecteur SPI3** |
| Horodatage de groupes d'échantillons | `../05_Firmware/platform/stm32/fifo_event_time.c`, `.h` | Provider DEC8 : huit XYZ attendent un deuxième anchor ; `fifo_event_time_consume` appelle directement `live_metrology_sample`. Un **dispatch unique** devra fournir le même XYZ/temps/index à l'archive et LIVE, sans inventer de nouveaux timestamps ni casser les contrôles |
| Ordonnancement R4U/R5B historique | `../05_Firmware/platform/stm32/stm32_r4u_runtime.c`, `stm32_r5b_live.c`, `../05_Firmware/src/application/live_runtime.c` | Work limite **1 000 µs**, service **2 500 µs**, gap FIFO **5 000 µs**, pacing optionnel **250 µs** ; les gardes sont pertinentes comme référence, **pas automatiquement des budgets industriels définitifs** |
| Mesures historiques R4V réelles | `../05_Firmware/LIVE_R4V.md`, profil candidat 250 µs exécuté sur **TR2-001 historique** | Maxima rapportés : **work 814 µs / service 2 063 µs / gap 2 274 µs ; FIFO max 68 mots, erreurs 0** pendant l'essai borné ; les maxima indépendants ne sont pas une combinaison temporelle simultanée ni une preuve d'endurance |
| Machine et mémoire | `../05_Firmware/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld` ; checkpoint **AC1** de la revue SPI2 | Région `RAM` principale = **768 Kio = 786 432 B** ; dernier BSS rapporté **659 176 B**, data **492 B** -> résidu arithmétique **126 764 B**, avant pile, heap, alignements et nouveau code : **pas une RAM libre garantie** ; **ne pas confondre les profiles R4V, LIVE_OPERATOR et manufacturing** |
| SPI3/FRAM/UART | `stm32_iis3dwb_fifo_capture.c`, `stm32_fram_storage.c`, `stm32_serial_transport.c` | Transferts HAL SPI bloquants dans plusieurs voies existantes ; LPUART1 et TIM6 priorité NVIC 5 ; **le découplage des bus SPI ne libère pas automatiquement le CPU pendant les latences SD** |
| CMake/compilateur | `../05_Firmware/platform/stm32/CMakeLists.txt`, `cmake/arm-none-eabi-gcc.cmake` | Options explicites `-mcpu=cortex-m33 -mthumb` pour cœur et cible ; **pas de `-mfpu`/`-mfloat-abi` dans ces deux fichiers**. Le STM32U575 a une FPU simple précision, mais le code compilé ne peut pas être présumé l'utiliser. Le fichier `compile_commands.json` final, les flags hérités GCC, les attributs ELF et le désassemblage doivent être vérifiés avant chiffrage float32 |

Les mesures R4V citées proviennent d'un autre artefact et d'une autre session que l'image AC1 aujourd'hui installée ; elles **ne donnent pas automatiquement la marge du build actuel**. La mission AC1 encore `BLOCKED` concerne le paquet de supervision Windows et sa qualification physique, non un échec prouvé du FIR /10 inexistant.

## 2. Calcul de charge du FIR /10, sans hypothèse sur le coût d'une MAC

- À ODR nominale `80 000/3 XYZ/s`, une sortie sur 10 : `8 000/3 ≈ 2666,667 XYZ/s`.
- Chaque sortie exige idéalement `107 taps × 3 axes = 321 MAC` ; par seconde **856 000 MAC**, **sans** copie/contrôles de phase/arrondi/gestion d'horloge/interruption.
- À fréquence du profil R4U **160 MHz**, `1 MAC/s` ne se transforme **pas** directement en `1 cycle/s` ; charge CPU idéale, selon coût moyen réellement observé :

| Hypothèse illustrative de cycles/MAC | Cycles DSP par seconde | Part des 160 MHz |
|---|---:|---:|
| 1 | 856 000 | 0,535 % |
| 4 | 3 424 000 | 2,14 % |
| 10 | 8 560 000 | 5,35 % |
| 20 | 17 120 000 | 10,70 % |
| 50 | 42 800 000 | 26,75 % |

**Intervalle d'émission nominal :** `10 / (80 000/3) = 375 µs`, soit **60 000 cycles à 160 MHz** par XYZ archivé ; un événement FIR de 321 MAC demande respectivement **3 210 cycles (~20,06 µs)** si 10 cycles/MAC, **6 420 cycles (~40,13 µs)** si 20 cycles/MAC. Mais les XYZ arrivent **par rafales FIFO**, non en interruptions régulièrement espacées : mesurer les durées de **tous les échantillons d'une rafale**, pas uniquement la moyenne à la seconde.

**Confrontation aux anciens maxima R4V** : soustraire séparément donne work **186 µs**, service **437 µs**, gap **2 726 µs** de marges arithmétiques avant leurs limites historiques. **Ne pas cumuler ces marges ni les promettre** : work/service/gap ont des fenêtres et maxima distincts et l'ajout DSP peut déplacer la distribution de chaque maximum. Pour un burst de `60 XYZ` bruts (environ six sorties), à 20 cycles/MAC le FIR seul peut consommer **~241 µs** ; ceci excéderait la seule marge work historique de 186 µs si les mêmes maxima coïncidaient, mais **ce n'est pas une prédiction de violation**. Profilage apparié et arbitrage de taille de quanta obligatoires.

## 3. Délai FIFO et politique de priorité

Le FIFO capteur stocke **512 mots de 7 B**, certains étant des timestamps. La réserve hypothétique **512/26 666,7 = 19,2 ms** ne s'applique que si chaque mot est un XYZ et la FIFO entièrement disponible. **Avec un mot timestamp pour huit jeux XYZ** (schéma DEC8 de la voie horodatée), le rythme moyen de mots devient environ `26 666,7 × 9/8 = 30 000 mots/s` et **512/30 000 ≈ 17,07 ms** serait une réserve *depuis FIFO vide*, encore irréaliste comme timeout admissible après watermark/poll. Par exemple avec **68 mots déjà présents**, réserve moyenne `(512−68)/30000 ≈ 14,8 ms` sans autre marge ; ce nombre n'est **pas** un budget d'ordonnancement garanti.

**Architecture d'essai conseillée** : maintenir l'acquisition SPI3 et les contrôles de tags/temps/OVR déjà éprouvés comme priorité maximale du foreground coopératif ; exploiter **un callback unique** avec éventuel dispatch `archive FIR /10` et `LIVE` secondaire sur la même séquence validée. La file archive en RAM est bornée et *ne doit jamais provoquer un wait SD bloquant dans le callback capteur*. Le FIR peut traiter une rafale **au sein d'un quantum borné** ou après un transfert en petite file source bornée : décider par mesure de la taille maximale des rafales, pas imposer une ISR, DMA ou RTOS pour principe.

- **Faute de source** (tag, compteur, overrun, timestamp) : invalider et signaler le segment, ne pas extrapoler silencieusement les données source.
- **Faute de FIR** (valeur numérique dépassant int16 suite à ringing) : clamp déterministe et **flag de saturation calculée distinct** de saturation physique capteur ; ne pas supprimer les dépassements.
- **Surcharge CPU/backpressure** : retarder le LIVE/FFT et les services facultatifs avant toute archive ; si le flux archive est néanmoins incomplet, publier une **anomalie mesurable** et un préfixe durable vrai, jamais « 8 h sans pertes ».
- **Temps** : la décimation /10 conserve l'origine des index/anchors ; le **délai de groupe ≈1,9875 ms** n'est ni une date de prélèvement automatique ni une latence totale. Différencier instant physique estimé, phase de filtre, acquisition FIFO et instant de publication.

## 4. Mémoire DSP et file microSD — allocations NON décidées

- **FIR direct, fenêtre d'entrées :** 107 échantillons × 3 axes × 2 B = **642 B** si états int16 ; **1 284 B** si états float32. Coefficients communs aux trois axes : **214 B Q15** ou **428 B float32** ; plus index/phase/accumulateur, alignement, buffers SPI/FAT et éventuelle conversion/qualité.
- **Flux écrit :** 16 000 B/s par TR2 à nominal. Un tampon **16 Kio** = **1,024 s**, **32 Kio** = **2,048 s**, **64 Kio** = **4,096 s** de payload, *sans compte des headers/CRC/filesystem/réserves et sans garantie contre un busy SD supérieur*. Aucun de ces buffers n'est alloué par la présente étude.
- **La marge RAM de ~126,8 kB est indicative avant pile/heap** sur l'image AC1 signalée. Exiger MAP, régions SRAM, stack high-water, alignment DMA/FS et mémoire déjà occupée de l'**artefact EXACT** ; refuser les allocations imaginées à partir du seul descriptif U575 « 786 Kio SRAM ».

## 5. Porte de mesure R1-ARM : étapes réalisables sans démolir le socle

| Porte | Mesure ou preuve requise | Verdict acceptable |
|---|---|---|
| **A0 — Build réel** | Compiler les profils de banc et de référence retenus avec options/versions exactes, compiler commands, ELF/map/size/sha ; inspecter si FPU effective, compatibilité ABI, instructions éventuelles DSP, pile et temps | Baseline qualifiable, sans faire croire à une mesure de rendement |
| **A1 — Oracle DSP** | Rejouer les vecteurs /10 `D10-00..D10-11` sur hôte et ARM, y compris phase arbitraire, pertes, stop, extrema, quantification, saturation numérique ; comparator indépendant | Compte/timing/qualité intègres, gain et alias dans les tolérances définies |
| **A2 — Bench CPU sans média** | Sur **TR2-002 identifiable**, mesurer cycles DWT par bloc FIFO, par émission FIR, par service (moy/P95/P99/max), niveaux FIFO, gaps, IRQ, erreur; comparer **profil sans FIR** au **même profil + FIR** | Zéro erreur FIFO non signalée et respect des limites de service établies pour ce build |
| **A3 — Coexistence CPU/FRAM/RTU** | Même stimulus avec UART réel, transactions FRAM légales et publication LIVE, comparer coûts sur séquences appariées sans modifier les règles B5/FRAM | Respect budgets et récupération / stop, aucune perte silencieuse |
| **A4 — Dimensionnement stockage** | Introduire un producteur synthétique de 16 ko/s indépendamment du capteur, mesurer busy tail SD/FAT/powercut ; seulement ensuite mesurer **ensemble** FIFO+FIR+SPI2/FRAM/RTU | Backlog borné, préfixe durable et interface PC prouvés ; endurance 10 min ->1 h ->8 h ultérieure |

**Préflight physique future** : TR2-002 seulement, relire STLINK/UID/CID, alimentation 3,3 V, câblage SPI2 et SWD sans conflit, SD sacrificielle 32 Go ; l'utilisateur fait les manipulations physiques. TR2-001 ne sert pas de banc expérimental.

**Séquence d'autorisations** : futur contrat technique ChatGPT → GitHub → Codex → GitHub → revue ChatGPT, `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true` si STM32 impliqué, avec les garde-fous AGENTS. **Aucune nouvelle mission avant clôture/reprise gouvernée de l'AC1 déjà active (`MISSION_EN_COURS: READY`, `DERNIER_RAPPORT: BLOCKED`)**. Aucune prétention d'exécution Codex dans cette note.

## 6. Archivage et références immuables

**Point de retour R0 demandé et conservé** : commit `a3f5ac33a728a5afd028192654c17e5b04ee8d7b`. Le seul SHA est immuable, mais sa conservation à long terme devient plus robuste lorsqu'il est référencé explicitement (tag annoté de sauvegarde distinct de `tr2/Mnnn` **ou branche d'archive contrôlée**), avant le premier développement expérimental ; vérifier distante la référence et le commit, sans `force push`, `reset --hard` ni changement de `main`. Un tag/branche ne restaure jamais FRAM, microSD, Flash ou données Windows externes.

Les valeurs de R4V et de la carte microSD sont des **données historiques déjà publiées**, jamais des mesures obtenues au cours de cette revue documentaire. Les ressources CPU et RAM sont **plausibles mais non qualifiées**.

**Références constructeur :** [STM32U575 ST, Cortex-M33 FPU SP et 160 MHz](https://www.st.com/en/microcontrollers-microprocessors/stm32u575ai.html) ; [GCC ARM -mfloat-abi et incompatibilités soft/hard ABI](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html). Mesure d'exécution et arithmétique sur image cible à vérifier via les options effectives et le désassemblage.
