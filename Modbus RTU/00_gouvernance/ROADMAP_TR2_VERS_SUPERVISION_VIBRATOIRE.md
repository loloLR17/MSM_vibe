# MSM TR2 — Roadmap vers la supervision vibratoire

> **Mise à jour A-06 / 15 octobre 2026 — nouvelle trajectoire adoptée.** Le découplage supervision live / sauvegarde des campagnes est décidé. [Contrat R0](CONTRAT_R0_EXTENSION_LIVE_SANS_STOCKAGE.md) publié : orientation normative adoptée, valeurs d'encodage et mapping successeur à finaliser et vérifier. Les mentions historiques de M016 « V1 » et de dépendance au stockage ci-dessous sont conservées pour traçabilité ; **la séquence R0–R7 suivante prime désormais pour la planification**. Aucun nouveau firmware ni test physique qualifié par cette décision. Référence d'audit : [A-06](ETUDE_A06_SUPERVISION_SANS_STOCKAGE.md).

## Trajectoire active A-06 — R0 à R7

| Lot | Statut actualisé au 17/10/2026 | Travaux et dépendances | Critères de sortie |
|---|---|---|---|
| **R0 — Norme LIVE** | **REÇU POUR ENTRÉE EN R1 (CONTRAT R1)** | Extension versionnée distincte de V1/V1.1 ; vérifier version B0/B4, commandes B5, durée live continue, B1/B3/B6/B7, FRAM, migration, compatibilité, FT/TT. Aucun code modifié. | [Profil LIVE](../01_Specification_source/LIVE_R0/README.md), mapping 187 champs / 390 registres, 106 scénarios et 384 contextes spécifiés ; CRC et géométrie contrôlés documentairement. Réception documentaire actée par la mission R1 ; aucun PASS physique LIVE. |
| **R1 — Configuration/FRAM** | **REÇU COMME TRANCHE LOGICIELLE POUR R2 (CONTRAT R2)** | Backend autonome LIVE, codecs format 2, partition disjointe, recovery sans SD, migration et provenance ; composition production V1 conservée. | 129 tests hôte, 814 budgets de coupure, erreurs aux barrières, cross-build ARM ; pas de provisioning matériel ni de serveur LIVE opérationnel. Voir [implémentation R1](../05_Firmware/LIVE_R1.md) et rapport. |
| **R2 — Moteur live** | **R4-V DIAGNOSTIC/CORRECTION OBSERVÉS BORNÉS** | START5/7/run9/STOP5/9 terminaux4/0 ; retry/collision27/refusbudget5/8 sansmutation. | Journal62/root75/epoch5/run9/config3, FRAM70e273…73587 conservée. STOPterminalMCU40,859s, latencerésiduelle importante ; pas de recoveryproduit/arrêtindustriel qualifié. [R4-V](../05_Firmware/LIVE_R4V.md). |
| **R3 — FIFO/calcul/B3** | **R4-V N4096 PHYSIQUE OBSERVÉ BORNÉ** |1833115XYZ/35séquences au dernierB3,1075ADUCOM4CRCexactes/447V-F/51HELD ; oracle4096exact, UTCabsente. | HELDpublié429µs aprèscallbackSTOP, défautcacheFRESHcorrigé ; Nsupérieurs/produitcomplet/corrélationsurfil àqualifier, contrôleChatGPTattendu. R5-AFIX corrige3002/3003 :563B3 réels acceptés par PCstrict enreplay ; anciens2068refusés conservés. [R4-V](../05_Firmware/LIVE_R4V.md). |
| **R4 — Temps réel** | **R4-V LATENCE DIAGNOSTIQUÉE ; R4GLOBAL OUVERT** | Work814/service2063/gap2274µs sous1000/2500/5000 ; pacing250opt-in aprèsfindrain, FIFOmax68/0erreur. | STOPterminalMCU64,319→40,859s, ACK→HELD56,355→32,866s, callback205µs ; sur-service dominant prouvé, barrièresinchangées. M016restauré/FRAM70epréservée ; IRQWCET/fautesphysiques/endurance/R6nonqualifiés. [R4-V](../05_Firmware/LIVE_R4V.md). |
| **R5 — PC/SQLite/Web** | **R5-AFIX CORRIGÉ/OBSERVÉ ; R5-B À FAIRE** | Défaut3002/3003levé ;1095ADU réelles/563B3 corrigés acceptés enreplaySQLite sansrejet. R5-A historiqueBLOCKED archivée ; PCproduction inchangé. | RevueChatGPT puis raccordementCOM4unique/SQLite/Web réel R5-B ; B1/B2/B7/capacitéscomplètes etR6distincts. [R5-AFIX](../05_Firmware/LIVE_R5AFIX.md). |
| **R6 — Physique bout en bout sans SD** | À faire, après R1–R5 | STM32 → Modbus → PC → SQLite → Web ; quatre fenêtres, défauts, arrêt/reprise, power-cycle utilisateur et endurance progressive. | Preuves physiques, SHA image, cible, budgets et revue indépendante ChatGPT ; pas de qualification industrielle présumée. |
| **R7 — Stockage réintégré** | Différé ; indépendant du GO R6 | Qualification SDMMC/alternative puis enregistrement du même flux ; débit, backpressure, préfixe durable, panne/plein/retrait et coexistence. | Campagnes durables prouvées sans régression du live ; qualification distincte. |

**Passerelle M014–M020 :** M014/M015 restent des acquis bornés ; M016 (publication B3) se réalise principalement en R3 ; M017 (Modbus physique) en R6 ; M018/M019 (PC, SQLite, Web) en R5/R6 ; M020 (démonstrateur) en R6. J3–J5 demeurent ouverts. Les dettes A04-04..08 et la revue A-01 M015 restent ouvertes ; aucun jalon historique requalifié.

**Règles de progression :** R0 doit être finalisé avant toute modification des domaines gelés ; R1–R3 par incréments réversibles et tests ; R4 et R5 peuvent avancer en parallèle selon interfaces ; R6 n'attend pas R7. La supervision live doit rester opérationnelle sans média de campagnes, sans dissimuler les défauts réels. Aucun flash/debug ni intervention physique implicite dans R0.

---


**Version de suivi :** A-02 — 13 octobre 2026. **Source :** `main` de `loloLR17/MSM_vibe`.
**Objectif :** démonstrateur physique capteur → STM32 → Modbus → supervision PC → SQLite → Web.

> **État de lecture :** M014 est terminé dans sa portée B0/B1 et perte/reprise ; M015-V est `DONE` pour l'acquisition physique bornée (32 768 XYZ / 4 096 timestamps et deux lectures B0/B1 simultanées). **La revue indépendante A-01 de M015 reste à formaliser, aucun tag présumé.** J3 reste EN COURS, car M016 (traitement/publication V1) n'est pas réalisée. M017–M020 restent à faire. La qualification industrielle est distincte du démonstrateur.

**Avant M016 :** A-02 et A-05 réalisés ; A04-01/A04-02 qualifiés dans leur portée. M016 peut commencer avec suivi explicite des dettes résiduelles A04-04 à A04-08 et sans prétendre à une qualification industrielle.

**Après A-04 :** audit firmware `TR2-20261013-A04-AUDIT-FIRMWARE` livré (`DONE` Codex), neuf constats consignés dans [DETTE_TECHNIQUE_A04.md](DETTE_TECHNIQUE_A04.md). Séquence validée : **A-05 (tests/preuves, audit seul) → correction A04-01 (profil sûr) → correction A04-02 (FRAM/Modbus) → M016 (A04-03, FIFO métier V1)**. Les autres constats restent suivis, notamment RAM, timestamps, démarrage et SD. **A04-01 qualifiée logiciel et A04-02 qualifiée logiciel/physique borné par ChatGPT le 13/10/2026** ; A04-03/M016 reste à réaliser. Voir registre A-04 et rapports archivés.

**Traçabilité :** historique exact avant A-02 dans [Archives/ROADMAP_TR2_AVANT_A02_20261013.md](Archives/ROADMAP_TR2_AVANT_A02_20261013.md) ; résultats détaillés M015-V dans `Echanges_Codex/DERNIER_RAPPORT.md` et preuves référencées. La roadmap pilote les jalons, pas le journal des tentatives.

## 1. Socle de référence (historique au 11 octobre 2026)

- **Socle Modbus RTU V1 :** spécification et mapping unifié gelés ; firmware transactionnel, persistance, campagnes et architecture A–J développés. Toutes les fonctions ne sont pas encore qualifiées sur matériel réel.
- **Supervision PC :** phases S0 à S7 développées et gelées (transport, persistance SQLite, orchestration, interface Web). Liaison réelle au TR2 à qualifier.
- **STM32U575 / RS-485 :** firmware BIN008 sauvegardé, flashé et vérifié. **Mission 011 validée :** réception d'une ADU exacte `01 03 03 E8 00 01 04 7A`, 8 octets, une trame, une ADU valide, zéro erreur CRC/UART. Cette preuve porte sur un seul essai physique RX.
- **Mission 012 :** raccordement serveur Modbus RTU et réponse TX avec DE sécurisé ; publiée et en cours au moment de l'adoption de la roadmap. Aucune qualification TX physique présumée.
- **Chaîne d'acquisition vibratoire physique jusqu'à la supervision :** non encore démontrée de bout en bout.

## 2. Chaîne cible

Capteur de vibrations → acquisition et traitement STM32U575 → registres Modbus V1 → transport RS-485 / Modbus RTU → service de supervision PC → horodatage / SQLite → interface Web : valeurs instantanées, tendances et qualité des données.

## 3. Missions principales

| Mission | Objectif | Statut courant | Découpage indicatif | Critère de sortie |
|---|---|---|---|---|
| **M012** | Intégrer le serveur Modbus RTU et le TX RS-485 avec commande DE sûre | HISTORIQUE — intégré aux acquis M013 | Selon résultats Codex | Chemin de réponse implémenté et testé ; qualification physique explicitement distinguée |
| **M013** | Qualifier le dialogue Modbus physique complet | ACQUIS BORNÉS / RÉSERVES | 2–3 sous-missions | Lecture répétable de registres STM32 par maître PC ; CRC, exceptions, turnaround et absence de contention vérifiés |
| **M014** | Connecter la supervision au TR2 physique | DONE — PORTÉE B0/B1/PERTE-REPRISE | 2–3 | Lecture périodique des blocs système, déconnexion et reprise correctement traitées |
| **M015** | Qualifier la chaîne d'acquisition vibratoire | DONE — ACQUISITION BORNÉE M015-V | 3–5 | Acquisition physique démontrée ou blocages matériels précisément identifiés |
| **M016** | Traiter et publier les mesures vibratoires dans les registres V1 | MISSION PUBLIÉE — À EXÉCUTER | 2–4 | Valeurs réelles cohérentes, unités, échelles et validité vérifiées dans le mapping gelé |
| **M017** | Lire les vibrations de bout en bout via Modbus | À FAIRE | 2 | Variation physique retrouvée correctement dans les valeurs lues par le PC |
| **M018** | Intégrer les mesures dans le service PC et SQLite | À FAIRE | 2–3 | Décodage, horodatage, persistance et gestion des données invalides opérationnels |
| **M019** | Afficher les vibrations dans l'interface Web S7 | À FAIRE | 2–3 | Valeurs instantanées, axes, unités, tendances, historique et état de mesure consultables |
| **M020** | Qualifier le démonstrateur intégré | À FAIRE | 3–4 | Vibration physique visible dans le Web, historique durable, essais de repos/excitation, coupures et reprise documentés |

**Périmètre de référence :** M013 à M020. Les jalons restent stables ; le découpage en sous-missions dépend des preuves et risques, sans quota artificiel.

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

| Jalon | Définition | Missions | État courant |
|---|---|---|---|
| **J0** | STM32 reçoit une requête Modbus physique | M011 | **VALIDÉ** pour l'essai unique |
| **J1** | STM32 répond correctement à un maître | M012–M013 | ACQUIS BORNÉS ; qualification générale ouverte |
| **J2** | Supervision communique avec le TR2 réel | M014 | VALIDÉ PORTÉE B0/B1/perte-reprise ; revue indépendante indiquée par contrat M015-A |
| **J3** | TR2 produit des mesures vibratoires physiques | M015–M016 | EN COURS — acquisition M015-V bornée démontrée ; publication M016 à faire |
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



## Constat R5-B — checkpoint du17/10/2026, reprise après interruption

Composition directe PC readonly préparée/testée (`dc773733a98d78016e1d0f8a341b155e1851e2c7`,561tests.NET et ARM). La démonstration physique reste **BLOQUÉE au déploiement Windows** : CodeIntegrity3033/3077 refuse la DLL du service, aucune acquisition COM4 R5-B exécutée. HTTP/SQLite Linux prouvés uniquement en CommunicationLost sans mesure ; aucune qualification J4/J5 ajoutée. M016 etFRAM4e7fab conformes au préflight17/10, jamais remplacés. Résoudre la voie de déploiement autorisée puis reprendre le plan [LIVE_R5B](../Supervision%20TR2/LIVE_R5B.md) ; contrôle indépendant ChatGPT et qualification des jalons inchangés.


## Constat R5-B2 —18/10/2026, preuve physique diagnostique

Chaîne capteur → STM32 → COM4 → vrai service PC Windows local → SQLite → API/Web/DOM Edge headless observée dans un essai borné :88 courantes,54 HELD,28 pertes logiques honnêtes,141 lignes API corrélées,824 ADU du service CRC exactes. M016 restauré, Flash entière c5bc et FRAM2b501a/run11/journal78 conservée sans rollback. [Bilan R5B2](../Supervision%20TR2/LIVE_R5B2.md). Aucun jalon J4/J5 qualifié unilatéralement : contrôle indépendant ChatGPT à faire. Profil partiel NON-PRODUCTION, B1/B2/B7 et UTC absents, STOP durable≈40s ; pas de qualification R6/endurance/industrie. Windows local fonctionnel désormais observé ; ancien checkpoint R5-B BLOCKED préservé historiquement. Tests/builds acquis réutilisés, seule la nouvelle campagne physique exécutée.


## Constat UX-L1 —18/10/2026, intégration logicielle readonly

Parc et fiche LIVE à six onglets raccordés au modèle R5 et à SQLite, avec API additive, provenance et historique non courant explicites. Tests .NET/JS/HTTP et DOM headless sur archive R5-B2 ; aucun accès matériel ou qualification supplémentaire J4/J5/R6. Tendance, graphiques, filtres avancés et export différés UX-L2 ; overlays documentés UX-L3. [Note UX-L1](../Supervision%20TR2/LIVE_UXL1.md). Gouvernance et acquis physiques inchangés.


## Constat UX-L2 —18/10/2026, consultation des tendances persistées

Courbes globales/XYZ, périodes bornées, ruptures et HELD explicites, outils Attention/bus/recherche/tri du parc et CSV du lot affiché livrés sur le socle UX-L1. API readonly historique500 observations/lot,10000 candidats et période≤7j ; couverture tronquée et provenance non courante affichées.578 tests.NET,8 tests JS ciblés, HTTP et vrai DOM Edge headless depuis archive R5-B2 :88 observations vibratoires/176 points globaux/264 XYZ ; CSV500 lignes exact, base inchangée. [Note UX-L2](../Supervision%20TR2/LIVE_UXL2.md). Aucun essai matériel ni qualification additionnelle J4/J5/R6. Affectations/noms personnalisés absents, UX-L3 et profil complet distincts ; contrôle indépendant ChatGPT après publication.


## Constat UX-L2 physique révision2 —19/10/2026

**BLOCKED Windows**, pas d'essai STM32 réalisé : Code Integrity3033/3077 refuse la nouvelle Domain.dll avant ouverture COM, malgré publication autonome locale vérifiée. Image LIVE opérateur persistante et commandes START/STOP à partage COM explicite préparées/testées logiciellement,350 tests hors Service+238 Service finaux/cross-build PASS. M016/FRAM2b501a relus conformes, jamais remplacés, aucun reset/commande physique/restauration. L'utilisateur demande publication du blocage. Lever la confiance Windows puis reprendre [le plan](../Supervision%20TR2/operator-demo/REPRISE.md) : flash LIVE, cycles START/STOP, run prolongé surveillé, vrai Edge/SQLite et firmware conservé si sain. Démonstration du20/10 NON VALIDÉE à ce checkpoint ; aucun jalon physique additionnel qualifié, lot2 ergonomique différé.


## Constat WINDOWS-TRUST-LIVE —19/10/2026, portée limitée autorisée

Paquet c36c971 accepté sous SAC1 inchangé et explicitement autorisé par l'utilisateur ; signature durable reportée à un chantier séparé. LIVE9bbe…0981 flashé/conservé, boot autonomeSTOP, deux START/STOP5/14–5/17 terminaux+effets et second run surveillé≥11.85min. Chaîne COM4/SQLite/API/Edge/CSV réelle corrélée ; FRAM finale journal94/root107/run13/historique+queue exacts. FinalSTOP/HELD, service fermé/COM libre/GDB détaché. Acquittement c36 oscillant, quatre handoffs exclusifs réussis ; correctif8a54 publié/testé mais nouvelle DLL refusée, non déployée. [Rapport courant](Echanges_Codex/DERNIER_RAPPORT.md). Aucun jalon J4/J5/R6 ni GO industriel prononcé ; contrôle indépendant ChatGPT requis.


## COM4 LOSS DIAG —19/10/2026

Cause d’audit GDB prouvée par reproduction : READ FRAM262144 injecté dans callbackchronométré, dépassement846ms/2,5ms, arrêt sûr du runtime et silenceModbus malgré RXactif. Procédure corrigée horsscope, aucun changement des garde-fous/firmware/Windows ; deux resets nécessaires, aucunflash. NouveauSTART5/18/STOP5/19 et B0/B3aprèsauditfinalsansreset réussis ; LIVEconservé/STOPHELD/serviceWindowsactif. Erratum de disponibilité après ancien audit, cyclesbornés antérieurs préservés. [Diagnostic](../Supervision%20TR2/operator-demo/COM4_DIAGNOSTIC.md), rapportcourant et preuves ; aucunGOindustriel/J4/J5/R6 ajouté.


ComplémentCOM4 : gardeHELD supplémentaire corrigéed677/248testsPASS ; nouveaupaquetWindows accepté(4HTTP200/SAC1/aucunCI), START5/20 depuisHELDsansreset puisSTOP5/21 vialanceur. Firmwarec36inchangé, PCqualifiéd677contenu, finaljournal110/root123/run15/FRAM97f43a…455f3, busvérifiéaprèsauditfinalsansreset. Ancienpaquet+DBpréservés, lanceurshabituelsredirectionsréversibles ; servicecorrigéactif5100/COM4unique/cibleSTOPHELD. Signaturedurable séparée, aucunjalonindustriel ajouté.
