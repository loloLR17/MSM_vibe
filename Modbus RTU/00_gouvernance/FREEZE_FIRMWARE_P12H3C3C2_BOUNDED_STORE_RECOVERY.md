# Freeze firmware — P12-H3c3-C2 — Recovery global du store borné

## 1. Statut

La tranche P12-H3c3-C2 est gelée.

Elle définit et implémente le scan global de recovery du futur journal B5 borné à 256 transactions persistantes, au-dessus de la sélection A/B gelée en C1.

Validation acquise :

- validation Host : OK ;
- tests Host : 74/74 OK ;
- cross-build Cortex-M33 : OK ;
- validation matérielle : HARDWARE PENDING.

## 2. Périmètre gelé

Le scanner parcourt les 256 slots logiques et ne modifie pas le média.

Classification globale, par priorité décroissante :

1. UNAVAILABLE ;
2. UNSUPPORTED ;
3. CORRUPTED ;
4. VALID ou EMPTY.

Le recovery est fail-closed : une anomalie empêche toute qualification VALID/EMPTY.

## 3. Invariants structurels gelés

Sur les records V3 sélectionnés VALID :

- les transaction_id doivent être uniques ;
- les admission_order doivent être uniques ;
- les completion_order non nuls des transactions COMPLETED doivent être uniques ;
- au plus une transaction peut être non terminale (RESERVED ou STARTED).

Toute violation donne CORRUPTED.

Les transaction_id 1 et 65535 restent admissibles indépendamment du slot physique.

## 4. Reconstruction des compteurs

Le scanner reconstruit :

- known_transaction_count ;
- next_admission_order ;
- next_completion_order.

Image vide :

- known_transaction_count = 0 ;
- next_admission_order = 1 ;
- next_completion_order = 1.

Image valide non vide :

- next_admission_order = max(admission_order) + 1 ;
- next_completion_order = max(completion_order des COMPLETED) + 1, ou 1 s'il n'existe aucun COMPLETED.

Aucun wrap n'est autorisé :

- max admission_order == UINT32_MAX -> UNSUPPORTED ;
- max completion_order == UINT32_MAX -> UNSUPPORTED.

## 5. Génération A/B

La génération reste locale au slot physique.

Une copie sélectionnée avec generation == UINT32_MAX reste récupérable et peut participer à une image globale VALID. Sa mutation ultérieure est interdite ; cette politique appartient aux tranches d'écriture suivantes.

## 6. Empreinte mémoire du scanner

La détection des doublons de transaction_id est bornée au nombre maximal de slots logiques.

Le scanner ne réserve pas une table indexée par les 65535 identifiants possibles.

Il utilise des tableaux bornés à 256 entrées et une recherche bornée O(256²), cohérente avec la fenêtre persistante gelée.

## 7. Preuves de test acquises

Les tests couvrent notamment :

- 256 slots vides ;
- reconstruction des compteurs ;
- transaction_id 1 et 65535 dans des slots arbitraires ;
- un RESERVED unique ;
- un STARTED unique ;
- plusieurs non-terminaux -> CORRUPTED ;
- transaction_id dupliqué -> CORRUPTED ;
- admission_order dupliqué -> CORRUPTED ;
- completion_order dupliqué -> CORRUPTED ;
- slot CORRUPTED ;
- slot UNSUPPORTED ;
- média UNAVAILABLE ;
- priorité UNSUPPORTED > CORRUPTED ;
- priorité UNAVAILABLE > UNSUPPORTED > CORRUPTED ;
- saturation admission_order -> UNSUPPORTED ;
- saturation completion_order -> UNSUPPORTED ;
- generation == UINT32_MAX récupérable.

## 8. Éléments explicitement non gelés par C2

C2 ne remplace pas encore le CommandJournalStore dense V2.

Restent hors périmètre :

- find du store borné ;
- visit du store borné ;
- latest_completed du store borné ;
- admission d'une transaction ;
- choix du slot libre ;
- éviction du plus ancien COMPLETED ;
- mutations A/B ;
- gestion d'un slot non mutable lors d'une admission/éviction ;
- migration finale du CommandJournalStore ;
- choix du média persistant STM32 de production.

La prochaine tranche est P12-H3c3-C3 : opérations de lecture du store borné (find / visit / latest_completed).
