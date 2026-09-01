# FT-BLK-05 — Exigences source normalisées

## 1. Référence normative

Source principale : `01_Specification_source/bloc6.md` V1 corrigé lors de l'audit final transversal.

Les compléments métier informatifs ne sont pas utilisés comme oracles normatifs.

## 2. Exigences normalisées

### BLK05-B6-001 — Sélection d’une entrée par index
La valeur écrite dans `selected_campaign_index` pilote la campagne exposée par l’entrée sélectionnée du Bloc 6.

**Classification** : COVERED.

### BLK05-B6-002 — Index 0
Pour un inventaire non vide, l’index `0` désigne la première campagne.

**Classification** : COVERED sous précondition d’inventaire connu.

### BLK05-B6-003 — Index N-1
Pour `N = total_campaign_count > 0`, l’index `N-1` désigne la dernière campagne.

**Classification** : COVERED sous précondition d’inventaire connu.

### BLK05-B6-004 — Index valide intermédiaire
Pour un index appartenant à `0..N-1`, la sélection doit être considérée valide et l’entrée exposée doit correspondre à l’élément logique sélectionné.

**Classification** : COVERED sous précondition d’un inventaire comportant plusieurs campagnes identifiables.

### BLK05-B6-005 — Index hors plage
Une valeur hors `0..N-1` est sémantiquement invalide et entraîne `selected_campaign_valid = 0`.

**Classification** : COVERED pour la conséquence fonctionnelle. L’acceptation de l’écriture Modbus et l’absence d’exception sont traitées par FT-LIM / FT-ACC.

### BLK05-B6-006 — Métadonnées non interprétables comme valides
Lorsque `selected_campaign_valid = 0`, les métadonnées de l’entrée sélectionnée ne doivent pas être interprétées comme valides.

**Classification** : COVERED au niveau de l’état de validité ; aucune valeur de remise à zéro des champs n’est imposée par la V1.

### BLK05-B6-007 — Identifiant de campagne valide
Une campagne valide ne doit jamais exposer `campaign_id = 0`.

**Classification** : COVERED.

### BLK05-B6-008 — Campagne en cours
Si `campaign_state = 2` (en cours), alors `end_timestamp = 0`.

**Classification** : CONDITIONAL côté exécution, car il faut disposer d’une campagne réellement en cours ; oracle normatif explicite.

### BLK05-B6-009 — Cohérence de durée en cas nominal
Pour une campagne terminée dont `start_timestamp` et `end_timestamp` appartiennent à une base de temps continue sans resynchronisation affectant l’intervalle, `duration_s = end_timestamp - start_timestamp`.

Pour une campagne en cours (`end_timestamp = 0`) ou traversant une discontinuité temporelle liée à une resynchronisation, la V1 ne définit pas de relation arithmétique obligatoire entre `duration_s` et les timestamps.

**Classification** : CONDITIONAL. L’oracle exact est applicable uniquement sous la précondition nominale ci-dessus.

### BLK05-B6-010 — Cohérence multi-registres de l’entrée sélectionnée
Les champs d’une même réponse doivent provenir d’une seule campagne et aucun mélange de deux campagnes n’est permis.

**Classification** : TRACE_ONLY vers FT-STR ; pas de duplication dans FT-BLK-05.

### BLK05-B6-011 — Relation total / valides
La V1 expose `total_campaign_count` et `valid_campaign_count` mais ne formalise pas explicitement l’invariant numérique entre eux.

**Classification** : NOT_DEFINED.

### BLK05-B6-012 — Espace utilisé / libre
La V1 expose `storage_used_mb` et `storage_free_mb` sans fournir la capacité totale ni une relation normative permettant une équation de cohérence.

**Classification** : NOT_DEFINED.

### BLK05-B6-013 — Dérivation état de santé stockage
Les codes de `storage_health_status` sont définis, mais les seuils ou conditions de dérivation ne le sont pas.

**Classification** : NOT_DEFINED.

### BLK05-B6-014 — Dérivation intégrité des données
Les codes de `data_integrity_status` sont définis, mais la méthode qui détermine `OK`, `corrompue` ou `partielle` n’est pas spécifiée.

**Classification** : NOT_DEFINED.

### BLK05-B6-015 — Unicité locale des identifiants de campagne
Deux campagnes distinctes présentes simultanément dans l’inventaire d’un même TR2 doivent exposer des valeurs de `campaign_id` distinctes. Aucune unicité inter-capteurs n’est exigée par la V1.

**Classification** : CONDITIONAL côté exécution, car il faut un inventaire contenant au moins deux campagnes distinctes et navigables.

## 3. Limites d’exécution

Les tests de navigation exigent un inventaire de référence dont l’ordre logique et les identifiants sont connus du banc. Le test de campagne en cours exige un scénario déterministe produisant réellement `campaign_state = 2`.

Le test de durée exacte exige une campagne terminée dont l’intervalle n’a pas été affecté par une resynchronisation temporelle. Aucune équation n’est imposée hors de cette précondition.

Aucune valeur n’est inventée pour les métadonnées lorsqu’une sélection est invalide ; seul `selected_campaign_valid = 0` constitue l’oracle normatif direct.
