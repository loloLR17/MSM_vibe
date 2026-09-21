# P12-H3c3-C4B — Planner d'admission et d'éviction du journal borné

## 1. Objet

Définir une décision d'admission pure, sans écriture persistante, pour la fenêtre de 256 transactions gelée en H3c1.

Le planner parcourt les slots logiques V3 qualifiés et produit une décision. Il ne modifie ni le média ni le CommandJournalStore V2 actif.

## 2. Entrée

- image persistante V3 déjà qualifiée par recovery ;
- transaction_id demandé, 1..65535 ;
- identité complète de la requête B5.

Le planner ne doit être utilisé que sur une image EMPTY ou VALID issue du recovery. La composition du futur store garantira cette précondition.

## 3. Priorité des décisions

### 3.1 Identifiant déjà présent

Si transaction_id est trouvé :
- même request_identity -> RETRY_EXISTING ;
- request_identity différente -> COLLISION_EXISTING.

Aucune recherche de slot libre ou victime n'est poursuivie.

### 3.2 Nouvelle transaction avec slot EMPTY

Si l'identifiant est absent et qu'au moins un slot EMPTY existe :
- choisir le premier slot EMPTY d'indice logique croissant ;
- décision ADMIT_EMPTY.

Ce choix déterministe n'est pas une règle métier B5 ; il simplifie la reproductibilité du stockage.

### 3.3 Journal plein

Si aucun slot EMPTY :
- ne considérer comme victime que lifecycle == COMPLETED ;
- choisir le plus petit admission_order, donc le COMPLETED le plus ancien ;
- exclure tout slot dont la génération courante vaut UINT32_MAX car il n'est plus mutable ;
- RESERVED et STARTED ne sont jamais évincés.

Décision : EVICT_COMPLETED.

Si aucune victime mutable n'existe : NO_CAPACITY.

## 4. Invariants

- transaction_id ne participe jamais à l'adresse physique ;
- l'éviction est fondée sur admission_order, jamais completion_order ;
- une transaction non terminale n'est jamais victime ;
- le planner n'incrémente aucun compteur ;
- le planner n'attribue pas le nouvel admission_order ;
- le planner n'écrit rien et ne commit rien ;
- une anomalie média/format rencontrée pendant le scan est une erreur, pas une décision de capacité.

## 5. Cohérence avec H3c1

La 257e transaction distincte évince le plus ancien COMPLETED.

Une transaction dont l'identifiant a disparu de la fenêtre après éviction peut être admise comme nouvelle transaction.

Une collision n'est définie que tant que l'identifiant est encore présent dans la fenêtre.

## 6. Résultat prévu

Le contrat C4-B exposera une structure contenant :
- kind : RETRY_EXISTING, COLLISION_EXISTING, ADMIT_EMPTY, EVICT_COMPLETED, NO_CAPACITY ;
- logical_slot lorsqu'une entrée existante ou une cible existe ;
- current record/selection lorsqu'il faut réutiliser ou évincer un slot VALID.

## 7. Tests minimaux

- store vide -> ADMIT_EMPTY slot 0 ;
- slot vide déterministe parmi plusieurs ;
- ID existant + même identité -> RETRY_EXISTING ;
- ID existant + identité différente -> COLLISION_EXISTING ;
- journal plein avec COMPLETED -> plus petit admission_order ;
- RESERVED/STARTED jamais victimes ;
- plus ancien COMPLETED à generation UINT32_MAX ignoré au profit du suivant mutable ;
- aucun COMPLETED mutable -> NO_CAPACITY ;
- ID 1 et 65535 indépendants des slots ;
- anomalie CORRUPTED/UNSUPPORTED/UNAVAILABLE propagée.

## 8. Hors périmètre

- écriture de la nouvelle admission ;
- attribution de next_admission_order ;
- mutation métier ;
- bascule du CommandJournalStore actif.
