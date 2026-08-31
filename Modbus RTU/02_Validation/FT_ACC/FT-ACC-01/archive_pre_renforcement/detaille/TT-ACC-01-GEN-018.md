# TT-ACC-01-GEN-018 — Lecture conforme mapping (plage)

## Objectif
Vérifier qu’une plage déclarée lisible est effectivement lisible.

## Exigence(s) couverte(s)
- FT-ACC-01
- Référentiel unique de mapping Modbus TR2

## Préconditions
- mapping disponible ;
- cible identifiée comme lisible ;
- simulateur nominal déterministe ;
- état stable pendant le test.

## Données d’entrée
- adresse de départ : non instanciée ;
- longueur : selon le cas générique.

## Scénario / étapes
1. Sélectionner une zone conforme au périmètre du cas.
2. Émettre une requête de lecture Modbus nominale.
3. Contrôler la réponse retournée.
4. Vérifier l’absence d’exception et la conformité de longueur.
5. Vérifier l’absence d’effet de bord si applicable.

## Résultat attendu
- lecture acceptée ;
- réponse de longueur conforme ;
- aucune modification non spécifiée de l’image registre.

## Critères d’acceptation
- pas d’exception Modbus ;
- réponse exploitable ;
- cohérence avec le mapping ;
- verdict conforme au périmètre du cas.

## Mode d’exécution
- simulateur déterministe
- puis banc automatisé

## Automatisation possible
Oui

## Traces à conserver
- cible testée ;
- trame requête ;
- trame réponse ;
- verdict ;
- anomalie associée le cas échéant.

## Niveau de criticité
P0

## Remarques / limites
- fiche générique sans adresse ;
- la sémantique des valeurs est hors périmètre FT-ACC-01.

## Type de test
Cohérence documentaire
