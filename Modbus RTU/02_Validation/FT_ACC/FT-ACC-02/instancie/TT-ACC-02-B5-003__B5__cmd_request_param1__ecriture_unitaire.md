# TT-ACC-02-B5-003 — Bloc 5 — cmd_request_param1

## Cible
`5002`, `uint16`, RW.

## Précaution
Maintenir `submit=0`.

## Scénario
Lire, écrire une valeur de test avec FC06, vérifier l'absence d'exception, relire puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande.
