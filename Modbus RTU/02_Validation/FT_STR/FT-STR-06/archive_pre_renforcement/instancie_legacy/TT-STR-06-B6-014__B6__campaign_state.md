# TT-STR-06-B6-014 — Bloc 6 — campaign_state

## Objectif
Valider que le champ logique `campaign_state` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `campaign_state`
- Champs source : `campaign_state`
- Offset début : `20`
- Offset fin : `20`
- Adresse début : `6020`
- Adresse fin : `6020`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État campagne`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `duration_s`
- Adresse de début du champ suivant attendue : `6021`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6020`.
2. Vérifier que la lecture couvre strictement la plage `6020` à `6020`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `duration_s` commence à `6021` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6020` à `6020` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `duration_s` commence à `6021` ;
- aucune dépendance implicite de lecture entre `campaign_state` et `duration_s` n'est observée.
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
