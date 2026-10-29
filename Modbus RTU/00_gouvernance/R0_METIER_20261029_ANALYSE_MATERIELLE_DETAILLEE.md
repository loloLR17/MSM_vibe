# MSM TR2 — R0-MÉTIER — Analyse matérielle approfondie, avant arbitrage

**Statut : ÉTUDE COMPARATIVE — RECOMMANDATIONS CONDITIONNELLES, AUCUN REMPLACEMENT VALIDÉ.** Date : 29/10/2026. Responsable : ChatGPT. Référence technique : branche `main` courante lors de la revue. Complément à [la première revue matérielle](R0_METIER_20261029_REVUE_ARCHITECTURE_MATERIELLE.md) et au [cahier métier R0](R0_METIER_20261029_CAHIER_DES_CHARGES.md). Aucune nouvelle mission Codex, aucun flash, aucune modification des contrats gelés.

## 1. Principe décidé par l'utilisateur

**Ne changer aucun composant par principe.** Une modification n'est recevable que si une exigence essentielle ne peut être tenue, ou si le gain mesurable (précision, robustesse, simplicité matérielle, coût et maintenance) justifie clairement le travail de portage. **Méthodologie ChatGPT → GitHub → Codex → GitHub → ChatGPT inchangée.**

Contrat produit à préserver : instrument ponctuel de vibrations, 2–25 Hz **prioritaire**, objectif étendu 2–700 Hz, ±8 g nominal, XYZ archive à 2 000 Hz/axe cible, 8 heures sur **microSD amovible** puis import PC. Pas de synchronisation de phase entre capteurs. Ethernet/PoE et Modbus TCP/téléchargement réseau sont à examiner pour le matériel final, non présumés arbitrés en V1.

## 2. Point d'architecture dominant : l'acquisition non bloquante

**Données constructeur :** IIS3DWB ~26 667 jeux XYZ/s, FIFO 3 Kio environ, ±2/4/8/16 g. À 6 octets par jeu XYZ, 3 072/(26 667×6) = **19,2 ms** de réserve **idéale** ; le format d'entrées FIFO avec tags et la marge d'alarme peuvent réduire ce chiffre. Cette estimation n'est pas une latence de service admissible qualifiée.

**Données du projet :** le rapport M017 a observé ~248 ms pour un transfert SD de 64 Kio dans son run qui a débordé. D'autres essais SDMMC à débit ponctuellement correct n'ont pas satisfait l'endurance. **Le besoin réduit de 12 ko/s d'archive n'évite donc absolument pas l'overflow du capteur si un transfert SD bloque la lecture FIFO.** Il faut une architecture permettant l'accès capteur préemptif/garanti : interrupteurs/DMA, trafic SPI délimité, traitement incrémental, FIFO drain, doubles tampons, répartition des périphériques, périodes d'écriture bornées et métriques de pertes. Une écriture potentiellement longue de microSD SPI devra être **non bloquante** vis-à-vis du service du capteur ou compatible avec les pires délais démontrés.

**Condition technique impérative** : vérifier d'abord les budgets réels (interruptions, temps de service maximum, CPU, mémoire SRAM, queue de fichiers et durée d'I/O), avant de décider qu'un MCU plus puissant est indispensable. Il n'existe encore **aucune qualification 8 h**.

Références : https://www.st.com/resource/en/datasheet/iis3dwb.pdf ; [M017 — rapport](Echanges_Codex/archives/TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION/RAPPORT.md).

## 3. Microcontrôleur : trois scénarios réels

| Scénario | Atout | Risque/limite | Verdict provisoire |
|---|---|---|---|
| **U575 existant + Ethernet externe à contrôleur TCP/IP (W5500, par exemple)** | Réutilisation maximale du firmware/driver déjà vérifié ; 160 MHz et ~786 Kio SRAM au total sur U575 ; Ethernet 10/100 avec SPI | **Trois interfaces SPI seulement** sur U575 : SPI1 FRAM, SPI3 IIS3DWB qualifiés, SPI2 possible pour SD seulement après revue pinout ; W5500 introduirait un nouveau partage/ordonnancement sur SPI ; aucun contrôleur Ethernet MAC interne. RAM de build et latences DMA doivent être vérifiées. | **Option de référence, à tester avant tout changement de MCU** |
| **STM32H563/H573 Ethernet MAC natif** | Ethernet intégré au MCU (PHY séparé), 250 MHz | SRAM ~640 Kio : **moins** que le U575 et **moins** que BSS du checkpoint AC1 (~659 176 octets) ; pas de reprise byte-identique de ce firmware. Portage HAL/STM32 + mémoire. | **Pas un remplacement automatiquement bénéfique**, exiger un projet mémoire démontré |
| **STM32 Ethernet plus puissant avec marge SRAM, ex. famille H7 adaptée** | Plus de RAM / CPU possibles, Ethernet MAC natif, interfaces supplémentaires selon référence | Portage, DMA/cache/mémoire, CEM, consommation, choix des références longévité/disponibilité, coûts de qualification. Un H743 donné possède ~1 Mo de SRAM mais le statut commercial doit être contrôlé **sur référence exacte** avant adoption. | **Solution de repli si les limites U575 sont prouvées** |

**Point réel à vérifier dans le dépôt** : le script de liaison `05_Firmware/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld` expose **768 Kio** dans sa région `RAM`, tandis que la documentation STM32 indique **786 Kio de SRAM totale** répartie en plusieurs régions. Le checkpoint ARM ±8g/AC1 indique `bss=659176`, `data=492`, mais il ne mesure pas la marge de pile effectivement disponible ni ne prouve la mémoire minimale du produit final. **Lire les MAP pour inventorier les plus gros buffers, distinguer instrumentation/production et établir une marge** avant comparaison H563/H7.

W5500 : 10/100 MAC+PHY+TCP/IPv4 matériel via SPI, 32 Kio buffers réseaux et 8 sockets ; **ne comprend pas PoE**, ne remplace pas la nécessité de sécuriser firmware/mode d'accès réseau. Le gain de vitesse Ethernet doit être confronté à la contention SPI et au débit de lecture SD, en particulier si téléchargement pendant acquisition.

Sources : https://www.st.com/en/microcontrollers-microprocessors/stm32u575ai.html ; https://www.st.com/resource/en/datasheet/stm32u575zi.pdf ; https://www.st.com/content/st_com/en/technical-documents/an5688-migrating-microcontroller-applications-from-stm32f427437-and-stm32f429439-to-stm32h573563-and-stm32h562-mcus-stmicroelectronics.html ; https://www.st.com/en/microcontrollers-microprocessors/stm32h743-753.html ; https://docs.wiznet.io/Product/Chip/Ethernet/W5500 .

## 4. Accéléromètre : l'option ADXL355 mérite une comparaison, PAS une migration automatique

| Critère | IIS3DWB actuel | ADXL355 candidat |
|---|---|---|
| Pleine échelle | ±2/4/8/16 g ; marge d'évolution vers ±16g | ±2/4/8 g ; **±8g maximum nominal** |
| Densité de bruit annoncée | ~75 µg/√Hz trois axes | 22,5 µg/√Hz à ±2g, **25 µg/√Hz à ±8g** |
| Débit sortie | ~26,7 kHz fixe utilisé dans TR2 | Configurable, 4 kHz maximum |
| FIFO interne | 3 Ko ; réserve ≈19 ms idéale au débit courant, à nuancer | **96 emplacements d'un axe**, soit **32 jeux XYZ à 4 kHz = 8 ms théoriques** ; malgré l'ODR inférieur, service FIFO plus urgent |
| Réponse filtrée jusqu'à 700 Hz | Large bande physique nominale, décimation externe à développer/qualifier | En mode **2 kHz ODR, LPF intégrée à 500 Hz** : **700 Hz non garanti** en passe-bande ; il faudrait considérer **4 kHz ODR, LPF 1 kHz** puis filtrage/décimation externe et caractérisation complète |

L'amélioration du bruit ADXL355 est réelle sur fiche, mais l'importance pratique dépend de l'amplitude minimale **que le TR2 doit discriminer**. Approximation théorique de bruit blanc intégré **dans 2–25 Hz** : IIS3DWB ~75×sqrt(23)=**0,36 mg RMS** ; ADXL355 ±8g ~25×sqrt(23)=**0,12 mg RMS** ; **ces estimations ne sont ni des mesures physiques du TR2, ni des niveaux absolus de vibration**, et omettent imperfections/filtres/montage. Si le signal utile fait 50–500 mg, le différentiel pourrait être peu déterminant ; si l'on veut caractériser ~1 mg, il devient significatif. Pour le projet actuel, le besoin primaire porte sur de fortes vibrations structurelles, sans seuil métrologique minimal encore spécifié.

**Ne pas sacrifier la capacité ±16g ni les acquis pour un simple chiffre de résolution 20 bits**, tant que les niveaux réels, l'absence de saturation et les coûts de décimation ne sont pas mieux connus. Tester le montage mécanique et l'emplacement avant d'envisager une substitution de capteur.

Sources : https://www.st.com/resource/en/datasheet/iis3dwb.pdf ; https://www.analog.com/media/en/technical-documentation/data-sheets/adxl354_adxl355.pdf (Rev D : tableaux bruit/FS, filtres, FIFO, ODR).

## 5. FRAM : maintien recommandé sauf preuve contraire

La FRAM SPI de 256 Kio contient une persistance A/B, CRC, configuration active et journal transactionnel. Elle n'enregistre pas le flux XYZ, qui relève de la microSD. La supprimer imposerait d'attribuer à une autre mémoire les mêmes garanties de taux d'écriture, état atomique/recovery, protection des données historiques et panne PoE. **Aucun défaut métier ni architectural attribuable à la FRAM n'a été démontré.** Conserver la FRAM par défaut, tout en permettant un futur benchmark si coût/complexité ou contraintes de bus deviennent avérés.

Référence : [layout physique FRAM A/B](CONCEPTION_FIRMWARE_P12H3D1D_PHYSICAL_FRAM_LAYOUT.md).

## 6. Ethernet + PoE : séparer connectique finale et services V1.5

Si un câble réseau est accessible aux sites de mesure, **un TR2 avec PoE peut éliminer une alimentation séparée** ; Modbus ne devient pas inutile, mais passe potentiellement à **Modbus TCP** pour états/commandes, distinct du transport de fichiers des campagnes. Le capteur doit **continuer sa capture sur microSD lorsque le PC ou le réseau IP est indisponible**, tant que la puissance électrique reste présente.

- **PoE n'est pas Ethernet** : module/controller PD, conversion DC/DC (isolation et protections selon architecture/normes), Ethernet magnetics, filtrage CEM, budget de puissance et injection de bruit capteur à concevoir.
- **Perte du switch ou du câble PoE = perte d'alimentation**, sauf source secourue ; prévoir détection brownout et format de fichiers/checkpoints récupérable, pas de faux flush garanti si l'énergie tombe instantanément.
- **Aucune obligation V1 de réseau TCP ou téléchargement** tant que la décision n'est pas prise ; même en matériel PoE-ready, le prototype actuel RS485 reste valable.
- **Risque principal sur U575** : SPI1 FRAM + SPI3 capteur et trois SPI totaux : SD et contrôleur réseau peuvent être en concurrence. Téléchargement pendant enregistrement **à qualifier ou différer explicitement** en V1.5.

## 7. Décisions techniques — statut d'arbitrage

| Sujet | Proposition fondée sur les preuves | Statut |
|---|---|---|
| **STM32U575** | **Conserver pour les essais R1/R2** ; autoriser MCU alternatif seulement si CPU/RAM/periph ou architecture IP démontrent un manque bloquant | Proposition, non décision de migration |
| **IIS3DWB** | **Conserver pour prototype et référence**, tester bruit/FS/montage/charge ; ADXL355 candidat si défaut de performance ou gain métrologique justifié | Proposition |
| **FRAM** | **Conserver** sauf défaut de cohérence, consommation, bus ou coût réel identifié | Proposition |
| **microSD** | Support amovible **déjà décidé** ; priorité au nouveau backend **SPI qualifié**, sans répéter les essais SDMMC épuisés | Décision métier antérieure, technologie non qualifiée |
| **Ethernet + PoE** | **Étudier comme architecture cible du PCB**, indépendamment du firmware banc RTU et de la V1.5 ; valider l'accessibilité des câbles et le besoin de secours | **Ouvert, décision utilisateur à prendre après analyse** |
| **RS-485 sur PCB** | Le banc est conservé ; suppression sur produit final possible si IP unique confirmé | Ouvert |

## 8. Gates d'arbitrage et tests ciblés : seulement ce qui peut faire changer une décision

1. **Porte mesure (capteur)** : fixer une vibration minimale utile à mesurer et les critères de comparaison, puis valider réponse du IIS3DWB à 2–25 Hz, amplitude/ruptures à ±8g et bande étendue à 700Hz ; privilégier les résultats de l'existant. Si les critères sont manqués, comparer ADXL355 en banc représentatif.
2. **Porte ressources (MCU)** : fichier MAP de l'image opérateur réelle, mémoire/piles, pire délai de service FIFO, CPU de décimation et communication, coût d'un service microSD non bloquant. **Ne pas choisir de MCU avant ce bilan.**
3. **Porte microSD** : essais **différents** de l'historique SDMMC, intégrité et latences d'écriture, accès fichiers PC standard, tampon et gestion pannes. La preuve 8h demeure le gate majeur.
4. **Porte réseau/PoE** : besoin d'un câble PoE sur chaque site, alimentation de secours éventuelle, budget énergétique, Ethernet/W5500 vs MAC natif, topologie SPI et débit de téléchargement. Ne pas acheter/souder un module réseau avant d'avoir identifié le chemin matériel acceptable.
5. **Porte intégration** : choisir le matériel du PCB de production uniquement après les quatre portes ; les interfaces portables `VibrationSource`, `PersistentMedia`, `CampaignBulkMedia` déjà présentes diminuent le risque d'un portage si nécessaire.

**Pas de glissement d'objectif :** aucun développement de l'interface Ethernet/PoE, aucun portage CPU, aucun remplacement de capteur décidé ici. Cette analyse sera contrôlée et arbitrée par l'utilisateur en R0 ; ChatGPT assurera ensuite les modifications de spécifications et les éventuelles missions Codex selon la gouvernance courante.

## 9. État de Codex lors de cette analyse

Le contrat `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` reste la mission active. **Le dernier rapport distant est désormais BLOCKED**, non IN_PROGRESS, car l'exécution Windows du paquet R2 de la supervision a été refusée par le contrôle de confiance/SAC, **bien que le firmware logiciel soit publié**. L'essai physique ±8g/AC1 n'est pas qualifié. Ce blocage est **distinct** du réexamen matériel R0 et ne doit pas servir de preuve de faiblesse du STM32 ou de l'IIS3DWB. Aucune nouvelle mission ne remplace la mission courante sans décision de clôture/reprise conforme.

