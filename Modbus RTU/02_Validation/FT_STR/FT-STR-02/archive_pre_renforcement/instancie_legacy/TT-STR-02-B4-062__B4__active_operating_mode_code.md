# TT-STR-02-B4-062 — Bloc 4 — active_operating_mode_code

## Objectif
Valider que le champ logique `active_operating_mode_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_operating_mode_code`
- Nature : `declared_as_is`
- Champs source : `active_operating_mode_code`
- Offset début : `164`
- Offset fin : `164`
- Adresse début : `4164`
- Adresse fin : `4164`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_navigation_zone_code`
- Adresse de début du champ suivant attendue : `4165`
- Vérifier l'absence d'empiètement entre `active_operating_mode_code` et `active_navigation_zone_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4164`.
2. Vérifier que la lecture couvre la plage `4164` à `4164` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4165`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `active_operating_mode_code` est possible sur la plage `4164` à `4164` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_navigation_zone_code` commence à `4165` ;
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
