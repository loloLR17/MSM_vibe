# FT-RBT-04 — Cas de test détaillés

## TT-RBT-GEN-020 — Lectures pendant une transition contrôlée

**Classification d'exécution : `CONDITIONAL`.**

### Objectif

Vérifier que la sollicitation du capteur pendant une évolution dynamique ne conduit pas à une réponse multi-registres mélangeant des données provenant d'instants logiques incompatibles.

### Oracle réutilisé

FT-STR-07 : chaque réponse multi-registres doit représenter un même instant logique. FT-RBT-04 ne modifie pas cet oracle.

### Condition d'exécution

Le banc doit disposer d'au moins un scénario où :
- une donnée ou un ensemble cohérent de données évolue de façon contrôlée ou identifiable ;
- des lectures multi-registres peuvent être émises pendant cette évolution ;
- la cohérence interne de chaque réponse peut être évaluée objectivement.

Si ces conditions ne sont pas réunies, verdict `NOT_EXECUTABLE`, jamais `PASS` par défaut.

### Procédure

1. établir un état initial valide et lisible ;
2. déclencher ou attendre une transition contrôlée dont les observables sont connus ;
3. pendant la fenêtre d'évolution, effectuer plusieurs lectures multi-registres pertinentes ;
4. analyser chaque réponse séparément avec l'oracle de cohérence FT-STR-07 ;
5. poursuivre jusqu'à obtention d'un état final valide ou jusqu'à la fin de la fenêtre d'essai définie par le protocole de banc.

### Résultat attendu

Pour chaque réponse effectivement obtenue :
- les champs multi-registres sont cohérents en eux-mêmes ;
- aucune reconstruction ne révèle un mélange d'instants logiques incompatible avec l'oracle FT-STR ;
- une variation entre deux réponses successives est admise si les données sont dynamiques.

### Verdict

- `PASS` : toutes les réponses analysables respectent l'oracle FT-STR ;
- `FAIL` : au moins une réponse viole cet oracle ;
- `NOT_EXECUTABLE` : aucune transition exploitable ne peut être produite/identifiée ou l'observation ne permet pas de juger la cohérence.

### Limites obligatoires

Le test ne conclut rien sur :
- la durée de la transition ;
- le nombre d'états intermédiaires ;
- l'ordre temporel exhaustif des états ;
- une fréquence de polling admissible ;
- une latence maximale de réponse ;
- la performance sous charge.

---

## Scénarios non instanciés

### Polling à X ms

Non normatif : aucun X n'est défini en V1.

### Capturer obligatoirement l'état intermédiaire Y

Non normatif : la V1 ne garantit pas l'observation de chaque état transitoire.

### Exiger N lectures réussies pendant la transition

Non normatif : aucun N n'est défini.
