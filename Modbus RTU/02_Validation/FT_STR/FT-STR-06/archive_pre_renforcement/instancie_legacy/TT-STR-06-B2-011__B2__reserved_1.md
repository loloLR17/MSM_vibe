# TT-STR-06-B2-011 — Bloc 2 — reserved_1

## Objectif
Valider que le champ logique `reserved_1` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `reserved_1`
- Champs source : `reserved_1`
- Offset début : `14`
- Offset fin : `14`
- Adresse début : `2014`
- Adresse fin : `2014`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_2`
- Adresse de début du champ suivant attendue : `2015`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2014`.
2. Vérifier que la lecture couvre strictement la plage `2014` à `2014`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `reserved_2` commence à `2015` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2014` à `2014` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `reserved_2` commence à `2015` ;
- aucune dépendance implicite de lecture entre `reserved_1` et `reserved_2` n'est observée.
- aucune dépendance implicite à un découpage particulier n'est observée sur ce champ.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- lecture valide sur la plage exacte ;
- lecture partielle valide si applicable ;
- absence d’empiètement ;
- aucune dépendance implicite au découpage.

## Classification
- Famille : `FT-STR-06`
- Sous-famille : `Accessibilité lecture`
- Niveau : `instancié`
