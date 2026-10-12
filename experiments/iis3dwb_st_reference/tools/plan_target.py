#!/usr/bin/env python3
"""Print an identity-constrained future connection plan. Does not execute hardware commands."""
import argparse
import json

PRIMARY_SN = '003500463235510B37333439'
PRIMARY_UID = '004A003D3842501820333157'

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--second-sn', required=True)
    parser.add_argument('--second-uid', required=True)
    parser.add_argument('--wiring-confirmed', action='store_true')
    args = parser.parse_args()
    sn, uid = args.second_sn.upper(), args.second_uid.upper()
    if not args.wiring_confirmed or len(sn) != 24 or len(uid) != 24 or \
            any(c not in '0123456789ABCDEF' for c in sn + uid) or \
            sn == PRIMARY_SN or uid == PRIMARY_UID:
        parser.error('Confirmed SECOND board wiring, distinct 24-hex SN and UID required')
    print(json.dumps({'status': 'PLAN_ONLY_NO_EXECUTION', 'expected_second_sn': sn,
                      'expected_second_uid': uid, 'connection': ['-c', 'port=SWD', 'sn=' + sn,
                      'freq=8000'], 'flash_address': '0x08000000',
                      'required_before_flash': ['independent identity readback', 'fresh 2MiB backup',
                      'artifact hash', 'chain qualification for second board', 'backup readback'],
                      'forbidden': ['primary TR2', 'HOTPLUG', 'mass erase', 'option bytes']}, indent=2))

if __name__ == '__main__':
    main()
