#!/usr/bin/env python3
"""Validation mécanique FT-STR-03 des champs uint32 de GEL-MAP-V1.

Ce script ne valide pas la sémantique métier ni l'atomicité temporelle.
Il vérifie uniquement les invariants structurels nécessaires à l'encodage
multi-registres défini par la charte de typage.
"""

from __future__ import annotations

import csv
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
MAPPING = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"


def as_int(row: dict[str, str], key: str) -> int:
    try:
        return int(row[key])
    except (KeyError, TypeError, ValueError) as exc:
        raise ValueError(f"{row.get('logical_name', '?')}: {key} invalide") from exc


def main() -> int:
    errors: list[str] = []
    checked = 0

    with MAPPING.open(newline="", encoding="utf-8-sig") as handle:
        rows = csv.DictReader(handle)
        for row in rows:
            if row.get("declared_type") != "uint32":
                continue

            checked += 1
            name = row.get("logical_name", "?")
            try:
                register_count = as_int(row, "register_count")
                address_start = as_int(row, "address_start")
                address_end = as_int(row, "address_end")
                offset_start = as_int(row, "offset_start")
                offset_end = as_int(row, "offset_end")
            except ValueError as exc:
                errors.append(str(exc))
                continue

            if register_count != 2:
                errors.append(f"{name}: register_count={register_count}, attendu 2")
            if address_end != address_start + 1:
                errors.append(f"{name}: adresses non consécutives {address_start}..{address_end}")
            if offset_end != offset_start + 1:
                errors.append(f"{name}: offsets non consécutifs {offset_start}..{offset_end}")

            if row.get("kind") == "uint32_from_split_words":
                parts = [part.strip() for part in row.get("source_fields", "").split(";") if part.strip()]
                if len(parts) != 2:
                    errors.append(f"{name}: 2 source_fields attendus, obtenu {len(parts)}")
                elif not (parts[0].lower().endswith("_msw") and parts[1].lower().endswith("_lsw")):
                    errors.append(f"{name}: ordre source_fields attendu *_msw;*_lsw, obtenu {';'.join(parts)}")

    if checked == 0:
        errors.append("aucun champ uint32 trouvé dans GEL-MAP-V1")

    if errors:
        print(f"FT-STR-03: ECHEC — {checked} champ(s) uint32 contrôlé(s)")
        for error in errors:
            print(f"- {error}")
        return 1

    print(f"FT-STR-03: CONFORME — {checked} champ(s) uint32 contrôlé(s)")
    print("Convention normative: registre N = MSW, registre N+1 = LSW")
    return 0


if __name__ == "__main__":
    sys.exit(main())
