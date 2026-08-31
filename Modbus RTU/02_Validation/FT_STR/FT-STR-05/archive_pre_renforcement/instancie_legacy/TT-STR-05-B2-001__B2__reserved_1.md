# TT-STR-05-B2-001 — Bloc 2 — reserved_1

## Objectif
Valider que le champ réservé/sentinelle `reserved_1` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `2`
- Champ logique : `reserved_1`
- Champs source : `reserved_1`
- Offset début : `14`
- Offset fin : `14`
- Adresse début : `2014`
- Adresse fin : `2014`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_2`
- Adresse de début du champ suivant attendue : `2015`
- Vérifier l'absence d'empiètement entre `reserved_1` et `reserved_2`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2014`.
2. Vérifier que la lecture couvre strictement la plage `2014` à `2014`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `2015`.
6. Vérifier que la longueur structurelle observée reste exactement de `1` registre(s).

## Résultat attendu
- la lecture de `reserved_1` est possible sur la plage `2014` à `2014` ;
- la taille observée est exactement de `1` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_2` commence à `2015` ;
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
