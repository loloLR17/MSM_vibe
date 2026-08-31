# TT-ACC-02-B5-007 — Bloc 5 — cmd_request_control

## Cible
`5007`, `bitfield16`, RW.

## Règle V1 critique
La commande est prise en compte sur front montant de `submit` et le firmware remet automatiquement `submit` à `0` après prise en compte. FT-ACC-02 ne doit donc jamais exiger la persistance d'un `submit=1` relu.

## Scénario nominal FT-ACC-02
1. Vérifier qu'aucune soumission n'est en cours et que `submit=0`.
2. Écrire `0x0000` avec FC06.
3. Vérifier l'absence d'exception Modbus.
4. Relire le registre et vérifier qu'aucune commande n'a été déclenchée.

## Résultat attendu
Droit d'écriture confirmé avec une valeur non déclenchante. Les scénarios `submit/cancel/clear` relèvent de la validation fonctionnelle du Bloc 5.
