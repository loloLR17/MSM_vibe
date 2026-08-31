# TT-STR-06-B6-020 — Bloc 6 — reserved_6B

## Objectif
Valider que le champ logique `reserved_6B` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6B`
- Champs source : `reserved_6B`
- Offset début : `58`
- Offset fin : `63`
- Adresse début : `6058`
- Adresse fin : `6063`
- Type déclaré : `uint16\[6]`
- Taille attendue : `6` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `6` registre(s) à partir de l'adresse `6058`.
2. Vérifier que la lecture couvre strictement la plage `6058` à `6063`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6058`, avec une longueur strictement inférieure à `6`.
4. Relire ensuite le champ complet de `6058` à `6063`.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6058` à `6063` est possible ;
- la taille observée est exactement de `6` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
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
