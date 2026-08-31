# TT-STR-02-B5-007 — Bloc 5 — cmd_request_control

## Objectif
Valider que le champ logique `cmd_request_control` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_control`
- Nature : `declared_as_is`
- Champs source : `cmd_request_control`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `5007`
- Adresse fin : `5007`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Bits de soumission / annulation / nettoyage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_active_code`
- Adresse de début du champ suivant attendue : `5008`
- Vérifier l'absence d'empiètement entre `cmd_request_control` et `cmd_active_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5007`.
2. Vérifier que la lecture couvre la plage `5007` à `5007` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5008`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `cmd_request_control` est possible sur la plage `5007` à `5007` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_active_code` commence à `5008` ;
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
