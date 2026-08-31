# TT-ACC-04-GEN-002 — Écriture interdite sur zone réservée multi-registres

## Objectif
Vérifier qu’une écriture visant une zone logique réservée de plusieurs registres est rejetée intégralement.

## Préconditions
- cible issue du mapping gelé et identifiée comme réservée ;
- état du simulateur stable ;
- état de référence de toute la zone capturé.

## Étapes
1. Capturer l’état de référence de la zone et des états pertinents.
2. Tenter une écriture de la zone réservée complète avec plusieurs valeurs de test.
3. Vérifier la réception d’une exception Modbus standard appropriée.
4. Relire toute la zone et les états surveillés.
5. Vérifier qu’aucun mot n’a été modifié.
6. Répéter si nécessaire pour vérifier le déterminisme.

## Résultat attendu
- requête rejetée ;
- aucun mot de la zone modifié ;
- aucun état interne modifié du fait de la requête ;
- aucune exécution partielle ;
- comportement déterministe.

## Critère d’échec
Toute acceptation, même partielle ou silencieusement ignorée, est non conforme.
