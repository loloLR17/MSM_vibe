# MSM TR2 — R0-MÉTIER — Étude microSD en SPI2 avec FRAM SPI1 et IIS3DWB SPI3

**Statut : ANALYSE DE FAISABILITÉ DOCUMENTAIRE — PINOUT CANDIDAT, PAS UN ESSAI NI UNE QUALIFICATION.**
Rédaction : ChatGPT, 29 octobre 2026. Référence : `loloLR17/MSM_vibe/main` et [R0 matériel](R0_METIER_20261029_ANALYSE_MATERIELLE_DETAILLEE.md). Déclencheur : l'utilisateur souhaite étudier puis tester la microSD en mode **SPI**, distinct des expériences antérieures SDMMC 1/4 bits, en **conservant FRAM et accéléromètre** sur leurs SPI actuels. Ne pas supposer qu'un test SDMMC réussi ou échoué qualifie SPI.

## Confirmation matérielle utilisateur — 29/10/2026 (complément R0, non-qualification)

L'utilisateur confirme explicitement :
- **TR2-002 est la seconde NUCLEO historique** des essais SDMMC, et non un troisième banc à identifier. Les archives M017 l'associent à ST-LINK **SN0032001F3235510C37333439**, UID STM32 **001B0046/38425018/20333157**. Ces identifiants historiques guident le contrôle avant intervention ; ils devront être relus réellement avant flash, debug ou écriture.
- **La microSD de test 32 Go associée à TR2-002 est intégralement disponible pour les écritures et essais**, y compris les opérations destructives usuelles nécessaires à sa qualification (initialisation, tests étendus, formatage ou repartitionnement si effectivement utile). Son contenu n'a plus à être préservé comme donnée utilisateur ; la carte et son CID devront néanmoins être vérifiés sur le banc avant toute opération. Le CID historiquement rapporté en M017 est **03534453/44333247/858BF092/3C01927A**, sans présomption de relecture actuelle.
- **TR2-002 n'est pas monté** : ce banc pourra être câblé, instrumenté et reconfiguré librement pour les tests utiles. Les opérations physiques demeurent exécutées par l'utilisateur ; ne pas les déduire déjà réalisées.
- **TR2-001 est exclu de ces expériences** : préserver sa FRAM, sa microSD, son firmware, son câblage, ses autorités et ses historiques.

Conséquence : le préflight porte désormais sur la **vérification d'identité réelle et de sécurité électrique**, non sur la recherche d'une zone non destructive sur une carte déclarée libre. Les premiers essais d'écriture resteront bornés pour leur valeur diagnostique, puis pourront être étendus à tout le média si les résultats l'exigent. Cette confirmation **ne vaut ni autorisation de publier une mission Codex maintenant, ni essai SPI exécuté**, la revue R0 restant en cours.

## 1. Conclusion : SPI2 dédié recommandé comme **premier candidat de banc**

| Périphérique | Ressource STM32U575 actuelle/candidate | Décision d'étude |
|---|---|---|
| FRAM SPI 256 Kio | **SPI1** PA5 SCK / PA6 MISO / PA7 MOSI ; CS PD14 | Conserver **sans modifier** |
| Accéléromètre IIS3DWB | **SPI3** PC10 SCK / PC11 MISO / PC12 MOSI ; CS PC9 | Conserver **sans modifier** ; la lecture FIFO reste la priorité |
| microSD Adafruit 4682 | **SPI2** à initialiser, **non présent dans le `main.c` actuel** ; pinout proposé ci-dessous | **Premier essai** ; pas de sharing avec FRAM/capteur |
| Modbus RTU | LPUART1 PG7 TX / PG8 RX | Préserver |
| PoE/Ethernet éventuel | Interface non décidée ; W5500 SPI externe demanderait un partage ou autre architecture | Étude séparée, **aucun blocage de R2 microSD** |

**Pourquoi pas les autres options ?**
- **SD sur SPI3 du capteur : non recommandé**, car les longues transactions et périodes busy de la carte peuvent retarder le drainage FIFO. Ne pas toucher au câblage capteur acquis.
- **SD sur SPI1 de la FRAM : possible** en principe avec CS séparé, mais ajoute arbitrage aux journaux/autorités transactionnelles, risque de contention, modes SPI/timings différents ; sans intérêt avec SPI2 disponible.
- **SPI2 dédié :** isole physiquement les transactions. Toutefois, la séparation matérielle **ne rend pas automatiquement les appels HAL SD non bloquants pour le CPU** ; les waits busy, SPI DMA/IRQ et priorités devront être maîtrisés.

Références du dépôt : [occupation physique](CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md), [pinout SPI3 IIS3DWB](CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md), `../05_Firmware/platform/stm32/main.c` (déclare `hspi1`, `hspi3`, `hsd2`, **pas `hspi2`**), et [rapport M017 SDMMC](Echanges_Codex/archives/TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION/RAPPORT.md).

## 2. Carte Adafruit 4682 : **déjà compatible SPI**

Le constructeur décrit officiellement le module « Micro SD **SPI or SDIO** Card Breakout Board - 3V ONLY! », product ID 4682. **Aucun changement de breakout n'est requis** pour valider SPI ; alimentation et niveaux **3,3 V exclusivement**, avec pull-ups déjà présents. Le sens des signaux microSD en SPI est :
- `CLK` : `SCK` ;
- `CMD` / `SI` : `MOSI` depuis STM32 ;
- `D0` / `SO` : `MISO` vers STM32 ;
- `D3` / `CS` : sélection carte, active bas ;
- `D1`, `D2` : inutilisés pour SPI (ne doivent pas être pilotés par l'hôte) ;
- `DET` : détection optionnelle, pas nécessaire à l'essai initial.

**Documents constructeur :**
https://www.adafruit.com/product/4682
https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts

## 3. Brochage candidat SPI2 — optimisé pour préserver l'existant

| Signal carte SPI | Broche STM32U575 | AF/GPIO | Connecteur NUCLEO | Ancien câblage SDMMC2 |
|---|---|---|---|---|
| `CLK` / SCK | **PB13** | SPI2_SCK AF5 | **CN12-30** | **Déplacer** le fil CLK depuis PD6 / CN11-43 |
| `D0` / MISO | **PB14** | SPI2_MISO AF5 | **CN12-28** | **Réutiliser** DAT0, déjà présent |
| `CMD` / MOSI | **PD4** | SPI2_MOSI AF5 | **CN11-39** | **Déplacer** le fil CMD depuis PD7 / CN11-45 |
| `D3` / CS | **PB4** | GPIO sortie, CS haut hors transaction | **CN12-27** | **Réutiliser** DAT3 déjà raccordé |
| `3V3`, `GND` | 3,3 V et GND | — | CN11-16/20 | **Conserver** |
| `D1` | ancien PB15 | GPIO haute impédance, **aucun AF SDMMC2** | CN12-26 | Déconnecter de préférence physiquement, sinon **garantir Hi-Z**, ne pas relier à MOSI |
| `DAT2` | ancien PB3 | GPIO haute impédance, **pas de trace/SWO** | CN12-31 | Déconnecter de préférence physiquement, sinon **garantir Hi-Z** |

**Points importants** :
- C'est un **pinout candidat fondé sur les fonctions alternatives STM32U575 DS13737 Rev10 et les connecteurs UM2861 Rev10** ; la disponibilité des broches dans le firmware de test et leur accès physique devront être recontrôlés sur le banc réel et l'image exacte.
- `PB4` (broche JTAG NJTRST en fonction de debug) est utilisée **comme GPIO CS** pour ce test ; **SWD** PA13/PA14 reste opérationnel, sans activer les fonctions JTAG conflictuelles.
- `PB3` peut servir de **SWO/trace** : si son ancien fil DAT2 est resté raccordé, l'absence de pilotage et l'état haute impédance réel sont impératifs. **Préférer le débranchement de DAT1 et DAT2 côté breakout** pour éviter des comportements de debug variables.
- Les anciens profils SDMMC configurent explicitement PD6/PD7 en AF11 et PB3/PB4/PB14/PB15 en AF12, **incompatibles** avec le câblage SPI ci-dessus. Créer un **profil de banc SPI séparé**, jamais lancer une image SDMMC sur une carte recâblée SPI.
- `PB13` est nouveau dans ce câblage ; le fait qu'un fil soit soudé ou non sur CN12-30 doit être vérifié, sans présumer l'accessibilité physique.
- **Interventions de fils/carte physiques : utilisateur seulement, alimentation retirée avant modification.** Préserver les configurations et données du banc principal. **Le banc identifié comme TR2-002 et sa microSD 32 Go sont désormais explicitement disponibles pour les essais destructifs usuels de stockage**, après relecture de l'identité de la cible et du média. TR2-001 demeure intégralement protégé.

Références constructeur officielles : DS13737 (notamment AF5 SPI2) https://www.st.com/resource/en/datasheet/stm32u575zi.pdf ; UM2861 table 19 https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf .

## 4. Débit : très raisonnable en théorie, mais la latence CPU reste le vrai risque

**Nouvelle charge V1 cible** : 2 000 XYZ/s × 3 axes × 2 octets = **12 000 B/s** (hors headers/CRC/FAT). Le flux brut IIS3DWB à ~26 667 jeux XYZ/s est de l'ordre de **160 002 B/s** avant éventuel overhead FIFO/tag et filtre ; il faut **continuer à le consommer** pendant les accès SD. Le format ancien P8 de 16 B/échantillon à 26 667 Hz (environ 426 672 B/s) **n'est plus le besoin V1 cible**.

| SPI2 SCK théorique | Plafond brut des bits utiles /8 | Ratio au payload 12 kB/s (avant protocole) |
|---|---:|---:|
| 1 MHz | 125 kB/s | ×10,4 |
| 4 MHz | 500 kB/s | ×41,7 |
| 8 MHz | 1 MB/s | ×83,3 |
| 12 MHz | 1,5 MB/s | ×125 |

Les valeurs sont des **maximums physiques idéaux**, pas des débits mesurés ni des garanties d'écriture microSD. Initialisation SD à fréquence conservatrice, ensuite augmenter prudemment selon les capacités de carte, GPIO, câblage et timings prouvés. Les périodes d'écriture interne de la carte peuvent être longues, indépendamment du débit SPI.

À 12 kB/s, **1 s de blocage → 12 kB accumulés** ; **5 s → 60 kB**. Un tampon 16/32/64 Kio représenterait seulement **1,37/2,73/5,46 s** de données utiles, **avant coûts de métadonnées et marges**. Pas de dimensionnement définitif sans distribution de latences et MAP de la mémoire. Le checkpoint AC1 ARM rapporte **659 176 B BSS**, et le linker STM32U575 courant place la `RAM` principale à **768 Kio** ; les réserves pour buffer/stack varient par profil. **Ne pas présumer un ring de 128 Kio disponible**.

**Risque dominant** : FIFO IIS3DWB de ~3 Kio / ~26 667 jeux XYZ/s = **réserve idéalisée ~16–19 ms** selon format tag 7 B/XYZ versus 6 B/XYZ. Si un `HAL_SPI_Transmit(... HAL_MAX_DELAY)` ou un `wait_ready` SD de 200 ms bloque la boucle d'acquisition **même sur SPI2 distinct de SPI3**, la FIFO perd des données. **La séparation des bus est nécessaire pour isoler les transferts, mais insuffisante sans ordonnanceur non bloquant ou préemption réelle.** Même DMA ne suffit pas si le code attend sa fin dans la boucle de FIFO. Il faut mesurer et garantir le délai de drainage capteur, rendre les transferts SD et leurs états `busy` coopératifs/bornés, prioriser la FIFO, compter toutes les lacunes.

## 5. Risques nouveaux et problèmes anciens

1. **Nouveau mode SPI de carte, donc nouveau chemin driver** : le `HAL_SD` SDMMC2 ne pilote pas une microSD en mode SPI. Prévoir un transport SPI SD (CMD0/CMD8/ACMD41/CMD58, CSD/CID, lecture, écriture, CRC/erreurs, timeouts bornés, busy), puis l'adaptateur `CampaignBulkMedia` / couche fichier ; ne pas porter aveuglément HAL_SD.
2. **Accès PC** : V1 exige retrait manuel et fichiers lisibles ; un secteur brut CRC n'apparaîtra pas comme fichier Windows. Avant intégration finale, définir le filesystem FAT32 (candidat) ou équivalent PC et un format d'archive versionné. Écriture préallouée et checkpoints à évaluer, **FAT seul ne garantit pas la résistance aux coupures**.
3. **Intégrité et coupure** : séparer préfixe confirmé, préfixe non durable, reprise, plein/retrait et séquence de STOP/flush. Débits moyens sans mesure des pauses longues insuffisants.
4. **Mode de carte après SDMMC** : initialisation SPI depuis un état d'alimentation clairement défini ; **power-cycle de carte à prévoir**, et aucune commutation à chaud entre profils SPI/SDMMC. Confirmer par test.
5. **Bench firmware** : le `main.c` actuel déclare `hspi1`, `hspi3`, `hsd2`, **pas `hspi2`**. Ajouter un profil isolé d'essai, relecture/backup et preuves, sans modification de la production active ni de l'autorité FRAM.
6. **Réseau V1.5** : si plus tard contrôleur Ethernet SPI (W5500, etc.), le SPI2 serait déjà dédié à la microSD ; le réseau nécessiterait un arbitrage (partage SPI1 avec FRAM et CS propre, autre interface/MCU). **Ce point n'invalide pas le banc SPI2** et ne doit pas retarder la V1 ; le choix des bus définitifs appartient à la revue PCB/PoE.

## 6. Plan de qualification proportionné — sans refaire SDMMC 1/4 bits

### P0 — préflight sans écriture ni recâblage

- Relever l'identité effective de **TR2-002 = seconde NUCLEO historique**, la confronter au SN/UID de M017 et vérifier le profil firmware et la carte 32 Go avant toute intervention ; les identités rapportées en 2026 ne valent pas relecture actuelle. Documenter l'état initial du banc, tout en préservant uniquement les données et historiques **TR2-001** ainsi que les preuves d'essais déjà publiées.
- Contrôler l'implantation réelle des broches (CN12-30 PB13, CN11-39 PD4), les fils DAT1/DAT2, niveaux 3,3 V et l'identité de la **microSD 32 Go librement inscriptible** ; **l'utilisateur effectue les manipulations physiques** sur TR2-002 non monté.
- Identifier un code de référence SPI SD existant (ST/HAL/protocoles tiers sous licence vérifiée) ; partir d'un exemple minimal connu puis adapter. Le fait qu'un exemple communautaire fonctionne sur une autre famille STM32 n'est pas une qualification du STM32U575.

### P1 — driver SPI2 isolé, première lecture uniquement

- Firmware banc dédié, aucune commande SDMMC2, FRAM ni SD de la carte principale, CS PB4 inactif à l'initialisation, SPI2 fréquence basse d'init puis fréquence justifiée.
- Read-only : état init, CID/CSD/OCR, capacité, quelques secteurs existants et double relecture CRC/conformité ; timings de commande et détails des refus.
- Si initialisation SPI fail, comparer signaux de commande et forme d'onde aux documents Adafruit/protocole, pas une série de clocks/pinouts aléatoires.

### P2 — premières écritures bornées sur la **microSD 32 Go TR2-002 autorisée**

- Une fois P1 reçu et l'identité TR2-002/microSD confirmée, effectuer d'abord des écritures **bornées pour diagnostiquer** puis relire bit à bit avec CRC + SHA et oracle PC. Les partitions et données antérieures **de cette microSD de test** ne sont pas à conserver ; un effacement, formatage ou repartitionnement de la seule carte identifiée pourra être effectué **si utile au protocole d'essai**, sans opération sur TR2-001.
- Caractériser tailles 512 B, 4 Kio, 16 Kio et transferts séquentiels (par exemple 64 Kio), débit et surtout latences d'écriture et busy ; documenter horloge, identité carte, répétabilité et erreurs.
- Tester juste assez de variantes pour attribuer la cause d'un échec ; ne pas lancer de longue campagne si le premier test échoue.

### P3 — charge réaliste V1 + coexistence

- Producteur simulé continu **12 000 B/s** au lieu des anciens 426 672 B/s ; files/ring correctement comptés, max queue, max latence SPI, pertes explicites et validation intégrale du flux sur carte.
- Ensuite tests simultanés **IIS3DWB (SPI3) + microSD (SPI2) + FRAM (SPI1) + Modbus RTU (LPUART1)**, en prouvant le débit d'archive, les marges FIFO, les budgets CPU, et aucune perte silencieuse sous les longues pauses de carte.
- Critères de progression : 10 minutes, puis 1 heure, puis 8 heures seulement si les étapes précédentes passent ; lectures et comparaison indépendante du contenu, métriques de qualité, arrêt/flush et reprise de coupure démontrés.

### P4 — fichier exploitable par PC

- Après preuve de la voie brute, sélectionner système de fichiers standard et format de campagne adapté ; tester insertion/retrait après arrêt, import et vérification PC ; tester interruption d'alimentation et récupération **sans prétendre que FAT seul suffit**.
- Le backend SPI retenu pourra remplacer l'accès SDMMC dans une interface de stockage déjà existante, sans écraser l'historique de qualification SDMMC et sans inventer une compatibilité au niveau des blocs.

**Conditions d'arrêt/escalade** : échec répétable init/CRC/integrité, GPIO en conflit, latences incompatibles sans buffer/ordonnancement réalisables, risk matériel incontrôlé. Ne jamais passer d'une relecture SPI réussie à une affirmation de produit validé.

## 7. Gouvernance et statut

La mission Codex `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` demeure le contrat actif et son dernier rapport consulté le 29/10 est `BLOCKED` (refus Windows/SAC du nouveau paquet R2), non clôturée/qualifiée. **Ne pas écraser `MISSION_EN_COURS.md` ni déclencher un flash supplémentaire sur le banc principal** dans ce document. L'étude prépare un futur contrat technique SPI2 distinct, qui sera publié par ChatGPT après traitement gouverné de la mission bloquée. **Autorisations futures si flash/debug STM32 : `allow_flash: true`, `allow_debug: true`, avec sauvegardes et garde-fous AGENTS.md, sans validation répétitive.**

**Aucune intervention physique n'a été réalisée ici**. Aucun chemin SPI2, aucun filesystem ni write SD n'est encore qualifié.

## Sources exactes

- [P12/H3h-B2 occupation connecteurs](CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md)
- [P12/H3h-B SDMMC2 pinout historique](CONCEPTION_FIRMWARE_P12H3H_B_MICROSD_SDMMC2_PINOUT.md)
- [M017 SDMMC charge en échec](Echanges_Codex/archives/TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION/RAPPORT.md)
- [R0 fiche matérielle](R0_METIER_20261029_ANALYSE_MATERIELLE_DETAILLEE.md)
- Interfaces réutilisables : `../05_Firmware/include/tr2/persistence/campaign_bulk_media.h` et `../05_Firmware/include/tr2/platform/persistent_media.h`.
- [Rapport Codex actif](Echanges_Codex/DERNIER_RAPPORT.md) et [AGENTS.md](../../AGENTS.md).
