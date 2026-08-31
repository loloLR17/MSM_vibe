# TT-STR-06-B1-005 — Bloc 1 — uptime_s

## Objectif
Valider que le champ logique `uptime_s` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `1`
- Champ logique : `uptime_s`
- Champs source : `uptime_s_msw;uptime_s_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `1004`
- Adresse fin : `1005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps de fonctionnement`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_reset_cause`
- Adresse de début du champ suivant attendue : `1006`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `1004`.
2. Vérifier que la lecture couvre strictement la plage `1004` à `1005`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `1004`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `1004` à `1005`.
5. Vérifier que le champ suivant `last_reset_cause` commence à `1006` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `1004` à `1005` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `last_reset_cause` commence à `1006` ;
- aucune dépendance implicite de lecture entre `uptime_s` et `last_reset_cause` n'est observée.
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
