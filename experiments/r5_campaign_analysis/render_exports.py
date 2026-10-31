#!/usr/bin/env python3
"""Render existing R5 aggregate exports: no input acquisition/filtering repeated.
Keep output in a NEW directory. Scientific results retain their original SHA.
"""
import argparse,csv,hashlib,json,shutil
from pathlib import Path
import numpy as np
from run_analysis import plots

def read_windows(path):
    with path.open(newline='') as f:rows=list(csv.DictReader(f))
    groups=[]
    for name in ['raw','band']:
        groups.append(dict(start_s=np.array([float(r['start_mcu_s']) for r in rows]),rms=np.array([[float(r[name+'_rms_'+a+'_mg']) for a in 'xyz'] for r in rows]),global_rms=np.array([float(r[name+'_rms_global_mg']) for r in rows]),vector_peak=np.array([float(r[name+'_peak_vector_mg']) for r in rows])))
    return groups

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('existing',type=Path);ap.add_argument('new_plots',type=Path);a=ap.parse_args();a.new_plots.mkdir(parents=True,exist_ok=False)
    r10,b10=read_windows(a.existing/'windows_10s.csv');r60,b60=read_windows(a.existing/'windows_60s.csv')
    s=np.load(a.existing/'spectrogram.npz')
    with (a.existing/'ridge_candidates.csv').open(newline='') as f:
        ridges=[[float(r['time_mcu_s']),float(r['candidate_center_est_Hz']),float(r['observed_est_Hz']) if r['observed_est_Hz'] else '',float(r['peak_global_mg2_per_Hz']) if r['peak_global_mg2_per_Hz'] else '',r['status']] for r in csv.DictReader(f)]
    spectra=[]
    with (a.existing/'segments_psd.csv').open(newline='') as f:rows=list(csv.DictReader(f))
    for label in ['low','median','high']:
        subset=[r for r in rows if r['segment']==label]
        spectra.append((label,float(subset[0]['start_mcu_s']),np.array([float(r['frequency_est_Hz']) for r in subset]),np.array([float(r['global_mg2_per_Hz']) for r in subset])))
    plots(a.new_plots,r10,b10,r60,b60,s['time_mcu_s'],s['frequency_est_Hz'],s['psd_xyz_mg2_per_Hz'],spectra,ridges)
    proof=dict(metric_source=str(a.existing),metric_processing_git_sha=json.loads((a.existing/'manifest.json').read_text())['processing_git_sha'],render_only=True,files=[])
    for p in sorted(a.new_plots.glob('*.png')):proof['files'].append(dict(name=p.name,bytes=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
    (a.new_plots/'render.json').write_text(json.dumps(proof,indent=2)+'\n')
if __name__=='__main__':main()
