# TT-STR-06-B4-030 — Bloc 4 — reserved_4C

## Objectif
Valider que le champ logique `reserved_4C` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4C`
- Champs source : `reserved_4C`
- Offset début : `47`
- Offset fin : `55`
- Adresse début : `4047`
- Adresse fin : `4055`
- Type déclaré : `uint16[9]`
- Taille attendue : `9` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_context_id`
- Adresse de début du champ suivant attendue : `4056`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `9` registre(s) à partir de l'adresse `4047`.
2. Vérifier que la lecture couvre strictement la plage `4047` à `4055`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4047`, avec une longueur strictement inférieure à `9`.
4. Relire ensuite le champ complet de `4047` à `4055`.
5. Vérifier que le champ suivant `campaign_context_id` commence à `4056` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4047` à `4055` est possible ;
- la taille observée est exactement de `9` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `campaign_context_id` commence à `4056` ;
- aucune dépendance implicite de lecture entre `reserved_4C` et `campaign_context_id` n'est observée.
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
