# TT-STR-06-B6-009 — Bloc 6 — reserved_6A

## Objectif
Valider que le champ logique `reserved_6A` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6A`
- Champs source : `reserved_6A`
- Offset début : `10`
- Offset fin : `11`
- Adresse début : `6010`
- Adresse fin : `6011`
- Type déclaré : `uint16\[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_id`
- Adresse de début du champ suivant attendue : `6012`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6010`.
2. Vérifier que la lecture couvre strictement la plage `6010` à `6011`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6010`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `6010` à `6011`.
5. Vérifier que le champ suivant `campaign_id` commence à `6012` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6010` à `6011` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `campaign_id` commence à `6012` ;
- aucune dépendance implicite de lecture entre `reserved_6A` et `campaign_id` n'est observée.
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
