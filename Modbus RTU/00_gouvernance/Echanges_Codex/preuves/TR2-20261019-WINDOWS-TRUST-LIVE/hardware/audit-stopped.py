from hardware import *
import sys
n=sys.argv[1];e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
debug(n,e,IDENTITY+'''p r4u_result
p r4_result
p work
p owner.acquisition.fifo.running
p owner.acquisition.fifo.total_xyz
p owner.acquisition.fifo.total_ts
p owner.acquisition.fifo.parser
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p engine.state
p engine.last
p store.generation
p store.epoch
p live.generation
p metric
p event_time
p server
p tr2_irq_observer
p r4r_first_value
'''+f'dump binary memory {win(P/(n+"-xyz.bin"))} r4r_xyz r4r_xyz+4096\n'+'''set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 4
end
'''+f'dump binary memory {win(P/(n+"-fram.bin"))} baseline baseline+262144\n')
a=(P/'fram-before.bin').read_bytes();b=(P/(n+'-fram.bin')).read_bytes();assert len(b)==262144
r=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),sha256=hashlib.sha256(b).hexdigest(),historical_prefix_122880_unchanged=a[:122880]==b[:122880],tail_16384_unchanged=a[245760:]==b[245760:],changed_bytes=sum(x!=y for x,y in zip(a,b)),scope='Only LIVE ownership range [122880,245760) may change')
(P/(n+'-summary.json')).write_text(json.dumps(r,indent=2));assert r['historical_prefix_122880_unchanged'] and r['tail_16384_unchanged'];print(r,flush=True)
