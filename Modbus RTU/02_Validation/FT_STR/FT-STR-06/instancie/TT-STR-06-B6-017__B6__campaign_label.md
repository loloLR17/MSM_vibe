# TT-STR-06-B6-017 — Bloc 6 — campaign_label

## Objectif
Valider que le champ logique `campaign_label` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `campaign_label`
- Champs source : `campaign_label`
- Offset début : `25`
- Offset fin : `40`
- Adresse début : `6025`
- Adresse fin : `6040`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Label campagne`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `6041`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6025`.
2. Vérifier que la lecture couvre strictement la plage `6025` à `6040`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6025`, avec une longueur strictement inférieure à `16`.
4. Relire ensuite le champ complet de `6025` à `6040`.
5. Vérifier que le champ suivant `mission_label` commence à `6041` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6025` à `6040` est possible ;
- la taille observée est exactement de `16` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `mission_label` commence à `6041` ;
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
