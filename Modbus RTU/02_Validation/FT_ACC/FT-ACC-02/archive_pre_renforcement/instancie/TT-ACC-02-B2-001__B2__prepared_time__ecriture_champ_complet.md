# TT-ACC-02-B2-001 — Bloc 2 — prepared_time

## Objectif
Vérifier que le champ logique `prepared_time` déclaré `RW` accepte une écriture conforme et restitue la valeur écrite en lecture immédiate.

## Exigence(s) couverte(s)
- FT-ACC-02
- Mapping unifié logique TR2
- Attribut d’accès : `RW`

## Référence mapping
- Adresse début : 2008
- Adresse fin : 2009
- Type déclaré : uint32
- Nombre de registres : 2
- Description : Temps préparé

## Préconditions
- FT-STR validée
- FT-ACC-01 validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Valeur initiale lisible avant écriture

## Données d’entrée
- Adresse de départ : 2008
- Longueur : 2
- Valeur de test : valeur de test sur 2 registres, différente de la valeur initiale

## Contrôle de frontière
- Champ RW suivant attendu : fin_de_zone_RW_du_bloc
- Adresse de début du champ RW suivant attendue : n/a
- Vérifier l’absence de débordement de l’écriture au-delà de la plage `2008..2009`.

## Scénario / étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2008` pour obtenir la valeur initiale.
2. Écrire exactement `2` registre(s) sur la même plage avec une valeur de test différente de la valeur initiale.
3. Contrôler l’absence d’exception Modbus sur l’écriture.
4. Relire exactement `2` registre(s) à partir de l'adresse `2008`.
5. Vérifier que la valeur relue correspond à la valeur écrite.
6. Vérifier que l’opération reste bornée à la plage `2008..2009`.

## Résultat attendu
- l’écriture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la relecture contient exactement `2` registre(s) ;
- la valeur relue correspond à la valeur écrite ;
- le champ `prepared_time` est modifiable conformément au mapping.

## Critères d’acceptation
- écriture réussie ;
- relecture cohérente ;
- longueur conforme ;
- comportement cohérent avec l’attribut d’accès `RW` ;
- aucune divergence mapping ↔ comportement réel.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Automatisation possible
Oui

## Traces à conserver
- trame de lecture initiale ;
- trame d’écriture ;
- trame de lecture de contrôle ;
- valeur initiale ;
- valeur écrite ;
- valeur relue ;
- verdict ;
- anomalie associée le cas échéant.

## Niveau de criticité
P0

## Remarques / limites
- cette fiche valide l’accessibilité en écriture et la cohérence write → read ;
- la validité métier de la valeur écrite est hors périmètre FT-ACC-02.
