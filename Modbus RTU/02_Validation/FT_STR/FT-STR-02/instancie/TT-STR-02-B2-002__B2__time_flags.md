# TT-STR-02-B2-002 — Bloc 2 — time_flags

## Objectif
Valider que le champ logique `time_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `time_flags`
- Nature : `declared_as_is`
- Champs source : `time_flags`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `2001`
- Adresse fin : `2001`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux de validité et synchronisation`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `current_time`
- Adresse de début du champ suivant attendue : `2002`
- Vérifier l'absence d'empiètement entre `time_flags` et `current_time`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2001`.
2. Vérifier que la lecture couvre la plage `2001` à `2001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `time_flags` est possible sur la plage `2001` à `2001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `current_time` commence à `2002` ;
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
