# TT-STR-02-B1-001 — Bloc 1 — system_status

## Objectif
Valider que le champ logique `system_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `system_status`
- Nature : `declared_as_is`
- Champs source : `system_status`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `1000`
- Adresse fin : `1000`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État global du capteur`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `system_flags`
- Adresse de début du champ suivant attendue : `1001`
- Vérifier l'absence d'empiètement entre `system_status` et `system_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1000`.
2. Vérifier que la lecture couvre la plage `1000` à `1000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `system_status` est possible sur la plage `1000` à `1000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `system_flags` commence à `1001` ;
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
