# TT-STR-06-B4-059 — Bloc 4 — active_mission_id

## Objectif
Valider que le champ logique `active_mission_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_mission_id`
- Champs source : `active_mission_id`
- Offset début : `130`
- Offset fin : `131`
- Adresse début : `4130`
- Adresse fin : `4131`
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
- Champ suivant attendu : `active_campaign_label`
- Adresse de début du champ suivant attendue : `4132`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4130`.
2. Vérifier que la lecture couvre strictement la plage `4130` à `4131`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4130`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4130` à `4131`.
5. Vérifier que le champ suivant `active_campaign_label` commence à `4132` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4130` à `4131` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_campaign_label` commence à `4132` ;
- aucune dépendance implicite de lecture entre `active_mission_id` et `active_campaign_label` n'est observée.
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
