# FT-ACC-01 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour valider la lecture des zones exposées.

## Règle d'instanciation
- **Option C — Hybride**
- 1 fiche de test par champ logique lisible
- 1 fiche complémentaire par plage bloc complète
- aucune adresse dans `detaille/`
- toutes les adresses issues du mapping unifié

## Convention d'identifiant
`TT-ACC-01-B<bloc>-NNN`

## Couverture par bloc
- Bloc 0 : 10 champs logiques + 1 plage bloc
- Bloc 1 : 18 champs logiques + 1 plage bloc
- Bloc 2 : 12 champs logiques + 1 plage bloc
- Bloc 3 : 26 champs logiques + 1 plage bloc
- Bloc 4 : 66 champs logiques + 1 plage bloc
- Bloc 5 : 18 champs logiques + 1 plage bloc
- Bloc 6 : 20 champs logiques + 1 plage bloc
- Bloc 7 : 13 champs logiques + 1 plage bloc

## Notes
- Les champs sont issus de `tr2_mapping_unifie_logique.csv`.
- Les zones RO et RW sont incluses dès lors qu'elles sont lisibles.
- Les champs multi-registres (`uint32`, `ASCII fixe`) sont testés comme unités logiques.
- Les lectures de plage bloc complètent la couverture terrain des lectures multi-registres.