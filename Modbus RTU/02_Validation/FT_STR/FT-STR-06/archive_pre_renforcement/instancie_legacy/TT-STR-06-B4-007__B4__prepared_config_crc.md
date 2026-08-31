# TT-STR-06-B4-007 — Bloc 4 — prepared_config_crc

## Objectif
Valider que le champ logique `prepared_config_crc` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `prepared_config_crc`
- Champs source : `prepared_config_crc`
- Offset début : `8`
- Offset fin : `9`
- Adresse début : `4008`
- Adresse fin : `4009`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `CRC configuration préparée`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_config_crc`
- Adresse de début du champ suivant attendue : `4010`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4008`.
2. Vérifier que la lecture couvre strictement la plage `4008` à `4009`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4008`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4008` à `4009`.
5. Vérifier que le champ suivant `active_config_crc` commence à `4010` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4008` à `4009` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_config_crc` commence à `4010` ;
- aucune dépendance implicite de lecture entre `prepared_config_crc` et `active_config_crc` n'est observée.
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
