# FT-LIM-02 — Source des exigences de validation

## 1. Référentiel

Source normative principale : spécification V1 Bloc 4 — Configuration acquisition.

FT-LIM-02 dérive uniquement les contraintes croisées ou dynamiques explicitement présentes dans la V1.

## 2. Exigences dérivées

### LIM02-RQ-001 — Relation période / fenêtre / fréquence

Pour qu'une configuration préparée soit validable :

`indicator_period_ms >= 1000 × window_size_samples / sampling_frequency_hz`

Champs concernés :
- `sampling_frequency_hz` — 4016 ;
- `window_size_samples` — 4021 ;
- `indicator_period_ms` — 4022.

La relation est évaluée mathématiquement sans arrondi préalable de la durée de fenêtre.

### LIM02-RQ-002 — Couverture du domaine combinatoire V1

Avec :
- `sampling_frequency_hz = 26667` ;
- `window_size_samples ∈ {4096,8192,16384,32768}` ;
- `indicator_period_ms ∈ {2000,5000,10000,30000,60000}` ;

les 20 couples `window_size_samples × indicator_period_ms` doivent satisfaire LIM02-RQ-001.

Cette exigence constitue une preuve de cohérence combinatoire des domaines V1. FT-LIM-02 ne crée pas artificiellement de contre-exemple à partir d'une valeur déjà invalide au titre de FT-LIM-01.

### LIM02-RQ-003 — Compatibilité de la limite de stockage

`storage_limit_mb` doit être compatible avec la capacité de stockage utilisable déterminée par le firmware.

Soit `C` cette capacité, exprimée en MB selon la même convention que `storage_limit_mb` :
- `1 <= storage_limit_mb <= C` satisfait cette contrainte dynamique ;
- `storage_limit_mb > C` ne permet pas la validation de la configuration.

Le domaine unitaire `storage_limit_mb != 0` reste couvert par FT-LIM-01.

### LIM02-RQ-004 — Capacité C comme donnée d'essai

FT-LIM-02 ne fixe aucune capacité arbitraire. Avant exécution de LIM02-RQ-003, `C` doit être connue par un moyen fiable et traçable propre à l'environnement d'essai : configuration contrôlée du simulateur, support de stockage contrôlé ou information firmware documentée.

Si `C` ne peut pas être établie de manière fiable, les instances dépendantes de `C` sont `N/A — ENVIRONMENT_NOT_CHARACTERIZED`, et non PASS.

### LIM02-RQ-005 — Sémantique d'une violation fonctionnelle

Une violation d'une contrainte FT-LIM-02 sur des champs RW valides ne constitue pas à elle seule une erreur d'accès Modbus. L'écriture de la valeur préparée est acceptée au niveau transport ; la configuration résultante ne doit pas pouvoir être appliquée comme configuration active.

La vérification détaillée des mécanismes CRC, états de validation et application appartient à FT-LIM-03 ; FT-LIM-02 utilise seulement l'application comme oracle fonctionnel lorsque nécessaire pour établir le verdict.

### LIM02-RQ-006 — Protection de l'image active

Une configuration violant une contrainte FT-LIM-02 ne doit pas modifier, même partiellement, l'image active 4E.

## 3. Relations explicitement non dérivées

La V1 ne fournit pas de règle normative suffisante pour créer des critères PASS/FAIL sur :
- ordre warning/alarm RMS ;
- ordre warning/alarm peak ;
- hystérésis par rapport aux seuils ;
- masque de supervision par rapport aux seuils ;
- durée de campagne par rapport au volume de stockage.

Ces relations restent `NOT_DEFINED` pour FT-LIM-02.

## 4. Frontières

FT-LIM-02 suppose que chaque valeur candidate appartient déjà à son domaine unitaire FT-LIM-01, sauf lorsqu'une instance dynamique est explicitement définie par rapport à `C`.

Les erreurs d'accès, adresses inexistantes, RO/réservés et atomicité Modbus restent couvertes par FT-ACC/FT-STR.
