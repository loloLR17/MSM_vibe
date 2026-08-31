# TT-STR-06-B4-033 — Bloc 4 — campaign_label

## Objectif
Valider que le champ logique `campaign_label` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `campaign_label`
- Champs source : `campaign_label`
- Offset début : `60`
- Offset fin : `75`
- Adresse début : `4060`
- Adresse fin : `4075`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label campagne`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `4076`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4060`.
2. Vérifier que la lecture couvre strictement la plage `4060` à `4075`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4060`, avec une longueur strictement inférieure à `16`.
4. Relire ensuite le champ complet de `4060` à `4075`.
5. Vérifier que le champ suivant `mission_label` commence à `4076` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4060` à `4075` est possible ;
- la taille observée est exactement de `16` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `mission_label` commence à `4076` ;
- aucune dépendance implicite de lecture entre `campaign_label` et `mission_label` n'est observée.
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
