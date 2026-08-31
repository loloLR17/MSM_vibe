# FT-BLK — Specifications de famille

## 1. Définition

FT-BLK valide la cohérence fonctionnelle interne de chaque bloc à partir des règles normatives V1, sans créer de second référentiel métier.

## 2. Principe de classement

Chaque règle normative intra-bloc est classée dans l'un des états suivants :

- `COVERED` : oracle V1 exécutable dans FT-BLK ;
- `CONDITIONAL` : oracle V1 présent mais moyen d'essai ou condition déterministe nécessaire ;
- `DELEGATED` : règle explicitement confiée à une autre famille ;
- `TRACE_ONLY` : règle tracée ici mais déjà couverte par la famille propriétaire ;
- `NOT_DEFINED` : la V1 ne fournit pas un oracle suffisamment déterministe.

## 3. Exclusions strictes

FT-BLK ne duplique pas :

- le structurel FT-STR ;
- les permissions FT-ACC ;
- les domaines et limites FT-LIM ;
- les dépendances croisées FT-INT ;
- le moteur de commandes FT-CMD ;
- les séquences FT-SEQ ;
- la robustesse FT-RBT ;
- la persistance FT-PER.

## 4. Format des cas détaillés

Chaque cas actif doit être traçable par un ID unique `TT-BLK-<scope>-<numéro>` et documenter au minimum : objectif, exigence/règle couverte, source normative, préconditions, entrées, étapes, résultat attendu, critère d'acceptation, mode d'exécution, automatisation, traces, criticité et limites/arbitrages.

Les séries numériques utilisées dans cette famille sont organisées pour éviter les collisions :

- série `001...` : principalement FT-BLK-01 ;
- série `101...` : principalement FT-BLK-02 ;
- série `201...` : FT-BLK-03 ;
- B4, B6 et B0 conservent leurs séries propres sans collision.

## 5. Critères de gel

La famille est gelable lorsque :

1. B0 à B7 ont une destination explicite ;
2. aucun ID de test n'est dupliqué ;
3. chaque exigence est couverte, conditionnelle, déléguée, tracée ou déclarée non définie ;
4. les matrices sont cohérentes avec les cas détaillés ;
5. aucune règle informative n'est promue au rang normatif ;
6. les dettes normatives sont consolidées ;
7. l'audit croisé avec charte de typage, mapping et familles gelées ne révèle pas de contradiction.
