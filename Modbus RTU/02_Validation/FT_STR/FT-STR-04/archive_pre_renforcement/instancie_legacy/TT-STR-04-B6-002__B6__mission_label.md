# TT-STR-04-B6-002 — Bloc 6 — mission_label

## Objectif
Valider que le champ ASCII fixe `mission_label` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

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
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ terminal ASCII du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6041`.
2. Vérifier que la lecture couvre strictement la plage `6041` à `6056`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier qu'aucune extension implicite du champ n'est observable au-delà de l'adresse de fin spécifiée.
7. Vérifier que la longueur structurelle observée reste exactement de `16` registre(s).

## Résultat attendu
- la lecture de `mission_label` est possible sur la plage `6041` à `6056` ;
- la taille observée est exactement de `16` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- aucune extension implicite du champ au-delà de la frontière spécifiée.
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
