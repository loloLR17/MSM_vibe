# FT-ACC-02 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour valider l’écriture des zones `RW`.

## Règle d'instanciation
- **Option C — Hybride**
- 1 fiche de test par champ logique `RW`
- 1 fiche complémentaire par plage bloc couvrant les zones `RW` du bloc
- aucune adresse dans `detaille/`
- toutes les adresses issues du mapping unifié

## Convention d'identifiant
`TT-ACC-02-B<bloc>-NNN`

## Couverture par bloc
- Bloc 2 : 1 champs logiques RW + 1 plage bloc RW
- Bloc 4 : 26 champs logiques RW + 1 plage bloc RW
- Bloc 5 : 7 champs logiques RW + 1 plage bloc RW
- Bloc 6 : 1 champs logiques RW + 1 plage bloc RW

## Notes
- Les champs sont issus de `tr2_mapping_unifie_logique.csv`.
- Seuls les champs déclarés `RW` sont instanciés comme cibles d’écriture.
- Les champs multi-registres (`uint32`, `ASCII fixe`) sont testés comme unités logiques.
- Les lectures de contrôle post-écriture sont incluses dans chaque scénario.