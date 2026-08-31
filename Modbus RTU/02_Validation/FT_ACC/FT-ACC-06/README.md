# FT-ACC-06 — README

## Objet
Valider le comportement des accès Modbus invalides :
- hors plage ;
- partiellement valides / partiellement invalides ;
- franchissant une frontière logique non autorisée ;
- de longueur incompatible avec le contrat d’exposition.

## Doctrine retenue
**Choix A**

Toute requête invalide doit :
- générer une exception Modbus explicite ;
- ne produire aucune modification mémoire ;
- être traitée de manière déterministe.

Aucune exécution partielle silencieuse n’est autorisée.

## Structure
```text
FT-ACC-06/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```
