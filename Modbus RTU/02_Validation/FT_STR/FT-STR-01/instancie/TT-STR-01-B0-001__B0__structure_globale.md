# TT-STR-01-B0-001 — B0 — Structure des plages

## Traçabilité
- Génériques : `TT-STR-01-GEN-001`, `TT-STR-01-GEN-002`
- Mapping : GEL-MAP-V1 (`ff948e5917becceed7637d9c7864ec9b279be0ca`)

## Référence
- Portée V1/GEL-MAP-V1 : `0..20`
- Longueur : `21` registres
- Trous internes attendus : `0`
- Chevauchements attendus : `0`

## Contrôles
1. Vérifier que la première adresse couverte est `0`.
2. Vérifier que la dernière adresse couverte est `20`.
3. Vérifier que la portée contient `21` registres.
4. Vérifier l'absence de trou interne.
5. Vérifier l'absence de chevauchement entre champs.

## Résultat attendu
B0 respecte exactement ses bornes et sa couverture structurelle V1/GEL-MAP-V1.

## Classification
- Famille : `FT-STR-01`
- Niveau : `instancié`
