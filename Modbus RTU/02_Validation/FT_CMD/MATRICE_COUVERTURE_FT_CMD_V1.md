# FT-CMD — Matrice consolidée de couverture V1

## 1. Synthèse

Cette matrice consolide les sept sous-familles FT-CMD validées pour le Bloc 5 — moteur de commandes.

Répartition des 66 exigences / points de couverture :
- `COVERED` : 41 ;
- `CONDITIONAL` : 9 ;
- `NOT_DEFINED` : 9 ;
- `TRACE_ONLY` : 4 ;
- `DELEGATED` : 3.

Nombre de tests identifiés dans FT-CMD : 54.

Les classifications `NOT_DEFINED`, `TRACE_ONLY`, `CONDITIONAL` et `DELEGATED` ne constituent pas des échecs de validation : elles matérialisent respectivement une absence d'oracle V1, une traçabilité sans test autonome, une exécutabilité conditionnelle ou une propriété confiée à une autre famille.

## 2. FT-CMD-01 — Soumission et transaction

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD01-R01 | Déclenchement sur front montant | COVERED | TT-CMD-B05-001, 002 | Évaluation uniquement sur 0→1 |
| CMD01-R02 | Maintien `submit=1` sans réexécution | CONDITIONAL | TT-CMD-B05-003 | Auto-clear peut empêcher l'observation directe |
| CMD01-R03 | Auto-clear de `submit` | COVERED | TT-CMD-B05-004 | Règle normative finale du Bloc 5 |
| CMD01-R04 | Code commande non nul requis | COVERED | TT-CMD-B05-005 | Aucun code résultat spécifique inventé |
| CMD01-R05 | `transaction_id` obligatoire | TRACE_ONLY | — | Absence non représentée distinctement |
| CMD01-R06 | Domaine d'un `transaction_id` invalide | COVERED | TT-CMD-B05-007 | `0` invalide à la soumission ; `1..65535` valides ; refus fonctionnel code 14 |
| CMD01-R07 | Bits réservés de contrôle soumis | COVERED | TT-CMD-B05-006 | Refus fonctionnel code 2 |

## 3. FT-CMD-02 — Idempotence et corrélation

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD02-IDEM-001A | Réutilisation du résultat précédent | COVERED | TT-CMD-B05-100 | Rejeu immédiat |
| CMD02-IDEM-001B | Absence de seconde exécution | CONDITIONAL | TT-CMD-B05-101 | Effet discriminant ou instrumentation requis |
| CMD02-IDEM-002 | Nouvel ID = nouvelle transaction | COVERED | TT-CMD-B05-102 | Même code commande possible |
| CMD02-CORR-001 | Corrélation nominale stricte | COVERED | TT-CMD-B05-103 | Cas concurrent traité en FT-CMD-04 |
| CMD02-MEM-001 | Profondeur/durée de mémoire d'idempotence | NOT_DEFINED | — | Aucun N ni durée imposés |
| CMD02-PAYLOAD-001 | Même ID avec contenu différent | NOT_DEFINED | — | Aucun oracle détaillé V1 |

## 4. FT-CMD-03 — États, résultats et historique

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD03-001 | Valeurs et états finaux de `cmd_status` | COVERED | TT-CMD-B05-200, 201, 202 | Pas de séquence intermédiaire imposée |
| CMD03-002 | Séquence exhaustive reçu→accepté→en cours→final | NOT_DEFINED | — | Séquence d'usage centrale seulement recommandée |
| CMD03-003 | Domaine `cmd_result_code` 0..22 | COVERED | TT-CMD-B05-200, 201, 202 | Conditions métier dans 05..07 |
| CMD03-004 | Cohérence nominale `cmd_active_*` | COVERED | TT-CMD-B05-203 | Concurrence exclue |
| CMD03-005 | Historique dernière commande terminée | COVERED | TT-CMD-B05-204, 205 | Quel que soit le résultat |
| CMD03-006 | Timestamp de fin | COVERED | TT-CMD-B05-206 | Encodage délégué FT-STR |
| CMD03-007 | Sémantique exhaustive `cmd_result_detail` | NOT_DEFINED | — | Exemples seulement |

## 5. FT-CMD-04 — Concurrence et contrôles

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD04-R01 | Une seule commande active | COVERED | TT-CMD-B05-300, 301 | Preuve interne limitée aux observables |
| CMD04-R02 | Nouvelle commande pendant exécution → code 13 | COVERED | TT-CMD-B05-300 | Refus normatif |
| CMD04-R03 | Représentation `cmd_active_*` du refus concurrent | NOT_DEFINED | — | A ou B non précisé |
| CMD04-R04 | Existence `cancel_request` | TRACE_ONLY | — | Pas de cycle complet |
| CMD04-R05 | Flag annulation supportée bit9 | TRACE_ONLY | — | Pas de liste de commandes annulables |
| CMD04-R06 | Refus commande non annulable → code 15 | CONDITIONAL | TT-CMD-B05-302 | Cas concret à construire |
| CMD04-R07 | Succès d'annulation | NOT_DEFINED | — | État final/code succès absents |
| CMD04-R08 | Existence `clear_request_fields` | TRACE_ONLY | — | Bit défini |
| CMD04-R09 | Effet exact `clear_request_fields` | NOT_DEFINED | — | Portée/timing non définis |

## 6. FT-CMD-05 — Configuration et temps

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD05-R01 | APPLY CONFIG admissible | COVERED | TT-CMD-B05-400 | Effets Bloc 4 délégués FT-INT |
| CMD05-R02 | APPLY refusée acquisition en cours | COVERED | TT-CMD-B05-401 | Code 5 |
| CMD05-R03 | APPLY refusée config incomplète | COVERED | TT-CMD-B05-402 | Code 20 |
| CMD05-R04 | APPLY refusée config invalide / CRC incohérent | COVERED | TT-CMD-B05-403 | Code 4, pas de code CRC distinct |
| CMD05-R05 | APPLY refusée état incompatible distinct | CONDITIONAL | TT-CMD-B05-404 | Cas distinct à construire |
| CMD05-R06 | SYNC TIME admissible | COVERED | TT-CMD-B05-405 | Effets Bloc 2 délégués FT-INT |
| CMD05-R07 | SYNC sans temps préparé | COVERED | TT-CMD-B05-406 | Code 19 |
| CMD05-R08 | SYNC horloge indisponible | COVERED | TT-CMD-B05-407 | Code 12 ; constructibilité pratique potentiellement conditionnelle |

## 7. FT-CMD-06 — Acquisition et diagnostic

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD06-START-001 | START nominal | COVERED | TT-CMD-B05-500 | Effet acquisition FT-INT |
| CMD06-START-002 | START sans config active valide | COVERED | TT-CMD-B05-501 | Code 22 |
| CMD06-START-003 | START SD absente | COVERED | TT-CMD-B05-502 | Code 6 |
| CMD06-START-004 | START mémoire insuffisante | COVERED | TT-CMD-B05-503 | Code 7 |
| CMD06-START-005 | START défaut critique actif | COVERED | TT-CMD-B05-504 | Code 8 |
| CMD06-START-006 | START acquisition déjà active / état incompatible | COVERED | TT-CMD-B05-505 | Code 3 |
| CMD06-START-007 | Priorité entre causes simultanées START | NOT_DEFINED | TT-CMD-B05-506 | Ne pas fabriquer de priorité |
| CMD06-STOP-001 | STOP nominal | COVERED | TT-CMD-B05-507 | Effets fermeture FT-INT |
| CMD06-STOP-002 | STOP acquisition non active | COVERED | TT-CMD-B05-508 | Code 21 |
| CMD06-SELF-001 | SELFTEST standard | COVERED | TT-CMD-B05-509 | Publication Bloc 7 FT-INT |
| CMD06-SELF-002 | SELFTEST échec | CONDITIONAL | TT-CMD-B05-510 | Injection déterministe requise |
| CMD06-SELF-003 | SELFTEST timeout | CONDITIONAL | TT-CMD-B05-511 | Injection déterministe requise |
| CMD06-SELF-004 | Masque sous-tests non nul | CONDITIONAL | — | Extension uniquement si implémentée |
| CMD06-ACK-001 | ACK nominal | COVERED | TT-CMD-B05-512 | Effet sur cause FT-INT |
| CMD06-ACK-002 | ACK défaut non acquittable | COVERED | TT-CMD-B05-513 | Code 16 |
| CMD06-ACK-003 | ACK paramètre invalide | CONDITIONAL | TT-CMD-B05-514 | Domaine concret non exhaustif |
| CMD06-REF-001 | REFRESH nominal | COVERED | TT-CMD-B05-515 | Périmètre exact non défini |
| CMD06-REF-002 | REFRESH ne modifie pas la configuration | DELEGATED | — | FT-INT |

## 8. FT-CMD-07 — Maintenance et commandes protégées

| ID | Objet | Classification | Tests | Remarque |
|---|---|---|---|---|
| CMD07-R01 | ENTER MAINTENANCE transactionnel | COVERED | TT-CMD-B05-600 | Acquisition arrêtée seulement recommandée |
| CMD07-R02 | EXIT MAINTENANCE transactionnel | COVERED | TT-CMD-B05-601 | Effet de mode FT-INT |
| CMD07-R03 | Protection limitée aux commandes 10 et 11 | COVERED | TT-CMD-B05-608 | Pas de code 9 pour 1..9 faute de clé |
| CMD07-R04 | Clé valide `0xA55A` | COVERED | TT-CMD-B05-603, 607 | Confirmation valide |
| CMD07-R05 | Commande protégée sans confirmation | COVERED | TT-CMD-B05-602, 606 | Code 9 |
| CMD07-R06 | RESET SOFTWARE / acquisition arrêtée | COVERED | TT-CMD-B05-603, 604 | Acquisition active → code 5 |
| CMD07-R07 | RESET SOFTWARE sans opération critique active | CONDITIONAL | TT-CMD-B05-605 | Définition et code exact non définis |
| CMD07-R08 | RESET SOFTWARE après reboot | DELEGATED | — | FT-PER |
| CMD07-R09 | RESET STATISTICS transaction protégée | COVERED | TT-CMD-B05-606, 607 | Clé obligatoire |
| CMD07-R10 | Non-effets RESET STATISTICS | DELEGATED | — | FT-INT |
| CMD07-R11 | Portée statistiques / `param1` futur | NOT_DEFINED | — | Liste/masque non définis |

## 9. Conclusion de couverture

Toutes les règles FT-CMD V1 disposant d'un oracle explicite sont couvertes par un test ou, lorsque l'exécution dépend d'un moyen de banc non garanti, sont classées `CONDITIONAL`.

Aucune zone non définie n'a été convertie artificiellement en exigence. Les propriétés appartenant à FT-INT, FT-STR, FT-ACC, FT-LIM, FT-RBT ou FT-PER restent déléguées à leur famille propriétaire.
