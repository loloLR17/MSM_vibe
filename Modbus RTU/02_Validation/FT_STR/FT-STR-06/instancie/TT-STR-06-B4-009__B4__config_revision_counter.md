# TT-STR-06-B4-009 — Bloc 4 — config_revision_counter

## Objectif
Valider que le champ logique `config_revision_counter` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_revision_counter`
- Champs source : `config_revision_counter`
- Offset début : `12`
- Offset fin : `13`
- Adresse début : `4012`
- Adresse fin : `4013`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Compteur de révision`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4A`
- Adresse de début du champ suivant attendue : `4014`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4012`.
2. Vérifier que la lecture couvre strictement la plage `4012` à `4013`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4012`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `4012` à `4013`.
5. Vérifier que le champ suivant `reserved_4A` commence à `4014` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4012` à `4013` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `reserved_4A` commence à `4014` ;
- aucune dépendance implicite de lecture entre `config_revision_counter` et `reserved_4A` n'est observée.
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
