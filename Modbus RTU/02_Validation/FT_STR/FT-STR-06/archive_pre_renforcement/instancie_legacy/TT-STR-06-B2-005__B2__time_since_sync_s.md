# TT-STR-06-B2-005 — Bloc 2 — time_since_sync_s

## Objectif
Valider que le champ logique `time_since_sync_s` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `time_since_sync_s`
- Champs source : `time_since_sync_s_msw;time_since_sync_s_lsw`
- Offset début : `6`
- Offset fin : `7`
- Adresse début : `2006`
- Adresse fin : `2007`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps écoulé depuis dernière sync`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_time`
- Adresse de début du champ suivant attendue : `2008`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2006`.
2. Vérifier que la lecture couvre strictement la plage `2006` à `2007`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `2006`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `2006` à `2007`.
5. Vérifier que le champ suivant `prepared_time` commence à `2008` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2006` à `2007` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `prepared_time` commence à `2008` ;
- aucune dépendance implicite de lecture entre `time_since_sync_s` et `prepared_time` n'est observée.
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
