# TT-STR-07-B6-002 — Bloc 6 — reserved_6A

## Objectif
Valider la stabilité du champ réservé `reserved_6A`.

## Référence mapping
- Adresse début : `6010`
- Adresse fin : `6011`
- Taille : `2` registre(s)

## Préconditions
- capteur en état stable
- accès Modbus opérationnel

## Étapes
1. Lire le champ 20 fois consécutivement.
2. Vérifier que toutes les lectures valent `0`.
3. Vérifier l'absence totale de variation.
4. Relire après lecture d’un autre champ du même bloc.
5. Vérifier à nouveau que la valeur reste identique.

## Résultat attendu
- champ toujours lu à `0` ;
- 0 variation ;
- aucun effet de bord.

## Critères d’acceptation
- 100% des lectures à `0` ;
- 0 variation ;
- aucune divergence après alternance de lecture.
