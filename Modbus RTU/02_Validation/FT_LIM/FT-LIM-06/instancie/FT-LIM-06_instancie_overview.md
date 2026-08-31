# FT-LIM-06 — Vue d’ensemble

## Couverture active

8 lignes d’instances couvrent les domaines RO et invariants normatifs du Bloc 6 qui n’étaient pas la responsabilité de FT-LIM-05.

### Domaines
- campaign_state : 0..5
- data_integrity_status : 0..3
- storage_health_status : 0..3

### Invariants
- campagne valide => campaign_id non nul
- campagne en cours => end_timestamp nul
- duration_s : exigence de cohérence tracée, mais contrôle `TRACE_ONLY` faute d’oracle numérique V1 suffisamment précis

## Doctrine d’exécution

Les registres sont RO. La couverture exhaustive des valeurs d’enum n’est pas obtenue par écriture forcée : on observe les états réellement atteignables et on marque les autres N/A/non observés.

## Non-invention

Aucune relation n’est créée entre état de campagne, intégrité des données et santé globale du stockage au-delà des règles explicitement écrites dans la V1. Aucun FAIL ne peut être prononcé sur `duration_s` à partir d’une appréciation subjective ou d’une formule non normative.
