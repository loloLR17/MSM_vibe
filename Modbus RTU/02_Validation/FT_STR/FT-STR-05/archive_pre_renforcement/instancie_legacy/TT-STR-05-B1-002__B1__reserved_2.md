# TT-STR-05-B1-002 — Bloc 1 — reserved_2

## Objectif
Valider que le champ réservé/sentinelle `reserved_2` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `1`
- Champ logique : `reserved_2`
- Champs source : `reserved_2`
- Offset début : `18`
- Offset fin : `18`
- Adresse début : `1018`
- Adresse fin : `1018`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_3`
- Adresse de début du champ suivant attendue : `1019`
- Vérifier l'absence d'empiètement entre `reserved_2` et `reserved_3`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1018`.
2. Vérifier que la lecture couvre strictement la plage `1018` à `1018`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `1019`.
6. Vérifier que la longueur structurelle observée reste exactement de `1` registre(s).

## Résultat attendu
- la lecture de `reserved_2` est possible sur la plage `1018` à `1018` ;
- la taille observée est exactement de `1` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_3` commence à `1019` ;
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
