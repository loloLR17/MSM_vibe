# Freeze P12-H3c3-C4A — Writer V3 borné

## Statut

C4-A est gelée après validation locale Host et Cortex-M33.

Validation rapportée :
- 76/76 tests CTest verts ;
- HOST VALIDATED ;
- CROSS-BUILD VALIDATED ;
- HARDWARE PENDING.

## Périmètre gelé

Le writer V3 isolé fournit :
- admission dans un slot EMPTY ;
- mutation d'un slot VALID ;
- écriture de la copie opposée ;
- génération strictement croissante ;
- conservation de admission_order lors des mutations ;
- propagation des erreurs write/commit.

Le CommandJournalStore V2 actif n'est pas modifié.

## Invariants

Admission initiale :
- slot réellement EMPTY ;
- copy 0 ;
- generation = 1 ;
- admission_order non nul fourni par l'appelant.

Mutation :
- sélection courante VALID ;
- destination = copie opposée ;
- generation = current + 1 ;
- generation UINT32_MAX non mutable ;
- admission_order inchangé.

Défauts :
- write en erreur -> aucun commit ;
- commit en erreur -> erreur propagée ;
- aucune ancienne copie n'est effacée par le writer.

Un write physiquement réalisé suivi d'un commit en erreur est volontairement considéré ambigu au niveau de l'appel. La qualification après reboot relève de la sélection A/B et du recovery ; C4-A ne prétend pas annuler l'octet écrit ni garantir qu'un média dont commit échoue n'a rien persisté.

## Hors périmètre

- choix du slot d'admission ;
- politique d'éviction ;
- retry/collision B5 ;
- store borné complet ;
- bascule V2 -> V3 ;
- média STM32 de production ;
- validation matérielle.

## Suite

C4-B définit puis implémente le planner d'admission/éviction, sans écriture persistante.
