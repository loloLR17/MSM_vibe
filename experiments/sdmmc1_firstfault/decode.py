"""Decode the proven ARM uint32 ABI and independently check returned data."""
from pathlib import Path
import argparse,struct,json,hashlib
SNAP='valid phase source_line kind error tick cycles ipsr sta respcmd resp1 arg cmd dctrl dlen dcount clkcr mask hal_error hal_state hal_context'.split()
SCALAR='magic profile stage trigger run_count terminal_reason'.split()
PHASE='phase result error_before error_after tick_start tick_end cycles_start cycles_end'.split()
def decode(data):
    if len(data)!=2260:raise ValueError('Wrong FF_Result ABI size')
    w=list(struct.unpack('<565I',data));pos=0
    def take(n):
        nonlocal pos
        r=w[pos:pos+n];pos+=n;return r
    def snap():return dict(zip(SNAP,take(21)))
    r=dict(zip(SCALAR,take(6)))
    r['uid']=take(3);r['cid']=take(4)
    r.update(zip('sectors sector_bytes ssr_width initial_state mismatch512 mismatch8192 first_mismatch'.split(),take(7)))
    r['gpio_c']=take(5);r['gpio_d']=take(5);r['rcc']=take(3)
    r.update(zip('core_clock primask basepri'.split(),take(3)))
    r['nvic_iser']=take(4);r.update(zip('nvic_ipr_sd nvic_ipr_tim nvic_ipr_usb'.split(),take(3)))
    r['first_error']=snap();r['phase_count']=take(1)[0]
    if r['phase_count']>10:raise ValueError('Corrupt phase count')
    phases=[]
    for i in range(10):
        p=dict(zip(PHASE,take(8)));p['before']=snap();p['after']=snap()
        if i<r['phase_count']:phases.append(p)
    assert pos==565
    r['phases']=phases
    if r['magic']!=0x46303133 or r['profile'] not in (1,4):raise ValueError('Wrong probe/profile')
    return r
def main():
    parser=argparse.ArgumentParser();parser.add_argument('proof_directory');a=parser.parse_args();p=Path(a.proof_directory)
    r=decode((p/'RUN-A-result.bin').read_bytes());checks=[]
    expected_hash={512:'a9f4d9b4604080bea7999abf2d2874653e71c43a7805bc1735068b1f004f3daf',
        8192:'2ac5183f9757f9b08e94e4cf2f4b1ee03a3a66171e8b366989e5b36b93ab563b'}
    for phase,size in [(6,512),(8,8192)]:
        rows=[x for x in r['phases'] if x['phase']==phase]
        b=(p/('RUN-A-data'+str(size)+'.bin')).read_bytes()
        q=bytes((i*89+(i>>8)*17+(i>>16)*31+0xB7)&255 for i in range(size))
        assert hashlib.sha256(q).hexdigest()==expected_hash[size]
        valid=bool(rows and rows[0]['result']==0 and rows[0]['error_after']==0)
        checks.append({'bytes':size,'HAL_data_success':valid,'sha256_dump':hashlib.sha256(b).hexdigest(),
            'sha256_expected':expected_hash[size],'mismatches_if_HAL_success':sum(x!=y for x,y in zip(b,q)) if valid else None,
            'full_comparison_if_HAL_success':b==q if valid else None})
    r['external_data_checks']=checks
    r['passed_bounded']=(r['stage']==100 and r['run_count']==1 and not r['first_error']['valid']
        and all(c['HAL_data_success'] and c['full_comparison_if_HAL_success'] for c in checks))
    (p/'RUN-A-decoded.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
if __name__=='__main__':main()
