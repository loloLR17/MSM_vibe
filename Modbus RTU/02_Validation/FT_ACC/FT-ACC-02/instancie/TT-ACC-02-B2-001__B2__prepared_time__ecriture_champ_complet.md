# TT-ACC-02-B2-001 — Bloc 2 — prepared_time

## Objectif
Vérifier le droit d'écriture nominal du champ logique complet `prepared_time` déclaré `RW`.

## Référence
- Adresse : `2008..2009`
- Type : `uint32`
- Accès : `RW`

## Préconditions
- FT-STR gelée ; accès Modbus opérationnel ; état initial lisible ; valeur V1 sûre.

## Étapes
1. Lire la valeur initiale.
2. Écrire les deux registres avec FC16.
3. Vérifier l'absence d'exception.
4. Relire le champ complet et vérifier sa prise en compte conformément à V1.
5. Restaurer si nécessaire.

## Résultat attendu
Accès accepté, champ inscriptible, aucune conclusion FT-LIM.
