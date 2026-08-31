# TT-STR-04-B4-001 — Bloc 4 — campaign_label

## Objectif
Valider que le champ ASCII fixe `campaign_label` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

## Référence mapping
- Bloc : `4`
- Champ logique : `campaign_label`
- Champs source : `campaign_label`
- Offset début : `60`
- Offset fin : `75`
- Adresse début : `4060`
- Adresse fin : `4075`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RW`
- Description : `Label campagne`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `4076`
- Vérifier l'absence d'empiètement entre `campaign_label` et `mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `4060`.
2. Vérifier que la lecture couvre strictement la plage `4060` à `4075`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier que le champ logique suivant commence à l'adresse `4076`.
7. Vérifier que la longueur structurelle observée reste exactement de `16` registre(s).

## Résultat attendu
- la lecture de `campaign_label` est possible sur la plage `4060` à `4075` ;
- la taille observée est exactement de `16` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- le champ suivant `mission_label` commence à `4076` ;
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
