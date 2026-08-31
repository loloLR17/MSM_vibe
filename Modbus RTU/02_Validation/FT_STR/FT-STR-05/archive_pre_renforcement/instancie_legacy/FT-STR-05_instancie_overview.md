# FT-STR-05 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour les registres réservés et sentinelles structurelles.

## Règles d’instanciation
- 1 fiche de test par champ logique réservé/sentinelle
- vérification de la plage d’adresses
- vérification de la taille en registres
- vérification de la lecture à `0`
- vérification de la stabilité par lectures répétées
- vérification de l’absence d’empiètement sur le champ suivant

## Convention d’identifiant
`TT-STR-05-B<bloc>-NNN`

## Nombre total de tests
- 19

- Bloc 0 : 1 champ(s) réservé(s)/sentinelle(s)
- Bloc 1 : 3 champ(s) réservé(s)/sentinelle(s)
- Bloc 2 : 2 champ(s) réservé(s)/sentinelle(s)
- Bloc 3 : 1 champ(s) réservé(s)/sentinelle(s)
- Bloc 4 : 9 champ(s) réservé(s)/sentinelle(s)
- Bloc 6 : 2 champ(s) réservé(s)/sentinelle(s)
- Bloc 7 : 1 champ(s) réservé(s)/sentinelle(s)

## Notes
- Les champs sont issus de `tr2_mapping_unifie_logique.csv`.
- La règle attendue est : lecture à `0` et stabilité totale en état capteur stable.