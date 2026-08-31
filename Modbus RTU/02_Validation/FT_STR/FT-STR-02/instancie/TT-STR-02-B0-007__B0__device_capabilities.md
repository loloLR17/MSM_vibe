# TT-STR-02-B0-007 — Bloc 0 — device_capabilities

## Objectif
Valider que le champ logique `device_capabilities` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `device_capabilities`
- Nature : `declared_as_is`
- Champs source : `device_capabilities`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `7`
- Adresse fin : `7`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Capacités du capteur`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `serial_number`
- Adresse de début du champ suivant attendue : `8`
- Vérifier l'absence d'empiètement entre `device_capabilities` et `serial_number`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7`.
2. Vérifier que la lecture couvre la plage `7` à `7` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `8`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `device_capabilities` est possible sur la plage `7` à `7` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `serial_number` commence à `8` ;
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
