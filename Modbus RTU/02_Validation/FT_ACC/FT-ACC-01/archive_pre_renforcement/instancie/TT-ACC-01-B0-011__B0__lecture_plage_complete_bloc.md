# TT-ACC-01-B0-011 — Bloc 0 — lecture_plage_complete_bloc

## Objectif
Vérifier que la plage complète couverte du bloc 0 est accessible en lecture multi-registres sans erreur.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping de couverture bloc

## Référence mapping
- Adresse début : 0
- Adresse fin : 20
- Couverture continue : 21 registres
- Registres manquants internes : 0

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Taille de lecture conforme aux limites supportées par le banc

## Données d’entrée
- Adresse de départ : 0
- Longueur : 21

## Scénario / étapes
1. Lire la plage complète du bloc 0, du registre 0 au registre 20.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 21 registres.
4. Vérifier que la lecture de bloc ne provoque pas d’effet de bord observable.

## Résultat attendu
- la lecture est acceptée ;
- la longueur retournée est conforme ;
- la plage complète documentée du bloc 0 est lisible.

## Critères d’acceptation
- lecture multi-registres réussie ;
- longueur conforme ;
- aucune divergence mapping ↔ comportement réel ;
- aucun effet de bord observable.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Automatisation possible
Oui

## Traces à conserver
- trame requête ;
- trame réponse ;
- dump brut de la plage lue ;
- verdict ;
- anomalie associée le cas échéant.

## Niveau de criticité
P0

## Remarques / limites
- cette fiche complète les fiches unitaires par champ ;
- elle ne valide pas la sémantique bloc ni le hors-plage.
