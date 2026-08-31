# TT-STR-06-B4-021 — Bloc 4 — storage_limit_mb

## Objectif
Valider que le champ logique `storage_limit_mb` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `storage_limit_mb`
- Champs source : `storage_limit_mb`
- Offset début : `26`
- Offset fin : `27`
- Adresse début : `4026`
- Adresse fin : `4027`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Limite stockage`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4B`
- Adresse de début du champ suivant attendue : `4028`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4026`.
2. Vérifier que la lecture couvre strictement la plage `4026` à `4027`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4026`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4026` à `4027`.
5. Vérifier que le champ suivant `reserved_4B` commence à `4028` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4026` à `4027` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `reserved_4B` commence à `4028` ;
- aucune dépendance implicite de lecture entre `storage_limit_mb` et `reserved_4B` n'est observée.
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
