# TT-ACC-02-B5-002 — Bloc 5 — cmd_request_transaction_id

## Cible
`5001`, `uint16`, RW.

## Précaution
Maintenir `submit=0`.

## Scénario
Lire, écrire une valeur de transaction de test sans soumettre de commande, vérifier l'absence d'exception, relire puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande.
