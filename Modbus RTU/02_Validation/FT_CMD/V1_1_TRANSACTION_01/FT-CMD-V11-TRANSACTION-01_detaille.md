# FT-CMD V1.1 TRANSACTION-01 — Cas détaillés

Statut des cas protocolairement mappés : `SPECIFIED_NOT_EXECUTED`.

## TT-CMD-V11-T01-001 — Admission nominale dans l'epoch courante
Soumettre une requête neuve `(N,T)`, `N=current_epoch`, `T∈1..65534`.
Oracle : admission K1, identité `(N,T)`.

## TT-CMD-V11-T01-002 — Retry exact
Rejouer exactement `(N,T,request)`.
Oracle : restitution/recovery de la transaction originale, aucun second effet, aucune nouvelle entrée `RESERVED`.

## TT-CMD-V11-T01-003 — Collision
Même `(N,T)` avec requête canonique différente.
Oracle : `cmd_status=5`, `cmd_result_code=27`, aucun redispatch, transaction originale inchangée, `last` inchangé par cette tentative.

## TT-CMD-V11-T01-004 — Même txid dans nouvelle epoch
Exécuter `(N,T)`, renouveler, puis `(N+1,T)`.
Oracle : deux identités différentes ; `(N+1,T)` est une nouvelle transaction.

## TT-CMD-V11-T01-005 — Epoch zéro
Soumettre `transaction_epoch=0`.
Oracle : `status=5`, `result=23`, aucun `RESERVED`, `last` inchangé.

## TT-CMD-V11-T01-006 — Epoch stale
Après N→N+1, soumettre une requête de N hors artefact spécial de renewal.
Oracle : `status=5`, `result=24`, aucun `RESERVED`, aucun effet métier.

## TT-CMD-V11-T01-007 — Epoch inconnue
Soumettre une epoch valide distincte de la courante et non reconnue stale.
Oracle : `status=5`, `result=25`, aucun `RESERVED`.

## TT-CMD-V11-T01-008 — Renouvellement nominal
Soumettre dans N la requête canonique `epoch=N, txid=65535, code=12, params=0, confirm=0`, sans autre transaction non terminale.
Oracle : une seule nouvelle epoch non nulle devient autoritative ; renouvellement K1 complet ; jamais N+2.

## TT-CMD-V11-T01-009 — Commande ordinaire avec txid 65535
Oracle : `status=5`, `result=2`, aucun `RESERVED`, aucun effet métier.

## TT-CMD-V11-T01-010 — RENEW avec txid !=65535
Oracle : `status=5`, `result=2`, aucune activation.

## TT-CMD-V11-T01-011 — Renouvellement interdit
Créer une autre transaction non terminale puis demander un renewal canonique.
Oracle : `status=5`, `result=26`, N reste courante, aucune N+1 préparée/activée.

## TT-CMD-V11-T01-012 — Retry renewal après activation
Perdre la réponse après activation N+1 puis retransmettre exactement `(N,65535,code12,0,0,0,0)`.
Oracle : retry de la même transition ; `current_epoch=N+1`; si la transaction n'est pas encore terminale, `active=(N,65535)` ; jamais N+2.

## TT-CMD-V11-T01-013 — Reboot capteur
Rebooter avec N autoritative.
Oracle : `epoch_status=VALID`, `current_epoch=N`; aucun nouveau namespace implicite.

## TT-CMD-V11-T01-014 — Perte allocation côté centrale
Centrale connaissant N mais ne pouvant plus prouver quels txid sont utilisés.
Oracle centrale : ne pas deviner ; obtenir une nouvelle frontière d'epoch avant toute nouvelle transaction.

## TT-CMD-V11-T01-015 — Deux allocateurs non coordonnés
Deux clients soumettent le même `(N,T)` avec contenus différents.
Oracle : collision sûre, résultat 27 pour la tentative conflictuelle, aucun double effet.

## TT-CMD-V11-T01-016 — Namespace ordinaire consommé
Tous les txid `1..65534` ont été admis dans N.
Oracle : aucun ancien txid n'est libéré ; 65535 reste réservé au renewal.

## TT-CMD-V11-T01-017 — Substitution d'epoch dans un retry historique
Remplacer N par N+1 dans une ancienne requête.
Oracle : ce n'est pas un retry ; une centrale conforme ne fait pas cette substitution pour récupérer l'historique.

## TT-CMD-V11-T01-018 — Autorité d'epoch non utilisable
Tester `CORRUPTED`, `UNAVAILABLE`, `UNSUPPORTED`, `INDETERMINATE`.
Oracle : `current_epoch=0`, `epoch_status` correspondant 2/3/4/5, aucune nouvelle admission B5.

## TT-CMD-V11-T01-019 — Première initialisation
Précondition `UNINITIALIZED` positivement prouvé.
Oracle : `epoch_status=0`, `current_epoch=0` jusqu'à activation durable ; ensuite `epoch_status=1`, epoch non nulle, puis seulement admission B5.

## TT-CMD-V11-T01-020 — Pas de reconstruction depuis CommandJournal
Perdre/corrompre l'autorité d'epoch tout en conservant un journal historique.
Oracle : le journal ne devient jamais autorité ; aucune epoch inventée.

## Cas transversaux mapping

### TT-CMD-V11-T01-021 — Canonicalisation renewal
Changer un champ inutilisé de la requête de renewal neuve.
Oracle : `status=5`, `result=2`, aucun `RESERVED`.

### TT-CMD-V11-T01-022 — Collision renewal
Même `(N,65535)` déjà connu mais contenu canonique différent.
Oracle : `status=5`, `result=27`, aucun N+1/N+2 supplémentaire.

### TT-CMD-V11-T01-023 — Clear mailbox après admission
Après capture/admission, activer `clear_request_fields`.
Oracle : mailbox à zéro y compris request_epoch ; transaction capturée, active/recovery/last et autorité epoch inchangés.

### TT-CMD-V11-T01-024 — Recovery transaction indéterminé
Récupérer une transaction dont l'effet ne peut être prouvé ni exclu.
Oracle : `cmd_status=9`, identité complète dans active, transaction non terminale, aucune nouvelle admission.

### TT-CMD-V11-T01-025 — Rejet pré-RESERVED ne remplace pas last
Précharger `last` avec une transaction terminale puis provoquer result 23/24/25/27.
Oracle : `last` reste inchangé.

## Tests de power-loss associés

Les scénarios J-EPOCH-01..25 et les frontières E1..E7 sont définis dans `RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md`.
