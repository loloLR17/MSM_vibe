# MSM TR2 — R0-MÉTIER — Comparaison 2 666,7 / 2 000 Hz et possibilité de flux XYZ sur Modbus RTU

**Statut : ÉTUDE DE CONCEPTION, NOUVELLE OPTION PRIVILÉGIÉE SOUS RÉSERVE DE QUALIFICATION, PAS D'ARBITRAGE NORMATIF NI D'IMPLÉMENTATION.** 29/10/2026, ChatGPT. Référence distante vérifiée au démarrage : `loloLR17/MSM_vibe/main` HEAD `23f5ddc28693dfb6bae63845f054aca58edf4bd9`. Demande utilisateur : comparer le maintien de l'étage intermédiaire de décimation à environ 2 666,7 Hz (économie de polyphasage) et étudier **par principe, en ordre de grandeur**, l'envoi de l'intégralité des données de vibration via Modbus RTU.

Cette note ne remplace **pas** le cahier métier préexistant qui définit toujours 2 000 Hz comme cible validée, tant que le choix de 2 666,7 Hz n'est pas formellement arbitré ; elle documente toutefois une **alternative sérieuse recommandée à la comparaison R1**.

## 1. Hypothèses : les deux sorties conservent XYZ et le DC

- IIS3DWB à ODR native **nominale** `80 000/3 = 26 666,666… Hz`, à mesurer réellement et non à supposer exacte ; trois axes signés de 16 bits (6 octets par triplet XYZ), après un filtre numérique de passe-bas qui **ne retire pas la pesanteur**.
- Bande utile métier prioritaire 2–25 Hz, étendue cible 2–700 Hz. Ce ne sont pas des exigences de fidélité à 1 000 Hz ; un taux de sortie plus élevé est permis si ses coûts/bénéfices sont documentés.
- **2 000 Hz :** FIR A225 puis FIR B69 en ×3/4, selon [étude de filtrage R0](R0_METIER_20261029_CHAINE_DONNEES_ARCHIVE_XYZ.md). **2 666,666… Hz :** **FIR A107 seul**, passe-bas à `1333,333 Hz`, décimation **/10** ; pas d'étage B.
- Aucune de ces voies n'est encore **physiquement ni numériquement qualifiée sur ARM** ; les coûts MAC/s ci-dessous sont des ordres de grandeur mathématiques d'implémentations polyphasées efficaces, pas des mesures de cycles STM32.

## 2. Comparaison fonctionnelle et temporelle

| Paramètre | 2 000 Hz, A225+B69 | 2 666,7 Hz, A107 /10 |
|---|---:|---:|
| Facteur de fréquence source/output nominal | 40/3 | 10 |
| Nyquist sortie | 1 000 Hz | 1 333,33 Hz |
| Marge Nyquist au-dessus de 700 Hz | 300 Hz | 633,33 Hz |
| FIR/complexité | 225 et 69 coefficients + phases polyphasées ×3/4 | 107 coefficients + décimation simple |
| Coût idéal XYZ | **1 938 000 MAC/s** | **856 000 MAC/s** |
| Gain sur ce coût | référence | **−55,83 %** |
| Débit payload XYZ 6 octets/échantillon | **12 000 B/s** | **16 000 B/s** |
| Volume 8 h, sans overhead | **345,6 Mo** | **460,8 Mo** |
| Volume 24 h, sans overhead | **1,0368 Go** | **1,3824 Go** |
| Délai de groupe FIR théorique | **8,45 ms** | **53/(80 000/3) ≈1,99 ms** |
| Cadence exacte réelle | `f_sensor × 3/40`, à estimer | `f_sensor / 10`, à estimer |

**Observation indépendante, calcul local de la réponse A107 seule** : `scipy.signal.firwin(107, 1333.3333333, window=('kaiser', 7.85726), fs=80000/3)`, `freqz` (1 048 576 points), gain DC normalisé 1. Erreur amplitude maximale en bande 0–700 Hz environ **0,001 dB**, pire réponse calculée pour `f >= f_out-700 = 1966,666… Hz` environ **−80,60 dB**. Cela rend A107 **candidat crédible** pour préserver 0–700 Hz après **une seule décimation /10**. La fuite de −30 dB constatée pour A107+B69 à 2 000 Hz provenait de la **cascade et de son second repliement**, **pas** d'un défaut de cet A107 employé seul pour 2 666,7 Hz. L'analyse des alias à toutes les fréquences, coefficients quantifiés, transitoires et acquisition physique reste à qualifier. La zone 700–1 333 Hz en sortie n'est **pas** contractuellement une bande à fidélité garantie.

**Contrepartie :** la carte SD reçoit **33,3 % de données en plus** et doit absorber 16 ko/s contre 12 ko/s, toujours très inférieur aux débits nominaux d'une carte microSD ; l'obstacle reste les pauses internes, la latence et la coexistence FIFO. Sur une carte 32 Go, 460,8 Mo/8h n'est pas une contrainte de capacité déterminante, mais l'endurance 8h et l'intégrité restent à prouver.

**Proposition R0 :** faire du **/10 à 2 666,7 Hz le premier essai DSP** avant toute adaptation ×3/4 ; ne modifier la cible métier 2 000 Hz qu'après confirmation utilisateur de ce choix, sans modifier silencieusement les spécifications héritées. Si les tests temporels ou spectraux invalident A107, rechercher le correctif **minimal pour /10**, et non réintroduire automatiquement un second étage.

## 3. Modbus RTU : hypothèses vérifiées dans le firmware réel

- `../05_Firmware/platform/stm32/stm32_serial_transport.c` : `BaudRate=115200`, `WordLength=9B` avec parité EVEN = **8E1** utile, un start/8 data/une parité/un stop = **11 bits par octet** ; TX buffer 256 octets, RX par interruption ; LPUART1/TIM6 IRQ priorité 5.
- `../05_Firmware/src/application/live_modbus_server.c` : FC03 lecture de **1 à 125 registres** (2 octets chacun), mais **aucun espace de registres bulk XYZ en continu**. Une transmission de fichier/stream serait un développement versionné nouveau, pas l'activation d'un registre existant.
- La spécification [Modbus Application Protocol V1.1b3, §6.3](https://www.modbus.org/file/secure/modbusprotocolspecification.pdf) définit 125 registres au maximum pour FC03. La [spécification Serial Line V1.02, §2.5.1.1](https://www.modbus.org/file/secure/modbusoverserial.pdf) recommande `t3.5=1,75ms` et `t1.5=0,75ms` en RTU au-dessus de 19 200 bauds. Le projet applique ces délais fixés au niveau du récepteur/temporisation UART.

### 3.1 Enveloppe de capacité théorique

**Modèle optimiste de tour complet, un seul maître et un seul capteur actif :** requête FC03 **8 octets RTU**, réponse **255 octets RTU** dont **250 octets data**, deux intervalles entre trames de **1,75 ms** ; `T=263×11/baud+2×0,00175` seconde par lecture, et `R_payload=250/T`. **Ne pas supposer** délais de traitement nuls sur carte réelle ; ajouter CRC/compteurs/indices/répétitions et supervision diminue le débit disponible. Échantillons 6B non nécessairement alignés avec les 250B de registres : un flux réel demandera fragmentation et metadata.

| Baud RTU hypothétique | Débit ligne brut max = baud/11 | Débit payload FC03 optimiste | Rapport demande 16 ko/s / capacité |
|---|---:|---:|---:|
| **115 200 — actuel** | **10,47 ko/s** | **8,74 ko/s** | **183 % : impossible** |
| 230 400 | 20,95 ko/s | 15,57 ko/s | 103 % : insuffisant même idéal |
| 460 800 | 41,89 ko/s | 25,57 ko/s | 63 % : possible en théorie pour un capteur |
| 921 600 | 83,78 ko/s | 37,66 ko/s | 42 % : plus de marge théorique |

À **2 kHz**, les données représentent 12 ko/s ; le débit Modbus **115 200 reste insuffisant** même au niveau physique, sans aucun overhead. Pour **2 666,7 Hz**, le minimum physique hors protocole serait **176 000 bit/s** (16 000 × 11), contre **132 000 bit/s** pour 2 kHz. Le fait qu'un adaptateur RS-485 accepte 460 800 ou 921 600 bauds ne valide **ni notre transceiver, ni le câblage de bord, ni les interruptions UART, ni notre implémentation RTU** à ces vitesses.

### 3.2 Autonomie, six capteurs et extraction a posteriori

- **Un seul capteur, streaming temps réel :** 460 800 bauds pourrait être étudié sur banc distinct mais utiliserait ~63 % de la capacité optimiste FC03, sans marge de garantie et au prix d'une forte activité UART/IRQ et de nombreux polls. Une déconnexion PC, panne du maître ou liaison défectueuse perd immédiatement le flux si la microSD n'enregistre pas parallèlement.
- **Six capteurs sur un même RTU partagé :** `6×16 000=96 000 B/s` rien qu'en XYZ ; **incompatible** avec 115 200 et 921 600 bauds, avant même les autres messages. Et Modbus RTU est un protocole **maître/interrogé**, pas un flux libre ou un bus multimaître.
- **Extraction d'un fichier de 8h enregistré à 2 666,7 Hz :** 460,8 Mo / 8,74 ko/s ≈ **14,65 h** à 115 200 ; /25,57 ko/s ≈ **5,01 h** à 460 800, *meilleurs cas non garantis*. Le retrait manuel de microSD reste beaucoup plus adapté à V1.
- **Alternative de principe :** pour un appareil câblé et supervisé en permanence, un flux UART/RS485 **non-Modbus ou protocolisé par blocs** pourrait employer autrement les silences et la longueur des trames, mais ce serait une **nouvelle interface distincte de la norme Modbus RTU V1** et n'apporterait pas l'autonomie exigée. Si un transfert réseau intégral devient utile, réserver l'étude à V1.5 **IP et téléchargement de fichier**, pas pousser artificiellement le flux dans les registres V1.

**Verdict :** **Modbus RTU n'est pas une absurdité technologique** pour quelques données bulk sur un appareil et une liaison rapide, mais **ne convient pas à notre besoin V1 de 6 capteurs autonomes, sans PC permanent, avec conservation 8 h minimum**. La microSD amovible reste indispensable ; Modbus conserve les commandes, diagnostics et indicateurs compacts.

## 4. Suites et garde-fous

1. R1-DSP : proposer et tester en premier **/10 A107, sortie 2 666,7 Hz** pour la bande 2–700 Hz et la fidélité DC, avec un oracle *causal*, tests d'alias à toutes fréquences, arithmétique embarquée, chronologie source et durée de campagne ; comparer au candidat 2 000 Hz seulement si un gain réel le justifie.
2. R2-SPI2 : dimensionner la file à **16 ko/s** si le candidat /10 est retenu, vérifier la pire latence/busy SD, la consommation SRAM et l'absence d'overflow FIFO. Le flux natif 26,667 kHz doit toujours être intégralement drainé.
3. V1 : **ne pas modifier** le protocole Modbus gelé ni démarrer un nouveau transport de fichiers sans décision ; traiter un streaming Modbus à haut débit comme une alternative théorique et non un nouveau prérequis.
4. **Versionnement :** conserver comme point de retour R0 demandé `a3f5ac33a728a5afd028192654c17e5b04ee8d7b`. Pas de tag nouveau ni de flash. TR2-002 libre pour futurs essais SPI2 après identification ; TR2-001 préservé.
5. **Échanges Codex :** `TR2-20261028-ACCEL-FS8G-VIBRATION-AC` demeure actif avec rapport `BLOCKED`. Aucune nouvelle mission ni évolution firmware dans cette étude ; une mission R1/R2 nécessite le traitement gouverné de la précédente.

**Nature des preuves :** chiffres de bande passante issus du protocole et du code actuel ; coût de FIR et fréquence comme modèles numériques, non résultats d'essais physiques, Ethernet et durabilité non qualifiés.
