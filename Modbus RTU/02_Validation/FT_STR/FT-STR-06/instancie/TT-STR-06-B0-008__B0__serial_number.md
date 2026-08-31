# TT-STR-06-B0-008 — Bloc 0 — serial_number

## Objectif
Valider que le champ logique `serial_number` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `0`
- Champ logique : `serial_number`
- Champs source : `serial_number_r0;serial_number_r1;serial_number_r2;serial_number_r3;serial_number_r4;serial_number_r5;serial_number_r6;serial_number_r7`
- Offset début : `8`
- Offset fin : `15`
- Adresse début : `8`
- Adresse fin : `15`
- Type déclaré : `ASCII fixe`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Numéro de série`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `manufacturer`
- Adresse de début du champ suivant attendue : `16`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `8`.
2. Vérifier que la lecture couvre strictement la plage `8` à `15`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `8`, avec une longueur strictement inférieure à `8`.
4. Relire ensuite le champ complet de `8` à `15`.
5. Vérifier que le champ suivant `manufacturer` commence à `16` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `8` à `15` est possible ;
- la taille observée est exactement de `8` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `manufacturer` commence à `16` ;
- aucune dépendance implicite de lecture entre `serial_number` et `manufacturer` n'est observée.
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
