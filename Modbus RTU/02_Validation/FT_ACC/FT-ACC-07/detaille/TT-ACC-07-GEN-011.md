# TT-ACC-07-GEN-011 — Vérification type ↔ comportement

## 1. Objectif
Valider la conformité mapping ↔ comportement observé pour le scénario `Vérification type ↔ comportement`.

## 2. Références
- FT-ACC-07 — Conformité mapping ↔ permissions observées
- Mapping Modbus unifié
- Gouvernance `reserved*`
- Doctrine d’exception explicite sur accès invalide

## 3. Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-06 validées
- système stable
- cible connue dans le mapping

## 4. Étapes
1. Identifier le type d’accès documentaire de la cible.
2. Exécuter la lecture de la cible.
3. Exécuter l’écriture ou la tentative d’écriture selon le scénario.
4. Relire la cible si nécessaire.
5. Comparer le comportement observé au comportement attendu issu du mapping.

## 5. Résultat attendu
- comportement strictement conforme au mapping ;
- aucune ambiguïté ;
- stabilité sur répétition.

## 6. Critères d’acceptation
- verdict cohérent avec le type d’accès ;
- absence de divergence documentaire ;
- comportement déterministe.

## 7. Criticité
P0
