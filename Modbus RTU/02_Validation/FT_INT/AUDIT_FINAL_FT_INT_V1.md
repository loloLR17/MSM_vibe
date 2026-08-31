# Audit final FT-INT V1

## Objet

Clore l'audit industriel de la famille FT-INT après reconstruction des sous-familles FT-INT-01 à FT-INT-05.

## Contrôles réalisés

- inventaire consolidé des relations inter-blocs B0 à B7 ;
- cohérence des statuts avec le vocabulaire officiel ;
- vérification des frontières FT-STR / FT-ACC / FT-LIM / FT-BLK / FT-CMD / FT-SEQ / FT-RBT / FT-PER ;
- contrôle des identifiants de tests FT-INT ;
- contrôle des blocs réellement observés par les IDs ;
- conservation explicite des relations `NOT_DEFINED` et `TRACE_ONLY` ;
- vérification qu'aucun complément métier informatif n'est utilisé comme oracle V1.

## Corrections issues de la passe finale

1. FT-INT-03 : matrice CSV normalisée sur le schéma commun `requirement_id,scope,relation,status,test_id,owner,justification`.
2. FT-INT-04 : matrice CSV normalisée ; le scénario RAZ statistiques est renommé `TT-INT-B00B04B05B06-001` afin que son scope reflète B0, B4, B5 et B6 réellement observés.
3. FT-INT-05 : remplacement du statut non canonique `COVERED_CONDITIONAL` par `CONDITIONAL`.
4. Création de `MATRICE_COUVERTURE_FT_INT_V1.md` comme vue consolidée de la famille.

## Résultat

- 44 exigences/classifications inventoriées ;
- 22 tests FT-INT distincts ;
- aucune collision d'identifiant identifiée dans la famille ;
- aucun oracle supplémentaire justifiable n'a été trouvé sans inventer une relation absente de la V1 ;
- aucune duplication fonctionnelle volontaire avec les familles spécialisées n'est requise.

## Décision proposée

FT-INT est techniquement **candidate au gel V1**.

Le merge et le gel restent conditionnés à la validation explicite de la passe finale.