#!/usr/bin/env python3
"""Validation mécanique de FT-STR-02 à partir de GEL-MAP-V1.

Ce script vérifie uniquement les invariants structurels de typage :
- type explicite et autorisé ;
- taille compatible avec le type ;
- `uint16[n]` utilisé uniquement comme regroupement documentaire cohérent ;
- absence de types historiques/interdits actifs.

Il ne valide ni la sémantique métier, ni l'ordre MSW/LSW, ni l'ASCII,
ni les droits d'accès, ni le comportement du serveur Modbus.
"""

from __future__ import annotations

import csv
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAPPING = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"

ALLOWED_SCALAR = {"uint16", "int16", "uint32", "bitfield16", "enum16", "ASCII fixe"}
FIXED_REGISTERS = {
    "uint16": 1,
    "int16": 1,
    "uint32": 2,
    "bitfield16": 1,
    "enum16": 1,
}
GROUP_RE = re.compile(r"^uint16\[(\d+)\]$")
FORBIDDEN = {"enum", "bitfield", "float", "float16", "float32", "float64", "réservé", "reserve"}


def validate_row(row: dict[str, str], line_no: int) -> list[str]:
    errors: list[str] = []
    typ = row["declared_type"].strip()
    try:
        count = int(row["register_count"])
    except ValueError:
        return [f"ligne {line_no}: register_count invalide: {row['register_count']!r}"]

    if not typ:
        errors.append(f"ligne {line_no}: type absent")
        return errors

    if typ.lower() in FORBIDDEN:
        errors.append(f"ligne {line_no}: type interdit/historique actif: {typ}")
        return errors

    if typ in FIXED_REGISTERS:
        expected = FIXED_REGISTERS[typ]
        if count != expected:
            errors.append(
                f"ligne {line_no}: {row['logical_name']} type {typ} -> {expected} registre(s), trouvé {count}"
            )
        return errors

    if typ == "ASCII fixe":
        if count <= 0:
            errors.append(f"ligne {line_no}: ASCII fixe avec taille non positive")
        return errors

    match = GROUP_RE.fullmatch(typ)
    if match:
        expected = int(match.group(1))
        if expected <= 0:
            errors.append(f"ligne {line_no}: regroupement {typ} invalide")
        elif count != expected:
            errors.append(
                f"ligne {line_no}: {row['logical_name']} {typ} -> {expected} registres, trouvé {count}"
            )
        return errors

    errors.append(f"ligne {line_no}: type non autorisé: {typ}")
    return errors


def main() -> int:
    if not MAPPING.exists():
        print(f"ERREUR: mapping introuvable: {MAPPING}", file=sys.stderr)
        return 2

    errors: list[str] = []
    counts: Counter[int] = Counter()
    rows = 0

    with MAPPING.open("r", encoding="utf-8-sig", newline="") as handle:
        reader = csv.DictReader(handle)
        required = {"block", "logical_name", "declared_type", "register_count"}
        missing = required - set(reader.fieldnames or [])
        if missing:
            print(f"ERREUR: colonnes manquantes: {sorted(missing)}", file=sys.stderr)
            return 2

        for line_no, row in enumerate(reader, start=2):
            rows += 1
            try:
                counts[int(row["block"])] += 1
            except ValueError:
                errors.append(f"ligne {line_no}: bloc invalide: {row['block']!r}")
            errors.extend(validate_row(row, line_no))

    expected_counts = {0: 10, 1: 18, 2: 12, 3: 26, 4: 66, 5: 18, 6: 20, 7: 13}
    if rows != 183:
        errors.append(f"couverture: 183 champs attendus, trouvé {rows}")
    if dict(sorted(counts.items())) != expected_counts:
        errors.append(
            f"couverture par bloc inattendue: {dict(sorted(counts.items()))}; attendu {expected_counts}"
        )

    if errors:
        print("FT-STR-02: NON CONFORME")
        for error in errors:
            print(f"- {error}")
        return 1

    print("FT-STR-02: CONFORME")
    print(f"- champs logiques contrôlés: {rows}")
    print(f"- couverture par bloc: {dict(sorted(counts.items()))}")
    print("- types protocolaires: conformes à la charte")
    print("- tailles structurelles: conformes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
