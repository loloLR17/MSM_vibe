# TT-STR-06-B7-010 — Bloc 7 — reset_cause

## Objectif
Valider que le champ logique `reset_cause` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `7`
- Champ logique : `reset_cause`
- Champs source : `reset_cause`
- Offset début : `11`
- Offset fin : `11`
- Adresse début : `7011`
- Adresse fin : `7011`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Cause dernier reset`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `internal_temp_dC`
- Adresse de début du champ suivant attendue : `7012`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7011`.
2. Vérifier que la lecture couvre strictement la plage `7011` à `7011`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `internal_temp_dC` commence à `7012` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `7011` à `7011` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `internal_temp_dC` commence à `7012` ;
- aucune dépendance implicite de lecture entre `reset_cause` et `internal_temp_dC` n'est observée.
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
