# MSM TR2 — R0-MÉTIER — Revue contradictoire de l'architecture matérielle

**Statut : ÉTUDE PRÉLIMINAIRE / OPTIONS NON ARBITRÉES — 29/10/2026.**
**Responsabilité documentaire : ChatGPT.** Source de vérité loloLR17/MSM_vibe `main` ; ce document ne modifie ni les gels normatifs antérieurs ni les permissions de la mission Codex en cours. Aucune décision de remplacement matériel ne résulte de ce seul audit.

## 1. Pourquoi rouvrir les choix matériels

L'utilisateur confirme que la méthodologie de développement et de gouvernance **ChatGPT → GitHub → Codex → GitHub → ChatGPT** est **à conserver intégralement**. Il demande en revanche de réexaminer les choix du **STM32U575, IIS3DWB et FRAM** à la lumière du véritable besoin V1 (contrôles vibratoires locaux 2–25 Hz prioritaires, plage élargie 2–700 Hz, XYZ continu enregistré à 2 000 Hz cible, 8 h sur microSD amovible) et de l'hypothèse réseau **Ethernet filaire + PoE avec Modbus TCP et téléchargement**.

**Aucune assimilation entre « réouverture d'architecture » et « migration décidée ».** La V1.5 IP/PoE a été initialement envisagée comme évolution future. L'option de faire d'IP/PoE la connectique finale unique est **une question nouvelle à trancher**, non une décision actée. Les essais U575/RTU restent utiles comme banc de qualification tant qu'aucun nouvel arbitrage n'en décide autrement.

## 2. Critères objectifs de sélection (à prouver, pas seulement à annoncer)

1. **Mesure** : résolution/bruit à 2–25 Hz en ±8 g, bruit basse fréquence, tenue aux vibrations et montage mécanique, capacité à mesurer également jusque 700 Hz avec réponse fréquentielle documentée, saturation/surplage.
2. **Continuité/temps réel** : coût FIFO SPI, filtrage/ré-échantillonnage 2 000 échantillons/s/axe, bornes temporelles et mémoire nécessaires pour absorber les latences d'écriture microSD ; pas de perte silencieuse.
3. **Stockage et extraction** : microSD physique amovible imposée en V1, fichier PC lisible, capacité 8 h, CRC/récupération après coupure, débit et pauses de carte. Le passage éventuel à IP ne change **pas** le format de campagne.
4. **Réseau/énergie** : pour un futur Ethernet/PoE, existence ou non du MAC et du PHY, broches/bus/IRQ et isolation, budget de puissance, consommation et interactions CPU/SD. Perte du switch PoE = perte possible d'alimentation, même si l'enregistrement est indépendant du réseau logique.
5. **Industrialisation** : composants actifs/approvisionnement, licences/BSP, CEM et perturbations électriques du bord, tenue de fixation, connecteurs, maintenance de six capteurs, coût complet et volume.
6. **Réutilisation** : conserver protocoles, codecs, FRAM transactionnelle, logic métier, tests, supervision; chiffrer les adaptations de drivers et pinout, sans raisonner en « lignes de code déjà investies ».

## 3. STM32 : garder U575 ou migrer ?

| Option à comparer | Forces | Limites / preuves manquantes |
|---|---|---|
| **A. STM32U575 conservé + contrôleur Ethernet externe** (par ex. W5500 SPI) | Conserve firmware, outils et preuves STM32, 160 MHz, **786 Kio SRAM**, réseau 10/100 possible avec MAC+PHY+TCP/IP dans le contrôleur externe ; migration plus limitée | Charge SPI supplémentaire en concurrence avec FRAM/SD/capteur, mémoire/BSS et débit de fichiers, isolation PoE/PCB, absence de MAC natif ; contrôler disponibilité d'un SPI dédié et latence de transferts |
| **B. MCU Ethernet natif, par exemple STM32H563** | Cortex-M33 250 MHz, Ethernet MAC 10/100 + DMA, davantage d'interfaces SPI ; PHY externe toujours requis ; carte d'évaluation Ethernet existante | **640 Kio SRAM**, moins que le U575 ; checkpoint AC1 actuel **659 176 octets BSS** sur image U575 : **cette image ne pourrait pas être reprise telle quelle sur H563** sans refonte mémoire ; retests HAL/bus/FRAM/FIFO, pinout, horloges et coupures |
| **C. Autre MCU Ethernet avec réserve SRAM supérieure** (familles STM32H7 ou autres selon modèle) | Intégration native Ethernet possible, davantage de puissance/ressources selon référence, DMA et mémoire accrue possibles | Portage matériel/logiciel plus significatif, coûts, consommation/PoE, complexité cache/DMA et robustesse à qualifier ; aucun modèle choisi à ce stade |

**Constats** : ST décrit STM32U575 786 Kio RAM et 160 MHz, H563 640 Kio/250 MHz et MAC Ethernet. WIZnet décrit le W5500 SPI avec MAC/PHY/TCP/IP 10/100. **Attention** : le chiffre BSS du checkpoint n'est pas une preuve du besoin de 659 ko en production ; analyser maps, buffers statiques réductibles, heap/stack et variantes de build avant tout arbitrage. La présence d'une interface Ethernet ne donne pas automatiquement un réseau IP sécurisé ni un transfert fiable.

Références constructeur : https://www.st.com/en/microcontrollers-microprocessors/stm32u575ri.html ; https://www.st.com/en/microcontrollers-microprocessors/stm32h563zi.html ; https://docs.wiznet.io/Product/Chip/Ethernet/W5500 .

## 4. Capteur : IIS3DWB actuel ou remplacement ?

| Option à comparer | Forces | Limites / preuves manquantes |
|---|---|---|
| **IIS3DWB actuel** | Triaxial SPI, ±2/4/8/**16 g**, bruit annoncé ~**75 µg/√Hz** en mode 3 axes, réponse jusqu'à 6 kHz, capteur et interface FIFO déjà qualifiés physiquement à leur portée | Cadence nominale **26,7 kHz** contre archive cible 2 kHz : charge CPU/SPI + filtre anti-alias/ré-échantillonnage non triviaux, même si bande métier principalement 2–25 Hz |
| **ADXL355 candidat de comparaison** | Triaxial SPI, **20 bits**, ±2/4/**8 g**, bruit annoncé ~**22,5 µg/√Hz** (à vérifier aux conditions de ±8g), fréquence de sortie configurable jusqu'à 4 kHz, **réduction potentielle** de la quantité de données brutes à traiter | ±8g maximum (pas marge ±16g) ; transfert à 2 kHz : réponse filtre numérique à 700 Hz non supposée plate ; besoin de mesurer réponse, retard, bruit, dérive, alimentation et portage SPI |
| **ADXL357 candidat optionnel** | ±10/20/40 g : garde de saturation plus forte ; sortie numérique, 20 bits | Bruit annoncé ~75 µg/√Hz à ±10g et nouvelle qualification ; pas de gain bruit automatique par rapport à IIS3DWB ; élargir le périmètre seulement si besoin fort d'amplitude démontré |

**Important** : « plus de bits » et « bruit plus bas sur la fiche » ne garantissent pas de meilleures mesures à bord. Le montage/fixation, les résonances de la carte, la stabilité de l'échelle et l'excitation physique peuvent dominer. **Ne pas remplacer l'IIS3DWB uniquement parce que ses 26,7 kHz semblent surdimensionnés** ; établir le coût réel de la décimation sur la cible et un test comparatif instrumenté si une alternative est sérieusement envisagée.

Références constructeur : https://www.st.com/en/mems-and-sensors/iis3dwb.html ; https://www.analog.com/en/products/adxl355.html ; https://www.analog.com/en/products/adxl357.html .

## 5. FRAM : suppression envisagée ?

La FRAM actuelle **256 Kio** contient un schéma A/B d'autorités persistantes, images CRC et journaux transactionnels, documenté dans [D1D FRAM](CONCEPTION_FIRMWARE_P12H3D1D_PHYSICAL_FRAM_LAYOUT.md) et confirmé dans les réalisations successives. **La FRAM n'enregistre pas les centaines de Mo de vibrations** : son rôle demeure configuration persistante, identités, cohérence des commandes, reprise et métadonnées compactes.

- **Conservation recommandée par défaut** : dépendance isolée et déjà intégrée, amortit les écritures répétées de journaux et facilite la survie aux coupures brusques.
- **Suppression ouverte seulement si justifiée** par un bilan de volumétrie, fréquence d'écriture, endurance et atomicité d'un support de remplacement (flash MCU, microSD, autre NVM), démontrant l'équivalence des garanties de coupure. Les protections de la carte SD ne suppriment pas le besoin d'état indépendant.
- Si migration MCU, conserver d'abord l'interface logique `PersistentMedia` pour limiter les changements, puis évaluer le coût de la FRAM.

## 6. Ethernet/PoE : décision du produit final à ne pas éluder

**Architecture proposée comme hypothèse** : un câble Ethernet PoE par TR2 ; Modbus TCP pour commandes/états ; fichiers microSD téléchargés par une interface de transfert distincte (HTTP(S) ou autre) ; archive locale fonctionne même pendant coupure du réseau IP, **tant qu'elle reste alimentée**. RS-485 reste disponible sur les prototypes, mais **son maintien sur le PCB final n'est pas obligatoire** si Ethernet est retenu.

La section **PoE PD** requiert contrôleur de négociation de puissance, extraction d'énergie, conversion et isolation/protections applicables (en plus du contrôleur Ethernet/PHY), routage/magnétiques/connectique adaptés, analyse CEM et du réseau de bord. Un module Ethernet n'est **pas** automatiquement un module PoE. Coupure switch/câble : prévoir stratégie alimentation secourue et mise en sécurité/flush si nécessaire, sans promettre un enregistrement autonome 8 h lors d'une coupure de puissance.

**Avant gel PCB** : décider si PoE doit être interface de production **unique** ; vérifier accessibilité des câbles/switchs, isolation, puissance, plans de masse/bruit capteur, connecteurs, fiabilité du réseau et séparation IT/OT, sécurité accès aux données et mises à jour.

## 7. Comparaisons bornées et décision de sortie R0 matériel

1. **Existant U575 + IIS3DWB + FRAM** : bilan fichier MAP/RAM actuel et marge, cycles CPU/échantillon, trajets SPI, contraintes SD et mesure d'accélération à basse fréquence. **Pas de nouveaux essais SDMMC à l'identique**.
2. **Scénarios CPU+IP** : estimation ressource RAM, contention SPI, latence microSD + réseau, besoins PWM/IRQ/DMA et effort portage pour A/B/C. Évaluer **sur papier d'abord** ; banc/achat seulement pour départager une question qui change réellement la décision.
3. **Comparaison capteur** : courbes bande 2–25 et 25–700 Hz, bruit, plage ±8/±16g, cadence, antirepliement, disponibilité, coût et montage ; essai comparatif uniquement si le capteur actuel ne satisfait pas les critères.
4. **FRAM** : vérifier rôle réel du journal, nombre maximal de writes et comportement coupure. Conserver faute de démonstration d'une alternative équivalente.
5. **Matériel PoE** : budget de puissance et temps de réserve, risques CEM/masse, interfaces réseau natives/externes ; vérifier disponibilité d'un prototype PHY/PoE industriel. **Aucune fabrication ni redessin de PCB** tant que les résultats ne sont pas arbitré.

### Critères de clôture de l'étude

- Matrice mesurée et sourcée des trois composants + contraintes SD/PoE, avantage physique/produit et coût logiciel ;
- Architecture de prototype/banc distincte de l'architecture finale ; choix matériel **explicite** utilisateur pour la production ou report justifié ;
- Scénarios de migration sans invalidation silencieuse de versions, formats, historiques ni essais acquis ;
- Si la question Ethernet/PoE ne peut pas être tranchée à R0, **autoriser le travail R1 métier sur le banc actuel** via interfaces portables et reporter les décisions PCB à une porte obligatoire.

## 8. Méthodologie conservée — règle utilisateur

La méthodologie **ChatGPT → GitHub → Codex → GitHub → ChatGPT** demeure **intégralement en vigueur** :
- ChatGPT gouverne, rédige les spécifications, publie les missions et vérifie sur GitHub ;
- Codex effectue l'analyse/implémentation/qualification dans le périmètre publié, avec ses droits permanents et ses preuves ;
- ChatGPT contrôle indépendamment les résultats et les commits ;
- l'utilisateur décide des choix d'architecture majeurs et des interventions physiques.
- Principe A-06 : **préserver les acquis** sans faire du code existant un obstacle au bon choix matériel.

**Mission Codex en cours** : TR2-20261028-ACCEL-FS8G-VIBRATION-AC ; rapport encore `IN_PROGRESS` lors de l'audit, ne pas lui superposer un remplacement matériel non prévu. Cette étude documentaire n'est ni une nouvelle mission Codex ni une autorisation de migrer ou flasher.

## 9. Liens internes

[Cahier de charges R0](R0_METIER_20261029_CAHIER_DES_CHARGES.md) · [Matrice R0](R0_METIER_20261029_MATRICE_ECARTS.md) · [Roadmap R0](R0_METIER_20261029_ROADMAP.md) · [État courant](ETAT_COURANT_TR2.md) · [Mission](Echanges_Codex/MISSION_EN_COURS.md) · [Rapport](Echanges_Codex/DERNIER_RAPPORT.md).
