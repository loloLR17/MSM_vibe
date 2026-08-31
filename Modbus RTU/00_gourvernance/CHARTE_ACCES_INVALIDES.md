# Doctrine de gestion des accès Modbus invalides

## 1. Référence normative

La présente doctrine est subordonnée à la **TR2 — Spécification Modbus RTU V1 gelée**, constituée des fichiers `bloc0.md` à `bloc7.md` et de `charte_typage.md`.

En cas de divergence, la spécification V1 gelée fait foi.

## 2. Accès Modbus invalide

Une requête Modbus est invalide lorsqu’elle vise notamment :

- une adresse inexistante ;
- un registre en lecture seule avec une opération d’écriture ;
- un registre réservé avec une opération d’écriture ;
- plus généralement, une adresse ou une opération non autorisée par le mapping dérivé de la spécification V1.

Toute requête Modbus invalide doit :

- générer une exception Modbus standard appropriée ;
- ne produire aucune modification de registre ni d’état interne ;
- ne jamais être exécutée partiellement ;
- être traitée de manière déterministe.

Aucun comportement implicite, aucune acceptation silencieuse et aucune correction automatique ne sont autorisés.

## 3. Lecture partielle d’une zone valide

Une lecture portant sur un sous-ensemble valide d’une zone exposée n’est **pas invalide du seul fait qu’elle est partielle**.

La validité de la requête dépend des adresses visées et des règles d’accès définies par la spécification et le mapping dérivé.

## 4. Valeur métier invalide dans un registre RW

Une valeur métier hors domaine écrite dans un registre explicitement `RW` ne constitue pas, à elle seule, un accès Modbus invalide.

Son acceptation, son rejet et les conséquences fonctionnelles éventuelles sont définis par les règles normatives du bloc concerné.

Il est interdit de transformer arbitrairement une erreur de domaine métier en erreur d’adressage Modbus.
