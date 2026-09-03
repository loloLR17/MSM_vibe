# FT-CMD V1.1 TRANSACTION-01 — Cas détaillés

Tous les cas sont fondés sur `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md`.

Tant que le mapping B5 V1.1 n'est pas gelé, les cas nécessitant les nouveaux champs/codes sont classés `PENDING_MAPPING`.

---

## TT-CMD-V11-T01-001 — Admission nominale dans l'epoch courante

Soumettre une requête neuve `(N,T)` avec N = epoch courante et T dans `1..65534`.

Oracle : la transaction est admise selon K1 ; son identité est `(N,T)`.

## TT-CMD-V11-T01-002 — Retry exact dans la même epoch

Rejouer exactement `(N,T,request)`.

Oracle : résultat historique/récupéré ; aucun second effet.

## TT-CMD-V11-T01-003 — Collision même identité, contenu différent

Rejouer `(N,T)` avec une identité de requête différente.

Oracle : collision ; aucun redispatch.

## TT-CMD-V11-T01-004 — Même txid dans une nouvelle epoch

Exécuter `(N,T)`, renouveler vers N+1, puis soumettre `(N+1,T)`.

Oracle : nouvelle identité transactionnelle ; la transaction de N+1 n'est pas un retry de N.

## TT-CMD-V11-T01-005 — Epoch zéro

Soumettre `transaction_epoch=0`.

Oracle : `TRANSACTION_EPOCH_INVALID` conceptuel ; aucun `RESERVED`.

## TT-CMD-V11-T01-006 — Ancienne epoch

Après passage N→N+1, soumettre une transaction ordinaire de N.

Oracle : `TRANSACTION_EPOCH_STALE` conceptuel ; aucun dispatch métier.

## TT-CMD-V11-T01-007 — Epoch inconnue

Soumettre une epoch distincte de la courante et non reconnue comme ancienne valide.

Oracle : `TRANSACTION_EPOCH_UNKNOWN` conceptuel ; aucun dispatch métier.

## TT-CMD-V11-T01-008 — Renouvellement nominal

Dans N, aucune autre transaction non terminale, soumettre `RENEW_TRANSACTION_EPOCH` avec txid 65535.

Oracle : une et une seule nouvelle epoch non nulle devient autoritative ; le renouvellement termine selon K1.

## TT-CMD-V11-T01-009 — Commande ordinaire avec txid 65535

Oracle : rejet ; aucun effet métier ; aucune consommation du renouvellement.

## TT-CMD-V11-T01-010 — RENEW avec txid différent de 65535

Oracle : rejet ; aucune nouvelle epoch.

## TT-CMD-V11-T01-011 — Renouvellement interdit par transaction non terminale

Créer une transaction `RESERVED` ou `STARTED` dans N puis demander le renouvellement.

Oracle : `TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED` conceptuel ; N reste courante.

## TT-CMD-V11-T01-012 — Retry du renouvellement après activation

Perdre la réponse après activation N+1 puis retransmettre exactement `(N,65535,RENEW...)`.

Oracle : aucun N+2 ; retry K1/finalisation de la même transition.

## TT-CMD-V11-T01-013 — Reboot capteur sans renouvellement

Rebooter avec N autoritative.

Oracle : N reste courante ; aucun nouveau namespace implicite.

## TT-CMD-V11-T01-014 — Perte de l'historique d'allocation côté centrale

Simuler une centrale connaissant N mais ne pouvant plus démontrer quels txid de N ont déjà été attribués.

Oracle côté centrale : ne pas choisir arbitrairement un txid ; obtenir une nouvelle frontière d'epoch avant de créer une nouvelle transaction.

## TT-CMD-V11-T01-015 — Deux allocateurs non coordonnés

Deux clients soumettent `(N,T)` avec deux requêtes différentes.

Oracle capteur : collision sûre, aucun double effet. Le protocole n'autorise pas cette situation comme mode nominal d'allocation.

## TT-CMD-V11-T01-016 — Namespace ordinaire consommé

Considérer tous les txid `1..65534` comme admis dans N.

Oracle : 65535 reste réservé au renouvellement ; aucun wrap implicite vers un ancien txid ordinaire.

## TT-CMD-V11-T01-017 — Requête historique avec substitution d'epoch

Prendre une ancienne requête `(N,T)` et tenter de la « rejouer » en remplaçant seulement N par N+1.

Oracle : ce n'est pas un retry. Une centrale conforme ne réalise pas cette substitution pour récupérer une transaction historique.

## TT-CMD-V11-T01-018 — Autorité d'epoch indisponible

Précondition interne : recovery `UNAVAILABLE`, `CORRUPTED`, `UNSUPPORTED` ou `INDETERMINATE`.

Oracle : aucune nouvelle transaction B5 n'est admise.

## TT-CMD-V11-T01-019 — Première initialisation

Précondition : `UNINITIALIZED` positivement démontré.

Oracle : une epoch non nulle est committed avant admission de la première transaction.

## TT-CMD-V11-T01-020 — Pas de reconstruction depuis le CommandJournal

Corrompre/perdre l'autorité d'epoch tout en laissant un journal contenant des transactions d'une epoch historique.

Oracle : le journal seul ne devient pas autorité de l'epoch courante et aucune valeur n'est inventée.

---

## Tests de power-loss associés

Les cas de crash E1..E7, reboots répétés et transition N→N+1 sont définis dans `RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md` et doivent être exécutés conjointement lors de la validation firmware.
