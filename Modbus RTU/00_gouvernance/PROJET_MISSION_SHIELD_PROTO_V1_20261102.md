# MSM TR2 — Contrat technique préparé : shield économique de connexion des breakouts

**Statut : PRÉPARÉ — NON ACTIF — NE PAS EXÉCUTER.**
**Rédacteur : ChatGPT — 02/11/2026.**

Ce document est un **projet de mission**, publié pour conserver le cadrage décidé par l'utilisateur. Il **ne remplace pas** `Modbus RTU/00_gouvernance/Echanges_Codex/MISSION_EN_COURS.md` et ne donne aucune autorisation opérationnelle à Codex. Selon `PROTOCOLE_ECHANGE_V2.md`, il sera transféré/actualisé par ChatGPT dans **MISSION_EN_COURS.md seulement après clôture vérifiée** de la mission active `TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR` et préservation/récupération des campagnes en cours. Pas de démarrage pendant les dernières mesures à bord, sauf changement de priorité explicite de l'utilisateur.

## Contrat à activer ultérieurement

Lors de l'activation, ChatGPT devra renseigner et vérifier sur GitHub un nouveau `mission_id`, le **HEAD réel de main** comme `base_sha`, l'horodatage réel, puis publier un contrat `status: READY` dans le fichier actif, avec les permissions ci-dessous. Toute modification de périmètre significative est à arbitrer par ChatGPT avec l'utilisateur.

**Mission proposée : conception et livraison des fichiers de fabrication d'un shield de connexions robuste pour le prototype MSM TR2 sur NUCLEO-U575ZI-Q.**

### Permissions au moment de l'activation

- `target_branch: main`
- `allow_commit: true`
- `allow_push: true`
- `allow_flash: false`
- `allow_debug: false`

Mission **uniquement CAO électronique/documentation** : aucun accès au STM32, lancement de campagne, essai sous tension, changement de câblage physique, formatage, intervention sur FRAM/microSD, ni commande ou fabrication payante. Les actes physiques, dépenses et validation de mise en service restent à l'utilisateur. Les autorisations générales de développement/commit/push de `AGENTS.md` demeurent applicables dans le contrat activé.

### Objectif

Éliminer les fils Dupont, les contacts de breadboard, les broches exposées aux arrachements et les raccordements instables lors du transport, de la pose en machine et des campagnes à bord. Concevoir **un shield d'interconnexion simple, reproductible, assemblable à la main et économique**, sans refaire la carte MCU ni anticiper les arbitrages d'architecture du produit final.

Objectif économique : **viser un budget complet d'environ 50 EUR maximum pour équiper deux prototypes**, hors matériels déjà possédés et outillage, en comptant cartes nues, connecteurs, visserie et expédition. C'est une **cible de conception à chiffrer**, pas un prix acquis ; si elle est irréalisable, montrer un devis réaliste et les alternatives plutôt que masquer des coûts. Ne rien acheter.

### Périmètre matériel à contrôler sur `main` et au banc

- Carte porteuse : **NUCLEO-U575ZI-Q**, révision réelle et connecteurs **Zio/Morpho** disponibles à confirmer d'après schémas ST, encombrements réels et assemblage. Les documents de pinout historiques ne sont pas une preuve que toutes les affectations sont encore utilisées.
- Tête de mesure : **IIS3DWB / STEVAL-MKI208V1K**, interface SPI3 documentée PC9/PC10/PC11/PC12 ; montage de capteur **déporté**, rigide, fixable et démontable, sans prise de mesure reposant sur un adhésif souple. La liaison capteur doit être courte, tenue mécaniquement et qualifiée en intégrité du signal à la vitesse SPI réellement utilisée (jusqu'à 8 MHz sur le banc actuel). Ne pas figer arbitrairement une longue nappe.
- Mémoire FRAM : breakout physique réellement présent, rôle SPI1 PA5/PA6/PA7 + PD14 selon les réalisations courantes ; vérifier référence, dimensions, connecteurs, tension et pinout au lieu de se fier uniquement au gel BSP P11 ancien.
- Module microSD : breakout **Adafruit 4682** et **câblage SPI2 actuel** à relever dans la mission R3 et le firmware testé (notamment PB13/PD3/PD4/PF12 pour le banc actuel). Les anciens essais/études SDMMC2 ne définissent pas à eux seuls la topologie active. Préserver un accès pratique à la carte amovible.
- RS-485 : vérifier le **module réellement installé**, sa révision, son alimentation logique, les nets TX/RX/DE et la séparation de masses **isolées** ; ne pas reprendre aveuglément les anciens candidats ADM2587E si le banc utilise un autre breakout. Préserver les distances et barrières d'isolement du module.
- Autres breakouts effectivement nécessaires : les inventorier dans les fichiers/code/banc, avec une **matrice utilisé / optionnel / absent**, sans ajouter automatiquement tous les candidats historiques (NOR, PoE, Ethernet, etc.).

En présence d'une donnée mécanique ou électrique impossible à établir sans accès physique, préparer une **liste courte et précise de cotes/photos/identifiants à relever par l'utilisateur**. Ne pas fabriquer ni valider des empreintes sur des dimensions présumées. Préférer les PDF/schémas/STEP/empreintes constructeur avec version et provenance établies.

### Travail attendu de Codex après activation

1. **Inventaire et matrice de connexions** : lire l'état matériel/firmware réel de `main`, les schémas constructeur et rapports pertinents, repérer chaque net (GPIO/connecteur Nucleo ↔ breakout), tension, référence de masse, fonction, vitesse, état et conflit potentiel. Produire nomenclature et tableau des broches sourcés, incluant les signaux laissés délibérément libres.
2. **Architecture mécanique du shield** : choisir le montage le moins coûteux effectivement compatible avec la Nucleo (Zio et/ou Morpho selon les possibilités réelles), implantation et fixation **positive** des breakouts et de la carte principale (vis/entretoises). Aucun fil Dupont ni contact de breadboard pour les liaisons de campagne. Prévoir détrompage, connecteurs avec retenue suffisante, trous/attaches de reprise d'effort, accès microSD, USB/ST-LINK, USER/RESET, LED, points de test, emplacement et marge pour un **boîtier imprimé 3D ultérieur**, sans concevoir le boîtier dans cette mission.
3. **Conception KiCad effective** : schéma électrique complet, PCB routé avec empreintes vérifiées, préférence **2 couches FR4 standard** et composants facilement approvisionnables/soudables à la main, masses/retours corrects, rails 3,3 V/5 V sûrs, polarités et détrompage. Limiter pistes de grande vitesse/stubs, contenir le bruit de masse et prévenir l'arrachement des faisceaux. Ne pas compromettre une isolation RS-485. Utiliser uniquement des résistances/terminaisons/condensateurs justifiés, en évitant le surdesign.
4. **Fabricabilité et vérifications** : lancer et documenter ERC/DRC KiCad, inspection des connecteurs (orientation, pas, numérotation, empilage, gabarits), vérification des alimentations/CS, absence de courts-circuits, collision mécaniques, assemblage et accessibilité. Les tests et preuves CAO ne constituent **pas** une qualification électrique ou vibratoire physique.
5. **Livrables de fabrication** : sources KiCad éditables (projet, schéma, PCB, bibliothèques/empreintes personnelles si nécessaires), exports Gerber et Excellon, schéma PDF, vues d'implantation/3D ou STEP si possible, nomenclature quantité/coût/fournisseur/alternative, plan d'assemblage et de test à réception. Fournir le devis de fabrication de 3–5 PCB à coût minimal et le **coût total estimé pour 2 bancs**, transport et consommables explicités, sans passer commande. Si un devis live n'est pas accessible, documenter une estimation datée et ce qui reste à confirmer.

### Critères d'acceptation

- Le shield **supprime effectivement** les connexions volantes et points de traction sur broches pour les modules retenus ; le capteur déporté dispose d'une liaison détrompée et immobilisée, dont **la validité à 8 MHz reste une porte de qualification physique**.
- L'ensemble est mécaniquement montable, démontable, reproductible **sur deux Nucleo** ; connecteurs et trous correspondent à des références et cotes vérifiées, ou l'incertitude est explicitement un **blocage avant fabrication**.
- Schéma/PCB éditables, Gerber/perçages/BOM/plan de montage complets ; ERC/DRC sans erreur non justifiée ; contrôles manuels documentés. Ne pas qualifier de fabricable sans contrôle des interfaces réelles.
- Réemploi des modules et du firmware opérationnel : **aucune modification de protocole, driver, pinout ni source de production** exigée par le shield sans nécessité démontrée et décision séparée.
- Budget confronté à la cible ~50 EUR / deux prototypes ; écart et options documentés honnêtement.
- Traçabilité : commits/push ordinaires sur `main`, rapport `DERNIER_RAPPORT.md` avec SHA, sources, exports, validations réelles et limites ; relecture GitHub distante. La qualification finale de la conception reste sous contrôle indépendant de ChatGPT, la qualification matérielle sous essais ultérieurs.

### Risques, exclusions, arrêt

- **Priorité de calendrier** : ne commencer qu'après clôture de la campagne actuelle et publication du contrat actif par ChatGPT.
- Préserver les acquis; aucune refonte de l'architecture TR2, aucun choix prématuré Ethernet/PoE ni conception du PCB final. Les documents R0 du 29/10/2026 restent des études conditionnelles.
- Ne pas déduire le **montage sûr** du capteur de la seule tenue d'un aimant ; fixation sur carter peint (~56,5 °C relevés), transmission des vibrations, sécurité antichute et tenue des interfaces restent à qualifier séparément. Aucune fixation par adhésif souple présentée comme qualifiée.
- Si un raccordement dépend d'un connecteur inconnu, d'un pas non vérifié, d'une révision de module ambiguë, d'une incompatibilité électrique ou d'une isolation menacée, **ne pas lancer de fabrication** ; établir l'écart, une alternative, et l'unique mesure physique nécessaire. Ne pas exiger des validations répétitives pour les étapes purement CAO.
- Aucune dépense, commande, assemblage physique ou test embarqué sans action du propriétaire du matériel. Flash/debug explicitement désactivés.

### Références à consulter à l'activation

- `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md`.
- `Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md` et dernier rapport de la mission physique, plus code/harness courant.
- `Modbus RTU/00_gouvernance/CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md`.
- `Modbus RTU/00_gouvernance/CONCEPTION_FIRMWARE_P12H3H_B_MICROSD_SDMMC2_PINOUT.md` **à titre historique**, à confronter au chemin SPI2 actuel.
- `Modbus RTU/00_gouvernance/BRINGUP_RS485_RX_008.md`.
- `Modbus RTU/00_gouvernance/R0_METIER_20261029_REVUE_ARCHITECTURE_MATERIELLE.md`, pour préserver la distinction **shield de banc / architecture du produit final**.

**Ce document n'est pas la mission active de Codex ; la publication dans `MISSION_EN_COURS.md` et sa vérification distante restent à effectuer par ChatGPT lorsque la mission actuelle sera clôturée.**
