"""Saved-result presentation, no raw data/refilter and no threshold inference."""
import argparse,csv,json
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from wm import *

def render(results,review,out):
 results,review,out=Path(results),Path(review),Path(out);out.mkdir(exist_ok=False,parents=True)
 defs=json.loads((results/'definitions.json').read_text());B=json.loads(review.read_text());assert B['selected_threshold'] is None
 with (results/'metrics.csv').open() as f:rows=list(csv.DictReader(f))
 for r in rows:
  for k in ['width_s','start_mcu_s']:r[k]=int(r[k])
  for k in ['rms_x','rms_y','rms_z','direction_max']:r[k]=float(r[k])
 main=[r for r in rows if r['name']=='MAIN'];plt.rcParams.update({'axes.spines.top':False,'axes.spines.right':False,'axes.grid':True,'grid.alpha':.17,'font.size':10})
 fig,axes=plt.subplots(2,1,figsize=(12,8),sharex=True,layout='constrained')
 for ax,width in zip(axes,[60,120]):
  for clock,ls in [('MCU','-'),('QPC_ESTIMATE','--')]:
   rr=[r for r in main if r['clock']==clock and r['width_s']==width];x=[(r['start_mcu_s']+width/2)/3600 for r in rr]
   ax.plot(x,[r['direction_max'] if r['quality']=='FULL' else np.nan for r in rr],lw=1.2,ls=ls,label=f'{clock} · max RMS XYZ')
   if clock=='MCU':
    for a,color in zip('xyz',['#6296b0','#d69c45','#66a07a']):ax.plot(x,[r['rms_'+a] if r['quality']=='FULL' else np.nan for r in rr],color=color,lw=.6,alpha=.65,label=a.upper())
  ax.axvspan(17600/3600,17630/3600,color='#ad4c47',alpha=.2,label='Épisode extrême conservé');ax.set_ylabel(f'RMS Wm · {width}s (mm/s²)');ax.legend(ncol=3,fontsize=8)
 axes[-1].set_xlabel('Temps relatif MCU estimé (h) · UTC inconnue');fig.suptitle('TR2_WM_A_MAX · mesure nominale locale, instrument non étalonné\nCampagne NON CLASSÉE · comparabilité BV non établie · aucun seuil',fontsize=12);fig.savefig(out/'wm_trends.png',dpi=145);plt.close(fig)
 f=np.geomspace(.1,1000,1001);H=analogue(f);fig,axes=plt.subplots(2,1,figsize=(11,7),sharex=True,layout='constrained');axes[0].semilogx(f,20*np.log10(abs(H)),label='Fonction analogique originale',color='black')
 responses=[];warp=[]
 for mid,d in defs.items():
  if d['variant']!='MAIN':continue
  hd=transfer(d['sos'],f,d['fs_est_Hz']);axes[0].semilogx(f,20*np.log10(abs(hd)),lw=.9,ls='--',label=d['clock']);axes[1].semilogx(f,np.unwrap(np.angle(hd))*180/np.pi,label=d['clock'])
  for ff,h in zip(f,hd):responses.append(dict(method_id=mid,clock=d['clock'],frequency_est_Hz=ff,analogue_magnitude=float(abs(analogue(ff))),digital_magnitude=float(abs(h)),phase_rad=float(np.angle(h))))
  grid=np.geomspace(1,80,10001);delta=transfer(d['sos'],grid,d['fs_est_Hz'])/analogue(grid);warp.append(dict(clock=d['clock'],band=[1,80],max_relative_magnitude_error=float(max(abs(abs(delta)-1))),max_phase_difference_rad=float(max(abs(np.angle(delta))))))
 for p in POINTS:axes[0].scatter(p['true_Hz'],20*np.log10(p['rounded_gain']),marker='x',color='#b44536',s=40)
 axes[0].set_ylim(-65,3);axes[0].set_ylabel('Gain (dB)');axes[0].legend();axes[1].set_ylabel('Phase causale (degrés)');axes[1].set_xlabel('Fréquence (Hz)');fig.suptitle('TR2-Wm v1 · réponse calculée, quatre points publics aux centres réels\nHh2 × Hl2 × Ht1 · aucune double limitation de bande',fontsize=11);fig.savefig(out/'wm_response.png',dpi=145);plt.close(fig)
 with (out/'frequency_response.csv').open('w',newline='') as stream:w=csv.DictWriter(stream,fieldnames=list(responses[0]),lineterminator='\n');w.writeheader();w.writerows(responses)
 pairs=[];lookup={(r['name'],r['clock'],r['width_s'],r['start_mcu_s']):r for r in rows}
 for r in main:
  if r['clock']!='MCU' or r['quality']!='FULL':continue
  row=dict(start_mcu_s=r['start_mcu_s'],width_s=r['width_s'],main_mm_s2=r['direction_max'])
  for variant,clock,k in [('LOW08','MCU','f1_times08_ratio'),('LOW12','MCU','f1_times12_ratio'),('MAIN','QPC_ESTIMATE','QPC_MCU_ratio')]:
   z=lookup.get((variant,clock,r['width_s'],r['start_mcu_s']));row[k]=z['direction_max']/r['direction_max'] if z and z['quality']=='FULL' else None
  pairs.append(row)
 with (out/'sensitivity.csv').open('w',newline='') as stream:w=csv.DictWriter(stream,fieldnames=list(pairs[0]),lineterminator='\n');w.writeheader();w.writerows(pairs)
 summary={k:dict(P50=float(np.median(v)),P95=float(np.quantile(v,.95)),minimum=float(min(v)),maximum=float(max(v))) for k in list(pairs[0])[3:] if (v:=[r[k] for r in pairs if r['width_s']==60 and r[k] is not None])}
 dump(out/'sensitivity_summary.json',summary);dump(out/'warping.json',warp)
 (out/'index.html').write_text('''<!doctype html><html lang="fr"><meta charset="utf-8"><title>TR2-Wm · analyse locale</title><style>body{font:17px system-ui;max-width:1120px;margin:32px auto;padding:0 20px;color:#19334c;background:#f5f7fb}img{width:100%;border-radius:8px;background:white}aside{padding:18px;border-left:5px solid #be8937;background:#fff0ce}a{color:#00658e}</style><h1>TR2-Wm · six heures de vibrations</h1><aside><strong>Indicateur local indicatif, instrument non étalonné.</strong> Maximum des RMS Wm d'axes, en mm/s². Campagne NON CLASSÉE ; comparabilité BV non établie. Aucun seuil ni verdict de confort.</aside><h2>Valeurs par axe et maximum directionnel</h2><img src="wm_trends.png" alt="RMS Wm60/120s, axes et hypothèses de cadence"><p>Le pic source de7,677g nominal reste dans le résultat principal. Son exclusion sur30s est un diagnostic séparé, jamais une correction silencieuse.</p><h2>Réponse vérifiable du calcul</h2><img src="wm_response.png" alt="Réponse amplitude phase et points publics"><p>La vitesse équivalente0,028×a_w dépend entièrement de la même grandeur ; elle n'est pas une deuxième mesure indépendante.</p><p><a href="NOTE_RESULTATS.md">Synthèse et limites</a> · <a href="metrics.csv">Valeurs non arrondies</a> · <a href="sensitivity.csv">Sensibilités</a> · <a href="BV_COMPARABILITY.json">Étude BV après validation</a> · <a href="README.md">Preuves et provenance</a></p></html>''')
 dump(out/'render_manifest.json',[dict(name=p.name,sha256=sha(p),bytes=p.stat().st_size) for p in sorted(out.iterdir()) if p.is_file()])
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('results');a.add_argument('review');a.add_argument('output');o=a.parse_args();render(o.results,o.review,o.output)
