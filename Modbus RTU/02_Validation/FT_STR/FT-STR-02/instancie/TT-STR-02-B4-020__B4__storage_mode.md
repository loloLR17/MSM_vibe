# TT-STR-02-B4-020 — Bloc 4 — storage_mode

## Objectif
Valider que le champ logique `storage_mode` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `storage_mode`
- Nature : `declared_as_is`
- Champs source : `storage_mode`
- Offset début : `25`
- Offset fin : `25`
- Adresse début : `4025`
- Adresse fin : `4025`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Mode stockage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_limit_mb`
- Adresse de début du champ suivant attendue : `4026`
- Vérifier l'absence d'empiètement entre `storage_mode` et `storage_limit_mb`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4025`.
2. Vérifier que la lecture couvre la plage `4025` à `4025` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4026`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `storage_mode` est possible sur la plage `4025` à `4025` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_limit_mb` commence à `4026` ;
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
