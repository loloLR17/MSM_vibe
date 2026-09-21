# Freeze P12-H3c3-C4B — Planner d'admission et d'éviction V3

## Statut

C4-B est gelée après validation locale Host et Cortex-M33.

Validation rapportée :
- 77/77 tests CTest verts ;
- HOST VALIDATED ;
- CROSS-BUILD VALIDATED ;
- HARDWARE PENDING.

## Périmètre gelé

Le planner V3 est read-only et produit :
- RETRY_EXISTING ;
- COLLISION_EXISTING ;
- ADMIT_EMPTY ;
- EVICT_COMPLETED ;
- NO_CAPACITY.

Il ne modifie aucun octet persistant, ne commit rien, n'incrémente aucun compteur et n'attribue aucun admission_order.

## Politique gelée

Priorité :
1. transaction_id déjà présent :
   - même request_identity -> RETRY_EXISTING ;
   - identité différente -> COLLISION_EXISTING ;
2. identifiant absent et slot EMPTY -> premier slot EMPTY d'indice croissant ;
3. journal plein -> COMPLETED mutable au plus petit admission_order ;
4. aucune victime admissible -> NO_CAPACITY.

Ne sont jamais victimes :
- RESERVED ;
- STARTED ;
- tout slot dont generation == UINT32_MAX.

transaction_id ne participe jamais à l'adressage physique.

## Validation

Les tests couvrent notamment :
- store vide ;
- choix déterministe du premier EMPTY ;
- retry et collision ;
- ID 65535 dans un slot arbitraire ;
- 256 slots occupés et sélection d'une victime ;
- non-terminaux non évincés ;
- génération maximale non réutilisable ;
- absence de capacité ;
- identifiant 0 invalide ;
- défaillance de lecture média.

Une fixture initiale créait deux admission_order identiques dans le scénario d'éviction. Elle a été corrigée sans modification de l'algorithme afin de respecter l'invariant d'unicité garanti par C2.

## Précondition

Le planner est destiné à une image V3 préalablement qualifiée par le recovery C2. Les anomalies média/format rencontrées pendant le scan sont propagées comme erreurs et ne sont jamais converties en NO_CAPACITY.

## Hors périmètre

- attribution de next_admission_order ;
- exécution physique de l'admission/éviction ;
- mutations métier du journal ;
- store borné complet ;
- bascule V2 -> V3 ;
- média STM32 de production ;
- validation matérielle.

## Suite

C4-C compose les briques V3 derrière un store borné isolé, sans remplacer le CommandJournalStore V2 actif.
