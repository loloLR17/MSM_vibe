# TT-STR-02-B3-011 — Bloc 3 — B3_PEAK_GLOBAL_MG

## Objectif
Valider que le champ logique `B3_PEAK_GLOBAL_MG` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_PEAK_GLOBAL_MG`
- Nature : `declared_as_is`
- Champs source : `B3_PEAK_GLOBAL_MG`
- Offset début : `16`
- Offset fin : `17`
- Adresse début : `3016`
- Adresse fin : `3017`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Crête globale en norme vectorielle`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_RMS_X_MG`
- Adresse de début du champ suivant attendue : `3018`
- Vérifier l'absence d'empiètement entre `B3_PEAK_GLOBAL_MG` et `B3_RMS_X_MG`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `3016`.
2. Vérifier que la lecture couvre la plage `3016` à `3017` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3018`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `B3_PEAK_GLOBAL_MG` est possible sur la plage `3016` à `3017` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_RMS_X_MG` commence à `3018` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint32`.

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
