# FT-STR-02 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2.

## Règle d'instanciation
- 1 fiche de test par champ logique
- vérification de l'adresse de début
- vérification de l'adresse de fin
- vérification de la taille en registres
- vérification du type déclaré
- vérification de l'absence d'empiètement sur le champ suivant du même bloc

## Convention d'identifiant
`TT-STR-02-B<bloc>-NNN`

## Couverture
- Bloc 0 : 10 champs logiques
- Bloc 1 : 18 champs logiques
- Bloc 2 : 12 champs logiques
- Bloc 3 : 26 champs logiques
- Bloc 4 : 66 champs logiques
- Bloc 5 : 18 champs logiques
- Bloc 6 : 20 champs logiques
- Bloc 7 : 13 champs logiques

## Notes
- Les champs sont issus de `tr2_mapping_unifie_logique.csv`.
- Les champs réservés sont inclus.
- Les champs multi-registres déjà regroupés logiquement (`uint32`, `ASCII fixe`) sont testés comme unités logiques.