# TT-ACC-02-GEN-018 — Écriture conforme mapping (plage)

## 1. Objectif
Vérifier qu’une plage déclarée RW est effectivement modifiable.

## 2. Références
- FT-ACC-02 — Écriture des zones RW
- Mapping Modbus unifié

## 3. Préconditions
- FT-STR validée
- FT-ACC-01 validée
- La cible testée est déclarée `RW`
- Le simulateur est en état stable
- La valeur initiale est connue ou lisible

## 4. Données d’entrée
- Adresse : `@ADDR`
- Longueur : `@LEN`
- Valeur initiale : `V0`
- Valeur(s) de test : `V1`, le cas échéant `V2`

## 5. Étapes
1. Lire la valeur initiale sur la cible.
2. Écrire la valeur de test sur la cible.
3. Relire immédiatement la cible.
4. Comparer la valeur relue à la valeur écrite.

## 6. Résultat attendu
- aucune exception Modbus ;
- écriture acceptée ;
- relecture immédiate cohérente avec la valeur écrite.

## 7. Critères d’acceptation
- accès en écriture conforme ;
- longueur conforme ;
- cohérence write → read ;
- comportement cohérent avec l’attribut `RW`.

## 8. Mode d’exécution
- simulateur déterministe
- automatisable

## 9. Traces à conserver
- adresse et longueur ;
- trame d’écriture ;
- trame(s) de lecture ;
- `V0`, `V1`, éventuellement `V2` ;
- verdict.

## 10. Niveau de criticité
P0
