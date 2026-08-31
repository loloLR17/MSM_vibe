# FT-STR-06 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour l’accessibilité et le découpage de lecture Modbus.

## Règles d’instanciation
- 1 fiche par champ logique
- vérification lecture exacte de la plage du champ
- vérification lecture unitaire ou partielle selon la taille
- vérification de frontière avec le champ suivant
- vérification de lecture sans dépendance implicite au découpage

## Convention d’identifiant
`TT-STR-06-B<bloc>-NNN`

## Nombre total de tests
- 183

- Bloc 0 : 10 champ(s) logique(s)
- Bloc 1 : 18 champ(s) logique(s)
- Bloc 2 : 12 champ(s) logique(s)
- Bloc 3 : 26 champ(s) logique(s)
- Bloc 4 : 66 champ(s) logique(s)
- Bloc 5 : 18 champ(s) logique(s)
- Bloc 6 : 20 champ(s) logique(s)
- Bloc 7 : 13 champ(s) logique(s)

## Notes
- Les tests sont instanciés sur la vue logique du mapping.
- Pour les champs multi-registres, le test vérifie aussi la lecture partielle et la lecture complète.
- Les exceptions Modbus exactes hors plage restent hors du présent lot si non figées par la spécification.