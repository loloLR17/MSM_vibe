# TT-STR-02-B4-010 — Bloc 4 — reserved_4A

## Objectif
Valider que le champ logique `reserved_4A` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4A`
- Nature : `declared_as_is`
- Champs source : `reserved_4A`
- Offset début : `14`
- Offset fin : `15`
- Adresse début : `4014`
- Adresse fin : `4015`
- Type déclaré : `uint16[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `sampling_frequency_hz`
- Adresse de début du champ suivant attendue : `4016`
- Vérifier l'absence d'empiètement entre `reserved_4A` et `sampling_frequency_hz`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4014`.
2. Vérifier que la lecture couvre la plage `4014` à `4015` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4016`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[2]`.

## Résultat attendu
- la lecture de `reserved_4A` est possible sur la plage `4014` à `4015` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `sampling_frequency_hz` commence à `4016` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[2]`.

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
