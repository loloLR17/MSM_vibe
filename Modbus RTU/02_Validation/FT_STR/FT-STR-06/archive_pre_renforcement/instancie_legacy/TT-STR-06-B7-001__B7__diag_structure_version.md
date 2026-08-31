# TT-STR-06-B7-001 — Bloc 7 — diag_structure_version

## Objectif
Valider que le champ logique `diag_structure_version` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `7`
- Champ logique : `diag_structure_version`
- Champs source : `diag_structure_version`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `7000`
- Adresse fin : `7000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version structure Bloc 7`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `system_health_status`
- Adresse de début du champ suivant attendue : `7001`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7000`.
2. Vérifier que la lecture couvre strictement la plage `7000` à `7000`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `system_health_status` commence à `7001` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `7000` à `7000` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `system_health_status` commence à `7001` ;
- aucune dépendance implicite de lecture entre `diag_structure_version` et `system_health_status` n'est observée.
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
