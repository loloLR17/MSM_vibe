# TT-STR-02-B1-006 — Bloc 1 — last_reset_cause

## Objectif
Valider que le champ logique `last_reset_cause` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `last_reset_cause`
- Nature : `declared_as_is`
- Champs source : `last_reset_cause`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `1006`
- Adresse fin : `1006`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Cause du dernier redémarrage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `internal_temp_dC`
- Adresse de début du champ suivant attendue : `1007`
- Vérifier l'absence d'empiètement entre `last_reset_cause` et `internal_temp_dC`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1006`.
2. Vérifier que la lecture couvre la plage `1006` à `1006` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1007`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `last_reset_cause` est possible sur la plage `1006` à `1006` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `internal_temp_dC` commence à `1007` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
