# TT-STR-06-B4-004 — Bloc 4 — active_config_id

## Objectif
Valider que le champ logique `active_config_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_config_id`
- Champs source : `active_config_id`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `4004`
- Adresse fin : `4005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `ID configuration active`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_state`
- Adresse de début du champ suivant attendue : `4006`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4004`.
2. Vérifier que la lecture couvre strictement la plage `4004` à `4005`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4004`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4004` à `4005`.
5. Vérifier que le champ suivant `config_state` commence à `4006` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4004` à `4005` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `config_state` commence à `4006` ;
- aucune dépendance implicite de lecture entre `active_config_id` et `config_state` n'est observée.
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
