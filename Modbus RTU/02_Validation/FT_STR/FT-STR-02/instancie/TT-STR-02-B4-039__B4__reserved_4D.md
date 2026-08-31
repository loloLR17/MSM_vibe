# TT-STR-02-B4-039 — Bloc 4 — reserved_4D

## Objectif
Valider que le champ logique `reserved_4D` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4D`
- Nature : `declared_as_is`
- Champs source : `reserved_4D`
- Offset début : `96`
- Offset fin : `99`
- Adresse début : `4096`
- Adresse fin : `4099`
- Type déclaré : `uint16[4]`
- Taille attendue : `4` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_sampling_frequency_hz`
- Adresse de début du champ suivant attendue : `4100`
- Vérifier l'absence d'empiètement entre `reserved_4D` et `active_sampling_frequency_hz`.

## Étapes
1. Lire exactement `4` registre(s) à partir de l'adresse `4096`.
2. Vérifier que la lecture couvre la plage `4096` à `4099` sans décalage.
3. Vérifier que la taille observée correspond exactement à `4` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4100`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[4]`.

## Résultat attendu
- la lecture de `reserved_4D` est possible sur la plage `4096` à `4099` ;
- la taille observée est exactement de `4` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_sampling_frequency_hz` commence à `4100` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[4]`.

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
