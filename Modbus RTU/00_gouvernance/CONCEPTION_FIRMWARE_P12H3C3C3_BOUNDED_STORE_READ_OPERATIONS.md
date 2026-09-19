# P12-H3c3-C3 — Opérations de lecture du store borné

## 1. Objet

Définir les invariants de find, visit et latest_completed du futur CommandJournalStore borné avant leur implémentation.

Ces opérations utilisent exclusivement les 256 slots logiques et la sélection A/B gelée en C1. Elles ne modifient aucun octet persistant.

## 2. Précondition commune

Le store doit être initialisé et son recovery global C2 doit avoir abouti à EMPTY ou VALID.

Si recovery_required est vrai, les opérations de lecture métier refusent l'accès avec TR2_ERROR_INVALID_STATE.

Cette règle empêche une lecture partielle d'une image non encore qualifiée ou qualifiée en erreur.

## 3. find(transaction_id)

- transaction_id 0 est invalide ;
- toute valeur 1..65535 est admissible ;
- aucun transaction_id n'est transformé en adresse physique ;
- les 256 slots logiques sont parcourus ;
- un slot EMPTY est ignoré ;
- un slot VALID portant l'identifiant demandé fournit l'entrée ;
- aucun match -> TR2_ERROR_NOT_FOUND.

Après un recovery C2 réussi, la présence de CORRUPTED, UNSUPPORTED ou UNAVAILABLE pendant une lecture ultérieure est une dégradation du média. L'opération propage une erreur et le store repasse en recovery_required afin d'interdire la poursuite sur une image dont la qualification n'est plus garantie.

## 4. visit

- parcourt les 256 slots logiques ;
- appelle le visitor exactement une fois pour chaque entrée courante VALID ;
- l'ordre de visite n'est pas contractuel ;
- EMPTY est ignoré ;
- l'erreur du visitor est propagée immédiatement ;
- aucune mutation persistante n'est effectuée.

Une anomalie de média/format détectée pendant le scan est propagée et force recovery_required.

## 5. latest_completed

- parcourt les 256 slots logiques ;
- ne considère que lifecycle == COMPLETED ;
- sélectionne l'entrée ayant le plus grand completion_order ;
- aucun COMPLETED -> TR2_ERROR_NOT_FOUND.

C2 garantit l'unicité des completion_order dans une image qualifiée. Une égalité observée après recovery constitue donc une dégradation/corruption et ne doit pas être arbitrée silencieusement.

## 6. Complexité

Chaque opération est bornée à 256 slots.

La complexité O(256) est indépendante de la valeur numérique du transaction_id et remplace le scan dense historique 1..max_transaction_id.

Aucun index persistant supplémentaire n'est introduit.

## 7. Compatibilité du contrat métier

Le contrat public CommandJournal n'est pas modifié :

- find ;
- latest_completed ;
- visit.

C3 remplace uniquement la stratégie physique de lecture.

Les règles B5 de retry/collision/idempotence restent hors de cette tranche et seront exercées lors de l'admission C4.

## 8. Tests minimaux C3

- find sur store vide -> NOT_FOUND ;
- find ID 1 ;
- find ID 65535 ;
- find d'un ID absent ;
- transaction_id 0 -> INVALID_ARGUMENT ;
- find indépendant du slot physique ;
- visit store vide -> zéro appel ;
- visit plusieurs entrées -> chacune exactement une fois ;
- erreur visitor propagée ;
- latest_completed sans COMPLETED -> NOT_FOUND ;
- latest_completed parmi plusieurs COMPLETED ;
- présence d'un non-terminal n'affecte pas latest_completed ;
- lecture refusée tant que recovery_required ;
- dégradation média après recovery -> erreur et recovery_required réarmé.

## 9. Hors périmètre

Restent hors C3 :

- reserve/admission ;
- sélection d'un slot libre ;
- éviction ;
- mutations A/B ;
- remplacement final de toutes les opérations du CommandJournalStore ;
- média persistant STM32 de production.
