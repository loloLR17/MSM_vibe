# MSM TR2 — R0-MÉTIER — Roadmap vers l'enregistreur autonome

**Statut : trajectoire R0–R6 VALIDÉE DANS SON PRINCIPE, détails techniques en revue.** Date 29/10/2026. Les noms `R0-MÉTIER` etc. **ne remplacent pas** les lots R0–R7 du [LIVE historique](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md). Aucun changement logiciel/firmware décidé par la présente publication.

| Lot | Résultat attendu | Porte de validation |
|---|---|---|
| **R0-MÉTIER — présent** | [Exigences métier V1](R0_METIER_20261029_CAHIER_DES_CHARGES.md), [réutilisation/écarts](R0_METIER_20261029_MATRICE_ECARTS.md), arbitrages sur bande, temps et extraction ; définir version normative successeur | Le besoin est cohérent, les choix **validés** distingués des hypothèses, pas de contradiction implicite avec le gel historique |
| **R1-MÉTIER — acquisition** | Un flux XYZ continu, filtré anti-repliement et décimé/ré-échantillonné vers **2kHz réels** ; **cœur métier 2–25 Hz et bande cible élargie 2–700 Hz** ; indices/timestamps/qualité ; sortie RAM/test | Rejeu multiton/impulsions/DC validé, notamment 2–25 Hz et 333/667 Hz ; réponse du filtre et alias qualifiés selon tolérances encore à définir ; aucune perte silencieuse ; coûts CPU/RAM et cadence de sortie exacts caractérisés |
| **R2-MÉTIER — stockage alternatif isolé** | Qualification physique d'un chemin **distinct des essais SDMMC répétés** : SPI microSD ou mémoire flash appropriée ; choix selon capacité, endurance, latences et extraction | Débit et latences mesurés sous charge réaliste, écriture et relecture exactes, gestion de pause/backpressure, erreurs et reprise ; pas de GO sur débit de pointe |
| **R3-MÉTIER — enregistreur seul** | Pipeline R1 vers stockage R2 avec métadonnées, formats versionnés, FRAM et commandes conservées | **10 min → 1 h → 8 h** de XYZ récupérés, comptés et vérifiés ; défauts, coupures, pleins/absences et préfixes durables documentés ; pas de PC nécessaire pendant capture |
| **R4-MÉTIER — multi-capteurs** | Réutilisation Modbus/centrale, synchronisation et identité des points ; 2 puis 6 appareils | Campagnes temporellement comparables selon la précision métier validée ; perte d'un appareil n'invalide pas les autres |
| **R5-MÉTIER — analyse PC** | Export/import et contrôle d'intégrité, graphiques XYZ, RMS/crêtes/spectres, annotations régimes et comparaison de campagnes | 8h de données importées et analysées hors ligne ; filtres transparents, aucun retrait DC irréversible ni changement silencieux des historiques |
| **R6-MÉTIER — essais navire** | Mesures au PC machine, moteurs/propulseurs et locaux représentatifs, montage et orientation documentés | Au moins une traversée exploitable multi-points, transitions annotées et comparables ; qualification industrielle finale distincte |

## Ordonnancement

**Chemin critique : R0 → R1 et R2 (possibles en parallèle après cadrage) → R3 → R4/R5 → R6.** R3 est le **premier GO produit** : un enregistreur autonome d'une vraie campagne 8h. Un démonstrateur LIVE et des tests C/.NET verts n'en tiennent pas lieu. R4/R5 peuvent ensuite progresser en parallèle, sans surdévelopper l'UX.

## Règles de pilotage A-06

- Garder les drivers FIFO, FRAM/identité, B5 transactionnel, Modbus, couches d'intégrité et socle PC ; adapter seulement le nécessaire. Ne pas imposer RTOS ni refonte totale sans démonstration de nécessité.
- **Une seule acquisition continue** : stockage prioritaire, indicateurs/FFT secondaires. Pertes, saturations, horloge invalide et média plein explicitement signalés.
- Interdire le retour aux séries de micro-variations SDMMC déjà épuisées sans hypothèse neuve testable. Banc R2 borné et décision de repli si résultats insuffisants.
- La première preuve de succès est l'intégrité du **fichier récupéré**, pas la compilation ni le chiffre de débit moyen.
- **Mission Codex actuelle ±8g/AC1 :** ne pas la remplacer au stade `IN_PROGRESS` ; achever/checkpointer honnêtement et contrôler son rapport indépendamment. Nouveau contrat Codex seulement lorsque R0 est cadré.
- Protéger l'histoire : les documents normatifs historiques restent gelés ; ChatGPT publiera la révision versionnée après les arbitrages fonctionnels, et Codex implémentera seulement ensuite.

## Décisions encore ouvertes avant clôture R0

**D1 ARBITRÉ MÉTIER (29/10/2026)** : cœur prioritaire **2–25 Hz**, bande cible élargie **2–700 Hz** validée par l'utilisateur, sans priorité diagnostic TS/roulements ; **performance physique et tolérances du filtre encore à qualifier en R1**. **D2 OUVERT. Synchronisation** (simple comparaison temporelle des phases de conduite, ou corrélation de phase instantanée). **D3 OUVERT. Récupération des archives** (support amovible ou transfert via interface dédiée). **D4 OUVERT. Format compact et politique de pertes**, à finaliser par audit/essais : 8h minimum obligatoire et 24h secondaire, aucune perte silencieuse.

Les valeurs physiques et la faisabilité de D1–D4 ne seront pas annoncées comme qualifiées avant essais correspondants.
