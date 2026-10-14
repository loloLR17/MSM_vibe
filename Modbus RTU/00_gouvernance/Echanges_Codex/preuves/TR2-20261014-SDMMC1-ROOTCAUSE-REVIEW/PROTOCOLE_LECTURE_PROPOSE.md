# Protocole proposé : lecture hors MSC, 1 bit / 4 bits à 12 MHz

**NON EXÉCUTÉ.** La mission documentaire n'autorise aucun flash/debug. Ce protocole exige un nouveau contrat et ne constitue pas un firmware préparé ou une qualification.

## Question et invariants

Isoler la première erreur SDMMC de la propagation MSC/recovery. Deux bras bornés : **A=1 bit**, puis **B=4 bits uniquement si A est conforme**. Même seconde Nucleo, ST-LINK0032001F3235510C37333439 / UID001B0046/38425018/20333157 ; même carte test CID03534453/44333247/858BF092/3C01927A,62333952secteurs512. Principal/FRAM/512Go/Modbus exclus.

Même câblage10cm/SB19–22, alimentation, GPIOAF12/PULLUP/HIGH, CPU160MHz, HAL tiers1.6.2, profil ReleaseOs, polling et fréquence12MHz. Deux profils du même probe isolé à construire : USBMSC/TinyUSB/FatFs non initialisés, séquence SD d'origine conservée, sélection explicite `HAL_SD_ConfigWideBusOperation` avec macro1B ou4B, aucun fallback silencieux. Diffs, build et hashes séparés ; image originale vérifiée existante conservée, pas de sauvegarde complète redondante. Pas de correction CMD12/recovery dans ce protocole.

## Manipulations futures minimales

Carte dans lecteur PC selon utilisateur ; Nucleo non présumée connectée. Dans le nouveau contrat : éjection PC sûre, seconde Nucleo hors alimentation, insertion de la carte de test au breakout, CN15 natif débranché. Aucun changement de fils/jumper/soudure. Pour qualifier un état électrique initial indépendant, mesurer **VDD du breakout par rapport à son GND proche de0V après décharge**, avant chaque bras ; si ce contrôle est impossible, le signaler et limiter la conclusion, sans le remplacer par un reset logiciel.

L'utilisateur manipule et mesure ; Codex attend sa confirmation. ST-LINK seul reconnecté pour flash/debug qualifiés. Si une transition l'exige, charger le probe SD sûr carte absente avant insertion hors tension ; ne pas présumer sûr un firmware GPIO inconnu.

Une coupure supplémentaire entre A et B contrôle l'état initial, **sans retry du bras précédent**. Maximum un départ par bras. Échec A : ne pas lancer B.

## Séquence et preuves

1. Vérifier cible, programmateur, hashes et garde CID/capacité. Init1bit HAL lente≤400kHz, puis sélection explicite1B/4B à12MHz. Enregistrer retours HAL_Init/WideBus, ErrorCode, CLKCR/POWER/RCC, GPIOC/D MODER/AFR/PUPDR/OSPEEDR, NVIC/MASK, TIM17tick et IPSR avant appels. Aucun trigger de données tant que les gardes manquent.
2. Une acquisition SSR via `HAL_SD_GetCardStatus`, phase séparée, vérifie la largeur côté carte. **C'est un transfert actif** : arrêter si elle échoue. Un CMD13 initial de readiness est autorisé et tracé, sans boucle100000. Exiger configuration et état concordants, sans erreur.
3. Zone connue **carte LBA506304..506319**,16secteurs/8192octets, dans ancien jeuB506304..506559. Deux lectures au plus par bras : CMD17/512octets à506304 ; seulement si conforme, CMD18/8192 au même LBA avec CMD12 normal de la HAL. Aucun write. Comparaison intégrale à l'oracle historique et SHA :

| Préfixe | SHA-256 attendu |
|---|---|
|512octets|a9f4d9b4604080bea7999abf2d2874653e71c43a7805bc1735068b1f004f3daf|
|8192octets|2ac5183f9757f9b08e94e4cf2f4b1ee03a3a66171e8b366989e5b36b93ab563b|

Oracle recalculé hors ligne depuis la lecture PC antérieure131072octets SHA`c8a5f1e01ab61ebcace803eb63d5cf360035b17a8aaaae309369c3922a611df4`. Les zones PC1GiB/3GiB ne la recouvrent pas ; l'actualité des données sera démontrée par les futures lectures.

4. Instrumentation RAM minimale : phase, CMD/ARG/LBA/taille, retour, durée DWT/tick, ErrorCode avant/après, **première erreur figée avant clear**, STA/RESPCMD/RESP1/DCTRL/DLEN/DCOUNT/CLKCR/MASK, contexte HAL/IPSR. Une copie séparée HAL/LL conserve toutes les conditions, cleanup et timeouts. Captures sur branches d'erreur et frontières de phase : HAL_ReadBlocks avant ICR, LL `SDMMC_GetCmdResp1` avant clear/décodage, chemins init/SSR correspondants si nécessaires. Pas de printf/semihost ni sérialisation dans le polling, pas de lecture FIFO par le debugger. Si certains premiers flags échappent aux hooks, documenter cette limite. Diff et coût inspectés ; aucune instrumentation présentée comme l'image originale stricte.
5. Après retour HAL en échec : **aucun CMD13/CMD12/abort/retry ajouté**. Terminal RAM sans nouvelle commande SD, first_error jamais écrasé. Après HAL_OK, un CMD13 de fin, tracé séparément, peut vérifier TRANSFER ; son échec est celui de la phase STATUS, pas une preuve que le payload exact est faux.
6. Exécution autonome : GDB détaché avant RUN, aucun halt/breakpoint pendant polling, lecture RAM/données seulement au terminal, aucun appel SD depuis GDB ou WindowsMSC. Délais logiciels bornés ; plafond opérateur10s par bras. Dépassement : arrêt diagnostique documenté comme perturbation, pas reset. Publier les bruts avant toute récupération.

## Arrêt et interprétation

| Issue | Décision et portée |
|---|---|
|Identité/CID/rail/configuration douteux|Aucune lecture de données ; clarification/intervention nécessaire.|
|Init/SSR/lecture A échoue|Figer la première erreur, arrêt total ; 4bits non comparé, examiner le chemin commun selon phase.|
|A conforme, B échoue|Arrêt B ; sensibilité4bits renforcée, mais lignes supplémentaires **et débit polling4×** changent ensemble : cause électrique non prouvée.|
|CMD17 passe, CMD18/CMD12 échoue|Événement multibloc isolé ; distinguer première erreur et défaut de récupération.|
|Données exactes, CMD13 de fin échoue|Conserver séparément intégrité et échec de statut.|
|A et B conformes|Lecture hors MSC bornée acquise ; contexte USB/IRQ/hôte ou intermittence à examiner dans une mission distincte, sans causalité USB prouvée.|
|Timeout ou différence sans erreur HAL|Arrêt et conservation des dumps/oracles ; pas de succès forcé.|

Pas de troisième bras, relance, testUSB immédiat,20×64MiB, endurance600s/7jours, recovery corrigé ou qualification TR2. Mesures dynamiques ultérieures seulement selon première erreur ; le multimètre DC ne tranche pas l'intégrité des fronts. Contrôle ChatGPT et publication du nouveau contrat avant exécution.
