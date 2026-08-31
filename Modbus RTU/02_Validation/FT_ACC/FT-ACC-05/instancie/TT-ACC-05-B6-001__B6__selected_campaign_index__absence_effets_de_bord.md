# TT-ACC-05-B6-001 — Bloc 6 — selected_campaign_index

## Objectif
Vérifier qu’une écriture sur `selected_campaign_index` n’entraîne aucune modification hors de la plage ciblée.

## Référence mapping
- Bloc : 6
- Adresse début cible : 6003
- Adresse fin cible : 6003
- Type déclaré : uint16
- Nombre de registres : 1
- Accès documentaire : RW

## Périmètre de snapshot
- Bloc observé : 6
- Adresse début bloc : 6000
- Adresse fin bloc : 6063
- Bloc précédent : 5 (5000..5019)
- Bloc suivant : 7 (7000..7015)

## Étapes
1. Lire l’image complète du bloc `6` sur `6000..6063`.
2. Écrire exactement `1` registre(s) sur `6003..6003`.
3. Relire l’image complète du bloc `6`.
4. Calculer le diff adressé.
5. Vérifier que seules les adresses `6003..6003` diffèrent.
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
