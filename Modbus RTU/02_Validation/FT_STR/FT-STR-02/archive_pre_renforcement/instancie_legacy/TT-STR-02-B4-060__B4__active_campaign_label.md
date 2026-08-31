# TT-STR-02-B4-060 — Bloc 4 — active_campaign_label

## Objectif
Valider que le champ logique `active_campaign_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_campaign_label`
- Nature : `declared_as_is`
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
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_mission_label`
- Adresse de début du champ suivant attendue : `4148`
- Vérifier l'absence d'empiètement entre `active_campaign_label` et `active_mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4132`.
2. Vérifier que la lecture couvre la plage `4132` à `4147` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4148`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `active_campaign_label` est possible sur la plage `4132` à `4147` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_mission_label` commence à `4148` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `ASCII fixe`.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- absence d’empiètement ;
- type structurellement cohérent ;
- aucune ambiguïté de frontière.

## Classification
- Famille : `FT-STR-02`
- Sous-famille : `Typage des champs`
- Niveau : `instancié`
