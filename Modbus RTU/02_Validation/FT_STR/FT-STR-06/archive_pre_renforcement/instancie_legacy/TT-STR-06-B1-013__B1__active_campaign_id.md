# TT-STR-06-B1-013 — Bloc 1 — active_campaign_id

## Objectif
Valider que le champ logique `active_campaign_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `1`
- Champ logique : `active_campaign_id`
- Champs source : `active_campaign_id_msw;active_campaign_id_lsw`
- Offset début : `13`
- Offset fin : `14`
- Adresse début : `1013`
- Adresse fin : `1014`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `ID campagne active`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `error_code`
- Adresse de début du champ suivant attendue : `1015`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `1013`.
2. Vérifier que la lecture couvre strictement la plage `1013` à `1014`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `1013`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `1013` à `1014`.
5. Vérifier que le champ suivant `error_code` commence à `1015` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `1013` à `1014` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `error_code` commence à `1015` ;
- aucune dépendance implicite de lecture entre `active_campaign_id` et `error_code` n'est observée.
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
