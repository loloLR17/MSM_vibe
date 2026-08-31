# TT-STR-05-B6-001 — Bloc 6 — reserved_6A

## Objectif
Valider que le champ réservé/sentinelle `reserved_6A` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6A`
- Champs source : `reserved_6A`
- Offset début : `10`
- Offset fin : `11`
- Adresse début : `6010`
- Adresse fin : `6011`
- Type déclaré : `uint16\[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_6B`
- Adresse de début du champ suivant attendue : `6058`
- Vérifier l'absence d'empiètement entre `reserved_6A` et `reserved_6B`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6010`.
2. Vérifier que la lecture couvre strictement la plage `6010` à `6011`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `6058`.
6. Vérifier que la longueur structurelle observée reste exactement de `2` registre(s).

## Résultat attendu
- la lecture de `reserved_6A` est possible sur la plage `6010` à `6011` ;
- la taille observée est exactement de `2` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_6B` commence à `6058` ;
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
