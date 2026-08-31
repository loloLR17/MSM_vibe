# FT-ACC-05 — Vue d’ensemble instanciée

Couverture active : **35 champs logiques RW** issus du mapping gelé.

- Bloc 2 : 1 — préparation de l’heure sans synchronisation implicite.
- Bloc 4 : 26 — configuration préparée ; `config_state -> BROUILLON` autorisé selon V1, image active inchangée.
- Bloc 5 : 7 — préparation de commande sans déclenchement ; `cmd_request_control` testé à `0x0000`.
- Bloc 6 : 1 — sélection de campagne avec mise à jour normative de la vue RO autorisée.

Le verdict porte sur l’absence d’**effets non spécifiés**, et non sur l’absence absolue de changements hors cible.
