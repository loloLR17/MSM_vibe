# FT-RBT — Audit final et proposition de gel V1

## 1. Référence de travail

- dépôt : `loloLR17/MSM_vibe` ;
- branche audit : `audit/ft-rbt-v1` ;
- base gelée : `main` au commit `7ba547275292bca8077ea3b9379e19822c58df55` ;
- famille : FT-RBT — robustesse protocolaire.

## 2. Résultat de la passe croisée

La famille FT-RBT a été relue transversalement après reconstruction des cinq sous-familles.

Contrôles effectués :
- cohérence des frontières avec FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT, FT-CMD, FT-SEQ et FT-PER ;
- absence de duplication d'oracle gelé ;
- absence d'exigence temporelle ou de charge inventée ;
- distinction entre CRC Modbus RTU et CRC applicatif ;
- distinction entre rejeu transactionnel nominal FT-CMD et perte de réponse FT-RBT ;
- cohérence des classifications et des tests ;
- cohérence des noms de sous-familles et des chemins de répertoire ;
- consolidation des dettes normatives.

Une incohérence documentaire mineure de nommage du répertoire FT-RBT-02 dans le README racine a été corrigée pendant la passe finale.

## 3. Couverture consolidée

36 points :
- 1 `COVERED` ;
- 3 `CONDITIONAL` ;
- 12 `DELEGATED` ;
- 1 `TRACE_ONLY` ;
- 19 `NOT_DEFINED`.

4 tests propriétaires :
- `TT-RBT-GEN-001` ;
- `TT-RBT-B05-001` ;
- `TT-RBT-B05-002` ;
- `TT-RBT-GEN-020`.

## 4. Conclusions par sous-famille

### FT-RBT-01 — Requêtes invalides et non-corruption

Un scénario composé est couvert : une requête invalide qualifiée par FT-ACC est intercalée entre deux échanges valides ; elle ne doit pas corrompre le fonctionnement nominal observable. Les délais de récupération, rafales et resets induits restent non définis.

### FT-RBT-02 — Perte de réponse et retransmission

Deux tests conditionnels couvrent la perte volontaire de la première réponse puis le rejeu du même `transaction_id`. La réutilisation du résultat et la non-double-exécution reposent sur l'oracle FT-CMD-02 ; la seconde exige un observable discriminant.

### FT-RBT-03 — Répétitions et sollicitations dégradées

Aucun nouveau test autonome n'est justifié. Les oracles déterministes sont déjà possédés par FT-CMD-02, FT-CMD-04 et FT-RBT-02. Les rafales, cadences et files d'attente restent non définies.

### FT-RBT-04 — Lectures sous transition

Un test conditionnel compose FT-STR-07 pendant une transition contrôlée. Chaque réponse est jugée individuellement ; aucune stabilité inter-requêtes, capture obligatoire d'état intermédiaire ou cadence de polling n'est inventée.

### FT-RBT-05 — Trames dégradées, timing et resynchronisation

Aucun test normatif autonome n'est possible en V1 pour CRC RTU invalide, framing/troncature, timeout, retry, charge ou resynchronisation. Ces points sont conservés explicitement `NOT_DEFINED`. Les accès invalides mapping et CRC applicatifs restent délégués à leurs familles propriétaires.

## 5. Non-conformités bloquantes

Aucune non-conformité documentaire bloquante n'a été identifiée à l'issue de la passe croisée.

Les `NOT_DEFINED` ne sont pas des échecs d'implémentation : ce sont des dettes de spécification explicitement conservées. Ils devront être arbitrés avant toute prétention de conformité sur ces comportements.

## 6. Décision proposée

**FT-RBT V1 est prête pour gel**, sous réserve de validation explicite du présent audit puis merge de `audit/ft-rbt-v1` dans `main`.

Aucun comportement supplémentaire ne doit être ajouté pendant le gel sans réouverture formelle de la spécification ou de la famille concernée.
