# FT-RBT — Spécifications de validation

## 1. Principe de propriété

FT-RBT possède uniquement la dimension de robustesse face à une perturbation de communication ou de séquencement lorsque la V1 permet d'établir un oracle normatif.

FT-RBT peut composer des oracles déjà gelés, mais ne doit ni les redéfinir ni les renforcer.

## 2. Doctrine d'oracle

Un scénario FT-RBT doit comporter :
1. un état ou échange nominal dont l'oracle est défini par V1 ou une famille propriétaire gelée ;
2. une perturbation explicitement identifiable ;
3. un observable après perturbation permettant d'établir un verdict sans inventer de délai, retry, priorité ou politique de récupération.

Lorsque V1 ne définit pas suffisamment le comportement après perturbation, le point est classé `NOT_DEFINED`, `TRACE_ONLY`, `CONDITIONAL` ou `DELEGATED`.

## 3. Frontières de propriété

- FT-STR : représentation, cohérence de snapshot et structures de registres ;
- FT-ACC : validité des accès, exceptions Modbus et absence d'effet de bord d'un accès rejeté ;
- FT-LIM : domaines simples et valeurs hors domaine ;
- FT-BLK : invariants internes ;
- FT-INT : effets et relations élémentaires entre blocs ;
- FT-CMD : moteur transactionnel B5, corrélation, idempotence, concurrence, résultats et préconditions des commandes ;
- FT-SEQ : chaînes métier nominales et refus → correction → reprise ;
- FT-PER : reboot, persistance et reprise après redémarrage.

FT-RBT ne possède que la composition de ces oracles sous perturbation lorsque cette composition reste déterministe.

## 4. Anti-fabrication

FT-RBT ne doit pas inventer :
- timeout Modbus client ou serveur ;
- temps maximal de réponse ;
- nombre maximal de retries ;
- stratégie de backoff ;
- débit maximal de requêtes ;
- ordre de traitement de requêtes simultanées ;
- comportement de trame CRC invalide, tronquée ou mal formée si V1 ne le définit pas ;
- durée ou profondeur de mémoire d'idempotence ;
- politique générique de récupération ou de resynchronisation ;
- comportement post-reboot, réservé à FT-PER.

## 5. Décomposition retenue

- FT-RBT-01 — Requêtes invalides et non-corruption ;
- FT-RBT-02 — Perte de réponse et retransmission transactionnelle ;
- FT-RBT-03 — Répétitions et sollicitations transactionnelles dégradées ;
- FT-RBT-04 — Lectures sous transition et sollicitations rapprochées ;
- FT-RBT-05 — Trames dégradées, timing et resynchronisation non spécifiés.

## 6. Critère de gel

Chaque exigence `COVERED` doit posséder un test réellement discriminant. Les propriétés déjà gelées doivent rester déléguées. Toute absence d'oracle V1 doit rester visible et ne doit pas être fermée par une bonne pratique industrielle implicite.
