"""Read the preserved PCB with native KiCad. Never save or alter the board."""
import hashlib
import json
import sys
from pathlib import Path

import pcbnew as k

source, output = map(Path, sys.argv[1:3])
board = k.LoadBoard(str(source))
pads, footprints, tracks = [], [], []
for footprint in board.GetFootprints():
    ref = footprint.GetReference()
    footprints.append({"ref": ref, "value": footprint.GetValue(),
                       "library": str(footprint.GetFPID()),
                       "layer": footprint.GetLayerName(),
                       "angle_deg": footprint.GetOrientationDegrees()})
    for pad in footprint.Pads():
        pads.append({"ref": ref, "pin": pad.GetNumber(), "net": pad.GetNetname(),
                     "x": k.ToMM(pad.GetPosition().x), "y": k.ToMM(pad.GetPosition().y),
                     "drill": k.ToMM(pad.GetDrillSize().x),
                     "size_x": k.ToMM(pad.GetSize().x), "size_y": k.ToMM(pad.GetSize().y),
                     "npth": pad.GetAttribute() == k.PAD_ATTRIB_NPTH})
for track in board.GetTracks():
    tracks.append({"net": track.GetNetname(), "layer": track.GetLayerName(),
                   "via": isinstance(track, k.PCB_VIA),
                   "start": [k.ToMM(track.GetStart().x), k.ToMM(track.GetStart().y)],
                   "end": [k.ToMM(track.GetEnd().x), k.ToMM(track.GetEnd().y)],
                   "width": k.ToMM(track.GetWidth(k.F_Cu) if isinstance(track, k.PCB_VIA) else track.GetWidth())})
output.write_text(json.dumps({"source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
                              "operation": "LoadBoard only; no Save/fill/DRC/export",
                              "footprints": footprints, "pads": pads, "tracks": tracks}, indent=2) + "\n")
print("Read-only PCB:", len(footprints), "footprints;", len(pads), "pads;", len(tracks), "tracks/vias")
