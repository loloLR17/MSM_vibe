# FT-OBS-04 — Procédures détaillées

## TT-OBS-CODE-001 — Code défini / code réservé

### Objectif
Valider la politique d'interprétation centrale des enums/codes V1 sans dupliquer FT-LIM.

### Méthode
1. Sélectionner au moins un champ à table normative dans chacun des blocs disponibles au banc, par exemple `time_status`, `config_state`, `cmd_status`, `campaign_state`, `system_health_status`.
2. Pour une valeur définie observée, vérifier que la centrale restitue exactement la signification normative.
3. À partir d'une trace/simulateur de protocole permettant de présenter une valeur réservée à la couche de décodage centrale, vérifier qu'elle est signalée `non supportée/réservée` et non convertie en état métier inventé.

### PASS
Les codes définis sont interprétés selon la V1 et les réservés restent sans signification métier.

### Frontière
Faire produire un réservé par le capteur réel n'est pas exigé par FT-OBS ; les interdictions de production et domaines sont FT-LIM.

---

## TT-OBS-SENT-001 — Sentinelles locales et non-généralisation

### Objectif
Démontrer qu'une centrale applique chaque convention spéciale uniquement au champ qui la définit.

### Cas normatifs
- B4 : ID `0` → non renseigné / interdit pour validation ;
- B5 : paramètres optionnels non utilisés → `0` ;
- B6 : `end_timestamp = 0` si campagne en cours ;
- B6 : campagne valide → `campaign_id != 0` ;
- B7 : `last_fault_code = 0` → aucun défaut connu.

### Méthode
Pour chaque cas disponible, injecter ou observer la valeur spéciale et vérifier la sémantique locale. Présenter ensuite `0` dans un autre champ où aucune sémantique spéciale n'est définie et vérifier que la couche d'exploitation ne lui attribue pas automatiquement l'une des significations précédentes.

### PASS
Chaque sentinelle conserve son scope local ; aucune règle globale `0 = absent/invalide/inconnu` n'apparaît.

---

## TT-OBS-TIME-001 — Interprétation des timestamps

### Objectif
Vérifier que les timestamps exposés sont interprétés uniquement selon leur définition normative et leur base temporelle déclarée.

### Cas
- B2 : base temporelle et statut du temps ;
- B6 : `start_timestamp`, `end_timestamp` sur base B2 ;
- B7 : `last_fault_timestamp` sur base B2.

### Méthode
1. Lire la qualification B2 de la base de temps.
2. Lire les timestamps B6/B7 disponibles.
3. Vérifier que la centrale les interprète dans la base B2 lorsqu'ils sont applicables.
4. Pour B6 en campagne en cours, vérifier l'interprétation locale de `end_timestamp = 0`.
5. Pour B7 avec `last_fault_code = 0`, ne poser **aucune** exigence sur la valeur de `last_fault_timestamp` ; la tracer seulement.

### PASS
La centrale n'invente ni base temporelle, ni fraîcheur, ni sentinelle hors des règles explicitement définies.

### TRACE_ONLY
Valeur de `last_fault_timestamp` quand aucun défaut n'est connu.

---

## Cas explicitement hors oracle

- valeur métier de `B1.system_status` ;
- détail des flags/codes B1 sans table normative ;
- interprétation détaillée de `B4.config_error_code` ;
- interprétation exhaustive de `B5.cmd_result_detail` ;
- interprétation exhaustive de `B7.selftest_result_code` et `selftest_detail`.

Ces champs peuvent être enregistrés bruts dans une trace, mais leur contenu ne doit pas être transformé en verdict V1 inventé.