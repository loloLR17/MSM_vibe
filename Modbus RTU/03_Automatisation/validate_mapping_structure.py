#!/usr/bin/env python3
"""Validation structurelle du mapping unifié TR2.

Ce script ne remplace pas l'audit normatif contre 01_Specification_source.
Il vérifie uniquement des invariants mécaniques du mapping dérivé.

Usage depuis la racine du dépôt :
    python3 "Modbus RTU/03_Automatisation/validate_mapping_structure.py"
"""

from __future__ import annotations

import csv
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAPPING_DIR = ROOT / "02_Validation" / "mapping_unifie"
LOGICAL = MAPPING_DIR / "tr2_mapping_unifie_logique.csv"

EXPECTED_BLOCK_RANGES = {
    0: (0, 20),
    1: (1000, 1019),
    2: (2000, 2015),
    3: (3000, 3047),
    4: (4000, 4175),
    5: (5000, 5019),
    6: (6000, 6063),
    7: (7000, 7015),
}

SCALAR_TYPES = {"uint16", "int16", "uint32", "bitfield16", "enum16", "ASCII fixe"}
ARRAY_TYPE_RE = re.compile(r"^uint16\[(\d+)\]$")


def fail(errors: list[str], message: str) -> None:
    errors.append(message)


def main() -> int:
    errors: list[str] = []

    with LOGICAL.open("r", encoding="utf-8-sig", newline="") as handle:
        rows = list(csv.DictReader(handle))

    required_columns = {
        "block",
        "logical_name",
        "offset_start",
        "offset_end",
        "address_start",
        "address_end",
        "declared_type",
        "register_count",
        "access",
        "source_file",
    }
    if not rows:
        fail(errors, "mapping logique vide")
    elif not required_columns.issubset(rows[0]):
        missing = sorted(required_columns - set(rows[0]))
        fail(errors, f"colonnes manquantes : {missing}")

    occupancy: dict[int, Counter[int]] = {block: Counter() for block in EXPECTED_BLOCK_RANGES}
    exact_ranges: Counter[tuple[int, int, int]] = Counter()

    for line_no, row in enumerate(rows, start=2):
        try:
            block = int(row["block"])
            offset_start = int(row["offset_start"])
            offset_end = int(row["offset_end"])
            address_start = int(row["address_start"])
            address_end = int(row["address_end"])
            register_count = int(row["register_count"])
        except (KeyError, TypeError, ValueError) as exc:
            fail(errors, f"ligne {line_no}: valeur numérique invalide ({exc})")
            continue

        name = row["logical_name"].strip()
        dtype = row["declared_type"].strip()
        access = row["access"].strip()

        if block not in EXPECTED_BLOCK_RANGES:
            fail(errors, f"ligne {line_no}: bloc inattendu {block}")
            continue

        if offset_end < offset_start or address_end < address_start:
            fail(errors, f"ligne {line_no}: plage inversée")
            continue

        span_by_offset = offset_end - offset_start + 1
        span_by_address = address_end - address_start + 1
        if register_count != span_by_offset or register_count != span_by_address:
            fail(
                errors,
                f"ligne {line_no}: register_count={register_count} incohérent avec la plage "
                f"offset={span_by_offset} / adresse={span_by_address}",
            )

        # L'adresse absolue doit être la base du bloc + l'offset.
        block_base = EXPECTED_BLOCK_RANGES[block][0]
        if address_start != block_base + offset_start or address_end != block_base + offset_end:
            fail(errors, f"ligne {line_no}: relation offset/adresse incohérente")

        if access not in {"RO", "RW"}:
            fail(errors, f"ligne {line_no}: accès non autorisé {access!r}")

        array_match = ARRAY_TYPE_RE.fullmatch(dtype)
        if dtype not in SCALAR_TYPES and not array_match:
            fail(errors, f"ligne {line_no}: type non autorisé {dtype!r}")
        if array_match and int(array_match.group(1)) != register_count:
            fail(errors, f"ligne {line_no}: {dtype} incompatible avec register_count={register_count}")

        if "reserved" in name.lower() and not name.lower().startswith("reserved_") and not name.upper().startswith("B3_RESERVED_"):
            fail(errors, f"ligne {line_no}: nom de réservé non conforme {name!r}")

        exact_ranges[(block, address_start, address_end)] += 1
        for address in range(address_start, address_end + 1):
            occupancy[block][address] += 1

    for key, count in exact_ranges.items():
        if count > 1:
            block, start, end = key
            fail(errors, f"bloc {block}: plage dupliquée {start}-{end} ({count} occurrences)")

    for block, (expected_start, expected_end) in EXPECTED_BLOCK_RANGES.items():
        counts = occupancy[block]
        expected = set(range(expected_start, expected_end + 1))
        present = set(counts)

        missing = sorted(expected - present)
        outside = sorted(present - expected)
        overlaps = sorted(address for address, count in counts.items() if count > 1)

        if missing:
            fail(errors, f"bloc {block}: adresses manquantes {missing}")
        if outside:
            fail(errors, f"bloc {block}: adresses hors plage {outside}")
        if overlaps:
            fail(errors, f"bloc {block}: chevauchements {overlaps}")

    if errors:
        print("ECHEC — mapping structurel non conforme")
        for error in errors:
            print(f"- {error}")
        return 1

    print("OK — mapping structurel conforme")
    for block, (start, end) in EXPECTED_BLOCK_RANGES.items():
        print(f"- Bloc {block}: {start}-{end}, aucune lacune ni chevauchement")
    return 0


if __name__ == "__main__":
    sys.exit(main())
