# FT-STR-04 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour les champs `ASCII fixe`.

## Règles d’instanciation
- 1 fiche de test par champ ASCII logique
- vérification de la plage d’adresses
- vérification de la taille en registres
- vérification de la compatibilité ASCII
- vérification du padding `0x00` en fin de zone non utilisée
- vérification de l’absence d’empiètement sur le champ suivant

## Convention d’identifiant
`TT-STR-04-B<bloc>-NNN`

## Nombre total de tests
- 8

- Bloc 0 : 2 champ(s) ASCII
- Bloc 4 : 4 champ(s) ASCII
- Bloc 6 : 2 champ(s) ASCII

## Notes
- La taille est exprimée en registres Modbus.
- La conversion registre/octet reste hors du présent livrable ; le test vise la conformité structurelle et le comportement ASCII/padding côté image registre.
- Les champs sont issus de `tr2_mapping_unifie_logique.csv`.