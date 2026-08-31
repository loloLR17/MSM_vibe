# TT-STR-04-B0-002 — Bloc 0 — manufacturer

## Objectif
Valider que le champ ASCII fixe `manufacturer` est exposé sur la bonne plage, avec la bonne taille, un contenu compatible ASCII et un padding terminal conforme.

## Référence mapping
- Bloc : `0`
- Champ logique : `manufacturer`
- Champs source : `manufacturer_r0;manufacturer_r1;manufacturer_r2;manufacturer_r3`
- Offset début : `16`
- Offset fin : `19`
- Adresse début : `16`
- Adresse fin : `19`
- Type déclaré : `ASCII fixe`
- Taille attendue : `4` registre(s)
- Accès : `RO`
- Description : `Fabricant`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible
- Méthode d’analyse des octets disponible côté banc

## Contrôle de frontière
- Champ terminal ASCII du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `4` registre(s) à partir de l'adresse `16`.
2. Vérifier que la lecture couvre strictement la plage `16` à `19`.
3. Reconstituer la séquence d’octets correspondant au champ lu.
4. Vérifier que la zone utile contient uniquement des caractères compatibles ASCII selon la spécification projet.
5. Vérifier que toute zone non utilisée en fin de champ est paddée en `0x00`.
6. Vérifier qu'aucune extension implicite du champ n'est observable au-delà de l'adresse de fin spécifiée.
7. Vérifier que la longueur structurelle observée reste exactement de `4` registre(s).

## Résultat attendu
- la lecture de `manufacturer` est possible sur la plage `16` à `19` ;
- la taille observée est exactement de `4` registre(s) ;
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
