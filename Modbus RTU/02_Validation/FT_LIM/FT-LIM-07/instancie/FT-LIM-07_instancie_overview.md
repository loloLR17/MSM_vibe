# FT-LIM-07 — Vue d’ensemble

## Couverture
10 instances décrivent les domaines et invariants fonctionnels normatifs du Bloc 7.

### Tests avec oracle PASS/FAIL
- system_health_status 0..3 ;
- bits réservés 10..15 de system_fault_flags à zéro ;
- selftest_status 0..3 ;
- reset_cause 0..6 ;
- uptime non décroissant hors reset.

### Contrôles conditionnels / traçabilité
- reset réellement observé ;
- last_fault_code=0 ;
- base temporelle du dernier défaut ;
- grandeurs physiques sans plage fonctionnelle ;
- codes uint16 sans table normative.

## Doctrine
Le Bloc 7 est RO. Aucun défaut, reset ou état autotest n’est fabriqué par écriture de registres de diagnostic. Les compléments métier informatifs ne servent jamais d’oracle V1.
