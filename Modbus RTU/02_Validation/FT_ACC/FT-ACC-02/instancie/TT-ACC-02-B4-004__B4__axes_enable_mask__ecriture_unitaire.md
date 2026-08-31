# TT-ACC-02-B4-004 — Bloc 4 — axes_enable_mask

## Cible
`4018`, `bitfield16`, RW.

## Scénario
Lire, écrire une valeur V1 sûre avec FC06, vérifier l'absence d'exception, relire et restaurer si nécessaire.

## Résultat attendu
Écriture autorisée. Les bits réservés et domaines métier sont traités hors FT-ACC-02.
