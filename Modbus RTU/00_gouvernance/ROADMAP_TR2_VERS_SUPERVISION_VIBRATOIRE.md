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

## Actualisation M015-C — 12 octobre 2026

**M015-C BLOCKED pour cause/fiabilité FIFO active ; M015 globale IN PROGRESS.** Une seule campagne read-only512transactions/128ms : WHO7B/CTRL3=04/FIFOvide0 cohérents, simples etbursts concordants, HALerrors0. Aucun remplissage ouXYZ, aucunecorrection ; anomaliesBnonrejouées etcauseinconnue. Tests ciblésneufs/profilARM seulement, validations A/B préservées. M014 restauréeexactement/Flash2Mio identique, contrôleminimal2FC03B0/B1réussi, FRAMgen12/SDOFF/partitionpréservée. Prochainepreuve envisagée : comparaisonwire↔RAM ettimingCS/SCK avecinstrumentidentifié etcontratdistinct ; protocolepréparé nonexécuté.

## Actualisation M015-D — 12 octobre 2026

**M015-D BLOCKED pour cause précise / fiabilité FIFO active complète ; M015 IN PROGRESS.** Diagnosticunique :26/28divergences CTRL2single↔burst pendantremplissage,0/32aprèsentrée coupée/FIFOconservée ;WHO/CTRL3stables,28statutsgroupésmonotones1→256. Défautdelecturelocalisé parcontexte, causeSPI/MCU/capteur etfastpollB nonisolés, aucunecorrectionheuristiqueouqualificationXYZ/cadence. M014restauréexactement,Flash2Mioidentique,contrôleminimalB0/B1passé ;FRAMgen12/SDOFF/partitionpréservée. Suite :preuvewire↔RAM88/87/BA avecinstrumentlogiqueidentifié etcontratdistinct, sansrejouer acquisA/B/C/D.

## Actualisation M015-E — 12 octobre 2026

**M015-E DONE documentaire** après publication10bcdeaf5e492dd5b87535ca8d536b42218dfc5f et relecture intégrale de11livrables identiques. Décision technique proposée : directA commeRAMdemo borné ;FIFO128bursts commevoieV1 àqualifier, DMA/service ensuite si nécessaire. Débit625kHz insuffisant (FIFO128+status≥1,500019Mbit/s), SPI3limité imposebatchs sous1024octets ;128mots=897octets. Prochaineactionproposée :mesurewire↔RAM ciblée88/87/BA à625kHz avecinstrumentidentifié etcontratdistinct, aucunbranchementsupposé. Enattente,analyses/contrats horsligne surdonnéesA, pas nouvellescaptures. M015 IN PROGRESS,B/C/DBLOCKED conservés,stockageV1/READYtoujoursindisponibles ;aucuntestouhardware exécuté enE.

## Actualisation M015-F — 12 octobre 2026

**M015-F BLOCKED ; M015 IN PROGRESS.** HarnessFIFO128/2,5MHz ettests préparés,maisunique tentative refusée avantactivationFIFO pargardereadbackconfiguration ;aucunXYZ/burst,aucundébit/latencemesuré. Valeurdivergente nonexportée,aucunecauseattribuée. M014restaurée/Flash2Mioexacte,FRAMgen12/SDOFF/partitionpréservée,repriseB0/B1réussie. V1ODR26667/fenêtres4096..32768/continuité/store-and-forward inchangées ;256motsprévus nevalentpasV1. Prochaincontrat :télémétrieaumismatch puis tentativebornée choisie ;625kHzFIFOfigée éventuellementpossiblemaisinsuffisant pourfluxcontinu,nonexécuté. Pasinstrumentabsentexigé,nirechercheexhaustive ourejeuA-F.

## Actualisation M015-G — 12 octobre 2026

**M015-G DONE pour refus caractérisé ; M015 globale IN PROGRESS.** Une seule tentative : CTRL1_XL/10 attenduA8/reçu50 en lecture simple, HAL_OK, groupé07..0A conforme ; arrêt avant FIFO/0XYZ. Gardes préservés et traces immuables publiées, aucune variante/rejeu A-F. M014-H restauré/Flash2Mio exacte, deux lectures B0/B1 réussies, FRAMgen12/SDOFF/partition préservées. Cause électrique et acquisition FIFO restent ouvertes ; prochain contrat limité pour distinguer write/readback CTRL1, sans affaiblissement V1. Résultats publiés au b3a3c428a9f2d35801f5601db1a38d58a120246a ; 50 livrables intégralement relus et identiques. Clôture documentaire contrôlée après son push.

## Actualisation M015-H — 12 octobre 2026

**M015-H DONE pour diagnostic différentiel ; M015 IN PROGRESS.** Campagneunique S G G S active=A8/A8/A8/50, même motif après20,276ms ; repos/arrêt concordants. Association position/historique et actif, pas cause HAL/électrique attribuée ; guardlatched/result9, aucune FIFO/XYZ. M014-H restauré/Flash2Mio exacte, reprise B0/B1 réussie, FRAMgen12/SDOFF/partitionpréservées. Suite proposée : audit horscible transitions SPI7→2 puis ordreSSSS/GS sous nouveau contrat ; aucune acquisition/mesureBM/remplacementcapteur enH. Résultats 1ccec7f7d7916564322ddfb6437e20afa7f4b269 publiés et56livrables intégralement relus/identiques ; clôture documentaire contrôlée après son push.

## Actualisation M015-I — 12 octobre 2026

**IN_PROGRESS, logiciel validé seulement.** Harness FIFO réutilisé avec contrôles/audit groupés et gardes HAL/compteurs/CS renforcés ;26cas ciblés et cross-build défaut/profil réussis. Campagne physique reportée après renouvellement, quota annoncé12% ; M014-H reste installé, aucun accès cible/média/HP enI. Checkpoints etpointde reprise publiés ; aucune fiabilitéFIFO/continuitéV1 annoncée.

## Actualisation M015-I reprise physique — 12 octobre 2026

**FIFO RAM bornée physiquement validée, M015 globale IN PROGRESS.** Unecampagne source6c504a9 :groupés conformes,256XYZ/tags en2bursts128,0erreur/perte signalée/saturation ; niveaux256→128→0,≈2,886ms/burst. M014-H restauré/Flash2Mio exacte/FRAMgen12/SDOFF/partitionpréservée/B0B1reprise réussie. 256motsnevalentpas4096..32768 ni fluxcontinu/store-and-forward ; prochainesressources/serviceFIFO/continuité/stockage souscontratdistinct. M015-I-PHYSICAL-RESUME DONE après publication 2cebe602ee5946de3adadb84a6361ab502c2ccdd et relecture intégrale de43livrables identiques ; clôture documentaire contrôlée après push. Aucune répétition ou intervention physique additionnelle.

## Actualisation M015-J — 12 octobre 2026

**M015-J BLOCKED pour fenêtre continue4096 ; M015 IN PROGRESS.** Diagnosticlogiciel/18cas/crossréussis puisunique campagne :unburst128reçu, groupé07..0Apostburstrefusé09=14vs0A et0A=0Cvs06, fenêtreINVALID/0XYZaccepté.19ecasderégressionhôtepostrefuspassé sansciblemodifiée/ARMrebuild/rejeuphysique. RAM/guards/timeboundspréparés, pasqualificationcontinu ;M014-Hrestauréexactement/Flash2Mio/FRAMgen12/SDOFF/B0B1reprise réussie. I256figé restevalidé. Suite :prochaincontratborné postreadbackactif/éventuellementABmodule, aucunbypassouinstrumentabsent requis. Résultats publiés au commit 2b96d8215a93c5012b813499abd61aea408f534d, 60 livrables relus intégralement et identiques ; BLOCKED technique maintenu, archives/worktrees/stashes préservés.


## Actualisation M015-K — 12 octobre 2026

**READY_FOR_PHYSICAL logiciel, M015 globale IN PROGRESS.** Projet indépendant NUCLEO/U575 + ST IIS3DWB préparé, sources officielles figées et licences/SHA conservés ;41scénarios C/5tests Python/4images ARM réussis. Audit I/J↔ST et pinout constructeur avec point GND/révisions/montage réel encore à confirmer. Défauts getter ST reproduits hors matériel, sans explication causale du refus J. Aucun accès cible/flash/debug/reset/essai pendant montage ; M014-H préservé, I256figé acquis/J4096BLOCKED inchangés. Prochaine étape : confirmation banc et identité certaine seconde carte, sauvegarde et chaîne qualifiée, puis WHO/polling/FIFO bornés suivant protocole. Projet publié au commit e222dc8ddb7cfd0829c655533ac8e646220721f2, 51 livrables relus intégralement et identiques ; statut READY_FOR_PHYSICAL conservé sans qualification physique.


## Actualisation M015-L — 12 octobre 2026

**BLOCKED matériel à WHO ; M015 IN PROGRESS.** Bancisolé confirméutilisateur/SN0032001F…/UID001B0046…, artefactsSTK réutilisés. UneseulelectureWHO reçoit00/attendu7B avecHAL_OK ;aucunwritecapteur, polling/FIFOstrict/STATUS10 nonexécutés auSTOP. Sourcee222dc8 intacte, aucunevalidationlogicielleK/I/J/M014rejouée. Secondecible restaurée Flash2MiB entièreexacte au backupfrais/SHAfcea004e…, debugdétaché/faults0 ;TR2principal jamaisaccédé/M014-H préservé. Cause nonattribuée, gettersSTnonatteints, aucunecomparaisoncausaleI/J. Suitecibléephysique MISO/orientationnappe/GND/VDDIO surMKI208 avantrepriseexplicitementcadrée ;pasnouvellevariantepourréussir. Résultats publiés au commit f36425707e97e233f079a9d37381567d92ea761b, 62 livrables relus intégralement et identiques ; BLOCKED technique maintenu, archives/worktrees/stashes préservés.


## Actualisation M015-L — reprise essai n°2 après correctionSPI

**WHO etpollingST physiquementvalidés dansportéebornée ; FIFOlong BLOCKED, M015INPROGRESS.** WHOautonome n°2=7B,64XYZdistincts/0saturation/309transfertsvalides, sourcesKinchangées/testsconservés. FIFOstrict reproduitgetter03F/API0/raw257 ;varianteSTATUS10séparée premierburst896→HAL_TIMEOUT/error104OVRSPISTM32,0motsacceptés. Bufferpartielpréinitialisé rejeté, tailleRXréellenonenregistrée/compteursHAL remis0 àfermeture ;pasqualification512 ou4096. Flashseconde2MiB restauréeexacte/SHAfcea004e…, TR2principal/M014-Hjamaisaccédé, preuveessai1conservée. Suitecibléelogicielle performance/diagnosticRXavantHALCloseTransfer sansmodifiervaleursST oumasquerOVR, pasnouvelleconfirmationphysiquemanquante invoquée. Résultats publiés au commit 0eda53843e64becf8dfa5ae4ec02dfb23a04c77d, 115 livrables relus intégralement et identiques ; WHO/pollingconformes etFIFOlongBLOCKED conservés.

## Point M015-M — 12 octobre 2026

Transfert SPI long du banc secondaire : O0 reproduit OVR/timeout et réception
partielle211/896 ; optimisation du seul HALSPI O2 permet deux bursts complets
896octets sans erreur, paramètres ST inchangés. FIFO512 reste BLOCKED :
capacité1024traces atteinte après256timestamps et aucunXYZ, cleanupinitial
empêché. STOP acquisition, image de sécurité met le capteur au repos.
Prochain contrat : réservecleanup/pollsbornés, puis qualificationabsenceXYZ ;
pas qualificationV1/4096/production ni résolution présumée des anomaliesTR2.
La Flash secondaire est jetable sans backup/restaurationinitiale répétés ;
identité/image/verify/readback maintenus, règle jamais applicable au principal.
M014-H et preuves historiques préservés ; DERNIER_RAPPORT porte les détails.

## Point M015-N — 12 octobre 2026

Réservecleanup etpolls bornés corrigés,testés etobservés :référence512 complète
maisuniquementtimestamps. Troisdiagnosticsautorisés :sansbatchTSFIFOvide,BDR
aprèsXLtoujours512TS,lecture7octetshuitTS ;zéroXYZ,touscleanupconformes.
FIFOXYZ resteBLOCKED,causeprécise nonattribuée malgréreadbacksBDR0A/XLAC.
Extensions512XYZ/4096nonengagées fautepréconditions ;préparerpreuveindépendante
wire/étatcapteur sousnouveaucontrat,aucunTR2principal/M014/média touché.
