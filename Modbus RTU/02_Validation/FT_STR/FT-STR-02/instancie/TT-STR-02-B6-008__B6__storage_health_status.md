# TT-STR-02-B6-008 — Bloc 6 — storage_health_status

## Objectif
Valider que le champ logique `storage_health_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `storage_health_status`
- Nature : `declared_as_is`
- Champs source : `storage_health_status`
- Offset début : `9`
- Offset fin : `9`
- Adresse début : `6009`
- Adresse fin : `6009`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État stockage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_6A`
- Adresse de début du champ suivant attendue : `6010`
- Vérifier l'absence d'empiètement entre `storage_health_status` et `reserved_6A`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6009`.
2. Vérifier que la lecture couvre la plage `6009` à `6009` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6010`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `storage_health_status` est possible sur la plage `6009` à `6009` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_6A` commence à `6010` ;
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
