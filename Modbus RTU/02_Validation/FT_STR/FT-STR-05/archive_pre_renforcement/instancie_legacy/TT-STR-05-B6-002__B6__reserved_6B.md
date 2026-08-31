# TT-STR-05-B6-002 — Bloc 6 — reserved_6B

## Objectif
Valider que le champ réservé/sentinelle `reserved_6B` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6B`
- Champs source : `reserved_6B`
- Offset début : `58`
- Offset fin : `63`
- Adresse début : `6058`
- Adresse fin : `6063`
- Type déclaré : `uint16\[6]`
- Taille attendue : `6` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ terminal réservé du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `6` registre(s) à partir de l'adresse `6058`.
2. Vérifier que la lecture couvre strictement la plage `6058` à `6063`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier qu'aucune extension implicite du champ n'est observable au-delà de l'adresse de fin spécifiée.
6. Vérifier que la longueur structurelle observée reste exactement de `6` registre(s).

## Résultat attendu
- la lecture de `reserved_6B` est possible sur la plage `6058` à `6063` ;
- la taille observée est exactement de `6` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- aucune extension implicite du champ au-delà de la frontière spécifiée.
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
