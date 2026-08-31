# AUDIT FINAL FT-SEQ V1

## 1. Périmètre audité

Famille : `Modbus RTU/02_Validation/FT_SEQ/`

Objet : séquences métier multi-actions / multi-transactions nécessaires pour valider de bout en bout les cas d'usage V1, sans dupliquer les oracles des familles FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT et FT-CMD.

Référence de branche source avant audit FT-SEQ : `main` au commit `c29d92a00687e872f318e27b5f1e549b7b30f041`.

Branche de reconstruction : `audit/ft-seq-v1`.

## 2. Sous-familles auditées

1. FT-SEQ-01 — Qualification initiale et établissement du contexte
2. FT-SEQ-02 — Préparation et activation d'une configuration
3. FT-SEQ-03 — Préparation et synchronisation temporelle
4. FT-SEQ-04 — Démarrage et établissement d'une campagne
5. FT-SEQ-05 — Arrêt, clôture et consultation de campagne
6. FT-SEQ-06 — Cycle métier nominal complet
7. FT-SEQ-07 — Refus puis reprise de séquence

## 3. Résultat de la passe croisée

### 3.1 Cohérence interne

Aucune contradiction de fond n'a été trouvée entre FT-SEQ-01 à FT-SEQ-07.

La progression est cohérente :
- FT-SEQ-01 conserve la qualification comme contexte non normé séquentiellement ;
- FT-SEQ-02 et 03 couvrent les chaînes préparé→appliqué ;
- FT-SEQ-04 et 05 couvrent le cycle acquisition/campagne ;
- FT-SEQ-06 assemble les chaînes nominales dans un scénario E2E ;
- FT-SEQ-07 couvre uniquement les reprises dont la correction est normativement déterministe.

### 3.2 Frontière FT-CMD

Aucun refus isolé, code résultat, mécanisme de transaction, règle d'idempotence, corrélation ou concurrence n'a été réattribué à FT-SEQ.

FT-SEQ-07 couvre la succession `refus → correction → nouvelle tentative → succès`, tandis que FT-CMD reste propriétaire du refus et du résultat.

### 3.3 Frontière FT-INT

Les effets élémentaires B5→B2, B5→B4, B5→B1/B6 restent propriétaires FT-INT.

FT-SEQ ne couvre que la chaîne complète qui compose ces effets avec les opérations précédentes/suivantes.

### 3.4 Frontière FT-BLK

Les invariants internes de temps, configuration et inventaire de campagnes ne sont pas dupliqués. Ils sont seulement utilisés comme jalons dans les scénarios FT-SEQ.

### 3.5 Frontières FT-PER / FT-RBT

Aucun scénario de reboot, persistance ou reprise après redémarrage n'est introduit dans FT-SEQ.

Aucune perte de trame, répétition agressive, retransmission hostile ou temporisation perturbée n'est transformée en scénario FT-SEQ. Ces sujets restent délégués à FT-PER et FT-RBT.

## 4. Couverture consolidée

La matrice finale comptabilise :
- `COVERED` : 10 ;
- `CONDITIONAL` : 1 ;
- `DELEGATED` : 33 ;
- `TRACE_ONLY` : 7 ;
- `NOT_DEFINED` : 28.

Total : 79 entrées classifiées.

Neuf tests FT-SEQ propriétaires sont définis :
- `TT-SEQ-CONFIG-001` ;
- `TT-SEQ-TIME-001` ;
- `TT-SEQ-CAMP-001` ;
- `TT-SEQ-CAMP-002` ;
- `TT-SEQ-SYS-001` ;
- `TT-SEQ-REC-001` ;
- `TT-SEQ-REC-002` ;
- `TT-SEQ-REC-003` ;
- `TT-SEQ-REC-004`.

FT-SEQ-01 ne comporte volontairement aucun test autonome, car aucun handshake initial normatif n'existe en V1.

## 5. Points de vigilance conservés

La passe finale confirme qu'il ne faut pas renforcer silencieusement V1 sur les points suivants :
- ordre obligatoire de qualification initiale ;
- observation obligatoire d'un état `VALIDE` intermédiaire ;
- égalités temporelles bit-à-bit entre lectures séparées ;
- tolérances temporelles inventées ;
- égalité `active_campaign_id` / `campaign_id` ;
- incrément `total_campaign_count +1` immédiatement après START ;
- égalité universelle de durée ;
- témoin direct de flush buffers / fermeture fichiers ;
- obligation de SYNC TIME avant chaque campagne ;
- ordre universel SYNC TIME / APPLY CONFIG ;
- diagnostic B7 obligatoire après STOP ;
- politique générique de retry ;
- récupération générique de causes physiques ou externes.

## 6. Dettes / ambiguïtés

Les ambiguïtés utiles à une future V1.1 sont regroupées dans `EVOLUTIONS_CANDIDATES_V1_1.md`.

Elles ne bloquent pas le gel V1 de FT-SEQ dès lors que :
- les limites restent explicitement `NOT_DEFINED`, `TRACE_ONLY` ou `CONDITIONAL` ;
- aucun test V1 ne dépend d'un comportement non spécifié ;
- les familles propriétaires conservent leurs oracles.

## 7. Conclusion d'audit

**Verdict : FT-SEQ V1 est cohérente, complète au regard des exigences séquentielles identifiables en V1 et prête pour gel après validation utilisateur.**

Aucune correction de fond supplémentaire n'est requise avant merge.

Le merge vers `main` et le gel définitif ne doivent être effectués qu'après accord explicite.
