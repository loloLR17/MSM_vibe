# TT-STR-02-B4-009 — Bloc 4 — config_revision_counter

## Objectif
Valider que le champ logique `config_revision_counter` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_revision_counter`
- Nature : `declared_as_is`
- Champs source : `config_revision_counter`
- Offset début : `12`
- Offset fin : `13`
- Adresse début : `4012`
- Adresse fin : `4013`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Compteur de révision`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4A`
- Adresse de début du champ suivant attendue : `4014`
- Vérifier l'absence d'empiètement entre `config_revision_counter` et `reserved_4A`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4012`.
2. Vérifier que la lecture couvre la plage `4012` à `4013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `config_revision_counter` est possible sur la plage `4012` à `4013` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_4A` commence à `4014` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint32`.

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
