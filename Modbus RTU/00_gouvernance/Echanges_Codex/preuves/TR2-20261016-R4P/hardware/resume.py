from hardware import *
q=json.loads((P/'readonly-qualified.json').read_text());assert q['result']=='PASS' and q['fram_writes']==0 and q['protected_bytes_compared']==139264
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/resume-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
assert (P/'fram-after-readonly.bin').read_bytes()==(P/'fram-after-physical.bin').read_bytes()
debug('r4p-resume-reset',elf,IDENTITY+'monitor reset\n');time.sleep(2)
debug('r4p-resume-trigger8',elf,IDENTITY+'''p r4_result
p r4m_guard
if r4_result.stage != 1 || r4_result.error || r4m_guard.phase != 0
 quit 4
end
set r4m_backup_token=0x52344D42
set r4_trigger=0x52340008
''')
(P/'r4p-resume-started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),code_sha='9f429e5',source_bridge_sha256=hashlib.sha256((P/'fram-after-readonly.bin').read_bytes()).hexdigest(),second_isolation=False,readonly_precheck=q),indent=2)+'\n')
