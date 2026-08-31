# TT-ACC-04-GEN-009 — Écriture plage réservés

## 1. Objectif
Valider le comportement attendu d’un registre ou d’une plage réservée dans le périmètre FT-ACC-04.

## 2. Références
- FT-ACC-04 — Comportement des registres réservés
- Mapping Modbus unifié
- Gouvernance `reserved*`

## 3. Préconditions
- FT-STR validée
- FT-ACC-01 validée
- cible identifiée comme `reserved*`
- simulateur stable
- valeur initiale lisible

## 4. Données d’entrée
- adresse : `@ADDR`
- longueur : `@LEN`
- valeur initiale : `V0`
- valeur de tentative : `V1` si le scénario inclut une écriture

## 5. Étapes
1. Lire la cible.
2. Si prévu, tenter une écriture sur la cible.
3. Relire la cible.
4. Comparer le comportement observé au comportement attendu.

## 6. Résultat attendu
- comportement déterministe ;
- aucune modification illégitime ;
- cohérence mapping ↔ comportement.

## 7. Critères d’acceptation
- lecture stable ;
- tentative d’écriture refusée ou sans effet observable ;
- aucune sémantique cachée.

## 8. Mode d’exécution
- simulateur déterministe
- automatisable

## 9. Traces à conserver
- trames de lecture ;
- trame d’écriture éventuelle ;
- valeur initiale ;
- valeur finale ;
- verdict.

## 10. Niveau de criticité
P0
