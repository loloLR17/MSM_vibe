# TT-STR-06-B4-057 — Bloc 4 — reserved_4E_B

## Objectif
Valider que le champ logique `reserved_4E_B` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_B`
- Champs source : `reserved_4E_B`
- Offset début : `123`
- Offset fin : `127`
- Adresse début : `4123`
- Adresse fin : `4127`
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
- Champ suivant attendu : `active_campaign_context_id`
- Adresse de début du champ suivant attendue : `4128`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `5` registre(s) à partir de l'adresse `4123`.
2. Vérifier que la lecture couvre strictement la plage `4123` à `4127`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4123`, avec une longueur strictement inférieure à `5`.
4. Relire ensuite le champ complet de `4123` à `4127`.
5. Vérifier que le champ suivant `active_campaign_context_id` commence à `4128` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4123` à `4127` est possible ;
- la taille observée est exactement de `5` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_campaign_context_id` commence à `4128` ;
- aucune dépendance implicite de lecture entre `reserved_4E_B` et `active_campaign_context_id` n'est observée.
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
