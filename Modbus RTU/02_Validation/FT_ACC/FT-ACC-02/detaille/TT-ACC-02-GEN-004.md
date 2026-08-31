# TT-ACC-02-GEN-004 — Écriture FC16 d'une plage contiguë entièrement RW

## Objectif
Vérifier une écriture multi-registres sur une plage dont **tous** les registres sont explicitement inscriptibles.

## Préconditions
- chaque adresse de la plage est RW ;
- aucune adresse RO, réservée ou non exposée n'est incluse ;
- les granularités logiques V1 sont respectées.

## Étapes
1. Déterminer une plage contiguë réellement RW.
2. Sauvegarder son état initial.
3. Écrire la plage avec FC16.
4. Vérifier l'absence d'exception.
5. Relire les éléments stables et contrôler leur prise en compte.
6. Restaurer si nécessaire.

## Résultat attendu
La requête entièrement RW est acceptée.

## Règle
Une plage traversant un seul registre non-RW n'est pas un cas nominal FT-ACC-02.
