# FT-LIM-07 — Domaines et invariants fonctionnels du diagnostic

## Objet
Valider les domaines normatifs et invariants fonctionnels des valeurs RO exposées par le Bloc 7 Diagnostic.

## Périmètre
- `system_health_status` : 0..3 ;
- `system_fault_flags` : bits 0..9 définis, bits 10..15 réservés à 0 ;
- `selftest_status` : 0..3 ;
- `reset_cause` : 0..6, valeurs 7..65535 réservées ;
- `uptime_s` monotone sauf reset ;
- sémantique `last_fault_code=0` ;
- traçabilité de la base temporelle de `last_fault_timestamp`.

## Hors périmètre
- permissions RO : FT-ACC ;
- cohérence structurelle uint32 : FT-STR ;
- plages physiques de température et tension : non définies par la V1 ;
- domaines détaillés de `selftest_result_code`, `selftest_detail` et codes défaut non nuls : non définis ;
- compléments métier informatifs.

Le Bloc 7 est strictement RO : aucun état n’est fabriqué par écriture directe.
