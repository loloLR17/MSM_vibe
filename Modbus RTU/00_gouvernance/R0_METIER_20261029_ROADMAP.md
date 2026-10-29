# MSM TR2 — R0-MÉTIER — Roadmap vers l'enregistreur autonome

**Statut : trajectoire R0–R6 VALIDÉE DANS SON PRINCIPE, détails techniques en revue.** Date 29/10/2026. Les noms `R0-MÉTIER` etc. **ne remplacent pas** les lots R0–R7 du [LIVE historique](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md). Aucun changement logiciel/firmware décidé par la présente publication. **D2 arbitré métier : contrôles locaux V1, pas de déphasage intercapteurs ni de propagation structurelle (V2 éventuelle).**

| Lot | Résultat attendu | Porte de validation |
|---|---|---|
| **R0-MÉTIER — présent** | [Exigences métier V1](R0_METIER_20261029_CAHIER_DES_CHARGES.md), [réutilisation/écarts](R0_METIER_20261029_MATRICE_ECARTS.md), arbitrages sur bande, temps et extraction ; définir version normative successeur | Le besoin est cohérent, les choix **validés** distingués des hypothèses, pas de contradiction implicite avec le gel historique |
| **R1-MÉTIER — acquisition** | Un flux XYZ continu, filtré anti-repliement et décimé/ré-échantillonné vers **2kHz réels** ; **cœur métier 2–25 Hz et bande cible élargie 2–700 Hz** ; indices/timestamps/qualité ; sortie RAM/test | Rejeu multiton/impulsions/DC validé, notamment 2–25 Hz et 333/667 Hz ; réponse du filtre et alias qualifiés selon tolérances encore à définir ; aucune perte silencieuse ; coûts CPU/RAM et cadence de sortie exacts caractérisés |
| **R2-MÉTIER — stockage microSD isolé** | **Support amovible microSD acté V1** ; qualifier un accès **SPI microSD candidat** (voie distincte des essais SDMMC en échec), capacité, format de fichiers PC, buffers, partage SPI/FRAM et matériel | Écriture/relecture exactes sur carte, capacité ≥8h, fichiers/format récupérables sur PC, latences longues et errors/overflows explicites, sécurité à la coupure et au retrait ; aucun GO sur pic de débit |
| **R3-MÉTIER — enregistreur seul** | Pipeline R1 vers microSD R2 avec fichiers versionnés, métadonnées, FRAM et commandes conservées ; contrôle d'arrêt/flush avant retrait physique | **10 min → 1 h → 8 h** de XYZ récupérés après **retrait manuel de carte puis lecture sur PC**, comptés et vérifiés ; défauts/coupures/pleins/absences et préfixes durables documentés ; pas de PC nécessaire pendant capture |
| **R4-MÉTIER — multi-capteurs et repérage temporel** | Réutilisation Modbus/centrale, identité et localisation des points ; 2 puis jusqu'à 6 appareils, mesures **ponctuelles simultanées ou successives** | Campagnes et phases de fonctionnement comparables par leurs horodatages et contextes ; aucune synchronisation de **phase vibratoire** exigée ; perte d'un appareil n'invalide pas les autres |
| **R5-MÉTIER — analyse PC** | **Import de fichiers de la microSD retirée**, contrôle d'intégrité, graphiques XYZ, RMS/crêtes/spectres, annotations régimes et comparaison de campagnes | 8h de données importées depuis SD et analysées hors ligne ; format de campagne indépendant du transport en vue de V1.5 ; filtres transparents, anciens historiques inchangés |
| **R6-MÉTIER — essais navire** | Mesures au PC machine, moteurs/propulseurs et locaux représentatifs, montage et orientation documentés | Au moins une traversée exploitable multi-points, transitions annotées et comparables ; qualification industrielle finale distincte |

## Réserve explicite V1.5 — réseau IP après la V1

**Décision utilisateur D3 du 29/10/2026 :** V1 = microSD amovible, copie manuelle sur PC. **V1.5 souhaitée** = connectivité IP, supervision/commandes par **Modbus TCP** et téléchargement des mêmes fichiers de campagnes par un transport de fichiers adapté, sans utilisation de Modbus pour véhiculer des centaines de Mo. **V1.5 ne bloque aucun GO V1.**

**Point matériel à anticiper sans le concevoir maintenant :** le **STM32U575 n'intègre pas de MAC Ethernet**. Une carte réseau/controller externe adaptée, ou une architecture réseau distincte, est nécessaire ; ne pas présumer qu'un PHY simple ou une prise RJ45 suffisent. Avant PCB final, vérifier disponibilité des bus, broches, puissance, IRQ et coexistence avec le capteur SPI3, FRAM SPI1 et microSD SPI à qualifier. **Ne réserver aucune ressource non vérifiée**, ne pas choisir le médium IP (filaire/sans fil) ni un module dès V1, ne pas lancer de code réseau.

La préparation V1 se limite à **préserver le format de fichier, le modèle logique de commandes et la couche de stockage indépendants du futur transport**. Après R6, une étude V1.5 séparée traitera hardware, Modbus TCP standard, transfert des fichiers (HTTP(S) ou équivalent), débit, sécurité/segmentation réseau, reprise et coexistence pendant l'acquisition.

## Porte d'architecture matérielle ajoutée à R0 (29/10/2026)

Après le recentrage fonctionnel, l'utilisateur demande explicitement de **réévaluer STM32U575, IIS3DWB, FRAM et l'hypothèse Ethernet/PoE**, tout en **conservant intégralement la méthodologie ChatGPT → GitHub → Codex → GitHub → ChatGPT**. [Étude comparative matérielle R0](R0_METIER_20261029_REVUE_ARCHITECTURE_MATERIELLE.md) et [analyse technique approfondie](R0_METIER_20261029_ANALYSE_MATERIELLE_DETAILLEE.md) publiées comme **études préliminaires non arbitrées**, distincte d'une migration et sans modification firmware.

**Nouvelle porte de décision avant le PCB définitif** : comparer (1) U575 + Ethernet SPI externe, (2) MCU avec Ethernet MAC intégré mais réserve SRAM qualifiée, (3) capteur IIS3DWB versus alternative basse fréquence, (4) maintien de la FRAM, et (5) PoE/coupures/ISO/CEM. Vérifier l'impact sur le filtrage 2 kHz et les buffers SD. **R1 de qualification sur le banc actuel peut se poursuivre avec interfaces portables** ; aucun choix de composant final ou suppression du RS-485 n'est implicite. La récupération microSD physique reste la fonction minimale V1 tant que le périmètre IP/PoE n'est pas arbitré.

## Ordonnancement

**Chemin critique : R0 → R1 et R2 (possibles en parallèle après cadrage) → R3 → R4/R5 → R6.** R3 est le **premier GO produit** : un enregistreur autonome d'une vraie campagne 8h. Un démonstrateur LIVE et des tests C/.NET verts n'en tiennent pas lieu. R4/R5 peuvent ensuite progresser en parallèle, sans surdévelopper l'UX.

## Règles de pilotage A-06

- Garder les drivers FIFO, FRAM/identité, B5 transactionnel, Modbus, couches d'intégrité et socle PC ; adapter seulement le nécessaire. Ne pas imposer RTOS ni refonte totale sans démonstration de nécessité.
- **Une seule acquisition continue** : stockage prioritaire, indicateurs/FFT secondaires. Pertes, saturations, horloge invalide et média plein explicitement signalés.
- Interdire le retour aux séries de micro-variations SDMMC déjà épuisées sans hypothèse neuve testable. Banc R2 borné sur **microSD SPI en premier candidat**, puis nouvelle décision en cas d'échec ; ne substituer aucune mémoire non amovible sans décision utilisateur.
- La première preuve de succès est l'intégrité du **fichier récupéré**, pas la compilation ni le chiffre de débit moyen.
- **Mission Codex actuelle ±8g/AC1 :** ne pas la remplacer au stade `IN_PROGRESS` ; achever/checkpointer honnêtement et contrôler son rapport indépendamment. Nouveau contrat Codex seulement lorsque R0 est cadré.
- Protéger l'histoire : les documents normatifs historiques restent gelés ; ChatGPT publiera la révision versionnée après les arbitrages fonctionnels, et Codex implémentera seulement ensuite.

## Décisions encore ouvertes avant clôture R0

**D1 ARBITRÉ MÉTIER (29/10/2026)** : cœur prioritaire **2–25 Hz**, bande cible élargie **2–700 Hz** validée par l'utilisateur, sans priorité diagnostic TS/roulements ; **performance physique et tolérances du filtre encore à qualifier en R1**. **D2 ARBITRÉ MÉTIER (29/10/2026)** : **V1 = contrôles ponctuels locaux**, sans analyse du déphasage ni propagation des harmoniques ; **V2 seulement si besoin confirmé**. Garder une chronologie exacte par capteur et un horodatage commun suffisant pour identifier les événements, sans précision numérique arbitraire ni synchronisation sub-milliseconde imposée. **D3 ARBITRÉ MÉTIER (29/10/2026)** : **V1 microSD amovible, retrait manuel puis lecture sur PC**. Prévoir **V1.5 IP + Modbus TCP + téléchargement des fichiers** sans rendre V1 dépendante du réseau. La méthode physique d'accès à microSD, le filesystem et le module IP futur restent des choix d'ingénierie à qualifier. **D4 OUVERT. Format compact et politique de pertes**, à finaliser par audit/essais : 8h minimum obligatoire et 24h secondaire, aucune perte silencieuse.

La précision événementielle de D2, les performances et la faisabilité des paramètres D1–D4 restent à qualifier, sans rouvrir la décision d'exclure l'analyse intercapteurs de phase de la V1.
