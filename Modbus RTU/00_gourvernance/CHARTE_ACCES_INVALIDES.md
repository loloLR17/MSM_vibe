\## Doctrine de gestion des accès invalides



Toute requête Modbus invalide (hors plage, partielle, non autorisée) doit :



\- générer une exception Modbus explicite (ex: ILLEGAL DATA ADDRESS) ;

\- ne produire aucune modification mémoire ;

\- être traitée de manière déterministe.



Aucun comportement implicite (acceptation silencieuse, exécution partielle) n’est autorisé.

