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
| **M013** | Qualifier le dialogue Modbus physique complet | À FAIRE | 2–3 sous-missions | Lecture répétable de registres STM32 par maître PC ; CRC, exceptions, turnaround et absence de contention vérifiés |
| **M014** | Connecter la supervision au TR2 physique | À FAIRE | 2–3 | Lecture périodique des blocs système, déconnexion et reprise correctement traitées |
| **M015** | Qualifier la chaîne d'acquisition vibratoire | À FAIRE | 3–5 | Acquisition physique démontrée ou blocages matériels précisément identifiés |
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
| **J1** | STM32 répond correctement à un maître | M012–M013 | À QUALIFIER |
| **J2** | Supervision communique avec le TR2 réel | M014 | À FAIRE |
| **J3** | TR2 produit des mesures vibratoires physiques | M015–M016 | À FAIRE |
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
