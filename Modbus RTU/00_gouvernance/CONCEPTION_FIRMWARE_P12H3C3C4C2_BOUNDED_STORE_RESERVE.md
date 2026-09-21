# P12-H3c3-C4C2 — Reserve du store borné V3

## Objet

Composer planner C4-B et writer C4-A dans CommandJournalBoundedStore::reserve, sans modifier le store V2 actif.

## Préconditions

reserve exige :
- store initialisé ;
- recovery_required == false ;
- request et entry non NULL ;
- transaction_id dans 1..65535 ;
- next_admission_order non nul et différent de UINT32_MAX.

Le refus lié à l'épuisement du compteur ne modifie pas le média.

## Décisions du planner

RETRY_EXISTING et COLLISION_EXISTING sont refusés par reserve avec TR2_ERROR_INVALID_STATE. La distinction reste exploitable en amont via find/identité comme dans le contrat CommandJournal actuel ; reserve ne réécrit jamais une entrée existante.

NO_CAPACITY retourne TR2_ERROR_UNAVAILABLE sans écrire et sans réarmer recovery_required : c'est une limite métier/capacité, pas une dégradation média.

ADMIT_EMPTY crée un record RESERVED :
- transaction_id et request_identity issus de la requête ;
- admission_order = next_admission_order ;
- completion_order = 0 ;
- generation physique gérée par writer_admit_empty.

EVICT_COMPLETED remplace la victime terminale :
- nouvelle transaction RESERVED ;
- nouvel admission_order = next_admission_order ;
- l'ancien admission_order de la victime ne doit pas être conservé.

Conséquence : writer_mutate ne convient pas à l'éviction car il impose la conservation de admission_order. C4-C2 doit donc introduire une primitive explicite de réadmission/éviction qui écrit la copie opposée avec generation+1 mais autorise un nouvel admission_order.

## Atomicité

Le compteur next_admission_order est incrémenté uniquement après write + commit réussis.

Toute erreur de write/commit réarme recovery_required, propage l'erreur originale et laisse le compteur runtime inchangé. Aucun retry automatique.

Après succès, entry reçoit l'entrée RESERVED persistée.

## Génération

Une victime generation == UINT32_MAX est exclue par le planner. La primitive de réadmission refuse également ce cas en défense en profondeur.

## Hors périmètre

set_recovery_context, mark_started et complete restent non implémentés jusqu'aux tranches suivantes.
