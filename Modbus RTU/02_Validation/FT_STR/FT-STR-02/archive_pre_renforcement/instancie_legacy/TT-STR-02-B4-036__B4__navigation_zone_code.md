# TT-STR-02-B4-036 — Bloc 4 — navigation_zone_code

## Objectif
Valider que le champ logique `navigation_zone_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `navigation_zone_code`
- Nature : `declared_as_is`
- Champs source : `navigation_zone_code`
- Offset début : `93`
- Offset fin : `93`
- Adresse début : `4093`
- Adresse fin : `4093`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Zone navigation`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `load_state_code`
- Adresse de début du champ suivant attendue : `4094`
- Vérifier l'absence d'empiètement entre `navigation_zone_code` et `load_state_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4093`.
2. Vérifier que la lecture couvre la plage `4093` à `4093` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4094`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `navigation_zone_code` est possible sur la plage `4093` à `4093` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `load_state_code` commence à `4094` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16`.

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
