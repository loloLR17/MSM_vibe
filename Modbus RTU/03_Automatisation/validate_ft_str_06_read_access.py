#!/usr/bin/env python3
"""Validation mécanique FT-STR-06 contre GEL-MAP-V1.

Ce script valide uniquement les invariants déductibles du mapping : plages exposées,
continuité, lacunes et besoin de segmentation FC03. Il n'exécute aucune requête Modbus.
"""

from __future__ import annotations

import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAPPING = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"
COVERAGE = ROOT / "02_Validation" / "FT_STR" / "FT-STR-06" / "instancie" / "COUVERTURE_FT-STR-06_GEL-MAP-V1.csv"

EXPECTED = {
    0: (0, 20),
    1: (1000, 1019),
    2: (2000, 2015),
    3: (3000, 3047),
    4: (4000, 4175),
    5: (5000, 5019),
    6: (6000, 6063),
    7: (7000, 7015),
}
FC03_MAX = 125


def read_csv(path: Path):
    with path.open("r", encoding="utf-8-sig", newline="") as f:
        return list(csv.DictReader(f))


def fail(msg: str) -> None:
    raise SystemExit(f"ERREUR FT-STR-06: {msg}")


def main() -> None:
    rows = read_csv(MAPPING)
    if not rows:
        fail("mapping logique vide")

    by_block: dict[int, list[tuple[int, int, str]]] = {b: [] for b in EXPECTED}
    for row in rows:
        block = int(row["block"])
        if block not in by_block:
            fail(f"bloc inattendu {block}")
        start = int(row["address_start"])
        end = int(row["address_end"])
        if end < start:
            fail(f"plage inversée {row['logical_name']}: {start}..{end}")
        if int(row["register_count"]) != end - start + 1:
            fail(f"register_count incohérent pour {row['logical_name']}")
        by_block[block].append((start, end, row["logical_name"]))

    for block, expected in EXPECTED.items():
        fields = sorted(by_block[block])
        if not fields:
            fail(f"bloc {block} vide")
        if (fields[0][0], fields[-1][1]) != expected:
            fail(f"bornes bloc {block}: {(fields[0][0], fields[-1][1])} != {expected}")
        cursor = expected[0]
        for start, end, name in fields:
            if start != cursor:
                fail(f"discontinuité bloc {block} avant {name}: attendu {cursor}, trouvé {start}")
            cursor = end + 1
        if cursor != expected[1] + 1:
            fail(f"fin incohérente bloc {block}")

    coverage = read_csv(COVERAGE)
    exposed = [r for r in coverage if r["kind"] == "exposed_block"]
    gaps = [r for r in coverage if r["kind"] == "invalid_gap"]
    if len(exposed) != 8:
        fail(f"couverture: {len(exposed)} blocs exposés au lieu de 8")
    if len(gaps) != 7:
        fail(f"couverture: {len(gaps)} lacunes au lieu de 7")

    for block, (start, end) in EXPECTED.items():
        match = [r for r in exposed if r["id"] == f"B{block}"]
        if len(match) != 1:
            fail(f"couverture bloc B{block} absente ou dupliquée")
        r = match[0]
        if (int(r["address_start"]), int(r["address_end"])) != (start, end):
            fail(f"couverture B{block} non alignée")
        count = end - start + 1
        expected_single = "yes" if count <= FC03_MAX else "no"
        if r["fc03_single_request"] != expected_single:
            fail(f"indicateur FC03 incorrect pour B{block}")

    print("OK FT-STR-06: 8 blocs continus, 7 lacunes couvertes, segmentation FC03 cohérente.")
    print("NOTE: ce contrôle est structurel; les réponses Modbus doivent être testées sur cible.")


if __name__ == "__main__":
    main()
