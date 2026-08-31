# TT-STR-02-B5-013 — Bloc 5 — cmd_engine_flags

## Objectif
Valider que le champ logique `cmd_engine_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_engine_flags`
- Nature : `declared_as_is`
- Champs source : `cmd_engine_flags`
- Offset début : `13`
- Offset fin : `13`
- Adresse début : `5013`
- Adresse fin : `5013`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Flags moteur de commande`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_last_code`
- Adresse de début du champ suivant attendue : `5014`
- Vérifier l'absence d'empiètement entre `cmd_engine_flags` et `cmd_last_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5013`.
2. Vérifier que la lecture couvre la plage `5013` à `5013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `cmd_engine_flags` est possible sur la plage `5013` à `5013` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_last_code` commence à `5014` ;
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
