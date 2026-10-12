"""Reviewed manifest/registry to C authority and expected B0, never accesses hardware."""
import argparse
import hashlib
import json
from pathlib import Path

def generate(manifest_path, registry_path, output):
    manifest_bytes = Path(manifest_path).read_bytes()
    registry_bytes = Path(registry_path).read_bytes()
    m = json.loads(manifest_bytes)
    r = json.loads(registry_bytes)
    if m['status'] != 'SUPERVISION_ONLY_DEFERRED_STORAGE_PLAN':
        raise ValueError('Reviewed supervision-only plan required')
    entries = r['entries']
    ids, uids = set(), set()
    for e in entries:
        uid = tuple(int(x, 16) for x in e['hardware_uid_words_hex'])
        if len(uid) != 3 or any(not 0 <= x <= 0xffffffff for x in uid):
            raise ValueError('Full UID96 required')
        if e['device_id'] in ids or uid in uids:
            raise ValueError('Duplicate registry assignment')
        ids.add(e['device_id']); uids.add(uid)
    if len(entries) != 1 or entries[0]['status'] != 'ASSIGNED':
        raise ValueError('This board profile requires one assigned device')
    e = entries[0]
    uid = tuple(int(x, 16) for x in e['hardware_uid_words_hex'])
    if not 0 < e['device_id'] <= 0xffffffff or not 1 <= e['unit_id'] <= 247:
        raise ValueError('Invalid attribution')
    def ascii_bytes(value, length):
        raw = value.encode('ascii')
        if not raw or len(raw) > length or any(x < 32 or x > 126 for x in raw):
            raise ValueError('Invalid ASCII identity')
        return raw.ljust(length, b'\0')
    serial = ascii_bytes(e['serial'], 16); manufacturer = ascii_bytes(e['manufacturer'], 8)
    v = m['b0']; fields = [v['hardware_version'], *v['firmware_version'], v['protocol_version'], v['capabilities']]
    if len(fields) != 6 or any(not 0 <= x <= 65535 for x in fields) or fields[-1] != 12 or fields[-2] != 1:
        raise ValueError('Profile supports V1 supervision/diagnostic only')
    s = m['storage_policy']
    if s['availability'] or s['configuration_environment_known'] or s['usable_capacity_mb'] or s['allocated_ranges'] or s['sd_access'] != 'NONE':
        raise ValueError('No SD authority in this profile')
    q = m['provisioning']
    if [q['expected_generation'], q['configuration_generation_after'], q['identity_generation_after']] != [10,11,12]:
        raise ValueError('Reviewed generation plan required')
    c = m['configuration']
    if c['status'] != 'STRUCTURAL_VALIDATION_ONLY_STORAGE_ACTIVATION_DEFERRED':
        raise ValueError('Configuration resource validation must remain deferred')
    words = [e['device_id'] >> 16, e['device_id'] & 65535, *fields]
    raw = serial + manufacturer
    words.extend(int.from_bytes(raw[i:i+2], 'big') for i in range(0,24,2)); words.append(0)
    out = Path(output); out.mkdir(parents=True, exist_ok=True)
    (out/'expected-b0.json').write_text(json.dumps(words)+'\n')
    identity = ['.generation=1', '.device_id='+str(e['device_id']), '.hardware_version='+str(fields[0]), '.firmware_version_major='+str(fields[1]), '.firmware_version_minor='+str(fields[2]), '.firmware_version_patch='+str(fields[3]), '.protocol_version='+str(fields[4]), '.device_capabilities='+str(fields[5]), '.serial_number='+json.dumps(e['serial']), '.manufacturer='+json.dumps(e['manufacturer'])]
    names = ['sampling_frequency_hz','axes_enable_mask','full_scale_code','acquisition_mode','window_size_samples','indicator_period_ms','campaign_duration_s','storage_mode','storage_limit_mb','campaign_context_id','mission_id']
    payload = ['.'+x+'='+str(c[x]) for x in names] + ['.campaign_label='+json.dumps(c['campaign_label']), '.mission_label='+json.dumps(c['mission_label'])]
    header = '#ifndef TR2_GENERATED_COMMISSIONING_H\n#define TR2_GENERATED_COMMISSIONING_H\n'
    header += '#include "tr2/application/production_commissioning.h"\n'
    header += '#define TR2_MANIFEST_SHA256 '+json.dumps(hashlib.sha256(manifest_bytes).hexdigest())+'\n'
    header += '#define TR2_REGISTRY_SHA256 '+json.dumps(hashlib.sha256(registry_bytes).hexdigest())+'\n'
    header += '#define TR2_EXPECTED_FRAM_CRC UINT32_C('+str(q['fram_backup_crc32'])+')\n'
    header += 'static const PersistentIdentityAssignment tr2_board_assignment={ {'+','.join('UINT32_C('+str(x)+')' for x in uid)+'}, '+str(e['device_id'])+' };\n'
    header += 'static const PersistentIdentityRecord tr2_board_identity={.generation=1,.identity={.hardware_uid={'+','.join('UINT32_C('+str(x)+')' for x in uid)+'},.functional={'+','.join(identity)+'},.unit_id='+str(e['unit_id'])+'}};\n'
    header += 'static const ActiveConfigurationSnapshot tr2_board_configuration={.generation=1,.config_id=1,.revision_counter=1,.payload={'+','.join(payload)+'}};\n'
    header += 'static const uint16_t tr2_board_expected_b0[21]={'+','.join(str(x) for x in words)+'};\n#endif\n'
    (out/'tr2_generated_commissioning.h').write_text(header)

if __name__ == '__main__':
    parser=argparse.ArgumentParser();parser.add_argument('manifest');parser.add_argument('registry');parser.add_argument('output');a=parser.parse_args()
    generate(a.manifest,a.registry,a.output)
