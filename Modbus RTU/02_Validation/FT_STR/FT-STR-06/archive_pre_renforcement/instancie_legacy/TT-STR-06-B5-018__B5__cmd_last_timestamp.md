# TT-STR-06-B5-018 — Bloc 5 — cmd_last_timestamp

## Objectif
Valider que le champ logique `cmd_last_timestamp` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_last_timestamp`
- Champs source : `cmd_last_timestamp_msw;cmd_last_timestamp_lsw`
- Offset début : `18`
- Offset fin : `19`
- Adresse début : `5018`
- Adresse fin : `5019`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Timestamp de fin de dernière commande, mot fort`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `5018`.
2. Vérifier que la lecture couvre strictement la plage `5018` à `5019`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `5018`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `5018` à `5019`.
5. Vérifier qu'aucune extension implicite du champ n'est nécessaire au-delà de l'adresse de fin spécifiée.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `5018` à `5019` est possible ;
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
