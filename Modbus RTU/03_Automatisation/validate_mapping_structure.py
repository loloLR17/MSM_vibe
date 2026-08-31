#!/usr/bin/env python3
"""Validation structurelle du mapping unifié TR2.

Ce script ne remplace pas l'audit normatif contre 01_Specification_source.
Il vérifie les invariants mécaniques des vues brut/logique, leur cohérence
réciproque et la synthèse de couverture.

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
BRUT = MAPPING_DIR / "tr2_mapping_unifie_brut.csv"
LOGICAL = MAPPING_DIR / "tr2_mapping_unifie_logique.csv"
COVERAGE = MAPPING_DIR / "tr2_mapping_couverture.csv"

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
VALID_ACCESS = {"RO", "RW"}
VALID_KINDS = {"declared_as_is", "uint32_from_split_words", "ascii_fixed_from_register_parts"}


def fail(errors: list[str], message: str) -> None:
    errors.append(message)


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8-sig", newline="") as handle:
        return list(csv.DictReader(handle))


def parse_int(row: dict[str, str], key: str, line_no: int, errors: list[str], label: str) -> int | None:
    try:
        return int(row[key])
    except (KeyError, TypeError, ValueError) as exc:
        fail(errors, f"{label} ligne {line_no}: {key} invalide ({exc})")
        return None


def validate_type(dtype: str, register_count: int, line_no: int, errors: list[str], label: str) -> None:
    array_match = ARRAY_TYPE_RE.fullmatch(dtype)
    if dtype not in SCALAR_TYPES and not array_match:
        fail(errors, f"{label} ligne {line_no}: type non autorisé {dtype!r}")
    if array_match and int(array_match.group(1)) != register_count:
        fail(errors, f"{label} ligne {line_no}: {dtype} incompatible avec register_count={register_count}")


def validate_reserved_name(name: str, line_no: int, errors: list[str], label: str) -> None:
    lowered = name.lower()
    if "reserved" in lowered and not lowered.startswith("reserved_") and not name.upper().startswith("B3_RESERVED_"):
        fail(errors, f"{label} ligne {line_no}: nom de réservé non conforme {name!r}")


def validate_ranges(rows: list[dict[str, str]], *, name_key: str, label: str, errors: list[str]) -> None:
    occupancy: dict[int, Counter[int]] = {block: Counter() for block in EXPECTED_BLOCK_RANGES}
    exact_ranges: Counter[tuple[int, int, int]] = Counter()

    for line_no, row in enumerate(rows, start=2):
        values = {
            key: parse_int(row, key, line_no, errors, label)
            for key in ("block", "offset_start", "offset_end", "address_start", "address_end", "register_count")
        }
        if any(value is None for value in values.values()):
            continue

        block = values["block"]
        offset_start = values["offset_start"]
        offset_end = values["offset_end"]
        address_start = values["address_start"]
        address_end = values["address_end"]
        register_count = values["register_count"]
        assert block is not None and offset_start is not None and offset_end is not None
        assert address_start is not None and address_end is not None and register_count is not None

        if block not in EXPECTED_BLOCK_RANGES:
            fail(errors, f"{label} ligne {line_no}: bloc inattendu {block}")
            continue

        if offset_end < offset_start or address_end < address_start:
            fail(errors, f"{label} ligne {line_no}: plage inversée")
            continue

        span_by_offset = offset_end - offset_start + 1
        span_by_address = address_end - address_start + 1
        if register_count != span_by_offset or register_count != span_by_address:
            fail(
                errors,
                f"{label} ligne {line_no}: register_count={register_count} incohérent avec "
                f"offset={span_by_offset} / adresse={span_by_address}",
            )

        block_base = EXPECTED_BLOCK_RANGES[block][0]
        if address_start != block_base + offset_start or address_end != block_base + offset_end:
            fail(errors, f"{label} ligne {line_no}: relation offset/adresse incohérente")

        access = row.get("access", "").strip()
        if access not in VALID_ACCESS:
            fail(errors, f"{label} ligne {line_no}: accès non autorisé {access!r}")

        dtype = row.get("declared_type", "").strip()
        validate_type(dtype, register_count, line_no, errors, label)

        name = row.get(name_key, "").strip()
        if not name:
            fail(errors, f"{label} ligne {line_no}: nom vide")
        else:
            validate_reserved_name(name, line_no, errors, label)

        exact_ranges[(block, address_start, address_end)] += 1
        for address in range(address_start, address_end + 1):
            occupancy[block][address] += 1

    for (block, start, end), count in exact_ranges.items():
        if count > 1:
            fail(errors, f"{label} bloc {block}: plage dupliquée {start}-{end} ({count} occurrences)")

    for block, (expected_start, expected_end) in EXPECTED_BLOCK_RANGES.items():
        counts = occupancy[block]
        expected = set(range(expected_start, expected_end + 1))
        present = set(counts)
        missing = sorted(expected - present)
        outside = sorted(present - expected)
        overlaps = sorted(address for address, count in counts.items() if count > 1)

        if missing:
            fail(errors, f"{label} bloc {block}: adresses manquantes {missing}")
        if outside:
            fail(errors, f"{label} bloc {block}: adresses hors plage {outside}")
        if overlaps:
            fail(errors, f"{label} bloc {block}: chevauchements {overlaps}")


def validate_brut_logical_consistency(
    brut_rows: list[dict[str, str]], logical_rows: list[dict[str, str]], errors: list[str]
) -> None:
    brut_by_key: dict[tuple[int, str], dict[str, str]] = {}
    for line_no, row in enumerate(brut_rows, start=2):
        try:
            block = int(row["block"])
        except (KeyError, TypeError, ValueError):
            continue
        key = (block, row.get("field_name", "").strip())
        if key in brut_by_key:
            fail(errors, f"brut ligne {line_no}: champ dupliqué dans un même bloc {key}")
        brut_by_key[key] = row

    consumed: Counter[tuple[int, str]] = Counter()

    for line_no, logical in enumerate(logical_rows, start=2):
        try:
            block = int(logical["block"])
            l_start = int(logical["address_start"])
            l_end = int(logical["address_end"])
            l_count = int(logical["register_count"])
        except (KeyError, TypeError, ValueError):
            continue

        kind = logical.get("kind", "").strip()
        if kind not in VALID_KINDS:
            fail(errors, f"logique ligne {line_no}: kind non autorisé {kind!r}")
            continue

        source_fields = [part.strip() for part in logical.get("source_fields", "").split(";") if part.strip()]
        if not source_fields:
            fail(errors, f"logique ligne {line_no}: source_fields vide")
            continue

        source_rows: list[dict[str, str]] = []
        for field in source_fields:
            key = (block, field)
            source = brut_by_key.get(key)
            if source is None:
                fail(errors, f"logique ligne {line_no}: source absente du brut {key}")
                continue
            source_rows.append(source)
            consumed[key] += 1

        if len(source_rows) != len(source_fields):
            continue

        addresses: list[int] = []
        for source in source_rows:
            try:
                addresses.extend(range(int(source["address_start"]), int(source["address_end"]) + 1))
            except (KeyError, TypeError, ValueError):
                pass

        if addresses:
            if min(addresses) != l_start or max(addresses) != l_end or len(set(addresses)) != l_count:
                fail(errors, f"logique ligne {line_no}: portée des source_fields incohérente avec la plage logique")
            if sorted(set(addresses)) != list(range(l_start, l_end + 1)):
                fail(errors, f"logique ligne {line_no}: source_fields non contigus ou incomplets")

        accesses = {source.get("access", "").strip() for source in source_rows}
        if accesses != {logical.get("access", "").strip()}:
            fail(errors, f"logique ligne {line_no}: accès différent entre brut et logique")

        source_files = {source.get("source_file", "").strip() for source in source_rows}
        if source_files != {logical.get("source_file", "").strip()}:
            fail(errors, f"logique ligne {line_no}: source_file différent entre brut et logique")

        logical_type = logical.get("declared_type", "").strip()
        source_types = [source.get("declared_type", "").strip() for source in source_rows]

        if kind == "declared_as_is":
            if len(source_rows) != 1:
                fail(errors, f"logique ligne {line_no}: declared_as_is doit référencer un seul champ brut")
            elif logical_type != source_types[0]:
                fail(errors, f"logique ligne {line_no}: type modifié sans règle de regroupement")
        elif kind == "uint32_from_split_words":
            if len(source_rows) != 2 or source_types != ["uint16", "uint16"] or logical_type != "uint32" or l_count != 2:
                fail(errors, f"logique ligne {line_no}: regroupement uint32 invalide")
        elif kind == "ascii_fixed_from_register_parts":
            if logical_type != "ASCII fixe" or any(dtype != "uint16" for dtype in source_types):
                fail(errors, f"logique ligne {line_no}: regroupement ASCII fixe invalide")
            if len(source_rows) != l_count:
                fail(errors, f"logique ligne {line_no}: nombre de registres ASCII incohérent")

    for key in brut_by_key:
        if consumed[key] == 0:
            fail(errors, f"brut: champ non représenté dans la vue logique {key}")
        elif consumed[key] > 1:
            fail(errors, f"brut: champ consommé plusieurs fois par la vue logique {key} ({consumed[key]})")


def validate_coverage(coverage_rows: list[dict[str, str]], errors: list[str]) -> None:
    seen_blocks: set[int] = set()
    for line_no, row in enumerate(coverage_rows, start=2):
        try:
            block = int(row["block"])
            address_start = int(row["address_start"])
            address_end = int(row["address_end"])
            covered = int(row["covered_registers"])
            span = int(row["span_registers"])
            missing = int(row["missing_inside_span"])
            overlap = int(row["overlap_address_count"])
            duplicate = int(row["duplicate_range_count"])
        except (KeyError, TypeError, ValueError) as exc:
            fail(errors, f"couverture ligne {line_no}: valeur invalide ({exc})")
            continue

        if block not in EXPECTED_BLOCK_RANGES:
            fail(errors, f"couverture ligne {line_no}: bloc inattendu {block}")
            continue
        seen_blocks.add(block)

        expected_start, expected_end = EXPECTED_BLOCK_RANGES[block]
        expected_span = expected_end - expected_start + 1
        if (address_start, address_end) != (expected_start, expected_end):
            fail(errors, f"couverture bloc {block}: bornes incorrectes")
        if covered != expected_span or span != expected_span:
            fail(errors, f"couverture bloc {block}: compte de registres incorrect")
        if missing != 0 or overlap != 0 or duplicate != 0:
            fail(errors, f"couverture bloc {block}: lacune/chevauchement/doublon déclaré")

    missing_blocks = sorted(set(EXPECTED_BLOCK_RANGES) - seen_blocks)
    if missing_blocks:
        fail(errors, f"couverture: blocs absents {missing_blocks}")


def main() -> int:
    errors: list[str] = []

    brut_rows = read_csv(BRUT)
    logical_rows = read_csv(LOGICAL)
    coverage_rows = read_csv(COVERAGE)

    brut_required = {
        "block", "offset_start", "offset_end", "address_start", "address_end",
        "field_name", "declared_type", "register_count", "access", "source_file",
    }
    logical_required = {
        "block", "logical_name", "kind", "source_fields", "offset_start", "offset_end",
        "address_start", "address_end", "declared_type", "register_count", "access", "source_file",
    }

    if not brut_rows:
        fail(errors, "mapping brut vide")
    elif not brut_required.issubset(brut_rows[0]):
        fail(errors, f"brut: colonnes manquantes {sorted(brut_required - set(brut_rows[0]))}")

    if not logical_rows:
        fail(errors, "mapping logique vide")
    elif not logical_required.issubset(logical_rows[0]):
        fail(errors, f"logique: colonnes manquantes {sorted(logical_required - set(logical_rows[0]))}")

    validate_ranges(brut_rows, name_key="field_name", label="brut", errors=errors)
    validate_ranges(logical_rows, name_key="logical_name", label="logique", errors=errors)
    validate_brut_logical_consistency(brut_rows, logical_rows, errors)
    validate_coverage(coverage_rows, errors)

    if errors:
        print("ECHEC — mapping structurel non conforme")
        for error in errors:
            print(f"- {error}")
        return 1

    print("OK — mapping structurel conforme")
    print("- vues brut et logique : plages, tailles, types et accès cohérents")
    print("- brut ↔ logique : tous les champs sont représentés une fois")
    print("- couverture : huit blocs complets, aucune lacune ni chevauchement")
    for block, (start, end) in EXPECTED_BLOCK_RANGES.items():
        print(f"- Bloc {block}: {start}-{end}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
