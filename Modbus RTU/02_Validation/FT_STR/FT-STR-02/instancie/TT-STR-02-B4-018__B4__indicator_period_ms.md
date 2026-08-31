# TT-STR-02-B4-018 — Bloc 4 — indicator_period_ms

## Objectif
Valider que le champ logique `indicator_period_ms` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `indicator_period_ms`
- Nature : `declared_as_is`
- Champs source : `indicator_period_ms`
- Offset début : `22`
- Offset fin : `22`
- Adresse début : `4022`
- Adresse fin : `4022`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Période indicateurs`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_duration_s`
- Adresse de début du champ suivant attendue : `4023`
- Vérifier l'absence d'empiètement entre `indicator_period_ms` et `campaign_duration_s`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4022`.
2. Vérifier que la lecture couvre la plage `4022` à `4022` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4023`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `indicator_period_ms` est possible sur la plage `4022` à `4022` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_duration_s` commence à `4023` ;
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
