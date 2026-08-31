# FT-ACC-06 — Fiche source

## 1. Identification
- **ID** : FT-ACC-06
- **Nom** : Rejet atomique des écritures composites invalides
- **Famille** : FT-ACC
- **Criticité** : P0

## 2. Objectif
Valider qu'une requête FC16 contenant au moins un registre non inscriptible est rejetée dans son ensemble, sans exécution partielle des registres RW présents dans la même requête.

## 3. Règle normative
Une requête FC16 est considérée invalide si sa plage contient au moins un registre dont l'écriture est interdite par la V1 : registre RO ou registre réservé.

Pour toute requête invalide :
- une exception Modbus standard appropriée est renvoyée ;
- aucun registre ciblé n'est modifié ;
- aucun état interne n'est modifié ;
- aucune partie valide de la requête n'est exécutée ;
- une répétition identique produit le même comportement.

## 4. Périmètre inclus
- frontière RW → RO ;
- frontière RO → RW ;
- frontière RW → réservé ;
- frontière réservé → RW ;
- contrôle explicite de l'atomicité du rejet ;
- répétition déterministe.

## 5. Périmètre exclu
- lectures partielles ou traversant des champs logiques contigus : valides si toutes les adresses sont exposées, couvertes par FT-STR-06 ;
- lecture d'adresses inexistantes et quantités FC03 invalides : FT-STR-06 ;
- écriture RO isolée : FT-ACC-03 ;
- écriture réservée isolée : FT-ACC-04 ;
- valeur métier invalide dans une cible RW valide : FT-LIM ;
- robustesse de trame/CRC/liaison.

## 6. Méthode
Pour chaque frontière instanciée :
1. capturer les valeurs pertinentes avant essai ;
2. construire une FC16 couvrant simultanément au moins un registre RW et au moins un registre non inscriptible ;
3. utiliser des valeurs de test non déclenchantes lorsque la cible appartient à une zone de commande ;
4. envoyer la requête ;
5. vérifier l'exception ;
6. relire les registres RW inclus et les états associés pertinents ;
7. confirmer l'absence totale d'exécution partielle ;
8. répéter la même requête et confirmer le déterminisme.

## 7. Critères d'acceptation
La sous-famille est satisfaite si toutes les frontières actives :
- sont rejetées par exception Modbus standard appropriée ;
- ne modifient aucun registre RW pourtant inclus dans la requête ;
- ne produisent aucun effet interne ;
- ne produisent aucune exécution partielle ;
- restent déterministes.

## 8. Note sur les adresses inexistantes
Le mapping V1 courant ne fournit pas de frontière adjacente permettant d'isoler proprement une requête FC16 `RW + adresse inexistante` sans rencontrer auparavant un registre RO ou réservé. Aucun test artificiel n'est donc créé pour prétendre isoler ce mécanisme. Les adresses inexistantes restent couvertes structurellement par FT-STR-06 et GEL-GOV-02.
