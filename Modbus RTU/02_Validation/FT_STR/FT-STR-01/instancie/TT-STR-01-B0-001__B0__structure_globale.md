# TT-STR-01-B0-001 — Bloc 0 — Structure globale

## Objectif
Valider la cohérence structurelle globale du bloc `0` :
- continuité de la portée couverte ;
- absence de chevauchement entre champs ;
- absence de trous internes dans la portée déclarée du bloc ;
- cohérence des frontières de début et de fin.

## Référence couverture
- Bloc : `0`
- Adresse début portée : `0`
- Adresse fin portée : `20`
- Registres couverts : `21`
- Registres dans la portée : `21`
- Nombre de champs bruts : `21`
- Premier champ : `device_id_msw`
- Dernier champ : `reserved`

## Préconditions
- Mapping unifié brut validé
- Couverture par bloc calculée
- Accès Modbus disponible si vérification instrumentée
- Aucun changement de spécification en cours sur le bloc `0`

## Contrôles à effectuer
- vérifier que le premier champ du bloc commence bien à `0` ;
- vérifier que le dernier champ du bloc se termine bien à `20` ;
- vérifier que la concaténation des plages de tous les champs ne crée aucun chevauchement ;
- aucun trou interne détecté dans la portée couverte du bloc.
- vérifier que l'ordre croissant des adresses est strictement respecté.

## Étapes
1. Lister tous les champs du bloc `0` dans l’ordre croissant des adresses.
2. Vérifier que chaque champ commence à une adresse supérieure ou égale à la fin du précédent + 1.
3. Vérifier que le premier champ commence à `0`.
4. Vérifier que le dernier champ se termine à `20`.
5. Vérifier que le nombre total de registres couverts est `21`.
6. Vérifier que le nombre de registres manquants à l’intérieur de la portée est `0`.

## Résultat attendu
- structure du bloc `0` cohérente ;
- aucune superposition de plages ;
- aucun trou interne dans la portée couverte ;
- bornes du bloc cohérentes avec la table de couverture.

## Critères d’acceptation
- ordre des adresses strictement croissant ;
- aucune plage chevauchante ;
- début conforme ;
- fin conforme ;
- couverture conforme ;
- aucun trou interne.

## Classification
- Famille : `FT-STR-01`
- Sous-famille : `Conformité structurelle`
- Niveau : `instancié`
