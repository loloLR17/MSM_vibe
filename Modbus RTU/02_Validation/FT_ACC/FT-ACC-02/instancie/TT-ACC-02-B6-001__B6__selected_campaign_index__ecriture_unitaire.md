# TT-ACC-02-B6-001 — Bloc 6 — selected_campaign_index

## Cible
`6003`, `uint16`, RW.

## Précondition
Choisir un index compatible avec l'état réel de l'inventaire afin de ne pas confondre droit d'accès et validité fonctionnelle.

## Scénario
Lire la valeur initiale, écrire une valeur sûre avec FC06, vérifier l'absence d'exception, relire conformément à la sémantique V1, puis restaurer si nécessaire.

## Résultat attendu
Droit d'écriture confirmé ; validité de sélection hors FT-ACC-02.
