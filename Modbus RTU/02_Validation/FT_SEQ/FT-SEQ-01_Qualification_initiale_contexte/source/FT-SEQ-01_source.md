# FT-SEQ-01 — Référentiel source

## 1. Objet

Auditer la notion de qualification initiale d'un capteur par la centrale et déterminer quelles propriétés séquentielles sont réellement normatives en V1.

## 2. Sources

Sources normatives principales :

- `01_Specification_source/bloc0.md` ;
- `01_Specification_source/bloc1.md` ;
- `01_Specification_source/bloc2.md` ;
- `01_Specification_source/bloc4.md` ;
- `01_Specification_source/bloc5.md` ;
- `01_Specification_source/bloc6.md` ;
- `01_Specification_source/bloc7.md`.

Source de gouvernance :

- `02_Validation/plan_test_modbus_tr2_squelette.md`, qui cite la qualification initiale dans le périmètre FT-SEQ.

## 3. Inventaire des points de couverture

| ID | Objet | Classification | Propriétaire / justification |
|---|---|---|---|
| SEQ01-R01 | B0 permet à la centrale d'identifier le capteur et de vérifier la compatibilité protocolaire | DELEGATED | Sémantique B0 et lecture : FT-BLK / FT-ACC ; aucune séquence FT-SEQ autonome |
| SEQ01-R02 | B1 fournit l'état système et d'acquisition utile au contexte initial | DELEGATED | Invariants B1 : FT-BLK ; relations externes : FT-INT |
| SEQ01-R03 | B2 permet de connaître validité et état de synchronisation temporelle | DELEGATED | Sémantique B2 : FT-BLK ; synchronisation : FT-INT/FT-CMD/FT-SEQ-03 |
| SEQ01-R04 | B4 expose l'état de configuration et l'image active | DELEGATED | B4 : FT-BLK/FT-INT ; activation : FT-SEQ-02 |
| SEQ01-R05 | B5 expose l'état du moteur de commandes | DELEGATED | FT-CMD |
| SEQ01-R06 | B6 permet de consulter l'inventaire de campagnes | DELEGATED | FT-BLK/FT-ACC ; clôture/consultation séquentielle : FT-SEQ-05 |
| SEQ01-R07 | B7 fournit le diagnostic interne | DELEGATED | FT-BLK/FT-INT |
| SEQ01-R08 | La qualification initiale comme phase d'exploitation appartient au périmètre FT-SEQ | TRACE_ONLY | Définie par la gouvernance, sans oracle séquentiel supplémentaire dans les blocs V1 |
| SEQ01-R09 | Ordre obligatoire B0→B1→B2→B4→B5→B6→B7 | NOT_DEFINED | Aucun ordre de lecture initial n'est imposé par la V1 |
| SEQ01-R10 | Liste minimale obligatoire de blocs à lire avant toute commande | NOT_DEFINED | Aucune procédure de handshake initial normée |
| SEQ01-R11 | Cohérence atomique globale de toutes les données lues lors de transactions successives | NOT_DEFINED | Les snapshots sont définis au niveau des lectures/blocs concernés, pas comme snapshot système multi-transaction |
| SEQ01-R12 | Interdiction de toute commande avant qualification complète | NOT_DEFINED | Seules les préconditions propres aux commandes sont normatives |

## 4. Analyse

La V1 rend disponibles toutes les informations nécessaires à une centrale pour qualifier un capteur : identité, version protocolaire, état, temps, configuration, moteur de commandes, inventaire et diagnostic.

Cependant, elle ne transforme pas cette possibilité en protocole de découverte séquentiel obligatoire. Aucun ordre exhaustif de lecture, aucune liste minimale de blocs, aucun marqueur « qualification terminée » et aucune interdiction générale de commander avant cette qualification ne sont définis.

La qualification initiale doit donc rester une **composition d'exploitation traçable**, et non devenir artificiellement une machine d'états supplémentaire.

## 5. Conséquence pour les tests

FT-SEQ-01 ne crée pas de test PASS/FAIL propriétaire pour un handshake inexistant.

Les propriétés réellement testables sont déjà couvertes par les familles propriétaires. Un futur scénario E2E FT-SEQ-06 pourra utiliser une phase de qualification comme préambule pratique, sans lui attribuer un ordre normatif.

## 6. Décision de cadrage FT-SEQ-01

- `COVERED` propriétaire FT-SEQ : 0 ;
- `CONDITIONAL` : 0 ;
- `DELEGATED` : 7 ;
- `TRACE_ONLY` : 1 ;
- `NOT_DEFINED` : 4.

Cette absence de test autonome n'est pas un trou de couverture : elle matérialise l'absence d'un oracle séquentiel V1 distinct pour la qualification initiale.
