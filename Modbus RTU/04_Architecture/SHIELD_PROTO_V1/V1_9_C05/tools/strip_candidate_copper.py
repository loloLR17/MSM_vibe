from pathlib import Path
import re,sys
p=Path(sys.argv[1]);s=p.read_text();spans=[]
for m in re.finditer(r'(?m)^\t\((?:segment|via|zone)\s',s):
 start=m.start();i=s.index('(',start);depth=0;quoted=False;escaped=False
 for j in range(i,len(s)):
  c=s[j]
  if quoted:
   if escaped:escaped=False
   elif c=='\\':escaped=True
   elif c=='"':quoted=False
  elif c=='"':quoted=True
  elif c=='(':depth+=1
  elif c==')':
   depth-=1
   if depth==0:spans.append((start,j+1));break
for a,b in reversed(spans):s=s[:a]+s[b:]
p.write_text(s);print('Removed candidate copper nodes for route rebuild',len(spans))
