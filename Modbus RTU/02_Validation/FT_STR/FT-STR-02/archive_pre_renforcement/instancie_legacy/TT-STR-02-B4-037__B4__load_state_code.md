# TT-STR-02-B4-037 — Bloc 4 — load_state_code

## Objectif
Valider que le champ logique `load_state_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `load_state_code`
- Nature : `declared_as_is`
- Champs source : `load_state_code`
- Offset début : `94`
- Offset fin : `94`
- Adresse début : `4094`
- Adresse fin : `4094`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `État charge`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `sea_state_code`
- Adresse de début du champ suivant attendue : `4095`
- Vérifier l'absence d'empiètement entre `load_state_code` et `sea_state_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4094`.
2. Vérifier que la lecture couvre la plage `4094` à `4094` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4095`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `load_state_code` est possible sur la plage `4094` à `4094` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `sea_state_code` commence à `4095` ;
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
