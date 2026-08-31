# TT-STR-06-B4-060 — Bloc 4 — active_campaign_label

## Objectif
Valider que le champ logique `active_campaign_label` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_campaign_label`
- Champs source : `active_campaign_label`
- Offset début : `132`
- Offset fin : `147`
- Adresse début : `4132`
- Adresse fin : `4147`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Miroir actif, ASCII fixe 32 caractères, padding 0x00`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_mission_label`
- Adresse de début du champ suivant attendue : `4148`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4132`.
2. Vérifier que la lecture couvre strictement la plage `4132` à `4147`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `4132`, avec une longueur strictement inférieure à `16`.
4. Relire ensuite le champ complet de `4132` à `4147`.
5. Vérifier que le champ suivant `active_mission_label` commence à `4148` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4132` à `4147` est possible ;
- la taille observée est exactement de `16` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `active_mission_label` commence à `4148` ;
- aucune dépendance implicite de lecture entre `active_campaign_label` et `active_mission_label` n'est observée.
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
