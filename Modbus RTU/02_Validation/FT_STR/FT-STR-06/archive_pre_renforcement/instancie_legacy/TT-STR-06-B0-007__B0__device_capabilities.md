# TT-STR-06-B0-007 — Bloc 0 — device_capabilities

## Objectif
Valider que le champ logique `device_capabilities` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `0`
- Champ logique : `device_capabilities`
- Champs source : `device_capabilities`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `7`
- Adresse fin : `7`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Capacités du capteur`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `serial_number`
- Adresse de début du champ suivant attendue : `8`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7`.
2. Vérifier que la lecture couvre strictement la plage `7` à `7`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `serial_number` commence à `8` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `7` à `7` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `serial_number` commence à `8` ;
- aucune dépendance implicite de lecture entre `device_capabilities` et `serial_number` n'est observée.
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
