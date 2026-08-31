# TT-STR-06-B4-066 — Bloc 4 — reserved_4E_C

## Objectif
Valider que le champ logique `reserved_4E_C` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_C`
- Champs source : `reserved_4E_C`
- Offset début : `168`
- Offset fin : `175`
- Adresse début : `4168`
- Adresse fin : `4175`
- Type déclaré : `uint16[8]`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `4168`.
2. Vérifier que la lecture couvre strictement la plage `4168` à `4175`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4168`, avec une longueur strictement inférieure à `8`.
4. Relire ensuite le champ complet de `4168` à `4175`.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4168` à `4175` est possible ;
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
