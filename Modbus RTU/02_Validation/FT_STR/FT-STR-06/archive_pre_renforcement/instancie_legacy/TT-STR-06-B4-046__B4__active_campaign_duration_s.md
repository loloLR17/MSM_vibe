# TT-STR-06-B4-046 — Bloc 4 — active_campaign_duration_s

## Objectif
Valider que le champ logique `active_campaign_duration_s` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_campaign_duration_s`
- Champs source : `active_campaign_duration_s`
- Offset début : `106`
- Offset fin : `107`
- Adresse début : `4106`
- Adresse fin : `4107`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_storage_mode`
- Adresse de début du champ suivant attendue : `4108`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4106`.
2. Vérifier que la lecture couvre strictement la plage `4106` à `4107`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4106`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4106` à `4107`.
5. Vérifier que le champ suivant `active_storage_mode` commence à `4108` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4106` à `4107` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_storage_mode` commence à `4108` ;
- aucune dépendance implicite de lecture entre `active_campaign_duration_s` et `active_storage_mode` n'est observée.
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
