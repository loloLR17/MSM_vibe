# TT-STR-02-B7-003 — Bloc 7 — system_fault_flags

## Objectif
Valider que le champ logique `system_fault_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `system_fault_flags`
- Nature : `declared_as_is`
- Champs source : `system_fault_flags`
- Offset début : `2`
- Offset fin : `2`
- Adresse début : `7002`
- Adresse fin : `7002`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Flags défauts`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_fault_code`
- Adresse de début du champ suivant attendue : `7003`
- Vérifier l'absence d'empiètement entre `system_fault_flags` et `last_fault_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7002`.
2. Vérifier que la lecture couvre la plage `7002` à `7002` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7003`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `system_fault_flags` est possible sur la plage `7002` à `7002` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `last_fault_code` commence à `7003` ;
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
