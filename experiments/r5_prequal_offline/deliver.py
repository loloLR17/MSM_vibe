"""Render saved aggregates only; no rerun of the six-hour filters."""
import argparse,csv,gzip
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from prequal import *

def make(results,output):
 results,output=Path(results),Path(output);output.mkdir(exist_ok=False,parents=True)
 with (results/'metrics.csv').open() as f:rows=list(csv.DictReader(f))
 for r in rows:
  for k in ['width_s','start_mcu_s','valid_n']:r[k]=int(r[k])
  for k in ['direction_max','rms_x','rms_y','rms_z','vector_technical']:r[k]=float(r[k])
 defs=json.loads((results/'definitions.json').read_text())
 plt.rcParams.update({'axes.spines.top':False,'axes.spines.right':False,'axes.grid':True,'grid.alpha':.18,'font.size':10})
 fig,axes=plt.subplots(2,1,figsize=(12,8),sharex=True,layout='constrained')
 for ax,name,title in zip(axes,['FLAT_1_80','TEST_LP10'],['Poids plat · passe-bande 1–80 Hz','Filtre test LP10 · sensibilité numérique, pas un modèle de confort']):
  for clock,width,color,ls in [('MCU',60,'#196b9b','-'),('MCU',120,'#ec9e24','--'),('QPC_ESTIMATE',60,'#5d9e76',':')]:
   rr=[r for r in rows if r['name']==name and r['clock']==clock and r['width_s']==width];x=[(r['start_mcu_s']+width/2)/3600 for r in rr];y=[r['direction_max'] if r['quality']=='FULL' else np.nan for r in rr];ax.plot(x,y,color=color,ls=ls,lw=1,label=f'{clock} · {width} s')
  ax.set_title(title);ax.set_ylabel('Max RMS XYZ (mg nominaux)');ax.legend(ncol=3,fontsize=8)
 axes[-1].set_xlabel('Temps relatif MCU estimé (h), UTC inconnue');fig.suptitle('MESURÉ sur archive · échelle nominale / pose et cadence non étalonnées\nAucun score ou seuil de confort',fontsize=12);fig.savefig(output/'global_60_120.png',dpi=145);plt.close(fig)
 with (results/'band_shares.csv').open() as f:bands=[r for r in csv.DictReader(f) if r['clock']=='MCU' and r['width_s']=='60']
 fig,ax=plt.subplots(figsize=(12,4.8),layout='constrained');x=np.array([int(r['start_mcu_s'])+30 for r in bands])/3600
 labels=['1–2 Hz','2–5 Hz','5–25 Hz','25–80 Hz'];names=list(BANDS)[:4];vals=np.array([[float(r[n]) for r in bands] for n in names]);ax.stackplot(x,100*vals,labels=labels,colors=['#345e8d','#4ca2a3','#e4ad52','#bd5e51'],alpha=.95);ax.set_ylim(0,100);ax.set_ylabel('% de la somme des quatre énergies');ax.set_xlabel('Temps relatif MCU estimé (h)');ax.legend(ncol=4,loc='upper left');ax.set_title('ESTIMÉ sur archive · proportions par bandes, axes XYZ sommés\nFiltres chevauchants : pas une partition exacte de la variance 1–80 Hz');fig.savefig(output/'band_energy.png',dpi=145);plt.close(fig)
 fig,ax=plt.subplots(figsize=(11,5),layout='constrained');responses=[]
 for mid,d in defs.items():
  if d['clock']!='MCU':continue
  f=np.geomspace(.1,120,401);H=transfer(d['sos'],f,d['fs_est_Hz'])
  ax.semilogx(f,20*np.log10(np.maximum(abs(H),1e-12)),label=d['name'],lw=1)
  for freq,h in zip(f,H):responses.append(dict(method_id=mid,name=d['name'],frequency_est_Hz=float(freq),amplitude=float(abs(h)),phase_rad=float(np.angle(h))))
 ax.set_ylim(-80,4);ax.set_xlabel('Fréquence selon horloge MCU estimée (Hz)');ax.set_ylabel('Gain calculé (dB)');ax.set_title('CALCUL NUMÉRIQUE · réponses des filtres tests, aucune pondération normative');ax.legend(ncol=3,fontsize=8);fig.savefig(output/'filter_responses.png',dpi=145);plt.close(fig)
 with (output/'filter_response.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(responses[0]),lineterminator='\n');w.writeheader();w.writerows(responses)
 comparisons=[]
 # Paired same-window edge, weighting, clock sensitivity with real directions.
 idx={(r['name'],r['clock'],r['width_s'],r['start_mcu_s']):r for r in rows}
 for r in rows:
  if r['name']!='FLAT_1_80' or r['clock']!='MCU' or r['quality']!='FULL':continue
  d=dict(start_mcu_s=r['start_mcu_s'],width_s=r['width_s'],flat_mg=r['direction_max'])
  for n,c,key in [('EDGE_08','MCU','edge08_over_flat'),('EDGE_12','MCU','edge12_over_flat'),('TEST_LP10','MCU','test_over_flat'),('FLAT_1_80','QPC_ESTIMATE','qpc_over_mcu')]:
   v=idx[n,c,r['width_s'],r['start_mcu_s']];d[key]=v['direction_max']/r['direction_max'] if v['quality']=='FULL' else None
  comparisons.append(d)
 with (output/'method_sensitivity.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(comparisons[0]),lineterminator='\n');w.writeheader();w.writerows(comparisons)
 stats={key:{'P50':float(np.median([r[key] for r in comparisons if r['width_s']==60 and r[key] is not None])),'P95':float(np.quantile([r[key] for r in comparisons if r['width_s']==60 and r[key] is not None],.95)),'min':min(r[key] for r in comparisons if r['width_s']==60 and r[key] is not None),'max':max(r[key] for r in comparisons if r['width_s']==60 and r[key] is not None)} for key in list(comparisons[0])[3:]}
 stats['band_sum_to_total_energy']={k:float(v) for k,v in zip(['min','P50','P95','max'],np.quantile([float(r['sum_band_energy_to_total']) for r in bands],[0,.5,.95,1]))}
 stats['mean_band_shares']=dict(zip(labels,vals.mean(axis=1).tolist()))
 dump(output/'sensitivity_summary.json',stats)
 html='''<!doctype html><html lang="fr"><meta charset="utf-8"><title>TR2 — Préqualification hors ligne</title><style>body{font:17px system-ui;max-width:1120px;margin:32px auto;padding:0 20px;background:#f5f7fa;color:#183349}img{width:100%;background:white;border-radius:8px}aside{padding:18px;background:#fff1ce}a{color:#00688f}</style><h1>Une métrique globale est-elle calculable ?</h1><aside><strong>Faisabilité LIMITÉE.</strong> Calcul global reproductible sur six heures, y compris signaux modulés. Instrument, fixation et horloge non étalonnés ; aucune valeur de confort certifiée. Le filtre LP10 est seulement un test de calcul.</aside><h2>Énergie globale et durée d'observation</h2><img src="global_60_120.png" alt="RMS plat et filtre test sous deux durées et horloges"><h2>Où se trouve l'énergie ?</h2><img src="band_energy.png" alt="Contributions par bandes non orthogonales"><h2>Ce que fait exactement chaque filtre</h2><img src="filter_responses.png" alt="Réponses calculées des filtres"><p>±2/±4 g ne sont pas recommandés sur l'ensemble de l'archive : un épisode contient des sorties nominales jusqu'à 7,677 g. Ce n'est pas une preuve de choc mécanique ; les bruts natifs manquent.</p><p><a href="NOTE_DECISION.md">Note de décision et limites</a> · <a href="metrics.csv">RMS non arrondis</a> · <a href="aligned_pairs.csv">Périodes alignées</a> · <a href="method_sensitivity.csv">Sensibilités</a> · <a href="filter_response.csv">Réponses numériques</a> · <a href="README.md">Provenance et preuves</a></p></html>'''
 (output/'index.html').write_text(html)
 dump(output/'render_manifest.json',[dict(name=p.name,sha256=sha(p),bytes=p.stat().st_size) for p in sorted(output.iterdir()) if p.is_file()])
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('results');a.add_argument('output');o=a.parse_args();make(o.results,o.output)
