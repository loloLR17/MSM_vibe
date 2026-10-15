from pathlib import Path
import csv,hashlib,json,re,collections,subprocess
root=Path.cwd(); out=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-A06'; out.mkdir(parents=True,exist_ok=True)
rows=[]; matches=[]; headings=[]; byhash=collections.defaultdict(list)
rx=re.compile(r'storag|stockage|micro.?sd|campagn|ActiveConfiguration|supervision|recovery|persist|sqlite',re.I)
for folder in ['Modbus RTU/01_Specification_source','Modbus RTU/02_Validation']:
 for f in sorted((root/folder).rglob('*')):
  if not f.is_file(): continue
  b=f.read_bytes(); t=b.decode('utf-8-sig'); lines=t.splitlines(); rel=f.relative_to(root).as_posix(); digest=hashlib.sha256(b).hexdigest(); byhash[digest].append(rel)
  historical=bool(re.search(r'archive|legacy',rel,re.I)); retired='/FT-ACC-01/' in rel
  category='historique' if historical else 'retire' if retired else 'specification' if '01_Specification' in rel else 'mapping derive' if '/mapping_unifie/' in rel else 'qualification' if '/Qualifications/' in rel else 'source' if '/source/' in rel else 'detaille' if '/detaille/' in rel else 'instancie' if '/instancie/' in rel else 'cadrage'
  hits=[(i,l) for i,l in enumerate(lines,1) if rx.search(l)]
  rows.append(dict(path=rel,bytes=len(b),lines=len(lines),sha256=digest,category=category,hits=len(hits),first_identical_path=byhash[digest][0],review='lecture integrale automatisee; qualification semantique par famille dans matrice; pas execution'))
  for i,l in hits: matches.append(dict(path=rel,line=i,text=l))
  if not historical:
   for i,l in enumerate(lines,1):
    if l.startswith('#'): headings.append(dict(path=rel,line=i,heading=l))
for name,items in [('corpus-inventory.csv',rows),('dependency-occurrences.csv',matches),('active-headings.csv',headings)]:
 with (out/name).open('w',newline='') as h:
  w=csv.DictWriter(h,fieldnames=list(items[0]));w.writeheader();w.writerows(items)
summary=dict(head=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip(),files=len(rows),bytes=sum(r['bytes'] for r in rows),categories=dict(collections.Counter(r['category'] for r in rows)),exact_duplicate_groups=sum(len(v)>1 for v in byhash.values()),exact_redundant_files=sum(len(v)-1 for v in byhash.values()),keyword_occurrences=len(matches),limitations='Parcours de tous les octets UTF-8, indexation de contenu et classement structurel; pas affirmation de relecture humaine individuelle des milliers de fiches ni de qualification des tests. Les doublons exacts ne resolvent pas les doublons semantiques.')
(out/'corpus-summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n')
(out/'inventory-script.py').write_bytes(Path('/tmp/a06_scan.py').read_bytes())
print(json.dumps(summary,ensure_ascii=False,indent=2))
