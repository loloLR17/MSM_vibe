# Incidents réellement observés, sans contrôle désactivé

- AccèsST intermittents : voirPROVENANCE et journaux.
- Import natifAltium : argumentAppendTo manquant, puis import expiré45s. Source extraite directement viaCFB/Pads6 et recoupéeODB, aucune CAO d’import partiel utilisée.
- Script candidat : suppressionZONE viawrapperSWIG non exposé (`thisown`) ; essais interrompus avant sauvegardePCB. Cuivre du candidat retiré par lecture structurée des noeudsS-expression (parenthèses et chaînes gérées), puis application des routes. Les originaux n’ont jamais été modifiés. Logs conservés.
- Premier routage local :7violationsDRC réellement remontées,0non-connecté/0parité. Cause : création automatique d’un via de raccord de préfixe trop proche d’une piste voisine ; début de route maintenant contraint à la couche du préfixe. J5 déplacé25µm réduisait la marge à unNC ; sa finDE déplacée de la même valeur. DRC final0 avec mêmes règles.
- Première comparaisonExcellon : coordonnées exportées auµm comparées à tort au0,1µm. Correction de la lecture à la précision effective du format, complétée par appariement individuel centres/diamètres. Erreur max réelle0,000131mm, sourceCAO non arrondie. Les tests historiques restent intacts.

Aucune suppression de test, règle ou assertion pour faire accepter le PCB. Aucun flash/debug/média ou matériel concerné.
