"""Negative controls in memory only. No PCB mutation or export performed."""
import runpy,copy,json
from pathlib import Path
P=Path(__file__).resolve().parents[1]
x=runpy.run_path(str(P/'tools/verify_m01.py'));T=x['T'];one=x['one'];snap=x['snapshots'];reference=snap(x['A'])[0];checks=[]

def must_fail(name,mutate):
 b=copy.deepcopy(x['B']);mutate(b);assert snap(b)[0]!=reference,name;checks.append({'mutation':name,'rejected':True})
def footprint(b,ref):return next(n for n in b if T(n,'footprint')and any(T(p,'property')and p[1:3]==['Reference',ref]for p in n))
must_fail('J1 shift1um',lambda b:one(footprint(b,'J1'),'at').__setitem__(1,float(one(footprint(b,'J1'),'at')[1])+.001))
must_fail('existing via shift1um',lambda b:one(next(n for n in b if T(n,'via')),'at').__setitem__(1,1.))
must_fail('ARNACO silkscreen text change',lambda b:next(n for n in b if T(n,'gr_text')and n[1]=='ARNACO').__setitem__(1,'CHANGED'))
must_fail('unrelated J9 DE reassignment',lambda b:one(next(p for p in footprint(b,'J9')if T(p,'pad')and p[1]=='3'),'net').__setitem__(1,'GND'))
must_fail('PCB thickness change',lambda b:one(one(b,'general'),'thickness').__setitem__(1,1.61))
must_fail('PTH diameter change',lambda b:one(next(p for p in footprint(b,'J8')if T(p,'pad')and p[1]=='19'),'drill').__setitem__(1,1.03))
must_fail('original non-SPI3 track loss',lambda b:b.remove(next(n for n in b if T(n,'segment')and one(n,'net')[1]=='RTU_DE')))
def add_unknown_net(b):
 n=copy.deepcopy(next(n for n in b if T(n,'segment')));one(n,'net')[1]='ACC_UNAUTHORIZED';b.append(n)
must_fail('extra unauthorized ACC-prefixed segment',add_unknown_net)
(P/'preuves/M01-negative-controls.json').write_text(json.dumps({'status':'PASS','controls':checks,'mutation_scope':'In-memory AST only, actual V2.4 file unchanged'},indent=2)+'\n');print('Rejected',len(checks),'forbidden changes in memory')
