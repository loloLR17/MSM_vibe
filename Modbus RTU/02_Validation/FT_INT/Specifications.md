# FT-INT — Spécifications de validation inter-blocs

## 1. Référentiel

Le référentiel normatif est constitué de la spécification Modbus RTU V1 (`01_Specification_source/bloc0.md` à `bloc7.md`, charte de typage et mapping unifié) et de la gouvernance de validation en vigueur.

Les compléments explicitement informatifs ne sont pas transformés en exigences de test.

## 2. Principe d'admission d'une exigence

Une exigence est admise dans FT-INT lorsque :
1. au moins deux blocs Modbus distincts sont nécessaires à son observation ;
2. la V1 établit explicitement la relation fonctionnelle ;
3. la relation n'est pas déjà entièrement validée par une famille plus spécialisée ;
4. l'oracle ne dépend pas d'une convention inventée ou d'une intention seulement implicite.

## 3. Doctrine d'oracle

FT-INT privilégie les invariants observables et bornés. Une relation logique mais non spécifiée est classée `NOT_DEFINED`. Une base commune ou une corrélation sans tolérance normative suffisante reste `TRACE_ONLY` ou `CONDITIONAL`.

Les tests ne doivent pas imposer d'égalité exacte entre des valeurs temporelles lues lors de transactions Modbus distinctes lorsque le temps peut légitimement progresser entre les lectures.

## 4. Commandes du Bloc 5

Lorsqu'une commande B5 sert de stimulus, FT-INT suppose que la commande a été exécutée avec succès selon le protocole B5. Le verdict FT-INT porte uniquement sur l'effet inter-blocs attendu.

Les mécanismes `submit`, `transaction_id`, acceptation/refus, idempotence, états du moteur et codes résultat restent sous responsabilité FT-CMD.

## 5. Traçabilité

Chaque exigence normalisée doit comporter :
- un identifiant FT-INT ;
- sa ou ses sources V1 ;
- les blocs impliqués ;
- sa classification ;
- son propriétaire de validation ;
- un test associé lorsqu'elle est `COVERED` ;
- une justification explicite lorsqu'elle est `CONDITIONAL`, `DELEGATED`, `TRACE_ONLY` ou `NOT_DEFINED`.

## 6. Critères de gel de la famille

Le gel FT-INT V1 exige notamment :
- inventaire complet des relations normatives inter-blocs B0 à B7 ;
- propriétaire unique pour chaque exigence ;
- au moins un test pour chaque exigence `COVERED` ;
- absence de duplication avec FT-STR, FT-ACC, FT-LIM et FT-BLK ;
- frontière documentée avec FT-CMD, FT-SEQ, FT-RBT et FT-PER ;
- absence d'oracle construit à partir d'une relation seulement implicite ;
- audit final des identifiants et de la matrice consolidée ;
- validation explicite avant merge et gel.
