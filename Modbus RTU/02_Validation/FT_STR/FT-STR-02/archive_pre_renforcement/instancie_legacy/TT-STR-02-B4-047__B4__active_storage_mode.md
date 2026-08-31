# TT-STR-02-B4-047 — Bloc 4 — active_storage_mode

## Objectif
Valider que le champ logique `active_storage_mode` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_storage_mode`
- Nature : `declared_as_is`
- Champs source : `active_storage_mode`
- Offset début : `108`
- Offset fin : `108`
- Adresse début : `4108`
- Adresse fin : `4108`
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
- Champ suivant attendu : `active_storage_limit_mb`
- Adresse de début du champ suivant attendue : `4109`
- Vérifier l'absence d'empiètement entre `active_storage_mode` et `active_storage_limit_mb`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4108`.
2. Vérifier que la lecture couvre la plage `4108` à `4108` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4109`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `active_storage_mode` est possible sur la plage `4108` à `4108` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_storage_limit_mb` commence à `4109` ;
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
