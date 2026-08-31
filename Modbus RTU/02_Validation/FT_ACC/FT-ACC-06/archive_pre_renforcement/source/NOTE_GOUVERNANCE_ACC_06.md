# NOTE_GOUVERNANCE_ACC_06.md

## Doctrine à intégrer

Toute requête Modbus invalide (hors plage, partielle, non autorisée) doit :
- générer une exception Modbus explicite ;
- ne produire aucune modification mémoire ;
- être traitée de manière déterministe.

Aucun comportement implicite (acceptation silencieuse, exécution partielle) n’est autorisé.

## Fichiers cibles recommandés
- `00_gourvernance/CHARTE_ARBORESCENCE.md`
- `02_Validation/FT_ACC/README.md`
- spécification protocolaire Modbus TR2, section gestion des erreurs d’adressage
