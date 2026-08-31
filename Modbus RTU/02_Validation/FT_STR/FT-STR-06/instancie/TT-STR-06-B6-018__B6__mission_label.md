# TT-STR-06-B6-018 — Bloc 6 — mission_label

## Objectif
Valider que le champ logique `mission_label` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `mission_label`
- Champs source : `mission_label`
- Offset début : `41`
- Offset fin : `56`
- Adresse début : `6041`
- Adresse fin : `6056`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Label mission`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `data_integrity_status`
- Adresse de début du champ suivant attendue : `6057`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6041`.
2. Vérifier que la lecture couvre strictement la plage `6041` à `6056`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6041`, avec une longueur strictement inférieure à `16`.
4. Relire ensuite le champ complet de `6041` à `6056`.
5. Vérifier que le champ suivant `data_integrity_status` commence à `6057` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6041` à `6056` est possible ;
- la taille observée est exactement de `16` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `data_integrity_status` commence à `6057` ;
- aucune dépendance implicite de lecture entre `mission_label` et `data_integrity_status` n'est observée.
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
