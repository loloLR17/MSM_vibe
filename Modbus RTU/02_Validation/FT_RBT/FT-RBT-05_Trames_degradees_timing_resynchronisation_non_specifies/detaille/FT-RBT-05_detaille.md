# FT-RBT-05 — Cas détaillés et scénarios non instanciés

## 1. Aucun test autonome V1

FT-RBT-05 ne contient aucun test PASS/FAIL autonome, car les comportements de transport dégradé et de timing recensés ne disposent pas d'un oracle V1 suffisamment précis.

## 2. Scénarios non instanciés

### CRC Modbus RTU invalide

Aucun verdict n'est créé sur la réaction du capteur. La V1 ne précise pas silence, exception, diagnostic, compteur d'erreur ou toute autre conséquence observable.

### Trame tronquée ou framing invalide

Aucun verdict n'est créé sur le vidage de buffer, la reprise de parsing ou le comportement applicatif suivant.

### Résynchronisation

Aucun test « une requête valide après N erreurs doit réussir en moins de X ms » n'est créé : ni N ni X ne sont définis.

### Timeout et retry

Aucun seuil de temps de réponse, délai d'attente maître, nombre de retries ou backoff n'est normatif en V1.

### Rafales d'erreurs

Aucune campagne de N trames invalides ne peut être présentée comme preuve de conformité V1 sans seuil défini par une spécification ultérieure.

## 3. Tests de caractérisation possibles hors conformité V1

Le banc pourra, si utile, mesurer :
- réaction observée à un CRC RTU erroné ;
- comportement après trame tronquée ;
- temps de reprise après bruit ;
- latence de réponse ;
- tolérance empirique aux rafales.

Ces essais devront être marqués `CARACTERISATION` ou équivalent et ne devront pas produire de verdict de conformité V1.

## 4. Condition d'évolution future

Pour rendre ces scénarios normatifs, une évolution de spécification devra définir explicitement :
- le comportement attendu ;
- les observables ;
- les seuils temporels ou quantitatifs nécessaires ;
- la hiérarchie normative avec le standard Modbus RTU retenu.
