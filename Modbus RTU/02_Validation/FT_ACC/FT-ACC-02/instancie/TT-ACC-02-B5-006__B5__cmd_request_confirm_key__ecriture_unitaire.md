# TT-ACC-02-B5-006 — Bloc 5 — cmd_request_confirm_key

## Cible
`5006`, `uint16`, RW.

## Précaution
Maintenir `submit=0` afin qu'une clé écrite ne puisse déclencher aucune commande.

## Scénario
Lire, écrire une valeur sûre avec FC06, vérifier l'absence d'exception, relire puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande.
