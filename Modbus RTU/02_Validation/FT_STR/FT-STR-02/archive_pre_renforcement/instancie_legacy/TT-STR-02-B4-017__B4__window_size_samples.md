# TT-STR-02-B4-017 — Bloc 4 — window_size_samples

## Objectif
Valider que le champ logique `window_size_samples` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `window_size_samples`
- Nature : `declared_as_is`
- Champs source : `window_size_samples`
- Offset début : `21`
- Offset fin : `21`
- Adresse début : `4021`
- Adresse fin : `4021`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Taille fenêtre`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `indicator_period_ms`
- Adresse de début du champ suivant attendue : `4022`
- Vérifier l'absence d'empiètement entre `window_size_samples` et `indicator_period_ms`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4021`.
2. Vérifier que la lecture couvre la plage `4021` à `4021` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4022`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `window_size_samples` est possible sur la plage `4021` à `4021` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `indicator_period_ms` commence à `4022` ;
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
