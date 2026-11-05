"""Simple insulating through-spacer; NO default mating height or print release.

One standard metal M2 bolt/nut per anchor, insulating washers and a compression
spacer. This supersedes a suspended threaded-rod/nut arrangement, not an approved
assembly. An actual H and copper/component-clearance review are prerequisites.
"""
import argparse,json
from pathlib import Path
import cadquery as cq
P=Path(__file__).resolve().parents[1]
def spacer(height):
    if not 4<=height<=18:raise ValueError('Height outside this bounded study')
    # OD5 stays behind the microSD mouth; 2.4 bore for M2, no printed thread.
    return cq.Solid.makeCylinder(2.5,height).cut(cq.Solid.makeCylinder(1.2,height))
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--height-mm',type=float,required=True)
    parser.add_argument('--measured-reference',required=True,help='Evidence reference for actual unforced mating height')
    args=parser.parse_args();s=spacer(args.height_mm);assert s.isValid()and len(s.Solids())==1
    out=P/'A_REVOIR_APPUI_MODULE';out.mkdir(exist_ok=True)
    cq.exporters.export(s,str(out/'appui-H-mesure.step'))
    cq.exporters.export(s,str(out/'appui-H-mesure.stl'),tolerance=.025)
    (out/'NON_LIBERE.json').write_text(json.dumps({'height_mm':args.height_mm,'measured_reference':args.measured_reference,'release':False,'remaining':'Verify module/shield copper, washer and bolt envelopes, SD passage and unforced contact before any operator-pack inclusion'},indent=2)+'\n')
    print('Candidate written outside printer pack; not released')
if __name__=='__main__':main()
