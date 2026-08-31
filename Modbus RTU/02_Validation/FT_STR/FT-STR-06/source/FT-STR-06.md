# FT-STR-06 — Fiche source

## Identification

- ID : `FT-STR-06`
- Nom : Accessibilité et découpage de lecture Modbus
- Famille parente : FT-STR
- Criticité : P0

## Objectif

Valider que toute plage d'adresses exposée par V1/GEL-MAP-V1 est lisible avec la fonction Modbus FC03 selon un découpage libre, tant que la requête respecte les adresses exposées et les limites du protocole.

## Règles normatives de validation

1. Une lecture d'un registre exposé isolé est valide.
2. Une lecture d'une sous-plage contiguë entièrement exposée est valide, même si elle ne couvre qu'une partie d'un champ logique multi-registres.
3. Une lecture contiguë peut traverser plusieurs champs logiques voisins si toutes les adresses demandées sont exposées.
4. Le découpage des champs logiques ne crée aucune contrainte implicite de requête.
5. FC03 limite une requête à 125 registres ; une zone exposée plus grande peut et doit être lue en plusieurs segments valides.
6. Une requête contenant au moins une adresse inexistante est invalide : exception Modbus standard appropriée, aucune réponse partielle, aucun effet de bord.
7. Une quantité nulle ou supérieure à la limite FC03 est invalide selon le protocole Modbus.

## Périmètre

Inclus :
- lectures unitaires ;
- sous-plages valides ;
- lectures multi-registres contiguës ;
- lectures traversant plusieurs champs logiques ;
- segmentation des blocs longs ;
- frontières de plages exposées ;
- lectures traversant une lacune non exposée ;
- quantités FC03 invalides.

Exclus :
- géométrie intrinsèque du mapping, couverte par FT-STR-01 ;
- typage, couvert par FT-STR-02 ;
- cohérence temporelle / snapshot, couverte par FT-STR-07 ;
- droits et refus d'écriture, couverts par FT-ACC ;
- performance temporelle Modbus non spécifiée.

## Préconditions

- FT-STR-01 gelée ;
- FT-STR-02 gelée ;
- GEL-MAP-V1 disponible ;
- communication Modbus opérationnelle pour l'exécution cible.

## Critères de réussite

- chaque segment valide demandé est retourné sans exigence de découpage implicite ;
- un champ multi-registres peut être lu partiellement si les adresses visées sont valides ;
- une lecture peut franchir une frontière entre champs voisins contigus ;
- le Bloc 4 est lisible par segmentation compatible FC03 ;
- toute requête incluant une lacune ou une quantité FC03 invalide est rejetée conformément à GEL-GOV-02 ;
- aucune réponse partielle n'est produite sur une requête invalide.

## Note sur les blocs

Les blocs exposés sont séparés par des plages d'adresses non exposées. Une requête qui tente de lire « plusieurs blocs » en une seule trame traverse donc nécessairement une lacune et est invalide.

## Note FT-PER

FT-PER désigne persistance/recovery. Toute ancienne utilisation de FT-PER pour désigner la performance Modbus est obsolète.
