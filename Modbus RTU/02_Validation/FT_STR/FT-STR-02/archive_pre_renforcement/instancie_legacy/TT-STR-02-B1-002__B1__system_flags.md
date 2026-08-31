# TT-STR-02-B1-002 — Bloc 1 — system_flags

## Objectif
Valider que le champ logique `system_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `system_flags`
- Nature : `declared_as_is`
- Champs source : `system_flags`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `1001`
- Adresse fin : `1001`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux d’état système`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `fault_flags`
- Adresse de début du champ suivant attendue : `1002`
- Vérifier l'absence d'empiètement entre `system_flags` et `fault_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1001`.
2. Vérifier que la lecture couvre la plage `1001` à `1001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `system_flags` est possible sur la plage `1001` à `1001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `fault_flags` commence à `1002` ;
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
