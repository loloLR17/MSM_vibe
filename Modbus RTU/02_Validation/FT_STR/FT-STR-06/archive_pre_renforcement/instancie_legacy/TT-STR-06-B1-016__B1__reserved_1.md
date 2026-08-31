# TT-STR-06-B1-016 — Bloc 1 — reserved_1

## Objectif
Valider que le champ logique `reserved_1` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `1`
- Champ logique : `reserved_1`
- Champs source : `reserved_1`
- Offset début : `17`
- Offset fin : `17`
- Adresse début : `1017`
- Adresse fin : `1017`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_2`
- Adresse de début du champ suivant attendue : `1018`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1017`.
2. Vérifier que la lecture couvre strictement la plage `1017` à `1017`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `reserved_2` commence à `1018` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `1017` à `1017` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `reserved_2` commence à `1018` ;
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
