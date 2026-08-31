# FT-STR-07 — Instancié renforcé

## Objet
Version renforcée de FT-STR-07 orientée stabilité d’image, déterminisme et cohérence multi-registres.

## Axes couverts
- snapshots complets de blocs
- stabilité de réservés
- stabilité de champs ASCII
- cohérence `uint32`
- alternance de lectures
- détection d’effet de bord
- détection d’instabilité localisée

## Règles quantifiées
- 20 lectures consécutives minimum pour les tests de stabilité
- 10 cycles alternés minimum pour les tests d’alternance
- 0 variation autorisée sur champ structurel stable
- 0 incohérence de reconstruction `uint32`
