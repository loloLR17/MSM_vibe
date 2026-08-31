# FT-PER-06 — Cas de test détaillé

## TT-PER-B01B07-002 — Power cycle puis cause power-on observable

**Classification d'exécution : `CONDITIONAL`.**

### Objectif

Vérifier qu'une véritable remise sous tension contrôlée est identifiée comme `power-on` par les observables normatifs de cause de reset, tout en caractérisant sans oracle temporel le retour de Modbus.

### Préconditions

- moyen d'essai autorisant une coupure puis remise sous tension réelle et contrôlée ;
- possibilité de reprendre les lectures Modbus après boot ;
- absence d'une autre cause de reset injectée pendant le scénario ;
- accès aux Blocs 1 et 7.

### Capture pré-coupure

- B1 `last_reset_cause` ;
- B7 `reset_cause` ;
- B7 `uptime_s` ;
- timestamp banc de la dernière réponse Modbus valide.

### Étapes

1. réaliser la capture pré-coupure ;
2. couper réellement l'alimentation du capteur selon la procédure du banc ;
3. rétablir l'alimentation ;
4. tenter de reprendre les interrogations Modbus ;
5. ne pas appliquer de timeout de conformité V1 ;
6. à la première fenêtre exploitable, lire B1 `last_reset_cause` ;
7. lire B7 `reset_cause` et `uptime_s` ;
8. enregistrer le temps entre remise sous tension et première réponse Modbus valide comme donnée de caractérisation.

### Résultats normatifs attendus

- `B1.last_reset_cause = 1` ;
- `B7.reset_cause = 1`.

### PASS

Le power cycle est établi et les deux champs normatifs identifient la cause `power-on`.

### FAIL

Le power cycle est établi mais B1 ou B7 expose une cause différente de `1`. La divergence doit être enregistrée séparément par bloc.

### NOT_EXECUTABLE / INCONCLUSIVE

- le banc ne permet pas un vrai power cycle ;
- la coupure/remise sous tension ne peut pas être établie ;
- une autre cause de reset interfère ;
- les lectures post-boot ne peuvent pas être obtenues.

### TRACE_ONLY

- `uptime_s` post-boot ;
- délai de première réponse Modbus ;
- nombre de tentatives de lecture ;
- comportement observé pendant l'initialisation.

### Aucun FAIL FT-PER-06 pour

- un boot jugé « long » sans limite V1 ;
- un uptime initial différent de zéro ;
- du silence Modbus pendant le boot ;
- une politique de persistance différente de RESET SOFTWARE sur des propriétés non spécifiées.
