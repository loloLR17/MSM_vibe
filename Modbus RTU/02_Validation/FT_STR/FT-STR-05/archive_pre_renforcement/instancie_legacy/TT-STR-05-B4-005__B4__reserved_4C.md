# TT-STR-05-B4-005 — Bloc 4 — reserved_4C

## Objectif
Valider que le champ réservé/sentinelle `reserved_4C` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4C`
- Champs source : `reserved_4C`
- Offset début : `47`
- Offset fin : `55`
- Adresse début : `4047`
- Adresse fin : `4055`
- Type déclaré : `uint16[9]`
- Taille attendue : `9` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4D`
- Adresse de début du champ suivant attendue : `4096`
- Vérifier l'absence d'empiètement entre `reserved_4C` et `reserved_4D`.

## Étapes
1. Lire exactement `9` registre(s) à partir de l'adresse `4047`.
2. Vérifier que la lecture couvre strictement la plage `4047` à `4055`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `4096`.
6. Vérifier que la longueur structurelle observée reste exactement de `9` registre(s).

## Résultat attendu
- la lecture de `reserved_4C` est possible sur la plage `4047` à `4055` ;
- la taille observée est exactement de `9` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_4D` commence à `4096` ;
- aucun chevauchement n'est observé.
- le champ reste totalement neutre et sans information parasite.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- lecture à `0` sur 100% du champ ;
- stabilité totale ;
- absence d’empiètement ;
- aucune ambiguïté de frontière.

## Classification
- Famille : `FT-STR-05`
- Sous-famille : `Réservés et sentinelles`
- Niveau : `instancié`
