# P12-H3c3-C4C0 — Composition et recovery_required du store borné V3 isolé

## 1. Objet

Figer la machine d'état runtime du futur store borné V3 avant toute composition derrière CommandJournal.

Cette tranche ne remplace pas CommandJournalStore V2. Elle définit un nouveau store V3 isolé qui composera les briques déjà validées : recovery C2, reader C3, planner C4-B et writer C4-A.

## 2. Etat runtime minimal

Le store V3 isolé mémorise :
- PersistentStorageCore *storage ;
- bool initialized ;
- bool recovery_required ;
- uint32_t next_admission_order ;
- uint32_t next_completion_order ;
- CommandJournal journal.

Aucun max_transaction_id : tout uint16 non nul, donc 1..65535, est admissible.

## 3. Initialisation

Après init réussi :
- initialized = true ;
- recovery_required = true ;
- next_admission_order = 1 ;
- next_completion_order = 1 ;
- aucune opération métier CommandJournal n'est autorisée avant recovery réussi.

L'initialisation n'écrit rien sur le média.

## 4. Recovery

recover appelle exclusivement command_journal_bounded_recovery_scan().

### EMPTY ou VALID
Seulement après scan complet réussi :
- copier next_admission_order et next_completion_order reconstruits ;
- recovery_required = false ;
- rendre le journal utilisable.

### CORRUPTED, UNSUPPORTED ou UNAVAILABLE
- recovery_required reste true ;
- les compteurs runtime ne sont pas activés à partir d'un scan invalide ;
- aucune réparation, écriture ou éviction implicite.

Le résultat de classification est exposé au caller.

Un nouvel appel explicite à recover est le seul moyen de quitter recovery_required.

## 5. Garde commune des opérations métier

find, visit, latest_completed, reserve, set_recovery_context, mark_started et complete exigent :
- store initialisé ;
- recovery_required == false.

Si recovery_required == true : TR2_ERROR_INVALID_STATE.

Cette règle est volontairement plus stricte que le store dense historique pour les lectures.

## 6. Réarmement de recovery_required

Une opération qui observe une dégradation de l'image persistante après un recovery réussi doit réarmer recovery_required.

Sont des dégradations persistantes :
- TR2_ERROR_STORAGE ;
- TR2_ERROR_UNAVAILABLE ;
- TR2_ERROR_CORRUPTED ;
- TR2_ERROR_UNSUPPORTED ;
- toute erreur de write ;
- toute erreur de commit.

Après une telle erreur :
- recovery_required = true ;
- l'erreur originale est propagée ;
- aucune autre opération métier n'est autorisée avant recover explicite.

Ne réarment pas recovery_required :
- TR2_ERROR_NOT_FOUND ;
- TR2_ERROR_INVALID_ARGUMENT ;
- TR2_ERROR_INVALID_STATE dû à une transition métier refusée ;
- NO_CAPACITY du planner ;
- COLLISION_EXISTING ;
- erreur retournée par un visitor métier, dès lors qu'aucune anomalie persistante n'a été observée.

## 7. Ambiguïté write OK / commit NOK

Un commit en erreur après un write réussi est fail-closed :
- l'appel retourne l'erreur ;
- recovery_required = true ;
- les compteurs runtime ne sont pas avancés ;
- aucun retry automatique de l'écriture.

Le média peut contenir l'ancienne copie, la nouvelle copie, ou un état dépendant de sa sémantique de commit. Le prochain recover + sélection A/B est l'unique autorité.

## 8. Compteurs et atomicité runtime

next_admission_order et next_completion_order ne sont avancés qu'après succès complet write + commit.

Si persistance échoue :
- compteur inchangé ;
- recovery_required réarmé.

Leur valeur en mémoire pendant recovery_required n'est pas utilisable et sera reconstruite par recover.

Aucun wrap :
- next_admission_order == 0 ou next_completion_order == 0 est un état non exploitable ;
- le recovery C2 classe déjà UINT32_MAX comme UNSUPPORTED pour les ordres persistés maximaux ;
- les opérations futures refusent avant tout incrément qui provoquerait un wrap.

## 9. Lecture après recovery

find/visit/latest_completed utilisent le reader C3.

Toute anomalie média/format détectée par C3 est propagée et réarme recovery_required.

NOT_FOUND reste un résultat métier normal et ne réarme pas recovery_required.

## 10. Admission et mutations futures

reserve composera planner C4-B + writer C4-A.

Les mutations set_recovery_context, mark_started et complete retrouveront d'abord le record courant qualifié puis utiliseront writer_mutate.

Le store, et non le writer, possède :
- recovery_required ;
- next_admission_order ;
- next_completion_order ;
- la traduction des décisions planner vers le contrat CommandJournal.

## 11. Invariants B5 conservés

- un seul non-terminal autorisé par l'image qualifiée C2 ;
- retry même ID + même identité ne réexécute pas ;
- collision même ID + identité différente est refusée ;
- aucune retry automatique après erreur persistante ambiguë ;
- transaction_id indépendant du slot physique ;
- IDs 1..65535 admissibles.

## 12. Bascule

Le store V3 isolé sera testé intégralement avant toute modification du CommandJournalStore V2 actif.

La bascule V2 -> V3 reste une tranche ultérieure atomique.

## 13. Tests exigés par la composition

Au minimum :
- init -> recovery_required true ;
- toute opération métier avant recovery -> INVALID_STATE ;
- recovery EMPTY/VALID -> false + compteurs reconstruits ;
- recovery erreur -> reste true ;
- dégradation reader après recovery -> true ;
- write failure -> true, compteur inchangé ;
- commit failure -> true, compteur inchangé ;
- visitor error métier -> recovery_required reste false ;
- NOT_FOUND -> recovery_required reste false ;
- second recover après défaut requalifie entièrement l'image avant réouverture.
