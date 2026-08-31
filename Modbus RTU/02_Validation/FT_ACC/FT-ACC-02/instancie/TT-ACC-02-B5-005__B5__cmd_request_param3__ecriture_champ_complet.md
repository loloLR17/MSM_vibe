# TT-ACC-02-B5-005 — Bloc 5 — cmd_request_param3

## Cible
`5004..5005`, `uint32`, RW.

## Précaution
Maintenir `submit=0`.

## Scénario
Sauvegarder le champ, écrire les deux registres avec FC16, vérifier l'absence d'exception, relire le `uint32` complet puis restaurer.

## Résultat attendu
Droit d'écriture confirmé sans exécution de commande ; aucune exigence d'écriture partielle.
