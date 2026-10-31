"""Fail-closed entry checks only; never authorize or emulate a successful START."""
from pathlib import Path
import tempfile,sys,json,contextlib,io
import launch_8h as m
with tempfile.TemporaryDirectory(prefix='n8a-entry-guards-') as d:
 m.P=Path(d);lock=m.P/'LONG_START_ATTEMPT.json'
 for argv in [[],['--signal','START']]:
  sys.argv=['launch_8h.py',*argv]
  try:
   with contextlib.redirect_stderr(io.StringIO()):m.main()
  except SystemExit as e:assert e.code==2
  else:raise AssertionError('missing/wrong explicit signal admitted')
  assert not lock.exists()
 (m.P/'READY.json').write_text(json.dumps(dict(prepared=False,long_never_started=True)))
 sys.argv=['launch_8h.py','--check-only']
 try:m.main()
 except AssertionError:pass
 else:raise AssertionError('unprepared check admitted')
 assert not lock.exists()
 sys.argv=['launch_8h.py','--signal','LANCE 8H']
 try:m.main()
 except AssertionError:pass
 else:raise AssertionError('unprepared start admitted')
 assert lock.exists()
 before=lock.read_bytes()
 try:m.main()
 except FileExistsError:pass
 else:raise AssertionError('second attempt admitted')
 assert lock.read_bytes()==before
 print('PASS: explicit signal required; unprepared state refused; check-only no START lock; failed attempt latched; second invocation refused before hardware access')
