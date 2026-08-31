# TT-STR-02-B4-034 — Bloc 4 — mission_label

## Objectif
Valider que le champ logique `mission_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `mission_label`
- Nature : `declared_as_is`
- Champs source : `mission_label`
- Offset début : `76`
- Offset fin : `91`
- Adresse début : `4076`
- Adresse fin : `4091`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label mission`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `operating_mode_code`
- Adresse de début du champ suivant attendue : `4092`
- Vérifier l'absence d'empiètement entre `mission_label` et `operating_mode_code`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4076`.
2. Vérifier que la lecture couvre la plage `4076` à `4091` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4092`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `mission_label` est possible sur la plage `4076` à `4091` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `operating_mode_code` commence à `4092` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `ASCII fixe`.

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
