# TT-ACC-02-B4-021 — Bloc 4 — campaign_label

## Cible
`4060..4075`, `ASCII fixe`, 16 registres, RW.

## Scénario
Sauvegarder le champ, encoder une chaîne ASCII de test sur toute la longueur avec padding `0x00`, écrire avec FC16, vérifier l'absence d'exception, relire/décoder, puis restaurer.

## Résultat attendu
Écriture complète autorisée ; aucune exigence d'écriture partielle.
