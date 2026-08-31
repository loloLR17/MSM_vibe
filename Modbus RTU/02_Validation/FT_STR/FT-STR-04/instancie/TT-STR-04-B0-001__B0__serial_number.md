# TT-STR-04-B0-001 — Bloc 0 — serial_number

## Objectif
Valider que le champ ASCII fixe `serial_number` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

## Référence mapping
- Bloc : `0`
- Champ logique : `serial_number`
- Champs source : `serial_number_r0;serial_number_r1;serial_number_r2;serial_number_r3;serial_number_r4;serial_number_r5;serial_number_r6;serial_number_r7`
- Offset début : `8`
- Offset fin : `15`
- Adresse début : `8`
- Adresse fin : `15`
- Type déclaré : `ASCII fixe`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Numéro de série`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ suivant attendu : `manufacturer`
- Adresse de début du champ suivant attendue : `16`
- Vérifier l'absence d'empiètement entre `serial_number` et `manufacturer`.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `8`.
2. Vérifier que la lecture couvre strictement la plage `8` à `15`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier que le champ logique suivant commence à l'adresse `16`.
7. Vérifier que la longueur structurelle observée reste exactement de `8` registre(s).

## Résultat attendu
- la lecture de `serial_number` est possible sur la plage `8` à `15` ;
- la taille observée est exactement de `8` registre(s) ;
- les octets utiles sont compatibles ASCII ;
- la zone terminale non utilisée est paddée en `0x00` ;
- le champ suivant `manufacturer` commence à `16` ;
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
