# TT-ACC-05-B2-001 — Bloc 2 — prepared_time

## Objectif
Vérifier qu’une écriture sur `prepared_time` n’entraîne aucune modification hors de la plage ciblée.

## Référence mapping
- Bloc : 2
- Adresse début cible : 2008
- Adresse fin cible : 2009
- Type déclaré : uint32
- Nombre de registres : 2
- Accès documentaire : RW

## Périmètre de snapshot
- Bloc observé : 2
- Adresse début bloc : 2000
- Adresse fin bloc : 2015
- Bloc précédent : 1 (1000..1019)
- Bloc suivant : 3 (3000..3047)

## Étapes
1. Lire l’image complète du bloc `2` sur `2000..2015`.
2. Écrire exactement `2` registre(s) sur `2008..2009`.
3. Relire l’image complète du bloc `2`.
4. Calculer le diff adressé.
5. Vérifier que seules les adresses `2008..2009` diffèrent.
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
