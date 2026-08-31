# TT-STR-06-B4-010 — Bloc 4 — reserved_4A

## Objectif
Valider que le champ logique `reserved_4A` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4A`
- Champs source : `reserved_4A`
- Offset début : `14`
- Offset fin : `15`
- Adresse début : `4014`
- Adresse fin : `4015`
- Type déclaré : `uint16[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `sampling_frequency_hz`
- Adresse de début du champ suivant attendue : `4016`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4014`.
2. Vérifier que la lecture couvre strictement la plage `4014` à `4015`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4014`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4014` à `4015`.
5. Vérifier que le champ suivant `sampling_frequency_hz` commence à `4016` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4014` à `4015` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `sampling_frequency_hz` commence à `4016` ;
- aucune dépendance implicite de lecture entre `reserved_4A` et `sampling_frequency_hz` n'est observée.
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
