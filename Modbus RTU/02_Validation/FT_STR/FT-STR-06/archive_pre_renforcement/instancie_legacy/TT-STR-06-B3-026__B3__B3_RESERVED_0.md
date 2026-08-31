# TT-STR-06-B3-026 — Bloc 3 — B3_RESERVED_0

## Objectif
Valider que le champ logique `B3_RESERVED_0` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_RESERVED_0`
- Champs source : `B3_RESERVED_0`
- Offset début : `40`
- Offset fin : `47`
- Adresse début : `3040`
- Adresse fin : `3047`
- Type déclaré : `réservé`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Réserve d’extension future`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `3040`.
2. Vérifier que la lecture couvre strictement la plage `3040` à `3047`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `3040`, avec une longueur strictement inférieure à `8`.
4. Relire ensuite le champ complet de `3040` à `3047`.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3040` à `3047` est possible ;
- la taille observée est exactement de `8` registre(s) ;
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
