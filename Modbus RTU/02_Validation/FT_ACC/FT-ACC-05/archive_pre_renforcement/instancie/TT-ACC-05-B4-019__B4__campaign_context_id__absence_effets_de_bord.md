# TT-ACC-05-B4-019 — Bloc 4 — campaign_context_id

## Objectif
Vérifier qu’une écriture sur `campaign_context_id` n’entraîne aucune modification hors de la plage ciblée.

## Référence mapping
- Bloc : 4
- Adresse début cible : 4056
- Adresse fin cible : 4057
- Type déclaré : uint32
- Nombre de registres : 2
- Accès documentaire : RW

## Périmètre de snapshot
- Bloc observé : 4
- Adresse début bloc : 4000
- Adresse fin bloc : 4175
- Bloc précédent : 3 (3000..3047)
- Bloc suivant : 5 (5000..5019)

## Étapes
1. Lire l’image complète du bloc `4` sur `4000..4175`.
2. Écrire exactement `2` registre(s) sur `4056..4057`.
3. Relire l’image complète du bloc `4`.
4. Calculer le diff adressé.
5. Vérifier que seules les adresses `4056..4057` diffèrent.
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
