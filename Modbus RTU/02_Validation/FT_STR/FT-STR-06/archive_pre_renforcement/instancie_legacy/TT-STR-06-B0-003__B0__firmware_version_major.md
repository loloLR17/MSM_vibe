# TT-STR-06-B0-003 — Bloc 0 — firmware_version_major

## Objectif
Valider que le champ logique `firmware_version_major` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `0`
- Champ logique : `firmware_version_major`
- Champs source : `firmware_version_major`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `3`
- Adresse fin : `3`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version firmware majeure`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `firmware_version_minor`
- Adresse de début du champ suivant attendue : `4`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3`.
2. Vérifier que la lecture couvre strictement la plage `3` à `3`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `firmware_version_minor` commence à `4` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3` à `3` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `firmware_version_minor` commence à `4` ;
- aucune dépendance implicite de lecture entre `firmware_version_major` et `firmware_version_minor` n'est observée.
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
