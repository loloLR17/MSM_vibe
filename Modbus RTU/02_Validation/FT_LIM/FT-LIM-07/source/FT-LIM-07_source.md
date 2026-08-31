# FT-LIM-07 — Source normative

## Référence
V1 Bloc 7 — Diagnostic.

## Exigences

### LIM07-RQ-001 — Santé système
`system_health_status` appartient à {0,1,2,3} : OK, Warning, Dégradé, Critique.

### LIM07-RQ-002 — Flags défaut
`system_fault_flags` utilise les bits 0..9. Les bits 10..15 sont réservés et doivent rester à 0. Oracle : `(flags & 0xFC00) == 0`.

### LIM07-RQ-003 — Autotest
`selftest_status` appartient à {0,1,2,3} : jamais exécuté, en cours, OK, échec.

### LIM07-RQ-004 — Cause reset
`reset_cause` appartient à {0,1,2,3,4,5,6}. Les valeurs 7..65535 sont réservées.

### LIM07-RQ-005 — Uptime
`uptime_s` est le temps de fonctionnement depuis le dernier reset et ne doit jamais revenir en arrière sauf reset.

### LIM07-RQ-006 — Aucun défaut connu
`last_fault_code=0` signifie aucun défaut connu.

### LIM07-RQ-007 — Horodatage défaut
`last_fault_timestamp` utilise la même base temporelle que le Bloc 2.

## Domaines non définis
- codes non nuls de `last_fault_code` ;
- `selftest_result_code` ;
- `selftest_detail` ;
- plage fonctionnelle de `internal_temp_dC` ;
- plage fonctionnelle de `supply_voltage_mV`.

## Relations non inventées
La V1 ne permet pas d’imposer notamment :
- `system_health_status=0 => system_fault_flags=0` ;
- un flag particulier => un niveau de santé précis ;
- `last_fault_code=0 => last_fault_timestamp=0` ;
- une relation détaillée entre selftest_status et selftest_result_code.
