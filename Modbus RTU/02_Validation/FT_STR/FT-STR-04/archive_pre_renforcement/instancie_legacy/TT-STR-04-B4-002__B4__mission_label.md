# TT-STR-04-B4-002 — Bloc 4 — mission_label

## Objectif
Valider que le champ ASCII fixe `mission_label` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

## Référence mapping
- Bloc : `4`
- Champ logique : `mission_label`
- Champs source : `mission_label`
- Offset début : `76`
- Offset fin : `91`
- Adresse début : `4076`
- Adresse fin : `4091`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label mission`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ suivant attendu : `active_campaign_label`
- Adresse de début du champ suivant attendue : `4132`
- Vérifier l'absence d'empiètement entre `mission_label` et `active_campaign_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4076`.
2. Vérifier que la lecture couvre strictement la plage `4076` à `4091`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier que le champ logique suivant commence à l'adresse `4132`.
7. Vérifier que la longueur structurelle observée reste exactement de `16` registre(s).

## Résultat attendu
- la lecture de `mission_label` est possible sur la plage `4076` à `4091` ;
- la taille observée est exactement de `16` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- le champ suivant `active_campaign_label` commence à `4132` ;
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
