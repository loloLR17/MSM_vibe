# TT-STR-02-B7-010 — Bloc 7 — reset_cause

## Objectif
Valider que le champ logique `reset_cause` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `reset_cause`
- Nature : `declared_as_is`
- Champs source : `reset_cause`
- Offset début : `11`
- Offset fin : `11`
- Adresse début : `7011`
- Adresse fin : `7011`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Cause dernier reset`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `internal_temp_dC`
- Adresse de début du champ suivant attendue : `7012`
- Vérifier l'absence d'empiètement entre `reset_cause` et `internal_temp_dC`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7011`.
2. Vérifier que la lecture couvre la plage `7011` à `7011` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7012`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `reset_cause` est possible sur la plage `7011` à `7011` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `internal_temp_dC` commence à `7012` ;
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
