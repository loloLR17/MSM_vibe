# TT-ACC-02-B5-001 — Bloc 5 — cmd_request_code

## Cible
`5000`, `uint16`, RW.

## Précaution
Maintenir `cmd_request_control.submit = 0` pendant tout le test : aucune commande ne doit être déclenchée.

## Scénario
Lire, écrire une valeur sûre avec FC06, vérifier l'absence d'exception, relire tant que `submit=0`, puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande.
