# TT-STR-06-B6-004 — Bloc 6 — selected_campaign_index

## Objectif
Valider que le champ logique `selected_campaign_index` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `selected_campaign_index`
- Champs source : `selected_campaign_index`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `6003`
- Adresse fin : `6003`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Index sélectionné`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `selected_campaign_valid`
- Adresse de début du champ suivant attendue : `6004`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6003`.
2. Vérifier que la lecture couvre strictement la plage `6003` à `6003`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `selected_campaign_valid` commence à `6004` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6003` à `6003` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `selected_campaign_valid` commence à `6004` ;
- aucune dépendance implicite de lecture entre `selected_campaign_index` et `selected_campaign_valid` n'est observée.
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
