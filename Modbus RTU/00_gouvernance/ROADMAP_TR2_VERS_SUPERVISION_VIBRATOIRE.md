# MSM TR2 — Roadmap vers la supervision vibratoire

**Version :** 1.0 — validée par l'utilisateur le 11 octobre 2026  
**Référence :** branche `main` du dépôt `loloLR17/MSM_vibe`  
**Objectif :** obtenir un démonstrateur physique dans lequel les vibrations acquises par le TR2 sont lisibles, historisées et visualisées dans la supervision Web PC.

> Cette roadmap pilote les jalons, sans modifier les spécifications Modbus RTU V1 gelées. Les statuts sont ceux du 11 octobre 2026, à actualiser après contrôle indépendant des rapports Codex. Une implémentation logicielle ne vaut pas qualification physique.

## 1. Situation de départ

- **Socle Modbus RTU V1 :** spécification et mapping unifié gelés ; firmware transactionnel, persistance, campagnes et architecture A–J développés. Toutes les fonctions ne sont pas encore qualifiées sur matériel réel.
- **Supervision PC :** phases S0 à S7 développées et gelées (transport, persistance SQLite, orchestration, interface Web). Liaison réelle au TR2 à qualifier.
- **STM32U575 / RS-485 :** firmware BIN008 sauvegardé, flashé et vérifié. **Mission 011 validée :** réception d'une ADU exacte `01 03 03 E8 00 01 04 7A`, 8 octets, une trame, une ADU valide, zéro erreur CRC/UART. Cette preuve porte sur un seul essai physique RX.
- **Mission 012 :** raccordement serveur Modbus RTU et réponse TX avec DE sécurisé ; publiée et en cours au moment de l'adoption de la roadmap. Aucune qualification TX physique présumée.
- **Chaîne d'acquisition vibratoire physique jusqu'à la supervision :** non encore démontrée de bout en bout.

## 2. Chaîne cible

Capteur de vibrations → acquisition et traitement STM32U575 → registres Modbus V1 → transport RS-485 / Modbus RTU → service de supervision PC → horodatage / SQLite → interface Web : valeurs instantanées, tendances et qualité des données.

## 3. Missions principales

| Mission | Objectif | Statut initial | Découpage indicatif | Critère de sortie |
|---|---|---|---|---|
| **M012** | Intégrer le serveur Modbus RTU et le TX RS-485 avec commande DE sûre | EN COURS | Selon résultats Codex | Chemin de réponse implémenté et testé ; qualification physique explicitement distinguée |
| **M013** | Qualifier le dialogue Modbus physique complet | ACQUIS BORNÉS / RÉSERVES | 2–3 sous-missions | Lecture répétable de registres STM32 par maître PC ; CRC, exceptions, turnaround et absence de contention vérifiés |
| **M014** | Connecter la supervision au TR2 physique | DONE — PORTÉE B0/B1/PERTE-REPRISE | 2–3 | Lecture périodique des blocs système, déconnexion et reprise correctement traitées |
| **M015** | Qualifier la chaîne d'acquisition vibratoire | IN PROGRESS — M015-A ACQUISITION BORNÉE | 3–5 | Acquisition physique démontrée ou blocages matériels précisément identifiés |
| **M016** | Traiter et publier les mesures vibratoires dans les registres V1 | À FAIRE | 2–4 | Valeurs réelles cohérentes, unités, échelles et validité vérifiées dans le mapping gelé |
| **M017** | Lire les vibrations de bout en bout via Modbus | À FAIRE | 2 | Variation physique retrouvée correctement dans les valeurs lues par le PC |
| **M018** | Intégrer les mesures dans le service PC et SQLite | À FAIRE | 2–3 | Décodage, horodatage, persistance et gestion des données invalides opérationnels |
| **M019** | Afficher les vibrations dans l'interface Web S7 | À FAIRE | 2–3 | Valeurs instantanées, axes, unités, tendances, historique et état de mesure consultables |
| **M020** | Qualifier le démonstrateur intégré | À FAIRE | 3–4 | Vibration physique visible dans le Web, historique durable, essais de repos/excitation, coupures et reprise documentés |

**Périmètre approuvé : huit missions futures M013 à M020, après M012.** Découpage indicatif de **18 à 27 sous-missions**, non figé et sans estimation de durée. Les missions principales restent stables ; les sous-missions sont créées progressivement en fonction des preuves et blocages.

### M013 — Dialogue physique complet

Vérifier les réponses aux requêtes du maître, CRC, adresses et fonctions V1, exceptions, temporisations RTU, fin réelle de TX avant désactivation de DE, retour RX et absence de contention. Réutiliser d'abord les registres système déjà définis. La réception réussie en M011 ne qualifie pas le TX.

### M014 — Supervision reliée au TR2

Configurer le transport série de la supervision existante, intégrer le TR2 réel, lire les blocs système, gérer les états de connexion, déconnexions et reprise, et contrôler les valeurs décodées.

### M015 — Acquisition physique

Auditer capteur, interface, alimentation, branchements, acquisition firmware et éventuels simulateurs. Qualifier le bring-up matériel et distinguer les fonctions disponibles de celles qui nécessitent une intervention physique.

### M016 — Publication des mesures

Raccorder les échantillons réels aux traitements et aux registres V1 existants ; contrôler axes, unités, échelles, saturation, fraîcheur et invalidité. Aucun changement silencieux du mapping gelé.

### M017 — Mesures vibratoires via Modbus

Comparer les valeurs internes STM32 aux registres lus depuis le PC sous repos et excitation contrôlée ; valider conversion, cohérence, actualisation et états invalides.

### M018 — Service PC / SQLite

Réutiliser le socle .NET et les services de supervision existants ; lire et décoder les blocs vibratoires, horodater, persister les séries et représenter les pertes ou invalidités sans fabriquer de valeurs.

### M019 — Interface Web

Étendre S7 pour présenter mesures instantanées, tendances, axes, unités, état de communication, validité et historique. Ne pas réécrire l'architecture Web existante.

### M020 — Démonstrateur de bout en bout

Campagne physique reproductible : repos, vibration contrôlée, variation du signal, perte de liaison, redémarrage et conservation des données. Produire les preuves capteur → STM32 → Modbus → PC → SQLite → Web, avec limites métrologiques explicites.

## 4. Jalons de qualification

| Jalon | Définition | Missions | État initial |
|---|---|---|---|
| **J0** | STM32 reçoit une requête Modbus physique | M011 | **VALIDÉ** pour l'essai unique |
| **J1** | STM32 répond correctement à un maître | M012–M013 | ACQUIS BORNÉS ; qualification générale ouverte |
| **J2** | Supervision communique avec le TR2 réel | M014 | VALIDÉ PORTÉE B0/B1/perte-reprise ; revue indépendante indiquée par contrat M015-A |
| **J3** | TR2 produit des mesures vibratoires physiques | M015–M016 | EN COURS — diagnostic acquisition M015-A prouvé |
| **J4** | Supervision reçoit et conserve les vibrations | M017–M018 | À FAIRE |
| **J5** | Vibrations réelles visibles dans le Web | M019–M020 | À FAIRE |

## 5. Dépendances et parallélisme

- **Chaîne communication :** M012 → M013 → M014.
- **Chaîne mesure :** M015 → M016, pouvant progresser partiellement en parallèle de la chaîne communication après audit.
- **Convergence :** M014 + M016 → M017 → M018 → M019 → M020.
- Les sous-missions peuvent être regroupées si les preuves sont suffisantes, ou subdivisées en cas de risque, dépendance ou blocage physique.

## 6. Gouvernance d'exécution

- Workflow obligatoire : **ChatGPT → GitHub → Codex → GitHub → ChatGPT**.
- ChatGPT publie les missions dans `Modbus RTU/00_gouvernance/Echanges_Codex/MISSION_EN_COURS.md` et vérifie leur publication distante.
- Codex développe, compile, teste, peut flasher et debugger dans le cadre des autorisations permanentes d'`AGENTS.md`, puis publie `DERNIER_RAPPORT.md` et les preuves.
- ChatGPT contrôle indépendamment les rapports avant de qualifier un jalon ou d'engager la suite.
- L'utilisateur reste responsable des interventions physiques et décisions importantes. Ne pas demander de validations répétitives pour les opérations logicielles autorisées.
- Chaque sous-mission doit préciser : dépendances, critères d'acceptation, preuves, risques et statut (`À FAIRE`, `EN COURS`, `VALIDÉE`, `BLOQUÉE`).
- Ne jamais assimiler tests simulés, compilation, flash, réception RX ou rapport Codex à une validation industrielle globale.

## 7. Hors périmètre de J5 : qualification industrielle

Le jalon M020 est un **démonstrateur fonctionnel intégré**, pas une autorisation de mise en service industrielle. Les étapes ultérieures comprendront notamment la qualification microSD/stockage bulk et recovery, l'endurance, la métrologie, la robustesse électrique et CEM, les modes dégradés, les campagnes, l'installation à bord et les critères d'acceptation industrielle.

Une roadmap distincte **« TR2 — Qualification industrielle et mise en service »** devra traiter ces sujets.

## 8. Entretien de la roadmap

Mettre à jour les statuts uniquement après preuves et contrôle indépendant. Ne pas renuméroter les jalons principaux pour masquer une difficulté ; détailler plutôt les sous-missions. La branche `main` et la gouvernance courante du dépôt restent les seules références de travail.

## Actualisation factuelle M014-G — 12 octobre 2026

M014 **IN PROGRESS** : attribution UID96/DeviceId1 acquise, backups FRAM/Flash et audit sensor/SD réalisés, inspecteur/provisioning préparatoire et outil PC borné testés (113 firmware/cross-build, 539 .NET). Cible actuellement en probe lecture seule, identité non provisionnée, aucun nouveau trafic Modbus. Partition microSD préservée ; aucun format ni écriture. Restent manifeste/binding/provisioning puis B0/B1 polling et perte/reprise physiques. J2 n'est pas déclaré validé, M015 reste À FAIRE. M013 conserve ses preuves historiques RX/TX et diagnostic mais la qualification complète (timings, DE/TC, contention, robustesse) n'est pas close. Le rapport intermédiaire G porte les preuves et le point de reprise ; contrôle indépendant ChatGPT non présumé.

## Actualisation M014-H — 12 octobre 2026

**M014 DONE dans la portée B0/B1/polling/perte-reprise : résultats physiques publiés et intégralement relus au SHA ae14c4c8e6081086ded87c5d505e83fa74c7a496.** Firmwareproduction réel etidentitéFRAM V2/gen12 provisionnée/readbackexact ; B0/B1complets surHP COM7/1152008E1/unit1, polling3+3, timeoutperte de disponibilité du répondant, fermeture/réouverture série, invalidationanciennevaleur/date puisidentiténeuve/B1frais. Uptimebootfigé détecté lors premièrecampagne, corrigé et requalifié physiquement9/11/14→217/219/221s ; RX72/TC8/erreurs0. Deux lecturesfinales réussies surimageproductionpermanente sansfenêtresilence/observer. Journauxcomplets publiés ethashes vérifiés, pas déduction d'un SHAseul.

READY/stockage restent0, configuration UNAVAILABLE sansressourceowned. MicroSDtotalementOFF, aucuneallocation ni écriture des32premiersMio oupartitionLBA65536 ; acquisition/bulkSD/industrie nonqualifiés. Perte de disponibilité survrai bus nequalifie pasretraitdecâble ni coupurealim ; timingDE/TC/dernierstopbit/robustesse/CEM/E4 restentouverts. M013 conserve sa qualificationfonctionnellehistorique bornée, passtatutDONE général. RevueindépendanteChatGPT du rapportH encoreattendue.

**Suite proposée M015, àcontractualiser** : qualification physique capteur/acquisition à partir du WHO_AM_I0x7B acquis, sans répéter les acquis ; activationopérationnelle etressourcesstockage traitées avecautorités/allocations/sauvegardesexplicites. Aucun essaiM015 lancé enH.

## Actualisation M015-A — 12 octobre 2026

**M015-A DONE : résultats publiés et 47 livrables intégralement relus au SHA 6d10287e85fc499c9f474e3d656253b00b071106 ; M015 globale IN PROGRESS.** Le diagnostic fournit 2 048 échantillons IIS3DWB cohérents en SPI mode 3 à 625 kHz/±4 g, avec WHO=0x7B, registres A8/44, timestamps MCU et cadence ≈6 538 lectures/s. L'auto-test mécanique interne positif/négatif produit des écarts de 1,6 à 1,9 g par axe, suivis du retour au repos. Les trois captures mode 0 anormales restent publiées. La cause électrique précise n'est pas mesurée ; ni calibration, ni excitation externe, ni acquisition continue de production ne sont qualifiées. J3 reste en cours.

M014 conserve sa clôture bornée B0/B1/polling/perte-reprise ; sa revue indépendante est indiquée par le contrat M015-A. Son image permanente est restaurée exactement, Flash 2 Mio identique, FRAM génération 12/configuration UNAVAILABLE et READY/stockage=0. Deux lectures physiques B0/B1 confirment la reprise, sans rejouer la campagne. SD entièrement désactivée et partition LBA65536 préservée. L'acquisition mode 3 reste un profil RAM temporaire ; le firmware permanent M014 conserve le mode 0, sans acquisition.

Suite proposée M015-B : raccordement production en mode 3, FIFO/DMA et débit, timestamps de conversion, détection des pertes, excitation externe contrôlée. Le stockage V1 avant campagne reste un lot distinct avec allocations et sauvegardes explicites. Aucune extension silencieuse vers M016/M017/Web ni qualification industrielle. Revue indépendante M015-A à faire après publication du rapport.

## Actualisation M015-B — 12 octobre2026

**M015-B BLOCKED ; M015 globale IN PROGRESS.** Le prototype FIFO borné256XYZ et son testhôte sont livrés, mais l'unique campagne fournit des statuts incohérents :171polls/11reculs du niveau/flagoverrun brut/readbackdivergent. Refus avantvidange,0XYZ accepté ; consécutivité/cadence/pertes nonqualifiées. Causeinconnue ; pas nouveauxessais/variantes danscette missionréduite, pasDMA/production/stockage/B5/FFT.

M014 restauréeexactement (Flash2Mio identique), FRAMgen12/configUNAVAILABLE, SDtotalementOFF/partitionLBA65536préservée,2lecturesB0/B1dereprise réussies. AcquisM015-A nonrejoués ; J3resteen cours. Prochaincontrat : établirla fiabilité desregistresFIFO avant nouvellequalification, sanscritèreimmobilitéduvoilier. Rapport etpreuvesB portentle verrou etleslimites.
