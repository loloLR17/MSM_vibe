# TT-ACC-02-B5-004 — Bloc 5 — cmd_request_param2

## Cible
`5003`, `uint16`, RW.

## Précaution
Maintenir `submit=0`.

## Scénario
Lire, écrire une valeur de test avec FC06, vérifier l'absence d'exception, relire puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande.
