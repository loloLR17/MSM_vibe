# TT-STR-06-B2-012 — Bloc 2 — reserved_2

## Objectif
Valider que le champ logique `reserved_2` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `reserved_2`
- Champs source : `reserved_2`
- Offset début : `15`
- Offset fin : `15`
- Adresse début : `2015`
- Adresse fin : `2015`
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
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2015`.
2. Vérifier que la lecture couvre strictement la plage `2015` à `2015`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2015` à `2015` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- aucune extension implicite du champ au-delà de la frontière spécifiée.
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
