# TT-ACC-02-B4-011 — Bloc 4 — storage_limit_mb

## Cible
`4026..4027`, `uint32`, RW.

## Scénario
Sauvegarder, écrire le champ complet avec FC16, vérifier l'absence d'exception, relire et restaurer.

## Résultat attendu
Écriture complète autorisée ; validité métier hors FT-ACC-02.
