#!/usr/bin/env python3
"""Validation mécanique FT-STR-05 sur GEL-MAP-V1.

Ce script vérifie l'inventaire et la géométrie des zones réservées du mapping
dérivé. Il ne lit pas le capteur et ne valide pas les droits d'écriture.
"""

import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAPPING = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"

EXPECTED = {
    (0, "reserved_0", 20, 20, 1),
    (1, "reserved_1", 1017, 1017, 1),
    (1, "reserved_2", 1018, 1018, 1),
    (1, "reserved_3", 1019, 1019, 1),
    (2, "reserved_1", 2014, 2014, 1),
    (2, "reserved_2", 2015, 2015, 1),
    (3, "B3_RESERVED_0", 3040, 3047, 8),
    (4, "reserved_4A", 4014, 4015, 2),
    (4, "reserved_4B_0", 4017, 4017, 1),
    (4, "reserved_4B", 4028, 4039, 12),
    (4, "reserved_4C", 4047, 4055, 9),
    (4, "reserved_4D", 4096, 4099, 4),
    (4, "reserved_4E_A", 4111, 4115, 5),
    (4, "reserved_4E_B", 4123, 4127, 5),
    (4, "reserved_4E_C", 4168, 4175, 8),
    (6, "reserved_6A", 6010, 6011, 2),
    (6, "reserved_6B", 6058, 6063, 6),
    (7, "reserved_7A", 7014, 7015, 2),
}


def is_reserved(name: str) -> bool:
    return name.lower().startswith("reserved_") or name.upper().startswith("B3_RESERVED_")


def main() -> int:
    if not MAPPING.exists():
        print(f"ERREUR: mapping absent: {MAPPING}")
        return 2

    errors = []
    actual = set()
    with MAPPING.open(encoding="utf-8-sig", newline="") as f:
        for row in csv.DictReader(f):
            name = row["logical_name"].strip()
            if not is_reserved(name):
                continue
            try:
                block = int(row["block"])
                start = int(row["address_start"])
                end = int(row["address_end"])
                count = int(row["register_count"])
            except (KeyError, ValueError) as exc:
                errors.append(f"ligne réservée mal formée {name}: {exc}")
                continue

            actual.add((block, name, start, end, count))
            if end - start + 1 != count:
                errors.append(f"{name}: plage {start}-{end} incompatible avec register_count={count}")

            dtype = row["declared_type"].strip()
            if count == 1:
                if dtype != "uint16":
                    errors.append(f"{name}: type attendu uint16, trouvé {dtype}")
            elif dtype != f"uint16[{count}]":
                errors.append(f"{name}: type attendu uint16[{count}], trouvé {dtype}")

            if row["access"].strip() != "RO":
                errors.append(f"{name}: accès documentaire attendu RO")

    missing = EXPECTED - actual
    extra = actual - EXPECTED
    if missing:
        errors.append("zones réservées manquantes: " + repr(sorted(missing)))
    if extra:
        errors.append("zones réservées inattendues: " + repr(sorted(extra)))
    if len(actual) != 18:
        errors.append(f"18 zones attendues, {len(actual)} trouvées")
    if sum(item[4] for item in actual) != 70:
        errors.append(f"70 registres réservés attendus, {sum(item[4] for item in actual)} trouvés")

    # Alias historiques explicitement interdits dans l'actif.
    legacy_names = {item[1] for item in actual} & {"reserved", "reserved_4B_17"}
    if legacy_names:
        errors.append("alias historiques actifs: " + ", ".join(sorted(legacy_names)))

    if errors:
        for error in errors:
            print("NON CONFORME:", error)
        return 1

    print("CONFORME: 18 zones réservées / 70 registres alignés sur GEL-MAP-V1")
    return 0


if __name__ == "__main__":
    sys.exit(main())
