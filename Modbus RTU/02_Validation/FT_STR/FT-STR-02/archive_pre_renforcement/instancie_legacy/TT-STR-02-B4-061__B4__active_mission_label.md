# TT-STR-02-B4-061 — Bloc 4 — active_mission_label

## Objectif
Valider que le champ logique `active_mission_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_mission_label`
- Nature : `declared_as_is`
- Champs source : `active_mission_label`
- Offset début : `148`
- Offset fin : `163`
- Adresse début : `4148`
- Adresse fin : `4163`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Miroir actif, ASCII fixe 32 caractères, padding 0x00`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_operating_mode_code`
- Adresse de début du champ suivant attendue : `4164`
- Vérifier l'absence d'empiètement entre `active_mission_label` et `active_operating_mode_code`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4148`.
2. Vérifier que la lecture couvre la plage `4148` à `4163` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4164`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `active_mission_label` est possible sur la plage `4148` à `4163` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_operating_mode_code` commence à `4164` ;
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
