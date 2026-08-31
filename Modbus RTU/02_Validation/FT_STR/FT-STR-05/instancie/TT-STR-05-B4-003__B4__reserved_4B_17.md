# TT-STR-05-B4-003 — Bloc 4 — reserved_4B_17

## Objectif
Valider que le champ réservé/sentinelle `reserved_4B_17` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4B_17`
- Champs source : `reserved_4B_17`
- Offset début : `17`
- Offset fin : `17`
- Adresse début : `4017`
- Adresse fin : `4017`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4B`
- Adresse de début du champ suivant attendue : `4028`
- Vérifier l'absence d'empiètement entre `reserved_4B_17` et `reserved_4B`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4017`.
2. Vérifier que la lecture couvre strictement la plage `4017` à `4017`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `4028`.
6. Vérifier que la longueur structurelle observée reste exactement de `1` registre(s).

## Résultat attendu
- la lecture de `reserved_4B_17` est possible sur la plage `4017` à `4017` ;
- la taille observée est exactement de `1` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_4B` commence à `4028` ;
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
