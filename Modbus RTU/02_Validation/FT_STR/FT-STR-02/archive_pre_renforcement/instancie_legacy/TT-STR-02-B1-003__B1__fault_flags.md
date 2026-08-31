# TT-STR-02-B1-003 — Bloc 1 — fault_flags

## Objectif
Valider que le champ logique `fault_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `fault_flags`
- Nature : `declared_as_is`
- Champs source : `fault_flags`
- Offset début : `2`
- Offset fin : `2`
- Adresse début : `1002`
- Adresse fin : `1002`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux de défauts actifs`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `warning_flags`
- Adresse de début du champ suivant attendue : `1003`
- Vérifier l'absence d'empiètement entre `fault_flags` et `warning_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1002`.
2. Vérifier que la lecture couvre la plage `1002` à `1002` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1003`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `fault_flags` est possible sur la plage `1002` à `1002` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `warning_flags` commence à `1003` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `bitfield16`.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- absence d’empiètement ;
- type structurellement cohérent ;
- aucune ambiguïté de frontière.

## Classification
- Famille : `FT-STR-02`
- Sous-famille : `Typage des champs`
- Niveau : `instancié`
