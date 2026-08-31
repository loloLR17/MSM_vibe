# FT-LIM-08 — Cas génériques

## LIM08-G01 — Bits réservés des capacités
Lire `device_capabilities`. PASS si `(value & 0xFFF0)==0`.

## LIM08-G02 — Stabilité Bloc 0
Lire deux snapshots complets du Bloc 0 séparés par une variation normale d’état/acquisition sans changement firmware/hardware ni maintenance. PASS si les champs d’identification restent identiques.

## LIM08-G03 — Persistance device_id
Comparer `device_id` avant/après un redémarrage autorisé si celui-ci est disponible dans la campagne. PASS si identique. Sans redémarrage, contrôle conditionnel/non observé.

## LIM08-G04 — Unicité device_id
Avec au moins deux capteurs distincts, PASS si leurs `device_id` diffèrent. Sinon N/A.

## LIM08-G05 — Domaine last_reset_cause
Lire 1006. PASS si valeur dans {0,1,2,3,4,5,6}.

## LIM08-G06 — Domaine storage_status
Lire 1010. PASS si valeur dans {0,1,2,3}.

## LIM08-G07 — Domaine acquisition_state
Lire 1012. PASS si valeur dans {0,1,2,3}.

## LIM08-G08 — Monotonie uptime
Lire 1004-1005 à t1 puis t2 sans reset observé. PASS si `uptime2 >= uptime1`.

## LIM08-G09 — Uptime avec reset observé
Si un reset réel/autorisé survient, une diminution de l’uptime est admise. Vérifier que `last_reset_cause` reste dans son domaine ; ne pas imposer de valeur exacte si la cause n’est pas contrôlée.

## LIM08-G10 — Persistance défaut/avertissement
Lorsqu’une condition réelle et identifiable reste présente sur deux observations, vérifier que l’indication de défaut/avertissement correspondante ne disparaît pas avant disparition de la condition. Si aucune correspondance normative précise n’est disponible, classer TRACE_ONLY/N/A plutôt que d’inventer un bit.

## LIM08-G11 — Champs sans domaine normatif
Consigner `system_status`, flags/codes non documentés, température et pourcentages sans PASS/FAIL de limite métier. Ne pas déduire automatiquement 0..100 du suffixe `_percent`.
