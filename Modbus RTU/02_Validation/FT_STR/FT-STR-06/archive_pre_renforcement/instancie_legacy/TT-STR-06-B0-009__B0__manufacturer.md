# TT-STR-06-B0-009 — Bloc 0 — manufacturer

## Objectif
Valider que le champ logique `manufacturer` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `0`
- Champ logique : `manufacturer`
- Champs source : `manufacturer_r0;manufacturer_r1;manufacturer_r2;manufacturer_r3`
- Offset début : `16`
- Offset fin : `19`
- Adresse début : `16`
- Adresse fin : `19`
- Type déclaré : `ASCII fixe`
- Taille attendue : `4` registre(s)
- Accès : `RO`
- Description : `Fabricant`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved`
- Adresse de début du champ suivant attendue : `20`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `4` registre(s) à partir de l'adresse `16`.
2. Vérifier que la lecture couvre strictement la plage `16` à `19`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `16`, avec une longueur strictement inférieure à `4`.
4. Relire ensuite le champ complet de `16` à `19`.
5. Vérifier que le champ suivant `reserved` commence à `20` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `16` à `19` est possible ;
- la taille observée est exactement de `4` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `reserved` commence à `20` ;
- aucune dépendance implicite de lecture entre `manufacturer` et `reserved` n'est observée.
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
