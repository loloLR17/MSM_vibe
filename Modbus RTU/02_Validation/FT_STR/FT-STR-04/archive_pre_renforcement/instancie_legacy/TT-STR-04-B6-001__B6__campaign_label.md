# TT-STR-04-B6-001 — Bloc 6 — campaign_label

## Objectif
Valider que le champ ASCII fixe `campaign_label` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

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
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `6041`
- Vérifier l'absence d'empiètement entre `campaign_label` et `mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6025`.
2. Vérifier que la lecture couvre strictement la plage `6025` à `6040`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier que le champ logique suivant commence à l'adresse `6041`.
7. Vérifier que la longueur structurelle observée reste exactement de `16` registre(s).

## Résultat attendu
- la lecture de `campaign_label` est possible sur la plage `6025` à `6040` ;
- la taille observée est exactement de `16` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- le champ suivant `mission_label` commence à `6041` ;
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
