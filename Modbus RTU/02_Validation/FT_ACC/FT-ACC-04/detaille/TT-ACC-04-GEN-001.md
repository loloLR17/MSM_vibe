# TT-ACC-04-GEN-001 — Écriture interdite sur réservé mono-registre

## Objectif
Vérifier qu’une écriture visant un registre réservé unique est rejetée.

## Préconditions
- cible issue du mapping gelé et identifiée comme réservée ;
- état du simulateur stable ;
- état de référence capturé.

## Étapes
1. Capturer l’état de référence de la cible et des états pertinents.
2. Tenter une écriture sur le registre réservé avec une valeur de test.
3. Vérifier la réception d’une exception Modbus standard appropriée.
4. Relire la cible et les états surveillés.
5. Répéter si nécessaire pour vérifier le déterminisme.

## Résultat attendu
- requête rejetée ;
- aucune modification de registre ;
- aucun état interne modifié du fait de la requête ;
- aucune exécution partielle ;
- comportement déterministe.

## Critère d’échec
Toute écriture acceptée, y compris silencieusement sans effet observable, est non conforme.
