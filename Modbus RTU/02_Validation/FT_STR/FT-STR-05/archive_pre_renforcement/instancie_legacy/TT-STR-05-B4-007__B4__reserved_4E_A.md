# TT-STR-05-B4-007 — Bloc 4 — reserved_4E_A

## Objectif
Valider que le champ réservé/sentinelle `reserved_4E_A` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_A`
- Champs source : `reserved_4E_A`
- Offset début : `111`
- Offset fin : `115`
- Adresse début : `4111`
- Adresse fin : `4115`
- Type déclaré : `uint16[5]`
- Taille attendue : `5` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4E_B`
- Adresse de début du champ suivant attendue : `4123`
- Vérifier l'absence d'empiètement entre `reserved_4E_A` et `reserved_4E_B`.

## Étapes
1. Lire exactement `5` registre(s) à partir de l'adresse `4111`.
2. Vérifier que la lecture couvre strictement la plage `4111` à `4115`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `4123`.
6. Vérifier que la longueur structurelle observée reste exactement de `5` registre(s).

## Résultat attendu
- la lecture de `reserved_4E_A` est possible sur la plage `4111` à `4115` ;
- la taille observée est exactement de `5` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_4E_B` commence à `4123` ;
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
