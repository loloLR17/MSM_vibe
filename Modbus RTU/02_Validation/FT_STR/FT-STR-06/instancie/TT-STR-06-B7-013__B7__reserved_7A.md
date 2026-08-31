# TT-STR-06-B7-013 — Bloc 7 — reserved_7A

## Objectif
Valider que le champ logique `reserved_7A` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `7`
- Champ logique : `reserved_7A`
- Champs source : `reserved_7A`
- Offset début : `14`
- Offset fin : `15`
- Adresse début : `7014`
- Adresse fin : `7015`
- Type déclaré : `uint16\[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `7014`.
2. Vérifier que la lecture couvre strictement la plage `7014` à `7015`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `7014`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `7014` à `7015`.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `7014` à `7015` est possible ;
- la taille observée est exactement de `2` registre(s) ;
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
