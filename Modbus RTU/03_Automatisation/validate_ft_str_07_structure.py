#!/usr/bin/env python3
"""Validation mécanique de la reconstruction FT-STR-07.

Ce script ne démontre aucun comportement runtime. Il vérifie seulement :
- présence des 8 blocs dans GEL-MAP-V1 ;
- présence de champs uint32 à couvrir par GEN-002 ;
- présence des blocs dynamiques prioritaires B1/B2/B3/B6/B7 ;
- détection de la plage B4 > 125 registres, qui interdit un snapshot FC03 monolithique ;
- présence des quatre tests génériques actifs ;
- absence des principaux artefacts legacy dans les répertoires actifs.
"""

from __future__ import annotations

import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "02_Validation" / "mapping_unifie" / "tr2_mapping_unifie_logique.csv"
BASE = ROOT / "02_Validation" / "FT_STR" / "FT-STR-07"


def fail(msg: str) -> None:
    print(f"ERREUR: {msg}")
    raise SystemExit(1)


def main() -> int:
    if not MAP.is_file():
        fail(f"mapping absent: {MAP}")

    with MAP.open(encoding="utf-8-sig", newline="") as fh:
        rows = list(csv.DictReader(fh))

    blocks = {int(r["block"]) for r in rows}
    if blocks != set(range(8)):
        fail(f"blocs mapping inattendus: {sorted(blocks)}")

    uint32_rows = [r for r in rows if r["declared_type"] == "uint32"]
    if not uint32_rows:
        fail("aucun uint32 détecté")

    priority = {1, 2, 3, 6, 7}
    if not priority.issubset(blocks):
        fail("un bloc dynamique prioritaire est absent")

    b4 = [r for r in rows if int(r["block"]) == 4]
    b4_start = min(int(r["address_start"]) for r in b4)
    b4_end = max(int(r["address_end"]) for r in b4)
    b4_span = b4_end - b4_start + 1
    if b4_span <= 125:
        fail(f"B4 devrait dépasser la limite FC03, span observé={b4_span}")

    for idx in range(1, 5):
        p = BASE / "detaille" / f"TT-STR-07-GEN-{idx:03d}.md"
        if not p.is_file():
            fail(f"test générique absent: {p}")

    legacy_active = [
        BASE / "detaille" / "TT-STR-07-001.md",
        BASE / "instancie" / "FT-STR-07_instancie_index.csv",
        BASE / "instancie" / "FT-STR-07_instancie_overview.md",
    ]
    for p in legacy_active:
        if p.exists():
            fail(f"artefact legacy encore actif: {p}")

    print("FT-STR-07 structure: OK")
    print(f"Blocs: {sorted(blocks)}")
    print(f"Champs uint32 à considérer pour GEN-002: {len(uint32_rows)}")
    print(f"Bloc 4: {b4_start}..{b4_end} = {b4_span} registres (>125, segmentation requise)")
    print("NOTE: aucune conformité runtime n'est démontrée par ce script.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
