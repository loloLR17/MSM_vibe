# TT-STR-06-B4-005 — Bloc 4 — config_state

## Objectif
Valider que le champ logique `config_state` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_state`
- Champs source : `config_state`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `4006`
- Adresse fin : `4006`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État configuration`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_error_code`
- Adresse de début du champ suivant attendue : `4007`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4006`.
2. Vérifier que la lecture couvre strictement la plage `4006` à `4006`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `config_error_code` commence à `4007` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4006` à `4006` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `config_error_code` commence à `4007` ;
- aucune dépendance implicite de lecture entre `config_state` et `config_error_code` n'est observée.
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
