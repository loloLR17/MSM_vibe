#!/usr/bin/env python3
"""Validation mécanique FT-STR-04 sur GEL-MAP-V1.

Ce script vérifie uniquement des invariants structurels du mapping des champs
`ASCII fixe`. Il ne prétend pas valider le contenu runtime, l'ordre réel des
octets renvoyés par le firmware ni le padding observé sur le terrain.
"""

from __future__ import annotations

import csv
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
MAPPING = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"
EXPECTED_ASCII_FIELDS = 8


def fail(message: str) -> None:
    print(f"ERREUR: {message}", file=sys.stderr)
    raise SystemExit(1)


def main() -> None:
    with MAPPING.open("r", encoding="utf-8-sig", newline="") as handle:
        rows = list(csv.DictReader(handle))

    ascii_rows = [row for row in rows if row["declared_type"].strip() == "ASCII fixe"]

    if len(ascii_rows) != EXPECTED_ASCII_FIELDS:
        fail(
            f"GEL-MAP-V1 doit contenir {EXPECTED_ASCII_FIELDS} champs ASCII fixe, "
            f"trouvé {len(ascii_rows)}"
        )

    seen: set[tuple[str, str]] = set()

    for row in ascii_rows:
        key = (row["block"].strip(), row["logical_name"].strip())
        if key in seen:
            fail(f"champ ASCII dupliqué: bloc={key[0]} champ={key[1]}")
        seen.add(key)

        try:
            address_start = int(row["address_start"])
            address_end = int(row["address_end"])
            register_count = int(row["register_count"])
        except ValueError as exc:
            fail(f"valeur numérique invalide pour {key}: {exc}")

        if register_count <= 0:
            fail(f"nombre de registres non positif pour {key}: {register_count}")

        span = address_end - address_start + 1
        if span != register_count:
            fail(
                f"plage incohérente pour {key}: span={span}, "
                f"register_count={register_count}"
            )

        capacity = 2 * register_count
        print(
            f"OK bloc={key[0]} champ={key[1]} "
            f"registres={register_count} capacité={capacity} caractères"
        )

    print(f"CONFORME: {len(ascii_rows)} champs ASCII fixe contrôlés")


if __name__ == "__main__":
    main()
