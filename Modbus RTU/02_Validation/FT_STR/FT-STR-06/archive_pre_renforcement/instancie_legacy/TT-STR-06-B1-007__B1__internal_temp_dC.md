# TT-STR-06-B1-007 — Bloc 1 — internal_temp_dC

## Objectif
Valider que le champ logique `internal_temp_dC` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `1`
- Champ logique : `internal_temp_dC`
- Champs source : `internal_temp_dC`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `1007`
- Adresse fin : `1007`
- Type déclaré : `int16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Température interne (déci °C)`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `cpu_load_percent`
- Adresse de début du champ suivant attendue : `1008`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1007`.
2. Vérifier que la lecture couvre strictement la plage `1007` à `1007`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `cpu_load_percent` commence à `1008` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `1007` à `1007` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `cpu_load_percent` commence à `1008` ;
- aucune dépendance implicite de lecture entre `internal_temp_dC` et `cpu_load_percent` n'est observée.
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
