"""Independent Python integer oracles, prepared before compiling the C service.
Uses math.isqrt and exact rational nearest mg, no C calculator/driver invocation.
"""
import json, math, pathlib
patterns=[[(0,0,0)],[(100,-200,300)],[(1000,0,0),(-1000,1000,0),(300,-400,0),(0,0,500)],[(32766,-32767,0),(-1,1,1)]]
rows=[]
for fs in range(4):
 for axes in range(1,8):
  for n in [4096,8192,16384,32768]:
   for pattern in patterns:
    def mg(x):
     # sensitivities 0.061 * 2**fs mg/LSB, nearest with symmetric ties
     return (1 if x>=0 else -1)*((abs(x)*61*(2**fs)+500)//1000)
    v=[[mg(p[a]) if axes&(1<<a) else 0 for a in range(3)] for p in pattern]
    count=n//len(v); sq=[sum(p[a]**2 for p in v)*count for a in range(3)]
    rms=[math.isqrt(q//n) for q in sq];peak=[max(abs(p[a]) for p in v) for a in range(3)]
    rows.append(dict(fs=fs,axes=axes,n=n,raw=pattern,rms_global=math.isqrt(sum(sq)//n),peak_global=math.isqrt(max(sum(x*x for x in p) for p in v)),rms=rms,peak=peak))
p=pathlib.Path(__file__).parent
(p/'live_metrology_oracles.json').write_text('[\n'+',\n'.join(json.dumps(row,separators=(',', ':')) for row in rows)+'\n]\n')
lines=['/* Generated independently by Python math.isqrt; do not hand-adjust expected results. */','typedef struct { unsigned fs,axes,n,length; int16_t raw[4][3]; uint32_t rms_global,peak_global,rms[3],peak[3]; } LiveOracle;','static const LiveOracle live_oracles[]={']
for q in rows:
 raw='{'+','.join('{'+','.join(map(str,x))+'}' for x in q['raw'])+'}'
 arr=lambda a:'{'+','.join(map(str,a))+'}'
 lines.append('{'+f"{q['fs']},{q['axes']},{q['n']},{len(q['raw'])},"+raw+f",{q['rms_global']},{q['peak_global']},"+arr(q['rms'])+','+arr(q['peak'])+'},')
lines.append('};\n');(p/'live_metrology_oracles.h').write_text('\n'.join(lines))
print(len(rows),'independent vectors')
