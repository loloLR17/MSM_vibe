# FT-OBS-02 — Procédures détaillées

## TT-OBS-B02-001 — Qualification de la base de temps

### Objectif
Vérifier qu'une centrale peut déterminer la validité et l'état de synchronisation de la base de temps à partir des champs normatifs B2, sans utiliser la valeur de `current_time` comme heuristique.

### Données à lire
- `time_status` ;
- `time_flags` ;
- `current_time` ;
- `last_sync_time` ;
- `time_since_sync_s` ;
- `sync_source`.

### Vérification
Pour chaque état B2 accessible au banc, décoder `time_status` et les bits définis de `time_flags` selon la V1. Vérifier notamment que la logique de supervision peut qualifier `Temps valide` à partir du bit dédié et distinguer les états définis de `time_status`.

`current_time != 0` ne doit jamais être utilisé comme condition de PASS de validité.

### PASS
Les états observés appartenant au domaine défini sont interprétables directement selon la table V1 et la validité est déterminée par les métadonnées normatives.

### FAIL
Une valeur définie ne peut pas être interprétée conformément à la V1, ou l'implémentation oblige la centrale à utiliser une heuristique sur `current_time` faute d'exposer correctement les discriminants normatifs.

### Limite
La cohérence exacte entre `time_status`, chaque bit de `time_flags`, `sync_source` et les timestamps n'est pas réattribuée à FT-OBS.

---

## TT-OBS-B03-001 — Qualification validité / fraîcheur / dégradation B3

### Objectif
Vérifier que les métadonnées B3 permettent à une centrale de distinguer les états normativement définis des valeurs de supervision.

### Données à lire
- `B3_STATUS_GLOBAL` ;
- `B3_VALIDITY_FLAGS` ;
- `B3_LAST_UPDATE_TR2` ;
- `B3_VALUE_AGE_MS` ;
- au moins une valeur vibratoire exposée, par exemple `B3_RMS_GLOBAL_MG`.

### Scénarios
Selon les états que le banc peut provoquer de manière contrôlée, couvrir autant que possible :
- valeurs valides ;
- valeurs dégradées mais exploitables ;
- valeurs invalides ;
- données fraîches ;
- erreur de calcul si injectable de manière déterministe.

L'exécution d'un scénario particulier est `CONDITIONAL` lorsque le banc ne permet pas de provoquer l'état correspondant sans modifier l'implémentation.

### PASS
La centrale peut qualifier les observations à partir de `B3_STATUS_GLOBAL` et des bits définis de `B3_VALIDITY_FLAGS`, sans déduire la validité ou la fraîcheur de la valeur numérique mesurée.

### FAIL
Un état provoqué et normativement défini n'est pas discriminable via les champs prévus, ou une valeur annoncée invalide doit être considérée valide pour la seule raison qu'elle est non nulle.

### Pas de FAIL FT-OBS
L'absence d'un seuil numérique permettant de recalculer `VALUES_FRESH` depuis `B3_VALUE_AGE_MS` n'est pas une non-conformité V1 : ce seuil est `NOT_DEFINED`.

---

## TT-OBS-B03-002 — Valeur conservée non fraîche

### Classification d'exécution
`CONDITIONAL` — le banc doit pouvoir créer une indisponibilité temporaire conduisant à conserver une dernière valeur sans recalcul récent.

### Objectif
Démontrer qu'une valeur B3 peut rester présente tout en étant explicitement qualifiée comme non fraîche, et qu'une centrale peut détecter ce cas sans heuristique.

### Précondition
Une valeur B3 valide a été calculée et observée.

### Étapes
1. Enregistrer `B3_RMS_GLOBAL_MG`, `B3_VALIDITY_FLAGS`, `B3_VALUE_AGE_MS` et `B3_LAST_UPDATE_TR2`.
2. Provoquer, par un moyen de banc conforme, une indisponibilité temporaire permettant au capteur de conserver la dernière valeur calculée.
3. Relire les mêmes champs.
4. Vérifier le bit `LAST_VALUE_HELD`.
5. Vérifier la qualification de fraîcheur via `VALUES_FRESH`.
6. Conserver la valeur RMS uniquement comme preuve qu'une valeur peut être encore présente ; sa valeur numérique n'est pas un oracle de validité.

### PASS
Lorsque le comportement « dernière valeur conservée sans recalcul récent » est effectivement présent, `LAST_VALUE_HELD=1` permet à la centrale de l'identifier et la centrale ne traite pas la simple présence de la valeur comme preuve de fraîcheur.

### FAIL
Le comportement est confirmé mais le discriminant normatif `LAST_VALUE_HELD` n'est pas exposé conformément à sa définition, rendant nécessaire une heuristique côté centrale.

### TRACE_ONLY
- évolution exacte de `B3_VALUE_AGE_MS` ;
- valeur conservée ;
- durée avant changement de `VALUES_FRESH`.

Aucun seuil temporel n'est inventé.

---

## Cas volontairement non instanciés

### `0 = absent` ou `0 = invalide`
`NOT_DEFINED` transversalement. Aucun test global ne doit imposer cette convention.

### `0xFFFF` / `0xFFFFFFFF = inconnu`
`NOT_DEFINED` transversalement. Une telle valeur ne reçoit une sémantique spéciale que si le champ concerné la définit explicitement.

### Recalcul de `VALUES_FRESH` à partir de l'âge
`NOT_DEFINED` faute de seuil normatif universel.

### Cohérence exhaustive statut ↔ flags B3
Les tables donnent des significations exploitables, mais FT-OBS-02 ne crée pas de matrice complète de combinaisons obligatoires qui n'est pas explicitement spécifiée en V1.