# FT-SEQ-04 — Démarrage et établissement d'une campagne

## 1. Objet

Valider la chaîne nominale complète START : depuis un contexte satisfaisant les préconditions V1 jusqu'à une acquisition effectivement active accompagnée d'une nouvelle campagne B6 cohérente avec l'état en cours.

## 2. Résultat d'audit

La V1 définit suffisamment la chaîne :

`contexte START valide` → `START réussi` → `acquisition active` → `nouvelle campagne ouverte` → `campagne en cours cohérente`.

Un test séquentiel propriétaire est créé :
- `TT-SEQ-CAMP-001` — démarrage nominal et établissement d'une campagne.

## 3. Couverture

- `COVERED` propriétaire FT-SEQ : 1
- `CONDITIONAL` : 0
- `DELEGATED` : 5
- `TRACE_ONLY` : 0
- `NOT_DEFINED` : 4

## 4. Délégations

- préconditions, acceptation/refus et résultat START : FT-CMD-06 ;
- START → acquisition active, ouverture de campagne et cohérence inter-blocs : FT-INT-04 ;
- invariants internes de la campagne B6 : FT-BLK-05 ;
- structure, accès et domaines : FT-STR / FT-ACC / FT-LIM.

## 5. Limites V1 conservées

FT-SEQ-04 ne transforme pas en oracles :
- `B1.active_campaign_id == B6.campaign_id` ;
- `total_campaign_count` augmentant exactement de 1 dès la première lecture post-START ;
- un délai maximal START→campagne visible ;
- une priorité entre causes simultanées de refus START.

L'ouverture d'une nouvelle campagne reste néanmoins normative et doit être démontrée avec les mécanismes B6 disponibles sans s'appuyer exclusivement sur ces relations non définies.

## 6. Frontières

- refus START : FT-CMD-06 ;
- arrêt et clôture de campagne : FT-SEQ-05 ;
- cycle métier complet : FT-SEQ-06 ;
- refus puis correction/reprise : FT-SEQ-07 ;
- reboot/persistance : FT-PER ;
- robustesse hostile : FT-RBT.

Voir `source/FT-SEQ-04_source.md` et `detaille/TT-SEQ-CAMP-001.md`.