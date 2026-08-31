# TT-STR-06-B0-001 — Bloc 0 — device_id

## Objectif
Valider que le champ logique `device_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `0`
- Champ logique : `device_id`
- Champs source : `device_id_msw;device_id_lsw`
- Offset début : `0`
- Offset fin : `1`
- Adresse début : `0`
- Adresse fin : `1`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Identifiant unique capteur`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `hardware_version`
- Adresse de début du champ suivant attendue : `2`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `0`.
2. Vérifier que la lecture couvre strictement la plage `0` à `1`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `0`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `0` à `1`.
5. Vérifier que le champ suivant `hardware_version` commence à `2` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `0` à `1` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `hardware_version` commence à `2` ;
- aucune dépendance implicite de lecture entre `device_id` et `hardware_version` n'est observée.
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
