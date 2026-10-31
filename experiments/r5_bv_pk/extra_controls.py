#!/usr/bin/env python3
"""Additional whole-campaign extrema control from saved peaks, no FFT repeated.
The initial velocity controls targeted qualified components. This audit also
covers the global weak/median/strong candidates even when not qualified.
"""
import argparse,csv,gzip,json
from pathlib import Path
from core import BANDS,temporal_component
from run import minutes,dump
from pk_catalog import typed_row,sha

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('results',type=Path);ap.add_argument('new_output',type=Path);a=ap.parse_args()
    if a.new_output.exists():raise ValueError('output already exists')
    m=json.loads((a.results/'manifest.json').read_text());old=json.loads((a.results/'independent_real.json').read_text())
    with gzip.open(a.results/'components.csv.gz','rt') as f:rows=[typed_row(r) for r in csv.DictReader(f)]
    have={(r['start_mcu_s'],r['band'],r['clock'],r['axis']) for r in old};selected=[]
    for band in BANDS:
        byminute={}
        for r in rows:
            if r['clock']=='MCU' and r['band']==band and r['window_mcu_s']==60 and r['component_pk'] is not None:
                s=r['start_mcu_s']
                if s not in byminute or r['component_pk']>byminute[s]['component_pk']:byminute[s]=r
        ordered=sorted(byminute.values(),key=lambda r:r['component_pk'])
        for label,r in zip(['global_low','global_median','global_high'],[ordered[0],ordered[len(ordered)//2],ordered[-1]]):
            for clock in m['clocks']:
                key=r['start_mcu_s'],band,clock,r['axis']
                if key not in have:
                    q=next(q for q in rows if (q['start_mcu_s'],q['band'],q['clock'],q['axis'])==key and q['window_mcu_s']==60);selected.append((label,q))
    needed={r['start_mcu_s'] for _,r in selected};data={}
    for start,t,x,index in minutes(Path(m['source']['path']),m['source']):
        if start in needed:data[start]=x
    result=[]
    for label,r in selected:
        fit=temporal_component(data[r['start_mcu_s']][:,'XYZ'.index(r['axis'])],m['clocks'][r['clock']],r['frequency_Hz'],r['band'])
        discrepancy=abs(fit['component_pk']/r['component_pk']-1);df=abs(fit['frequency_Hz']-r['frequency_Hz']);agree=discrepancy<=.05 and df<=.02
        result.append(dict(selection=label,start_mcu_s=r['start_mcu_s'],axis=r['axis'],clock=r['clock'],band=r['band'],spectral_pk=r['component_pk'],spectral_frequency_Hz=r['frequency_Hz'],temporal=fit,relative_amplitude_discrepancy=discrepancy,frequency_discrepancy_Hz=df,initial_status=r['status'],comparison_eligible=r['comparison_eligible'],agreement=agree,fixed_limits=dict(amplitude_relative=.05,frequency_Hz=.02)))
    dump(a.new_output,result)
    if any(r['comparison_eligible'] and not r['agreement'] for r in result):raise ValueError('new qualified real control failed: review whole family before publication')
    print(json.dumps(dict(additional_controls=len(result),FFT_repeated=False),indent=2))

if __name__=='__main__':main()
