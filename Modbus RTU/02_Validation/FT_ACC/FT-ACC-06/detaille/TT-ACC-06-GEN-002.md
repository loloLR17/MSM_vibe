# TT-ACC-06-GEN-002 — FC16 composite RW + réservé

## Objectif
Valider le rejet atomique d'une écriture FC16 couvrant au moins un registre RW et au moins un registre réservé.

## Préconditions
- V1 et GEL-MAP-V1 disponibles ;
- frontière RW/réservé identifiée ;
- état initial observable ;
- valeur de test choisie sans déclenchement fonctionnel parasite.

## Étapes
1. Lire les registres RW ciblés et les états associés pertinents.
2. Construire une FC16 couvrant simultanément la partie RW et la partie réservée.
3. Envoyer la requête.
4. Vérifier l'exception Modbus standard appropriée.
5. Relire les registres RW et états associés.
6. Vérifier qu'aucune valeur RW de la requête n'a été appliquée.
7. Vérifier l'absence d'effet interne ou fonctionnel.
8. Répéter la requête et confirmer le même verdict.

## Résultat attendu
- requête rejetée intégralement ;
- aucune exécution partielle ;
- aucun registre RW de la requête modifié ;
- aucun effet interne ;
- comportement déterministe.

## Criticité
P0
