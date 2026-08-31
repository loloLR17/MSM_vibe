# TT-ACC-05-GEN-004 — Sélecteur et mise à jour normative de la vue

## Objectif
Vérifier qu’une écriture de sélecteur ne modifie que le sélecteur et la vue RO explicitement associée par la V1.

## Préconditions
- cible RW de sélection ;
- index nominal existant ;
- état de référence capturable.

## Étapes
1. Capturer le bloc avant sélection.
2. Écrire un index nominal différent lorsque possible.
3. Attendre la mise à jour cohérente de la vue.
4. Capturer le bloc et classifier les différences.

## Résultat attendu
Le sélecteur et la vue associée peuvent changer conformément à la sélection ; aucune donnée persistante de campagne ni aucun état sans lien avec la sélection ne doit être modifié.

## Criticité
P0
