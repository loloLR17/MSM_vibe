# TT-STR-01-B1-001 — Bloc 1 — Structure globale

## Objectif
Valider la cohérence structurelle globale du bloc `1` :
- continuité de la portée couverte ;
- absence de chevauchement entre champs ;
- absence de trous internes dans la portée déclarée du bloc ;
- cohérence des frontières de début et de fin.

## Référence couverture
- Bloc : `1`
- Adresse début portée : `1000`
- Adresse fin portée : `1019`
- Registres couverts : `20`
- Registres dans la portée : `20`
- Nombre de champs bruts : `20`
- Premier champ : `system_status`
- Dernier champ : `reserved_3`

## Préconditions
- Mapping unifié brut validé
- Couverture par bloc calculée
- Accès Modbus disponible si vérification instrumentée
- Aucun changement de spécification en cours sur le bloc `1`

## Contrôles à effectuer
- vérifier que le premier champ du bloc commence bien à `1000` ;
- vérifier que le dernier champ du bloc se termine bien à `1019` ;
- vérifier que la concaténation des plages de tous les champs ne crée aucun chevauchement ;
- aucun trou interne détecté dans la portée couverte du bloc.
- vérifier que l'ordre croissant des adresses est strictement respecté.

## Étapes
1. Lister tous les champs du bloc `1` dans l’ordre croissant des adresses.
2. Vérifier que chaque champ commence à une adresse supérieure ou égale à la fin du précédent + 1.
3. Vérifier que le premier champ commence à `1000`.
4. Vérifier que le dernier champ se termine à `1019`.
5. Vérifier que le nombre total de registres couverts est `20`.
6. Vérifier que le nombre de registres manquants à l’intérieur de la portée est `0`.

## Résultat attendu
- structure du bloc `1` cohérente ;
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
