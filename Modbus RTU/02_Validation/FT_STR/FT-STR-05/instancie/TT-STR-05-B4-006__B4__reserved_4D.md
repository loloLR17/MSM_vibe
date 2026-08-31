# TT-STR-05-B4-006 — Bloc 4 — reserved_4D

## Objectif
Valider que le champ réservé/sentinelle `reserved_4D` est exposé sur la bonne plage, avec la bonne taille, qu’il lit `0` et qu’il reste parfaitement stable.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4D`
- Champs source : `reserved_4D`
- Offset début : `96`
- Offset fin : `99`
- Adresse début : `4096`
- Adresse fin : `4099`
- Type déclaré : `uint16[4]`
- Taille attendue : `4` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel
- État capteur stable
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4E_A`
- Adresse de début du champ suivant attendue : `4111`
- Vérifier l'absence d'empiètement entre `reserved_4D` et `reserved_4E_A`.

## Étapes
1. Lire exactement `4` registre(s) à partir de l'adresse `4096`.
2. Vérifier que la lecture couvre strictement la plage `4096` à `4099`.
3. Vérifier que tous les registres lus valent `0`.
4. Répéter la lecture au moins 10 fois et vérifier l'absence totale de variation.
5. Vérifier que le champ logique suivant commence à l'adresse `4111`.
6. Vérifier que la longueur structurelle observée reste exactement de `4` registre(s).

## Résultat attendu
- la lecture de `reserved_4D` est possible sur la plage `4096` à `4099` ;
- la taille observée est exactement de `4` registre(s) ;
- tous les registres du champ valent `0` ;
- aucune variation n'est observée sur les lectures répétées ;
- le champ suivant `reserved_4E_A` commence à `4111` ;
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
