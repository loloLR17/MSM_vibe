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

## TRANSACTION-04 — RESERVED interrompu avant effet

### TT-CMD-V11-T01-026 — RESERVED récupéré, STARTED jamais franchi
Admettre `(N,T)`, rendre `RESERVED` durable, provoquer une coupure avant `STARTED`, puis fournir une preuve positive que `STARTED` n'a jamais été franchi et qu'aucun effet significatif n'a pu commencer.
Oracle : `COMPLETED`, `cmd_status=6`, `cmd_result_code=28 TRANSACTION_ABORTED_BEFORE_EFFECT`, aucun redispatch, identité consommée.

### TT-CMD-V11-T01-027 — Retry exact après 28
Retransmettre exactement `(N,T,request)` après terminalisation durable 6/28.
Oracle : restitution 6/28, aucun nouveau `RESERVED`, aucun nouvel effet.

### TT-CMD-V11-T01-028 — Collision après 28
Réutiliser `(N,T)` avec une requête canonique différente.
Oracle : `status=5`, `result=27`; la transaction originale 6/28 et `last` ne sont pas remplacés par la tentative conflictuelle.

### TT-CMD-V11-T01-029 — Projection active/last après 28
Observer avant puis après la barrière durable de terminalisation.
Oracle : avant barrière la transaction reste récupérable comme active ; après terminalisation durable `active=(0,0)` et la transaction peut devenir `last=(N,T)` avec `last_status_final=6`, `last_result_code=28`. Aucun timestamp historique n'est fabriqué.

## TRANSACTION-05 — STARTED avec absence d'effet prouvée

### TT-CMD-V11-T01-030 — STARTED + ABSENCE_PROVEN
Admettre `(N,T)`, franchir `STARTED` durablement, provoquer une coupure, puis établir par autorité métier `ABSENCE_PROVEN`.
Oracle : `COMPLETED`, `cmd_status=6`, `cmd_result_code=29 TRANSACTION_ABORTED_NO_EFFECT`, aucun redispatch, identité consommée ; result 28 interdit.

### TT-CMD-V11-T01-031 — Retry exact après 29
Retransmettre exactement la requête `(N,T)` après terminalisation durable 6/29.
Oracle : restitution 6/29, aucun nouvel effet.

### TT-CMD-V11-T01-032 — Collision après 29
Même identité `(N,T)` avec requête canonique différente.
Oracle : `status=5`, `result=27`; transaction 6/29 originale inchangée.

### TT-CMD-V11-T01-033 — Absence d'observation non suffisante
Après `STARTED` et reboot, ne disposer que d'une absence de trace, d'un timeout ou d'un état compatible avec plusieurs historiques.
Oracle : ne pas produire 29 ; si aucun effet terminal n'est prouvé, `cmd_status=9 RECOVERY_INDETERMINATE`, active conservée, admission bloquée.

### TT-CMD-V11-T01-034 — Second crash avant barrière 28/29
Interrompre le recovery après conclusion probatoire mais avant terminalisation durable.
Oracle : reprise du recovery sans redispatch métier ; aucune publication terminale non durable ne devient autorité.

### TT-CMD-V11-T01-035 — Second crash après barrière 28/29
Interrompre après terminalisation durable mais avant publication B5.
Oracle : résultat 6/28 ou 6/29 restauré depuis l'autorité durable ; active neutre ; aucun redispatch.

### TT-CMD-V11-T01-036 — Perte de la preuve ABSENCE_PROVEN avant terminalisation
Après `STARTED`, une observation provisoire suggère l'absence mais la preuve autoritative n'est plus disponible avant la barrière et aucun résultat probatoire fiable n'a été capturé.
Oracle : result 29 interdit ; chemin indéterminé si aucun autre résultat terminal n'est prouvé.

## TRANSACTION-06 — Validité de `cmd_last_timestamp`

### TT-CMD-V11-T01-037 — Terminalisation avec temps civil fiable
Terminaliser une transaction admise alors qu'un instant civil fiable de terminalisation est disponible.
Oracle : `last` terminal cohérent, `cmd_engine_flags.bit11 LAST_TIMESTAMP_VALID=1`, `cmd_last_timestamp` égal à l'instant civil fiable de terminalisation autoritative.

### TT-CMD-V11-T01-038 — Terminalisation sans temps civil fiable
Terminaliser une transaction admise sans instant civil fiable disponible.
Oracle : terminalité et résultat inchangés, `LAST_TIMESTAMP_VALID=0`, `cmd_last_timestamp=0x00000000`; aucune attente de synchronisation avant `COMPLETED`.

### TT-CMD-V11-T01-039 — Retry exact préserve la temporalité
Après un terminal avec timestamp valide puis après un terminal sans timestamp, effectuer un retry exact.
Oracle : même statut/résultat, même `LAST_TIMESTAMP_VALID`, même timestamp ; aucun nouveau `RESERVED` ni nouvelle terminalisation.

### TT-CMD-V11-T01-040 — État temporel courant indépendant du last
Faire varier B2 : temps valide/invalide, synchronisation, perte de synchronisation, nouvelle synchronisation.
Oracle : identité/status/result de `last`, `LAST_TIMESTAMP_VALID` et `cmd_last_timestamp` restent inchangés.

### TT-CMD-V11-T01-041 — Nouveau last sans timestamp remplace un last valide
Précharger un last A avec timestamp valide, puis terminaliser B sans temps civil fiable.
Oracle : last=B, `LAST_TIMESTAMP_VALID=0`, timestamp=0 ; le timestamp de A ne subsiste pas.

### TT-CMD-V11-T01-042 — Nouveau last valide remplace un last sans timestamp
Précharger A avec `LAST_TIMESTAMP_VALID=0`, puis terminaliser B avec temps fiable.
Oracle : last=B, `LAST_TIMESTAMP_VALID=1`, timestamp de B.

### TT-CMD-V11-T01-043 — Mailbox indépendante du last
Avec un last autoritatif, utiliser `clear_request_fields`, modifier la mailbox et préparer une nouvelle requête sans la terminaliser.
Oracle : toute la zone last, le timestamp et `LAST_TIMESTAMP_VALID` restent inchangés.

### TT-CMD-V11-T01-044 — Projection neutre sans LastCommandSnapshot
Boot/recovery sans snapshot terminal autoritatif reconstructible.
Oracle : code/txid/status/result/timestamp/last_epoch tous à zéro et `LAST_TIMESTAMP_VALID=0`.

### TT-CMD-V11-T01-045 — Cohérence intra-réponse Modbus
Pendant le remplacement de last A par B, lire dans une même réponse plusieurs champs de la zone last incluant timestamp/epoch/flags.
Oracle : snapshot A complet ou B complet, jamais mélange A/B ; MSW/LSW du timestamp issus du même uint32.

### TT-CMD-V11-T01-046 — Restauration après reboot
Récupérer successivement un terminal durable avec timestamp valide puis un terminal durable avec timestamp invalide.
Oracle : le premier restaure exactement le timestamp et valid=1 ; le second restaure valid=0/timestamp=0 même si le temps courant est devenu fiable.

### TT-CMD-V11-T01-047 — Corruption temporelle isolée
Conserver identité/status/result terminal fiables mais rendre la composante temporelle non authentifiable de façon isolable.
Oracle : conserver le last, dégrader uniquement la temporalité : valid=0, timestamp=0.

### TT-CMD-V11-T01-048 — Corruption globale du snapshot
Rendre impossible la preuve d'un snapshot cohérent identité/status/result.
Oracle : neutralisation complète de la zone last ; un timestamp éventuellement lisible ne suffit jamais à recréer le snapshot.

### TT-CMD-V11-T01-049 — Result 28 et timestamp de recovery
Terminaliser par recovery 6/28 successivement avec puis sans temps civil fiable à l'instant de terminalisation.
Oracle : si fiable, valid=1 et timestamp de terminalisation recovery ; sinon valid=0/timestamp=0. Le timestamp ne date ni le `RESERVED` ni le crash.

### TT-CMD-V11-T01-050 — Result 29 et timestamp de recovery
Terminaliser par recovery 6/29 successivement avec puis sans temps civil fiable.
Oracle : même règle que 28 ; la disponibilité temporelle ne change jamais la classification 6/29.

### TT-CMD-V11-T01-051 — Renewal terminal et last_epoch
Terminaliser le renouvellement `(N,65535)` après activation autoritative de N+1.
Oracle : `current_epoch=N+1`, `last_epoch=N`, txid=65535 ; timestamp/validité suivent les règles TRANSACTION-06 ordinaires.

## Tests de power-loss associés

Les scénarios J-EPOCH-01..34 et les frontières E1..E7 / R1..R4 sont définis dans `RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md`.

Les scénarios J-T06-01..24 du même document couvrent les frontières de terminalisation, reboot, corruption, retry, cohérence Modbus, results 28/29, renewal et projection neutre relatives à TRANSACTION-06.
