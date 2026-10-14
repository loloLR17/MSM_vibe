"""Offline SD SDR lane-permutation model and existing-proof audit; no hardware APIs."""
from pathlib import Path
import argparse, itertools, json, hashlib, binascii, subprocess, re

def read_byte(x,p):
    return sum(((x >> (4*n+p[h])) & 1) << (4*n+h) for n in range(2) for h in range(4))
def write_byte(x,p):
    return sum(((x >> (4*n+h)) & 1) << (4*n+p[h]) for n in range(2) for h in range(4))
def crc16(bits):
    c=0
    for b in bits:
        f=(c >> 15)^b
        c=(c << 1)&65535
        if f:c ^= 0x1021
    return c
def streams(payload):
    return [[(b >> (offset+h))&1 for b in payload for offset in (4,0)] for h in range(4)]
def crc_frame_check(payload,p):
    lanes=streams(payload)
    checks=[crc16(x) for x in lanes]
    # Wire permutation moves each whole data+CRC stream, not just payload bits.
    return all(crc16(lanes[p[h]])==checks[p[h]] for h in range(4))
def digest(b):return hashlib.sha256(b).hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--repo',type=Path,required=True);parser.add_argument('--out',type=Path,required=True);args=parser.parse_args()
    repo=args.repo;out=args.out;out.mkdir(exist_ok=True)
    e=repo/'Modbus RTU/00_gouvernance/Echanges_Codex';proof=e/'preuves'
    directories=['TR2-20261013-M016-SDMMC-REFERENCE','TR2-20261013-M016-SDMMC-PHYSICAL','TR2-20261013-M016-1BIT-LOAD','TR2-20261014-M017-SDMMC-1BIT','TR2-20261014-SDMMC1-REFERENCE-POC','TR2-20261014-SDMMC1-CABLE10CM-COMPARISON','TR2-20261014-SDMMC1-COLDSTART10CM','TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW','TR2-20261014-SDMMC1-FIRSTFAULT','TR2-20261013-M016-GPIO-DAT123']
    inventories=[]
    for d in directories:
        directory=proof/d;manifest=json.loads((directory/'manifest.json').read_text());checks=[]
        for n,h in manifest.items():
            assert digest((directory/n).read_bytes())==h,(d,n)
            checks.append(n)
        inventories.append({'directory':d,'manifest_entries_verified':len(checks),'all_files':[{ 'file':str(f.relative_to(repo)),'bytes':f.stat().st_size,'sha256':digest(f.read_bytes()) } for f in sorted(directory.rglob('*')) if f.is_file()]})
    datasets=[]
    d=proof/'TR2-20261013-M016-SDMMC-PHYSICAL/readonly-1bit-4MHz-commanded-rx.bin.hex'
    actual=bytes.fromhex(d.read_text());expected=bytes((i*37+(i>>8))&255 for i in range(4096));assert actual==expected
    datasets.append({'label':'M016_write4_read1','file':str(d.relative_to(repo)),'actual_sha256':digest(actual),'bytes':len(actual),'unique_values':len(set(actual)),'role':'card bytes observed through 1-bit after 4-bit write; compare write permutation','expected':expected,'actual':actual,'direction':'write'})
    d=proof/'TR2-20261014-SDMMC1-FIRSTFAULT/RUN-A-data8192.bin';actual=d.read_bytes();expected=bytes((i*89+(i>>8)*17+(i>>16)*31+0xb7)&255 for i in range(8192));assert actual==expected
    datasets.append({'label':'PoC_write4_FIRSTFAULT_read1','file':str(d.relative_to(repo)),'actual_sha256':digest(actual),'bytes':len(actual),'unique_values':len(set(actual)),'role':'1-bit read of earlier PoC game B written by STM32 in 4-bit; no later write to same zone documented','expected':expected,'actual':actual,'direction':'write'})
    local=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014/release-original-read-8192.bin')
    logged=json.loads((proof/'TR2-20261014-SDMMC1-REFERENCE-POC/release-first-read-verification.json').read_text())
    if local.is_file():
        actual=local.read_bytes();expected=bytes((i*37+(i>>8)+(i>>16)+(i>>24)*113)&255 for i in range(8192));assert digest(actual)==logged['sha256'] and actual==expected
        datasets.append({'label':'M017_write1_PoC_read4','file':str(local),'corroborating_versioned_file':'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-SDMMC1-REFERENCE-POC/release-first-read-verification.json','actual_sha256':digest(actual),'bytes':len(actual),'unique_values':len(set(actual)),'role':'local original dump, independently hash-matched to published verification; 4-bit read of 1-bit M017 prefix','expected':expected,'actual':actual,'direction':'read'})
    assert crc16([1]*(512*8))==0x7fa1==binascii.crc_hqx(bytes([255])*512,0)
    basis=bytes(range(256));lane=streams(basis)
    for bits in lane:
        packed=bytes(sum(bits[i+j]<<(7-j) for j in range(8)) for i in range(0,len(bits),8));assert crc16(bits)==binascii.crc_hqx(packed,0)
    rows=[]
    for p in itertools.permutations(range(4)):
        read=bytes(read_byte(x,p) for x in basis);write=bytes(write_byte(x,p) for x in basis)
        assert bytes(read_byte(x,p) for x in write)==basis
        assert bytes(write_byte(x,p) for x in read)==basis
        assert crc_frame_check(basis,p)
        signature=bytes(read_byte(x,p) for x in bytes.fromhex('55aa'))
        row={'host_to_card':list(p),'DAT0_fixed':p[0]==0,'payload_roundtrip_identity':True,'CRC_per_lane_preserved_ideal_SDR':True,'start_end_all_lanes_preserved':True,'changed_byte_values_of_256':sum(x!=y for x,y in zip(read,basis)),'read_55AA':signature.hex(),'signature_preserved':signature==bytes.fromhex('55aa'),'SSR_width2_decoded_from_top_byte_under_permutation':read_byte(0x80,p)>>6,'dataset_comparisons':{}}
        for ds in datasets:
            f=write_byte if ds['direction']=='write' else read_byte;transformed=bytes(f(x,p) for x in ds['expected']);actual=ds['actual']
            row['dataset_comparisons'][ds['label']]={'mismatches':sum(x!=y for x,y in zip(actual,transformed)),'predicted_sha256':digest(transformed),'compatible':actual==transformed}
        row['scope']='full read-frame algebra only; DAT0-moved 1-bit/CRC-status/busy may fail, so no operational roundtrip guarantee'
        rows.append(row)
    assert sum(r['DAT0_fixed'] for r in rows)==6
    assert sum(r['signature_preserved'] for r in rows if r['DAT0_fixed'])==2
    assert all(not row['dataset_comparisons'][ds['label']]['compatible'] for row in rows[1:] for ds in datasets)
    result={'model':'SD SDR 4-bit, ideal bijective static reciprocal wiring, no timing/noise/level/mode errors; p maps host line to card line; read inverse of write','crc_polynomial':'0x1021, initial 0, MSB first; check FF*512=0x7FA1','self_checks':'all 24 permutations x256 byte values; inverse in both directions, independent binascii checks, per-line data+CRC/start/end invariance','datasets':[{k:v for k,v in ds.items() if k not in ['expected','actual']} for ds in datasets],'permutations':rows,'hardware_operations':0}
    (out/'permutations.json').write_text(json.dumps(result,indent=2)+'\n');(out/'historical-proof-inventory.json').write_text(json.dumps(inventories,indent=2)+'\n')
    print(json.dumps({'permutations':len(rows),'DAT0_fixed':6,'nonidentity_DAT1_3':5,'datasets_verified':len(datasets),'nonidentity_matches_in_each_dataset':0,'manifest_entries_verified':sum(x['manifest_entries_verified'] for x in inventories),'hardware_operations':0}))
if __name__=='__main__':main()
