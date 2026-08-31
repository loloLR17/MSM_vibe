# TT-STR-02-B1-004 — Bloc 1 — warning_flags

## Objectif
Valider que le champ logique `warning_flags` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `warning_flags`
- Nature : `declared_as_is`
- Champs source : `warning_flags`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `1003`
- Adresse fin : `1003`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux d’avertissements`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `uptime_s`
- Adresse de début du champ suivant attendue : `1004`
- Vérifier l'absence d'empiètement entre `warning_flags` et `uptime_s`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1003`.
2. Vérifier que la lecture couvre la plage `1003` à `1003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `warning_flags` est possible sur la plage `1003` à `1003` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `uptime_s` commence à `1004` ;
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
