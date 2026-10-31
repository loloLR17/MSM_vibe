#!/usr/bin/env python3
"""Presentation and sensitivity from SAVED components only. No signal FFT rerun."""
import argparse,csv,gzip,json,hashlib
from pathlib import Path
from collections import Counter
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from pk_catalog import typed_row
from core import BANDS,THRESHOLDS

def dump(p,x):p.write_text(json.dumps(x,indent=2,ensure_ascii=False,allow_nan=False)+'\n')

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('results',type=Path);ap.add_argument('output',type=Path);a=ap.parse_args();a.output.mkdir(parents=True,exist_ok=False);out=a.output
    with gzip.open(a.results/'components.csv.gz','rt',newline='') as f:rows=[typed_row(r) for r in csv.DictReader(f)]
    summary=[];sensitivity=[];clocks=[]
    for clock in ['MCU','QPC_ESTIMATE']:
        for band in BANDS:
            long=[r for r in rows if r['clock']==clock and r['band']==band and r['window_mcu_s']==60 and r['component_pk'] is not None]
            byminute={}
            for r in long:byminute.setdefault(r['start_mcu_s'],[]).append(r)
            maxima=[max(rs,key=lambda r:r['component_pk']) for rs in byminute.values()]
            v=np.array([r['component_pk'] for r in maxima]);accepted=[r for r in long if r['comparison_eligible']]
            summary.append(dict(clock=clock,band=band,unit=BANDS[band][2],candidate_minutes=len(v),candidate_max_axis_P50=float(np.median(v)),candidate_max_axis_P95=float(np.quantile(v,.95)),candidate_max_axis_max=float(v.max()),robust_axis_windows=len(accepted),robust_axis_zone_counts=dict(Counter(r['comparison_zone'] for r in accepted)),robust_axis_peak_range=[min(r['reported_pk'] for r in accepted),max(r['reported_pk'] for r in accepted)] if accepted else None,all_axes_minutes=0,interpretation='Candidate statistics include unresolved cases; NOT a comfort classification'))
            lookup={(r['start_mcu_s'],r['axis']):r for r in rows if r['clock']==clock and r['band']==band and r['window_mcu_s']==10}
            for r in long:
                short=[lookup.get((r['start_mcu_s']+10*k,r['axis'])) for k in range(6)]
                short=[q for q in short if q and q['component_pk'] is not None]
                if len(short)<6:continue
                matching=[q for q in short if abs(q['frequency_Hz']-r['frequency_Hz'])<=.2]
                sensitivity.append(dict(clock=clock,band=band,axis=r['axis'],start_mcu_s=r['start_mcu_s'],pk_60=r['component_pk'],max_10=max(q['component_pk'] for q in short),ratio_max10_to60=max(q['component_pk'] for q in short)/r['component_pk'],matching_top1_frames=len(matching),status_60=r['status'],meaning='max strongest component may switch source; NOT MRA conversion'))
    qlookup={(r['band'],r['window_mcu_s'],r['start_mcu_s'],r['axis']):r for r in rows if r['clock']=='QPC_ESTIMATE'}
    manifest=json.loads((a.results/'manifest.json').read_text());ratio=manifest['clocks']['QPC_ESTIMATE']/manifest['clocks']['MCU']
    for r in rows:
        if r['clock']!='MCU' or r['component_pk'] is None:continue
        q=qlookup[r['band'],r['window_mcu_s'],r['start_mcu_s'],r['axis']]
        if q['component_pk'] is None:continue
        same=abs(q['frequency_Hz']/r['frequency_Hz']-ratio)<1e-7
        clocks.append(dict(band=r['band'],window_mcu_s=r['window_mcu_s'],start_mcu_s=r['start_mcu_s'],axis=r['axis'],same_frequency_bin=same,frequency_MCU=r['frequency_Hz'],frequency_QPC=q['frequency_Hz'],relative_pk_difference_percent=100*(q['component_pk']/r['component_pk']-1),status_MCU=r['status'],status_QPC=q['status']))
    for name,data in [('window_sensitivity.csv',sensitivity),('clock_sensitivity.csv',clocks)]:
        with (out/name).open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(data[0]),lineterminator='\n');w.writeheader();w.writerows(data)
    diagnostic=[]
    for band in BANDS:
        for clock in ['MCU','QPC_ESTIMATE']:
            subset=[r for r in sensitivity if r['band']==band and r['clock']==clock];ratios=np.array([r['ratio_max10_to60'] for r in subset])
            diagnostic.append(dict(band=band,clock=clock,ratios_compared=len(ratios),ratio_max10_to60_P50=float(np.median(ratios)),ratio_max10_to60_P95=float(np.quantile(ratios,.95)),ratio_max10_to60_max=float(max(ratios))))
    dump(out/'candidate_statistics.json',summary);dump(out/'sensitivity_statistics.json',diagnostic)
    plt.rcParams.update({'font.size':10,'axes.grid':True,'grid.alpha':.22,'axes.spines.top':False,'axes.spines.right':False})
    for band in BANDS:
        frame=[r for r in rows if r['clock']=='MCU' and r['band']==band and r['window_mcu_s']==60 and r['component_pk'] is not None]
        good=[r for r in frame if r['comparison_eligible']]
        fig,axes=plt.subplots(3,1,figsize=(12,10),sharex=True,layout='constrained')
        for axis,color in zip('XYZ',['#2587bb','#dd8416','#188b69']):
            rr=[r for r in frame if r['axis']==axis];xx=np.array([r['start_mcu_s']+30 for r in rr])/3600
            axes[0].scatter(xx,[r['component_pk'] for r in rr],s=4,color=color,alpha=.65,label=axis+' · candidat60s')
            axes[1].scatter(xx,[r['frequency_Hz'] for r in rr],s=3,color=color,alpha=.5,label=axis)
            gg=[r for r in good if r['axis']==axis]
            if gg:axes[2].scatter(np.array([r['start_mcu_s']+30 for r in gg])/3600,[r['reported_pk'] for r in gg],s=10,color=color,label=axis+' · comparaison partielle')
        axes[0].set_ylabel('Crête candidate ('+BANDS[band][2]+')');axes[0].legend(ncol=3,fontsize=8)
        axes[0].set_title('Zoom sur les estimations candidates — cas non résolus inclus, pas un verdict')
        axes[1].set_ylabel('Fréquence estimée (Hz)');axes[1].set_ylim(BANDS[band][0],BANDS[band][1]);axes[1].set_title('Rang1 par axe : points non reliés, identité de source non démontrée')
        if good:
            for j,level in enumerate(THRESHOLDS[band],1):axes[2].axhline(level,ls='--',lw=.9,label=f'Repère{j}PK : {level:g}')
            axes[2].set_ylim(0,THRESHOLDS[band][-1]*1.12);axes[2].legend(ncol=3,fontsize=8)
            axes[2].set_title('Axes recevables seulement — aucune minute recevable sur les 3 axes, pas de grade')
        else:
            axes[2].text(.5,.5,'Méthodologie indéterminée sur cette bande\nAucune comparaison — repères non superposés',ha='center',va='center',transform=axes[2].transAxes)
            axes[2].set_yticks([])
        axes[2].set_xlabel('Temps relatif MCU nominal (h) — UTC inconnue')
        axes[2].set_ylabel('Lecture indicative\n('+BANDS[band][2]+')')
        fig.suptitle(band+' · composante individuelle · CF=1\nRéférence BV PC machine occupé en permanence : correspondance à confirmer\nPrototype non étalonné · XYZ non orientés navire · peak-hold spectral ≠ MRA garanti',fontsize=11)
        fig.savefig(out/(band+'.png'),dpi=150);plt.close(fig)
    cards=''
    for s in summary:
        if s['clock']!='MCU':continue
        cards+=f"<article><strong>{s['candidate_max_axis_P50']:.1f} {s['unit']}</strong><p>Médiane du maximum spectral candidat par minute<br>{s['band']} · cas non résolus inclus</p><p>{s['robust_axis_windows']} fenêtres-axes recevables ; aucune minute sur les 3 axes.</p></article>"
    tables=''
    for band in BANDS:
        levels=THRESHOLDS[band]
        tables+=f"<h3>{band}</h3><p>Unité : {BANDS[band][2]}, composante individuelle. Repères contextuels : 1PK {levels[0]:g} ; 2PK {levels[1]:g} ; 3PK {levels[2]:g}. Ces chiffres ne sont pas ceux d'un RMS large bande.</p><img src='{band}.png' alt='Composantes candidates, fréquences et comparaisons éventuellement recevables de {band}'>"
    html=f'''<!doctype html><html lang="fr"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>PC machine · composantes PK exploratoires</title><style>body{{margin:0;background:#f4f7fa;color:#20374b;font:16px/1.6 system-ui}}main{{max-width:1200px;margin:auto;padding:30px}}h1,h2,h3{{line-height:1.2}}header{{border-bottom:3px solid #147e9a}}.notice{{background:#fff1d7;border-left:5px solid #b87c12;padding:16px}}.cards{{display:flex;gap:20px;flex-wrap:wrap}}article{{background:white;border-radius:10px;padding:20px;flex:1;min-width:260px}}strong{{font-size:24px}}img{{width:100%}}section{{margin:30px 0}}a{{color:#056b93}}small{{color:#536d80}}@media print{{section{{break-inside:avoid}}}}</style><main><header><small>MSM TR2 · archive E6N0005 · campagne du 31 octobre 2026</small><h1>PC machine — lecture spectrale PK exploratoire</h1><p>Deux bandes, deux unités, des composantes individuelles — aucune conversion des anciens RMS.</p></header><section class="notice"><b>La comparaison globale PK reste indéterminée.</b> La bande1–5Hz n'offre ici aucune composante suffisamment robuste selon les contrôles retenus. En vitesse, quelques fenêtres-axes sont recevables, mais aucune minute ne l'est sur les 3 axes. Aucun grade ou jugement de confort n'est attribué.</section><section class="cards">{cards}</section><section><h2>Comprendre les deux repères</h2><p>Une crête de composante décrit l'amplitude d'une vibration quasi sinusoïdale à une fréquence précise. Ce n'est pas la crête instantanée du signal total ni √2 fois son RMS global. Entre 1 et 5 Hz, on examine l'accélération ; entre 5 et 100 Hz, la vitesse de la composante est calculée en divisant son accélération par 2πf.</p><p>Référence : BV NR467F juillet2026, chapitre6 section3 tableaux5/6, PC machine occupé en permanence à la mer, <b>correspondance réelle à confirmer</b>. Montage au double-face, sensibilité nominale et axes non repérés rendent les comparaisons strictement indicatives et non normatives.</p></section><section>{tables}</section><section><h2>Pourquoi les réserves ?</h2><p>On confronte60s et six sous-fenêtres de 10 s, avec Hann corrigé et estimation de fréquence entre les bins. Un second calcul ajuste une sinusoïde dans le temps. Modulation, raies proches, faible contraste, frontières de bande et divergences sont conservés, pas forcés dans une classe rassurante. En accélération, un contrôle réel diverge d'environ31 % : sa valeur reste candidate. Quelques composantes de vitesse concordent dans les limites fixées, sans prouver un confort du local.</p><p>Le maximum de pics spectraux courts n'est pas une mesure garantie du maximum répétitif d'un signal modulé. Une valeur faible non résolue ne permet pas de conclure «sous seuil».</p></section><section><h2>Preuves et diagnostics conservés</h2><p><a href="README.md">Méthode et résultats</a> · <a href="components.csv.gz">Composantes10/60s CSVcompressé</a> · <a href="independent_real.json">Contre-calculs réels</a> · <a href="window_sensitivity.csv">Durées</a> · <a href="clock_sensitivity.csv">Horloges</a> · <a href="sqlite-proof.json">SQLite</a></p><p><a href="../TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/index.html">RMS2–25Hz, PSD et raies initiaux</a> · <a href="../TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/index.html">Vitesse physique1–80Hz non pondérée</a></p><p>Temps relatif MCU, UTC inconnue ; autre base PC testée (+environ0.68% de cadence), sans correction SI qualifiée. Fichier original inchangé, intégrité interne vérifiée ; digest RAM indépendant perdu après coupure. Aucun nouveau test matériel. La voie RMS pondérée1–80Hz ISO/Wm reste distincte et bloquée.</p></section><footer><small>Ni classification, conformité, diagnostic causal, exposition humaine ni dommage structurel déduit. Revue indépendante ChatGPT requise.</small></footer></main></html>'''
    (out/'index.html').write_text(html)
    dump(out/'render_manifest.json',[dict(name=p.name,bytes=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in sorted(out.iterdir()) if p.is_file()])
    print(json.dumps(summary,indent=2,ensure_ascii=False))

if __name__=='__main__':main()
