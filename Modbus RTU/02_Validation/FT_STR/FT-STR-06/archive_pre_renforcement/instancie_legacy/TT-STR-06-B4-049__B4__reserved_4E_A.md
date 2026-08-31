# TT-STR-06-B4-049 — Bloc 4 — reserved_4E_A

## Objectif
Valider que le champ logique `reserved_4E_A` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_A`
- Champs source : `reserved_4E_A`
- Offset début : `111`
- Offset fin : `115`
- Adresse début : `4111`
- Adresse fin : `4115`
- Type déclaré : `uint16[5]`
- Taille attendue : `5` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_supervision_enable_mask`
- Adresse de début du champ suivant attendue : `4116`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `5` registre(s) à partir de l'adresse `4111`.
2. Vérifier que la lecture couvre strictement la plage `4111` à `4115`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4111`, avec une longueur strictement inférieure à `5`.
4. Relire ensuite le champ complet de `4111` à `4115`.
5. Vérifier que le champ suivant `active_supervision_enable_mask` commence à `4116` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4111` à `4115` est possible ;
- la taille observée est exactement de `5` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_supervision_enable_mask` commence à `4116` ;
- aucune dépendance implicite de lecture entre `reserved_4E_A` et `active_supervision_enable_mask` n'est observée.
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
