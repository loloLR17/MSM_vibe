# TT-ACC-05-B5-005 — Bloc 5 — cmd_request_param3

## Objectif
Vérifier qu’une écriture sur `cmd_request_param3` n’entraîne aucune modification hors de la plage ciblée.

## Référence mapping
- Bloc : 5
- Adresse début cible : 5004
- Adresse fin cible : 5005
- Type déclaré : uint32
- Nombre de registres : 2
- Accès documentaire : RW

## Périmètre de snapshot
- Bloc observé : 5
- Adresse début bloc : 5000
- Adresse fin bloc : 5019
- Bloc précédent : 4 (4000..4175)
- Bloc suivant : 6 (6000..6063)

## Étapes
1. Lire l’image complète du bloc `5` sur `5000..5019`.
2. Écrire exactement `2` registre(s) sur `5004..5005`.
3. Relire l’image complète du bloc `5`.
4. Calculer le diff adressé.
5. Vérifier que seules les adresses `5004..5005` diffèrent.
6. Vérifier que le voisinage immédiat et le reste du bloc sont inchangés.
7. Effectuer un contrôle inter-bloc ponctuel si applicable.

## Résultat attendu
- diff limité strictement à la cible ;
- aucune modification sur le reste du bloc ;
- aucun effet inter-bloc non documenté.

## Traces à conserver
- snapshot avant ;
- trame d’écriture ;
- snapshot après ;
- diff adressé ;
- verdict.

## Criticité
P0
