# TT-STR-05-B4-009 — Bloc 4 — reserved_4E_C

## Objectif
Valider que le champ réservé/sentinelle `reserved_4E_C` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_C`
- Champs source : `reserved_4E_C`
- Offset début : `168`
- Offset fin : `175`
- Adresse début : `4168`
- Adresse fin : `4175`
- Type déclaré : `uint16[8]`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ terminal réservé du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `4168`.
2. Vérifier que la lecture couvre strictement la plage `4168` à `4175`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier qu'aucune extension implicite du champ n'est observable au-delà de l'adresse de fin spécifiée.
6. Vérifier que la longueur structurelle observée reste exactement de `8` registre(s).

## Résultat attendu
- la lecture de `reserved_4E_C` est possible sur la plage `4168` à `4175` ;
- la taille observée est exactement de `8` registre(s) ;
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
