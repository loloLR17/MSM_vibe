# TT-STR-02-B7-002 — Bloc 7 — system_health_status

## Objectif
Valider que le champ logique `system_health_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `system_health_status`
- Nature : `declared_as_is`
- Champs source : `system_health_status`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `7001`
- Adresse fin : `7001`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État global`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `system_fault_flags`
- Adresse de début du champ suivant attendue : `7002`
- Vérifier l'absence d'empiètement entre `system_health_status` et `system_fault_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7001`.
2. Vérifier que la lecture couvre la plage `7001` à `7001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `system_health_status` est possible sur la plage `7001` à `7001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `system_fault_flags` commence à `7002` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
