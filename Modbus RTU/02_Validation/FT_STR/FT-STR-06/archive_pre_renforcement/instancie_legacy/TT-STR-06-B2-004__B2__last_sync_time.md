# TT-STR-06-B2-004 — Bloc 2 — last_sync_time

## Objectif
Valider que le champ logique `last_sync_time` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `last_sync_time`
- Champs source : `last_sync_time_msw;last_sync_time_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `2004`
- Adresse fin : `2005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Dernière synchronisation`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `time_since_sync_s`
- Adresse de début du champ suivant attendue : `2006`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2004`.
2. Vérifier que la lecture couvre strictement la plage `2004` à `2005`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `2004`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `2004` à `2005`.
5. Vérifier que le champ suivant `time_since_sync_s` commence à `2006` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2004` à `2005` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `time_since_sync_s` commence à `2006` ;
- aucune dépendance implicite de lecture entre `last_sync_time` et `time_since_sync_s` n'est observée.
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
