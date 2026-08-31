# FT-LIM-07 — Cas génériques

## LIM07-G01 — Domaine santé
Lire 7001. PASS si valeur dans {0,1,2,3}.

## LIM07-G02 — Bits réservés défaut
Lire 7002. PASS si `(value & 0xFC00)==0`. Les bits 0..9 peuvent apparaître selon l’état réel.

## LIM07-G03 — Domaine autotest
Lire 7006. PASS si valeur dans {0,1,2,3}. Ne pas forcer un état RO.

## LIM07-G04 — Domaine reset_cause
Lire 7011. PASS si valeur dans {0,1,2,3,4,5,6}.

## LIM07-G05 — Monotonie uptime sans reset
Effectuer au moins deux lectures ordonnées de 7009-7010 dans une période où aucun reset n’est observé. PASS si `uptime_2 >= uptime_1`.

## LIM07-G06 — Uptime après reset observé
Si un reset réel/autorisé est observé entre deux mesures, une diminution de l’uptime est permise et ne constitue pas un échec de G05. Documenter `reset_cause` et les mesures. Ne pas exiger une valeur exacte de redémarrage non spécifiée.

## LIM07-G07 — last_fault_code nul
Si 7003 vaut 0, interpréter uniquement « aucun défaut connu ». Ne pas imposer `last_fault_timestamp=0`, non spécifié.

## LIM07-G08 — Timestamp défaut
Si un défaut significatif avec timestamp est disponible, vérifier que son interprétation utilise la même base temporelle que le Bloc 2. Ce contrôle est de traçabilité ; aucune relation numérique supplémentaire n’est inventée.

## LIM07-G09 — Grandeurs physiques sans domaine fonctionnel
Consigner température et tension si utile, mais aucun PASS/FAIL de limite fonctionnelle n’est prononcé faute de plage V1. Leur type/unité ne doit pas devenir une limite métier inventée.

## LIM07-G10 — Codes non définis
Pour `last_fault_code` non nul, `selftest_result_code` et `selftest_detail`, consigner la valeur sans qualifier son domaine au-delà du uint16 tant qu’une table normative n’existe pas.
