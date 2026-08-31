# TT-STR-06-B2-003 — Bloc 2 — current_time

## Objectif
Valider que le champ logique `current_time` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `current_time`
- Champs source : `current_time_msw;current_time_lsw`
- Offset début : `2`
- Offset fin : `3`
- Adresse début : `2002`
- Adresse fin : `2003`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps courant`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_sync_time`
- Adresse de début du champ suivant attendue : `2004`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2002`.
2. Vérifier que la lecture couvre strictement la plage `2002` à `2003`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `2002`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `2002` à `2003`.
5. Vérifier que le champ suivant `last_sync_time` commence à `2004` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2002` à `2003` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `last_sync_time` commence à `2004` ;
- aucune dépendance implicite de lecture entre `current_time` et `last_sync_time` n'est observée.
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
