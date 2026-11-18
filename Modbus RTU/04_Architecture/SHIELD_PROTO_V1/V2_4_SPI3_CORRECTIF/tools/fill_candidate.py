"""Fill candidate only through native KiCad, output temporary readback for merge.
Never saves a historical PCB or a manufacturer source.
"""
from pathlib import Path
import sys
import pcbnew as k
source,output=map(Path,sys.argv[1:3]);assert not output.exists()
b=k.LoadBoard(str(source));b.BuildConnectivity();k.ZONE_FILLER(b).Fill(b.Zones());k.SaveBoard(str(output),b)
print('Candidate zones filled, private temporary board:',output)
