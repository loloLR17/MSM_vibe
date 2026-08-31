# TT-STR-02-B7-012 — Bloc 7 — supply_voltage_mV

## Objectif
Valider que le champ logique `supply_voltage_mV` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `supply_voltage_mV`
- Nature : `declared_as_is`
- Champs source : `supply_voltage_mV`
- Offset début : `13`
- Offset fin : `13`
- Adresse début : `7013`
- Adresse fin : `7013`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Tension alimentation`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_7A`
- Adresse de début du champ suivant attendue : `7014`
- Vérifier l'absence d'empiètement entre `supply_voltage_mV` et `reserved_7A`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7013`.
2. Vérifier que la lecture couvre la plage `7013` à `7013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `supply_voltage_mV` est possible sur la plage `7013` à `7013` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_7A` commence à `7014` ;
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
