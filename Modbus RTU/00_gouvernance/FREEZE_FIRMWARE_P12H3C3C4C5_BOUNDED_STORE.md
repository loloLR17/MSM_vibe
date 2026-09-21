# FREEZE — P12-H3c3-C4C5 — Command Journal borné V3

## Statut

Tranche C4-C5 gelée après validation locale utilisateur.

Validation déclarée :
- CTest : 78/78 verts ;
- HOST VALIDATED ;
- CROSS-BUILD VALIDATED ;
- HARDWARE PENDING.

Ce gel porte sur le store V3 borné isolé. Il ne constitue pas encore la bascule du store V2 actif.

## Périmètre gelé

Le CommandJournal borné V3 compose :
- codec/record V3 ;
- sélection physique A/B ;
- recovery scan C2 ;
- reader C3 ;
- writer/mutator C4-A ;
- planner d'admission C4-B ;
- store borné C4-C.

Capacité logique : 256 transactions retenues.
Géométrie persistante : 256 slots x 2 copies x 70 octets = 35 840 octets.

## Cycle métier couvert

Le store expose le contrat CommandJournal complet :
- find ;
- visit ;
- latest_completed ;
- reserve ;
- set_recovery_context ;
- mark_started ;
- complete.

Cycle :
RESERVED -> STARTED -> COMPLETED, avec recovery_context optionnel avant STARTED.

## Admission et rétention

- transaction_id reste uint16, 1..65535 ;
- l'adresse physique est indépendante du transaction_id ;
- même ID + même identité encore retenue : retry existant, aucune nouvelle admission ;
- même ID + identité différente encore retenue : collision, aucune nouvelle admission ;
- slot vide disponible : première position vide déterministe ;
- store plein : éviction du COMPLETED ayant le plus petit admission_order ;
- RESERVED et STARTED ne sont jamais évincés ;
- une victime generation UINT32_MAX n'est pas mutable et n'est pas éligible ;
- absence de capacité éligible : refus sans altération média.

## Persistance A/B

- admission vide : copie 0, generation 1 ;
- mutation : copie opposée, generation + 1, admission_order conservé ;
- réadmission après éviction : copie opposée, generation + 1, nouvel admission_order ;
- aucune stratégie erase-first ;
- write puis commit ;
- write/commit en erreur : erreur propagée, recovery_required réarmé, aucun retry automatique.

## Compteurs

next_admission_order et next_completion_order sont reconstruits par recovery.
Ils avancent uniquement après persistance réussie.
Aucun wrap silencieux n'est autorisé.

## Recovery

Après init, recovery_required est vrai et les opérations métier sont bloquées.
Seuls les résultats recovery EMPTY ou VALID ouvrent le store.
UNAVAILABLE, UNSUPPORTED et CORRUPTED maintiennent le store fermé.
Le recovery est read-only et ne répare pas automatiquement le média.

## Provenance des erreurs visit

Une erreur retournée par le visitor métier est propagée sans réarmer recovery_required, y compris si sa valeur est STORAGE, UNAVAILABLE, CORRUPTED ou UNSUPPORTED.
Seules les erreurs issues de la lecture/sélection persistante déclenchent la politique de recovery.

## Validation couverte

Les tests couvrent notamment :
- blocage avant recovery ;
- recovery EMPTY et VALID ;
- reconstruction des compteurs ;
- IDs indépendants des slots, dont 65535 ;
- lecture, visite et latest_completed ;
- admission vide ;
- retry/collision sans écriture ;
- 256 slots pleins et éviction du plus ancien COMPLETED ;
- réadmission avec continuité A/B ;
- recovery_context ;
- RESERVED -> STARTED ;
- completion et résultat terminal ;
- write/commit failures et fail-closed ;
- reboot/recovery après completion ;
- provenance des erreurs visitor.

## Hors périmètre du gel

Restent explicitement hors C4-C5 :
- bascule atomique V2 -> V3 dans le runtime actif (C4-E) ;
- choix du média persistant STM32 de production ;
- validation sur matériel réel ;
- autres points de revue P12 non liés directement au store borné.

## Règle pour C4-E

La bascule devra être atomique : aucune composition hybride V2/V3.
Le store V2 reste la référence active tant que C4-E n'est pas implémenté et validé.
