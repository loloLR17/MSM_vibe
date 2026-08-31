# FT-LIM-02 — Cas génériques détaillés

## 1. Préconditions communes

Sauf mention contraire :

1. disposer d'une configuration active valide A ;
2. préparer une copie B complète et connue valide ;
3. acquisition arrêtée ;
4. utiliser un `prepared_config_id` non nul et un `transaction_id` neuf ;
5. ne modifier que les champs nécessaires au cas ;
6. recalculer `prepared_config_crc` après modification de 4B+4C+4D ;
7. conserver un snapshot de l'image active A.

Les valeurs unitaires utilisées doivent satisfaire FT-LIM-01.

## 2. LIM02-G01 — Cohérence période / fenêtre / fréquence

### Cible

- 4016 `sampling_frequency_hz` ;
- 4021 `window_size_samples` ;
- 4022 `indicator_period_ms`.

### Règle

`indicator_period_ms >= 1000 × window_size_samples / sampling_frequency_hz`

### Stratégie

Fixer `sampling_frequency_hz = 26667`, puis exécuter exhaustivement le produit cartésien :

- fenêtres : 4096, 8192, 16384, 32768 ;
- périodes : 2000, 5000, 10000, 30000, 60000 ms.

Cela produit 20 instances.

Pour chaque instance, calculer :

`lhs = indicator_period_ms × sampling_frequency_hz`

`rhs = 1000 × window_size_samples`

La relation est satisfaite si `lhs >= rhs`. Cette forme entière évite toute ambiguïté d'arrondi.

### Verdict

Chaque combinaison V1 doit satisfaire l'invariant. Si une combinaison échoue mathématiquement, il existe une contradiction interne du référentiel et l'exécution doit être arrêtée pour anomalie normative.

Lorsque l'invariant est satisfait, la combinaison ne doit pas être refusée pour cette relation. L'application peut encore échouer pour une autre cause indépendante ; l'environnement doit donc utiliser une baseline valide et maîtrisée.

Aucun cas négatif n'est créé avec `indicator_period_ms = 1000` ou une autre valeur hors domaine : cela dupliquerait FT-LIM-01.

## 3. LIM02-G02 — Compatibilité limite de stockage / capacité utilisable

### Cible

4026-4027 `storage_limit_mb`.

### Donnée d'environnement

`C = capacité de stockage utilisable déterminée par le firmware`, exprimée en MB selon la même convention que le champ.

`C` doit être établi avant l'essai et enregistré dans le rapport.

### Instances

- `storage_limit_mb = 1` : admissible si `C >= 1` ;
- `storage_limit_mb = C` : borne dynamique admissible ;
- `storage_limit_mb = C + 1` : hors capacité, non validable.

La troisième instance n'est exécutable que si `C < 4294967295`, afin que `C+1` reste représentable en `uint32`. Si `C` atteint la borne de représentation, l'absence de valeur représentable immédiatement supérieure est consignée et le cas est `N/A — NO_REPRESENTABLE_VALUE_ABOVE_C`.

### Verdict pour valeur compatible

La valeur ne doit pas provoquer un échec au titre de la capacité de stockage. Avec toutes les autres préconditions satisfaites, la configuration peut être validée/appliquée.

### Verdict pour `C+1`

1. l'écriture Modbus sur le RW doit rester acceptée ;
2. la configuration préparée devient non validable ;
3. elle ne doit pas devenir active ;
4. l'image active A doit rester inchangée ;
5. aucune application partielle n'est admise.

Le code d'erreur exact n'est pas inventé si la V1 ne le fixe pas univoquement pour cette cause.

## 4. Critères d'acceptation généraux

Une instance est PASS uniquement si les critères de sa classe sont tous satisfaits.

Un rejet Modbus causé uniquement par la violation métier de capacité est FAIL.

Une configuration incompatible qui devient active, même partiellement, est FAIL.

Une relation non définie normativement ne doit pas être transformée en test actif FT-LIM-02.
