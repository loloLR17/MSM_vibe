# TT-STR-02-B4-040 — Bloc 4 — active_sampling_frequency_hz

## Objectif
Valider que le champ logique `active_sampling_frequency_hz` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_sampling_frequency_hz`
- Nature : `declared_as_is`
- Champs source : `active_sampling_frequency_hz`
- Offset début : `100`
- Offset fin : `100`
- Adresse début : `4100`
- Adresse fin : `4100`
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
- Champ suivant attendu : `active_axes_enable_mask`
- Adresse de début du champ suivant attendue : `4101`
- Vérifier l'absence d'empiètement entre `active_sampling_frequency_hz` et `active_axes_enable_mask`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4100`.
2. Vérifier que la lecture couvre la plage `4100` à `4100` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4101`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `active_sampling_frequency_hz` est possible sur la plage `4100` à `4100` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_axes_enable_mask` commence à `4101` ;
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
