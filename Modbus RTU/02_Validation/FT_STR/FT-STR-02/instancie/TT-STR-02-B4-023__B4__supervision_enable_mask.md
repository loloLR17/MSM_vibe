# TT-STR-02-B4-023 — Bloc 4 — supervision_enable_mask

## Objectif
Valider que le champ logique `supervision_enable_mask` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `supervision_enable_mask`
- Nature : `declared_as_is`
- Champs source : `supervision_enable_mask`
- Offset début : `40`
- Offset fin : `40`
- Adresse début : `4040`
- Adresse fin : `4040`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Masque supervision`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `rms_warn_threshold_mg`
- Adresse de début du champ suivant attendue : `4041`
- Vérifier l'absence d'empiètement entre `supervision_enable_mask` et `rms_warn_threshold_mg`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4040`.
2. Vérifier que la lecture couvre la plage `4040` à `4040` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4041`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `supervision_enable_mask` est possible sur la plage `4040` à `4040` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `rms_warn_threshold_mg` commence à `4041` ;
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
