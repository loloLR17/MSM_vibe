# MSM – Capteur de vibration (TR2)
# Plan de test protocole Modbus RTU
## Squelette macro de validation – version de travail

## 1. Objet du document

Ce document définit le **squelette macro du plan de test** du protocole Modbus RTU du système TR2.

L’objectif est de poser une structure industrielle exploitable pour la phase de validation, avant rédaction détaillée des cas de test individuels.

Le présent document couvre :

- l’architecture globale du plan de test,
- la logique de validation,
- la structuration officielle des familles de tests,
- les risques couverts,
- l’ordre d’exécution recommandé,
- les principes d’automatisation et de simulation.

Le présent document **ne détaille pas encore les cas de test individuels**.

---

## 2. Contexte technique

### 2.1 Système visé

Le système TR2 est un système industriel de capteurs de vibration avec les caractéristiques suivantes :

- capteurs autonomes en mode **store-and-forward**,
- acquisition locale,
- stockage local sur SD,
- communication filaire **Modbus RTU sur RS-485**,
- aucune dépendance à une liaison permanente pendant la campagne,
- centrale chargée de :
  - configuration,
  - synchronisation temporelle,
  - supervision,
  - diagnostic,
  - inventaire des données.

### 2.2 Architecture protocolaire

Le protocole est organisé en 8 blocs :

- **Bloc 0** : Identification
- **Bloc 1** : État système
- **Bloc 2** : Temps
- **Bloc 3** : Supervision vibratoire
- **Bloc 4** : Configuration acquisition
- **Bloc 5** : Commandes
- **Bloc 6** : Inventaire campagnes
- **Bloc 7** : Diagnostic

### 2.3 Choix techniques validés

- types autorisés : `uint16`, `uint32`, `bitfield`, `enum`, `ASCII fixe`
- pas de float
- `uint32` codé en **MSW puis LSW**
- chaînes ASCII fixes avec padding `0x00`
- registres réservés = `0`
- `ID = 0` → non renseigné

### 2.4 Spécificités métier

- capteurs autonomes sans liaison permanente,
- données critiques stockées localement,
- interrogation possible à tout moment par la centrale,
- protocole devant être robuste aux coupures, reprises et incohérences.

---

## 3. Périmètre de validation

### 3.1 Inclus dans cette phase

La présente phase couvre :

- conformité de mapping Modbus,
- conformité structurelle et sémantique des blocs,
- cohérence inter-blocs,
- validation des transactions et séquences métier,
- validation des valeurs limites,
- validation de la robustesse protocolaire,
- validation de la persistance et de la reprise,
- validation sur **simulateur**, sans matériel réel.

### 3.2 Exclus temporairement

Sont hors périmètre de cette phase :

- performances physiques du capteur,
- justesse métrologique vibratoire,
- robustesse électrique réelle du bus RS-485,
- bruit et perturbations physiques sur ligne,
- endurance matérielle réelle SD,
- CEM,
- tests environnementaux.

---

## 4. Architecture globale du plan de test

Le plan de test doit être structuré comme une **validation en couches**, et non comme une simple suite de lectures de registres.

L’architecture recommandée comprend 7 niveaux de validation.

### Niveau A — Validation de base protocolaire et structurelle

Objectif : vérifier la conformité de base de l’interface Modbus.

Couvre notamment :

- accessibilité des plages de registres,
- tailles des blocs,
- ordre des mots 32 bits,
- droits RO/RW,
- chaînes ASCII fixes,
- registres réservés,
- sentinelles,
- comportement hors plage.

### Niveau B — Validation fonctionnelle intra-bloc

Objectif : valider chaque bloc isolément selon sa sémantique propre.

### Niveau C — Validation de cohérence inter-blocs

Objectif : vérifier l’absence de contradiction entre blocs et la cohérence des dépendances croisées.

### Niveau D — Validation transactionnelle et séquentielle

Objectif : valider les séquences de préparation, commande, transition et résultat final.

### Niveau E — Validation de robustesse et résilience protocolaire

Objectif : valider le comportement sous répétition, accès incomplets, lectures concurrentes, erreurs de séquencement.

### Niveau F — Validation de persistance et reprise après incident

Objectif : vérifier ce qui survit, ce qui est réinitialisé et ce qui doit rester cohérent après reboot ou coupure simulée.

### Niveau G — Validation système orientée exploitation

Objectif : vérifier que le protocole est exploitable par une centrale industrielle et ne laisse pas de zones d’ambiguïté opérationnelle.

---

## 5. Stratégie globale de validation

La stratégie recommandée est :

- validation **incrémentale**,
- validation du plus structurel au plus métier,
- validation nominale d’abord,
- validation des limites ensuite,
- robustesse ensuite,
- persistance ensuite,
- non-régression enfin.

### Étapes recommandées

1. validation documentaire statique,
2. validation sur simulateur déterministe,
3. validation automatique par scripts,
4. validation de robustesse avec injection de fautes,
5. validation de conformité système orientée exploitation.

---

## 6. Structure formelle recommandée du document de plan de test

Le document maître de validation doit comporter au minimum les sections suivantes :

1. Introduction
2. Référentiel
3. Objet testé
4. Stratégie de validation
5. Moyens de test
6. Familles de tests
7. Matrice de couverture
8. Critères d’entrée / sortie
9. Gestion des anomalies
10. Annexes

### 6.1 Contenu recommandé des chapitres

#### Chapitre 1 — Introduction

- objectif du plan,
- périmètre,
- hypothèses,
- exclusions.

#### Chapitre 2 — Référentiel

- documents de spécification,
- versions,
- conventions de codage,
- terminologie.

#### Chapitre 3 — Objet testé

- architecture des blocs 0 à 7,
- principes transactionnels,
- hypothèses de fonctionnement store-and-forward.

#### Chapitre 4 — Stratégie de validation

- niveaux de validation,
- ordre d’exécution,
- logique nominal / limites / robustesse / persistance,
- politique de non-régression.

#### Chapitre 5 — Moyens de test

- simulateur,
- scripts,
- journalisation,
- injections de fautes,
- gestion des jeux de données.

#### Chapitre 6 — Familles de tests

- FT-STR,
- FT-ACC,
- FT-BLK,
- FT-INT,
- FT-LIM,
- FT-CMD,
- FT-SEQ,
- FT-RBT,
- FT-PER,
- FT-OBS.

#### Chapitre 7 — Matrice de couverture

- exigences ↔ tests,
- risques ↔ tests,
- blocs ↔ tests,
- automatisation ↔ tests.

#### Chapitre 8 — Critères d’entrée / sortie

- conditions de lancement,
- critères de succès,
- critères de blocage,
- critères de gel.

#### Chapitre 9 — Gestion des anomalies

- classification,
- sévérité,
- règles de re-test,
- traitement des ambiguïtés de spécification.

#### Chapitre 10 — Annexes

- mappings,
- conventions de décodage,
- dictionnaires enums / bitfields,
- format des journaux,
- jeux de données simulés,
- scénarios d’état du simulateur.

---

## 7. Convention d’identification des tests

Convention recommandée :

`TT-<famille>-<bloc ou scope>-<numéro>`

Exemples :

- `TT-STR-GEN-001`
- `TT-ACC-B04-003`
- `TT-BLK-B02-011`
- `TT-INT-B03B04-004`
- `TT-CMD-B05-007`
- `TT-SEQ-SYS-012`
- `TT-PER-SYS-005`

Règles :

- `GEN` = transversal,
- `SYS` = système,
- `B00` à `B07` = bloc ciblé,
- double bloc ou scope composite pour les tests de cohérence.

---

## 8. Format standard recommandé d’une fiche de test

Chaque cas de test détaillé devra comporter au minimum :

- identifiant,
- titre,
- objectif,
- exigence(s) couverte(s),
- préconditions,
- données d’entrée,
- scénario / étapes,
- résultat attendu,
- critères d’acceptation,
- mode d’exécution,
- automatisation possible,
- traces à conserver,
- niveau de criticité,
- remarques / limites.

---

## 9. Matrice de couverture recommandée

Le plan doit intégrer une matrice de couverture formelle.

### 9.1 Objectif

Permettre la traçabilité entre :

- exigences,
- risques,
- familles de tests,
- cas de test,
- statut de validation.

### 9.2 Colonnes recommandées

- `Req_ID`
- `Bloc`
- `Intitulé`
- `Source`
- `Type`
- `Risque_ID`
- `Priorité`
- `Famille_Test`
- `Test_ID`
- `Auto`
- `Statut`
- `Remarques`

---

## 10. Priorisation recommandée

### 10.1 Priorité des tests

- **P0** — critique protocole
- **P1** — critique exploitation
- **P2** — important mais contournable
- **P3** — confort / lisibilité

### 10.2 Classification des anomalies

- **A1** — bloquante
- **A2** — majeure
- **A3** — mineure
- **A4** — observation
- **AS** — ambiguïté de spécification

---

## 11. Table officielle des familles de tests

### 11.1 FT-STR — Conformité structurelle

**Objectif**

Vérifier la conformité binaire et structurelle de l’exposition Modbus.

**Périmètre**

- adresses,
- tailles,
- types,
- ordre MSW/LSW,
- chaînes ASCII,
- réservés = 0,
- sentinelles.

**Risques couverts**

- erreur de mapping,
- inversion de mots 32 bits,
- incompatibilité centrale/capteur.

**Priorité**

P0

---

### 11.2 FT-ACC — Accès et permissions

**Objectif**

Vérifier le respect des droits d’accès et l’absence d’effet de bord non prévu.

**Périmètre**

- lecture zones RO,
- écriture zones RW,
- refus d’écriture sur RO,
- comportement des réservés,
- stabilité des champs non ciblés.

**Risques couverts**

- écriture interdite acceptée,
- état critique modifiable,
- effet implicite caché.

**Priorité**

P0

---

### 11.3 FT-BLK — Fonctionnel intra-bloc

**Objectif**

Valider, bloc par bloc, la cohérence fonctionnelle interne.

**Périmètre**

- Bloc 0 : identification,
- Bloc 1 : état système,
- Bloc 2 : temps,
- Bloc 3 : supervision vibratoire,
- Bloc 4 : configuration acquisition,
- Bloc 5 : commandes,
- Bloc 6 : inventaire campagnes,
- Bloc 7 : diagnostic.

**Risques couverts**

- bloc lisible mais faux sémantiquement,
- enum ou bitfield incohérent,
- sentinelle ambiguë.

**Priorité**

P0 à P1 selon bloc et champ.

---

### 11.4 FT-INT — Cohérence inter-blocs

**Objectif**

Vérifier l’absence de contradiction entre blocs et la cohérence des dépendances croisées.

**Périmètre typique**

- Bloc 1 ↔ Bloc 7,
- Bloc 2 ↔ Bloc 5,
- Bloc 4 ↔ Bloc 5,
- Bloc 4 ↔ Bloc 3,
- Bloc 1 ↔ Bloc 3,
- Bloc 1 ↔ Bloc 6,
- Bloc 6 ↔ Bloc 7.

**Risques couverts**

- protocole localement correct mais globalement incohérent,
- duplication contradictoire,
- diagnostic faux.

**Priorité**

P0/P1

---

### 11.5 FT-LIM — Valeurs limites et domaines invalides

**Objectif**

Vérifier le comportement aux frontières des domaines et sur les valeurs interdites.

**Périmètre**

- min/max,
- zéro,
- sentinelles,
- enums hors domaine,
- index invalides,
- paramètres incohérents.

**Risques couverts**

- corruption silencieuse,
- refus non déterministe,
- débordement logique.

**Priorité**

P0/P1

---

### 11.6 FT-CMD — Validation du moteur de commandes

**Objectif**

Valider le comportement transactionnel du Bloc 5 et les actions qu’il pilote.

**Périmètre**

- soumission,
- transaction_id,
- prise en compte,
- refus,
- exécution,
- statut,
- résultat final,
- répétition,
- concurrence logique,
- unicité d’exécution.

**Risques couverts**

- double exécution,
- exécution fantôme,
- commande perdue,
- blocage sur commande en cours.

**Priorité**

P0

---

### 11.7 FT-SEQ — Séquences métier

**Objectif**

Valider les enchaînements réalistes d’exploitation par la centrale.

**Périmètre**

Exemples :

- qualification initiale du capteur,
- préparation puis application de configuration,
- synchronisation temporelle,
- consultation état / inventaire / diagnostic,
- séquence maintenance,
- démarrage / arrêt si applicable.

**Risques couverts**

- protocole inutilisable dans les scénarios réels,
- ordre des opérations ambigu,
- états intermédiaires non observables.

**Priorité**

P1, avec certaines séquences en P0.

---

### 11.8 FT-RBT — Robustesse protocolaire

**Objectif**

Valider le comportement sous défauts de communication et séquencement dégradé.

**Périmètre**

- répétition de requêtes,
- répétition d’écritures,
- écritures incomplètes,
- lectures pendant transition,
- doute sur accusé,
- timeout logique,
- image transitoire.

**Risques couverts**

- état partiellement mis à jour,
- exécution multiple,
- perte de resynchronisation logique.

**Priorité**

P0/P1

---

### 11.9 FT-PER — Persistance et reprise

**Objectif**

Vérifier ce qui est conservé, restauré ou réinitialisé après reboot ou coupure simulée.

**Périmètre**

- identité,
- configuration active,
- configuration préparée,
- campagnes,
- diagnostics,
- état système,
- temps,
- cohérence post-coupure.

**Risques couverts**

- perte de traçabilité,
- configuration fantôme,
- campagnes corrompues,
- ambiguïté après redémarrage.

**Priorité**

P0

---

### 11.10 FT-OBS — Observabilité et exploitabilité

**Objectif**

Vérifier que le protocole fournit une vision exploitable par une centrale industrielle.

**Périmètre**

- lisibilité des états,
- distinction défaut actif / défaut mémorisé,
- donnée absente / invalide / zéro réel,
- qualité des codes état / résultat,
- aptitude au diagnostic distant.

**Risques couverts**

- protocole techniquement valide mais inutilisable,
- ambiguïtés d’exploitation,
- maintenance dégradée.

**Priorité**

P1

---

## 12. Vue synthétique des familles de tests

| Famille | But principal | Portée | Priorité |
|---|---|---|---|
| FT-STR | conformité binaire | structure protocolaire | P0 |
| FT-ACC | contrôle des droits | accès registres | P0 |
| FT-BLK | validité locale | bloc isolé | P0/P1 |
| FT-INT | non-contradiction | multi-blocs | P0/P1 |
| FT-LIM | robustesse de domaine | valeurs frontières | P0/P1 |
| FT-CMD | sûreté transactionnelle | commandes | P0 |
| FT-SEQ | usage réel | scénarios métier | P1 |
| FT-RBT | tolérance aux défauts | protocole dégradé | P0/P1 |
| FT-PER | reprise et stockage | reboot/persistance | P0 |
| FT-OBS | lisibilité exploitation | supervision/maintenance | P1 |

---

## 13. Dépendances entre familles

### Dépendances fortes

- **FT-STR** avant tout le reste,
- **FT-ACC** avant FT-BLK sur zones RW,
- **FT-BLK** avant FT-INT,
- **FT-BLK + FT-ACC** avant FT-CMD,
- **FT-CMD** avant FT-SEQ,
- **FT-RBT** après validation nominale,
- **FT-PER** après clarification de la doctrine de persistance,
- **FT-OBS** après premiers scénarios valides.

---

## 14. Ordre d’exécution recommandé

### Phase 1 — Base protocolaire

1. FT-STR
2. FT-ACC

### Phase 2 — Validation bloc par bloc

3. FT-BLK / Bloc 0
4. FT-BLK / Bloc 1
5. FT-BLK / Bloc 2
6. FT-BLK / Bloc 4
7. FT-BLK / Bloc 5
8. FT-BLK / Bloc 6
9. FT-BLK / Bloc 7
10. FT-BLK / Bloc 3

### Phase 3 — Cohérence et usage

11. FT-INT
12. FT-LIM
13. FT-CMD
14. FT-SEQ

### Phase 4 — Résilience

15. FT-RBT
16. FT-PER
17. FT-OBS

---

## 15. Architecture recommandée du banc de test

### 15.1 Composants du banc

Le banc de validation doit comporter :

- un client de test,
- un simulateur de capteur,
- un moteur de scénarios,
- un journal d’échanges,
- un moteur d’assertions,
- un générateur de rapport.

### 15.2 Modes de simulation

Le simulateur doit supporter :

- mode nominal déterministe,
- mode défaut injecté,
- mode incohérence volontaire,
- mode redémarrage simulé,
- mode persistance simulée.

### 15.3 Granularité d’observation

Les vérifications doivent pouvoir s’appuyer sur :

- valeur brute registre,
- valeur décodée,
- état métier simulé,
- transition d’état,
- verdict de test.

---

## 16. Architecture recommandée du simulateur capteur

Le simulateur ne doit pas être un simple faux serveur Modbus statique.

Il doit être structuré en couches.

### Couche 1 — Modèle métier

État interne simulé du capteur :

- identité,
- temps,
- état système,
- configuration préparée / active,
- campagnes,
- défauts,
- diagnostics,
- commandes en cours.

### Couche 2 — Projection registre

Transformation de l’état métier en image registre Modbus.

### Couche 3 — Moteur d’événements

Capacité à déclencher :

- commande acceptée,
- commande refusée,
- timeout logique,
- reboot,
- corruption simulée,
- perte de persistance,
- défaut actif.

### Couche 4 — Persistance simulée

Conservation sélective de l’état selon les règles de test.

### Profils d’état minimaux recommandés

- `S00_INIT`
- `S01_IDLE_SYNCED`
- `S02_IDLE_UNSYNCED`
- `S03_ACQ_RUNNING`
- `S04_ALARM_ACTIVE`
- `S05_FAULT_LATCHED`
- `S06_MAINTENANCE`
- `S07_CAMPAIGNS_PRESENT`
- `S08_STORAGE_DEGRADED`
- `S09_REBOOT_RECOVERY`
- `S10_INCOHERENT_FOR_TEST`

---

## 17. Risques principaux à couvrir

- **R1** — erreur de mapping binaire
- **R2** — contradiction de sémantique
- **R3** — effet implicite non voulu
- **R4** — séquence transactionnelle ambiguë
- **R5** — perte de cohérence après coupure
- **R6** — persistance incorrecte
- **R7** — supervision non exploitable
- **R8** — angle mort de spécification

---

## 18. Doctrines à verrouiller avant les cas détaillés

Avant de rédiger les cas de test individuels, il faudra figer explicitement :

### Doctrine 1 — Non renseigné / zéro réel / non applicable / invalide

Éviter toute ambiguïté entre :

- zéro réel,
- non renseigné,
- non applicable,
- inconnu,
- jamais mesuré.

### Doctrine 2 — Atomicité logique

Préciser ce qui est garanti atomique :

- image d’un bloc,
- groupe de registres,
- application d’une commande,
- bascule config préparée → active.

### Doctrine 3 — Persistance normative

Lister ce qui survit :

- au reboot propre,
- à la coupure brutale,
- à la reprise avec stockage dégradé.

### Doctrine 4 — Répétition de commande

Formaliser le comportement attendu si une même commande est répétée, avec ou sans même transaction_id.

### Doctrine 5 — Cohérence temporelle

Définir le comportement attendu si le temps est invalide, absent ou jamais synchronisé.

---

## 19. Recommandation de suite de travail

Le squelette du plan de test étant désormais établi, la suite recommandée est :

1. une discussion dédiée par famille de test,
2. détaillage des sous-familles,
3. ensuite détaillage des cas individuels,
4. puis préparation progressive du simulateur et de l’automatisation.

Ordre recommandé des prochaines discussions :

1. FT-STR
2. FT-ACC
3. FT-BLK
4. FT-INT
5. FT-LIM
6. FT-CMD
7. FT-SEQ
8. FT-RBT
9. FT-PER
10. FT-OBS

---

## 20. Conclusion

Le présent squelette pose une base industrielle cohérente pour la validation du protocole Modbus RTU TR2.

Le plan est structuré autour :

- d’une validation en couches,
- de familles de tests orientées risques,
- d’une matrice de couverture,
- d’un simulateur scénarisé,
- d’une stratégie compatible avec une validation sans matériel réel.

La prochaine étape logique consiste à ouvrir une discussion dédiée à la première famille : **FT-STR — conformité structurelle**.
