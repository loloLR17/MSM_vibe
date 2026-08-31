# TT-STR-02-B4-011 — Bloc 4 — sampling_frequency_hz

## Objectif
Valider que le champ logique `sampling_frequency_hz` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `sampling_frequency_hz`
- Nature : `declared_as_is`
- Champs source : `sampling_frequency_hz`
- Offset début : `16`
- Offset fin : `16`
- Adresse début : `4016`
- Adresse fin : `4016`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Fréquence d’échantillonnage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved`
- Adresse de début du champ suivant attendue : `4017`
- Vérifier l'absence d'empiètement entre `sampling_frequency_hz` et `reserved`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4016`.
2. Vérifier que la lecture couvre la plage `4016` à `4016` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4017`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `sampling_frequency_hz` est possible sur la plage `4016` à `4016` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved` commence à `4017` ;
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
