# TT-STR-04-B4-003 — Bloc 4 — active_campaign_label

## Objectif
Valider que le champ ASCII fixe `active_campaign_label` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

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
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ suivant attendu : `active_mission_label`
- Adresse de début du champ suivant attendue : `4148`
- Vérifier l'absence d'empiètement entre `active_campaign_label` et `active_mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4132`.
2. Vérifier que la lecture couvre strictement la plage `4132` à `4147`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier que le champ logique suivant commence à l'adresse `4148`.
7. Vérifier que la longueur structurelle observée reste exactement de `16` registre(s).

## Résultat attendu
- la lecture de `active_campaign_label` est possible sur la plage `4132` à `4147` ;
- la taille observée est exactement de `16` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- le champ suivant `active_mission_label` commence à `4148` ;
- aucun chevauchement n'est observé.
- le champ reste décodable sans heuristique.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- aucun caractère hors plage ASCII autorisée ;
- padding terminal conforme ;
- absence d’empiètement ;
- aucune ambiguïté de frontière.

## Classification
- Famille : `FT-STR-04`
- Sous-famille : `Encodage ASCII fixe`
- Niveau : `instancié`
