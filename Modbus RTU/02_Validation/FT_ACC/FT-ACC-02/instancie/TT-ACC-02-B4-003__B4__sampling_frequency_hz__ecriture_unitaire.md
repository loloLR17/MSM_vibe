# TT-ACC-02-B4-003 — Bloc 4 — sampling_frequency_hz

## Cible
`4016`, `uint16`, RW.

## Scénario
Lire la valeur initiale, écrire une valeur V1 sûre avec FC06, vérifier l'absence d'exception, relire et restaurer si nécessaire.

## Résultat attendu
Écriture autorisée et prise en compte. La validation métier de la fréquence est hors FT-ACC-02.
