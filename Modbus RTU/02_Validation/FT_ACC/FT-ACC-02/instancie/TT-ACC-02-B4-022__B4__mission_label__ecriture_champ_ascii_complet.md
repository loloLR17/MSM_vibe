# TT-ACC-02-B4-022 — Bloc 4 — mission_label

## Cible
`4076..4091`, `ASCII fixe`, 16 registres, RW.

## Scénario
Sauvegarder le champ, encoder une chaîne ASCII de test complète avec padding `0x00`, écrire avec FC16, vérifier l'absence d'exception, relire/décoder, puis restaurer.

## Résultat attendu
Écriture complète autorisée ; aucune exigence d'écriture partielle.
