# TT-ACC-02-B4-009 — Bloc 4 — campaign_duration_s

## Cible
`4023..4024`, `uint32`, RW.

## Scénario
Sauvegarder la valeur, écrire le champ complet avec FC16, vérifier l'absence d'exception, relire le `uint32` complet et restaurer si nécessaire.

## Résultat attendu
Écriture complète autorisée. Aucune exigence d'écriture partielle n'est créée.
