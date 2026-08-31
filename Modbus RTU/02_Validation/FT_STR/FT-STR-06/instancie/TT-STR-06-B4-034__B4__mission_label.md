# TT-STR-06-B4-034 — Bloc 4 — mission_label

## Objectif
Valider que le champ logique `mission_label` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `mission_label`
- Champs source : `mission_label`
- Offset début : `76`
- Offset fin : `91`
- Adresse début : `4076`
- Adresse fin : `4091`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label mission`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `operating_mode_code`
- Adresse de début du champ suivant attendue : `4092`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4076`.
2. Vérifier que la lecture couvre strictement la plage `4076` à `4091`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4076`, avec une longueur strictement inférieure à `16`.
4. Relire ensuite le champ complet de `4076` à `4091`.
5. Vérifier que le champ suivant `operating_mode_code` commence à `4092` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4076` à `4091` est possible ;
- la taille observée est exactement de `16` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `operating_mode_code` commence à `4092` ;
- aucune dépendance implicite de lecture entre `mission_label` et `operating_mode_code` n'est observée.
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
