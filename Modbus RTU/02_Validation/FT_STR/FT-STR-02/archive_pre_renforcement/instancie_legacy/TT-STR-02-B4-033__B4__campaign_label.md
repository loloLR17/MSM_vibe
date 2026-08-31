# TT-STR-02-B4-033 — Bloc 4 — campaign_label

## Objectif
Valider que le champ logique `campaign_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `campaign_label`
- Nature : `declared_as_is`
- Champs source : `campaign_label`
- Offset début : `60`
- Offset fin : `75`
- Adresse début : `4060`
- Adresse fin : `4075`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `4076`
- Vérifier l'absence d'empiètement entre `campaign_label` et `mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4060`.
2. Vérifier que la lecture couvre la plage `4060` à `4075` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4076`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `campaign_label` est possible sur la plage `4060` à `4075` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `mission_label` commence à `4076` ;
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
