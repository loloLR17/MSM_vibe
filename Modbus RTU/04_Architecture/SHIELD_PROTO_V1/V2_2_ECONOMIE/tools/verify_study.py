"""Recalculate the V22 study and verify preservation; no network or hardware access."""
from pathlib import Path
from decimal import Decimal as D, ROUND_HALF_UP
import ast
import csv
import hashlib
import json
import subprocess
import xml.etree.ElementTree as ET

P = Path(__file__).resolve().parents[1]
S = P.parent
W = S.parents[2]
assert (W / 'AGENTS.md').is_file()


def read(name):
    return json.loads((P / name).read_text())


def money(x):
    return str(x.quantize(D('.01'), rounding=ROUND_HALF_UP))


def write(name, value):
    (P / name).write_text(json.dumps(value, indent=2, ensure_ascii=False) + '\n')


cfg = read('parametres.json')
sources = read('sources.json')
rows = list(csv.DictReader((P / 'BOM_2_PROTOTYPES.csv').open()))
by_ref = {r['repere']: r for r in rows}
rate, vat = D(cfg['exchange_USD_per_EUR']), 1 + D(cfg['VAT'])
assert rate == D(str(sources['X1']['facts']['USD_per_EUR']))
for r in rows:
    f = sources[r['source_id']]['facts']
    qty = int(r['achat_etude'])
    assert qty >= int(r['besoin_2'])
    assert qty >= int(r['MOQ']) and qty % int(r['multiple']) == 0
    for col, key in [('USD_HT_unitaire', 'unit_USD_HT_at_selected_quantity'),
                     ('EUR_HT_unitaire', 'unit_EUR_HT'),
                     ('EUR_TTC_unitaire_affiche', 'unit_EUR_TTC')]:
        if r[col]:
            assert D(r[col]) == D(str(f[key])), (r['MPN'], key)
assert sources['T4']['facts']['stock_displayed_not_reserved'] == 0
assert sources['T5']['facts']['useful_insertion_depth_mm'] is None
assert by_ref['J5']['MPN'] == 'SSW-103-01-F-D'
assert sources['A4']['facts']['head_diameter_mm'] is None


def price(ref):
    r = by_ref[ref]
    if r['USD_HT_unitaire']:
        return D(r['USD_HT_unitaire']) / rate
    return D(r['EUR_HT_unitaire'])


def goods(refs):
    return sum((price(k) * D(by_ref[k]['achat_etude']) for k in refs), D(0))


tme_refs = ['J1..J4', 'J8', 'J9', 'R1..R4', 'C1', 'C2']
dk_refs = ['J5', 'J6/J7']
tg = goods(tme_refs)
dg = goods(dk_refs)
tme_ship = D(cfg['shipping_TME_EUR_TTC'])
dk_ship = [D(s) for s in cfg['shipping_DigiKey_EUR_TTC']]
electronics = [tg * vat + tme_ship + dg * vat + s for s in dk_ship]
reserved = [(tg + dg + price('J5') + price('J6/J7')) * vat + tme_ship + s for s in dk_ship]
fallback_c1 = [x + (price('C1 repli') - price('C1')) * 3 * vat for x in electronics]
unavailable_all_tme = tg + goods(['J5 option', 'J6/J7 option'])
# Two real Samtec from D1, nine-way Connfly at TME only after documented restock.
wait_tme = tg + goods(['J6/J7 option'])
wait_two_suppliers = [(wait_tme + goods(['J5'])) * vat + tme_ship + s for s in dk_ship]
ajile_goods = sum((price(k) * D(by_ref[k]['achat_etude']) for k in ['MEC-VIS', 'MEC-ECROUS']), D(0))
assert D('4.51') <= ajile_goods <= D('15')
ajile_untracked = (ajile_goods + D(cfg['shipping_Ajile_untracked_EUR_HT'])) * vat
ajile_tracked = (ajile_goods + D(cfg['shipping_Ajile_tracked_EUR_HT'])) * vat
legacy = json.loads((S / 'REVUE_PRECOMMANDE/budget-TTC.json').read_text())
pcb = legacy['public_PCB_quote']
pcb_budget = [(D(str(pcb['fabrication_USD'])) + D(str(q['USD']))) / rate * vat for q in pcb['shipping_options']]
budget = {
    'status': 'BUDGET_PARTIEL_NON_DEVIS_NON_COMMANDE',
    'rate_USD_per_EUR': str(rate), 'VAT_assumed': str(vat - 1),
    'TME_goods_USD_HT': money(tg * rate),
    'TME_goods_plus_one_shipping_EUR_TTC': money(tg * vat + tme_ship),
    'DigiKey_goods_EUR_HT': money(dg),
    'DigiKey_goods_EUR_TTC': money(dg * vat),
    'two_electronics_suppliers_EUR_TTC': list(map(money, electronics)),
    'same_with_J5_and_9way_spare_each_EUR_TTC': list(map(money, reserved)),
    'two_suppliers_with_existing_Kemet_C1_EUR_TTC': list(map(money, fallback_c1)),
    'unavailable_all_TME_counterfactual_EUR_TTC': money(unavailable_all_tme * vat + tme_ship),
    'all_TME_verdict': 'NON_ADMISSIBLE_J5_CONTACT_INCONNU_ET_STOCK_9_ZERO',
    'TME_plus_exact_Samtec_if_9_restock_EUR_TTC': list(map(money, wait_two_suppliers)),
    'wait_scenario_verdict': 'PAS_DE_DELAI_VERIFIE_9_VOIES',
    'TME_only_if_Samtec_and_9way_already_owned_EUR_TTC': money(tg * vat + tme_ship),
    'personal_exact_stock': 'INCONNU, NE PAS SUPPOSER GRATUIT_DISPONIBLE',
    'Ajile_goods_EUR_HT': money(ajile_goods), 'Ajile_goods_EUR_TTC': money(ajile_goods * vat),
    'Ajile_8_screws_16_nuts_untracked_EUR_TTC': money(ajile_untracked),
    'Ajile_8_screws_16_nuts_tracked_EUR_TTC': money(ajile_tracked),
    'Ajile_verdict': 'OFFRES_IDENTIFIEES_TETE_VIS_NON_COTEE_COMPATIBILITE_COMPLETE_NON_DEMONTREE',
    'five_PCB_legacy_quote_date': pcb['date'],
    'five_PCB_legacy_budget_EUR_TTC': {q['method']: money(b) for q, b in zip(pcb['shipping_options'], pcb_budget)},
    'known_partial_electronics_Ajile_untracked_cheapest_PCB_budget_EUR_TTC':
        [money(x + ajile_untracked + min(pcb_budget)) for x in electronics],
    'same_partial_with_Ajile_tracked_EUR_TTC':
        [money(x + ajile_tracked + min(pcb_budget)) for x in electronics],
    'all_in_delivered_total_EUR_TTC': None,
    'unknowns': ['date stock9 TME', 'tête et tolérances vis Ajile', 'TVA réelle port DigiKey',
                 'prix France TME et conversion de facturation', 'TTC/import/frais définitifs PCB',
                 'fabrication D126 et retenues modules', 'visserie modules', 'boîtiers/vitrages',
                 'inventaire réemploi et port de postes inconnus'],
}
write('budget-TTC.json', budget)

h = cfg['clip_hypothesis']
t_min = h['board_mm'] * (1 - h['board_pm_fraction'])
t_max = h['board_mm'] * (1 + h['board_pm_fraction'])
play_per_end = h['capture_gap_assumed_mm'] - t_min
collapsed = (h['barb_OD_assumed_mm'] - (h['hole_shield_mm'] - h['hole_pm_assumed_mm'])) / 2
strain = 1.5 * h['arm_thickness_assumed_mm'] * collapsed / h['free_arm_length_assumed_mm'] ** 2
calc = {
    'status': 'CONTRE_EXEMPLE_CLIP_RIGIDE_SIMPLE_PAS_UNE_PREUVE_UNIVERSELLE',
    'assumptions': h, 'PCB_thickness_range_mm': [t_min, t_max],
    'capture_axial_play_per_end_mm': play_per_end,
    'two_ends_axial_play_mm': 2 * play_per_end,
    'possible_G_mm_if_no_spring_preload': [h['G_mm'], h['G_mm'] + 2 * play_per_end],
    'shield_barb_radial_collapse_mm': collapsed,
    'nucleo_barb_radial_collapse_mm': (h['lower_barb_OD_assumed_mm'] - (h['hole_nucleo_mm'] - h['hole_pm_assumed_mm'])) / 2,
    'nucleo_min_diametral_neck_clearance_mm': h['hole_nucleo_mm'] - h['hole_pm_assumed_mm'] - h['lower_neck_OD_assumed_mm'],
    'shield_min_diametral_neck_clearance_mm': h['hole_shield_mm'] - h['hole_pm_assumed_mm'] - h['neck_OD_assumed_mm'],
    'beam_root_strain_classical_fraction': strain,
    'beam_root_strain_percent': strain * 100,
    'J5_nominal_insertion_at_max_play_mm': 8.22 + 8.51 - (h['G_mm'] + 2 * play_per_end),
    'beam_derivation': 'Euler-Bernoulli petit déplacement: delta=F L^3/(3EI); epsilon=F L t/(2EI); epsilon=1.5 t delta/L^2. L/t=5 court: ordre de grandeur seulement, pas une limite admissible FFF.',
    'fatigue_creep_vibration_and_pullout_capacity': None,
    'verdict': 'NO_GO_ADOPTION_CLIP_SIMPLE_IMPRIME_DANS_CETTE_MISSION; D126 CONSERVE',
}
assert play_per_end > h['G_pm_mm']
write('preuves/calcul-clips.json', calc)

# AVX nominal body fits the existing Fab reservation; no claim of 3D qualification.
native = json.loads((S / 'V1_9_C05/docs/checks/pcb-readback.json').read_text())
c1 = [x for x in native['pads'] if x['ref'] == 'C1']
j5 = [x for x in native['pads'] if x['ref'] == 'J5']
assert len(c1) == 2 and abs(c1[1]['x'] - c1[0]['x'] - 2.54) < 1e-6
assert all(x['drill'] == .8 for x in c1)
assert [x['number'] for x in j5 if x['net'] == 'RTU_DE'] == ['5']
assert all('unconnected' in x['net'] for x in j5 if x['number'] != '5')
write('preuves/compatibilite-candidats.json', {
    'C1': {'source': 'PCB V1.9/readback conservé + dessin AVX M3', 'pads': c1,
           'lead_diameter_nominal_mm': .508, 'drill_nominal_mm': .8,
           'Fab_reserved_relative_mm': [-1.3, 3.8, -2.5, 2.5],
           'AVX_nominal_max_body_relative_mm': [-1.27, 3.81, -1.5875, 1.5875],
           'body_difference_Fab_right_mm': .01,
           'Crtyd_reserved_y_pm_mm': 1.4, 'AVX_body_y_pm_mm': 1.5875,
           'note': 'dépasse Fab de0,01 à droite et courtyard de0,1875 enY; aucun contact voisin dans cette zone. Pas une correction PCB silencieuse, pas un DRC avec variante modifiée.',
           'tail_trim_max_assembly_mm': 3.5, 'physical': 'NON_VERIFIE'},
    'J5': {'active': 'SSW-103-01-F-D', 'candidate_rejected': 'DS1023-2*3S21',
           'native_pads': j5, 'only_DE_pad': '5 / CN12-69 / PG4',
           'Connfly_recommended_drill_mm': 1.02, 'existing_drill_mm': 1.04,
           'unknown_contact_depth': True, 'silently_replaced': False},
})

protected = read('preuves/anciens-shield-SHA256.json')
for name, expected in protected['files'].items():
    assert hashlib.sha256((W / name).read_bytes()).hexdigest() == expected, name
    blob = subprocess.check_output(['git', 'show', f"{cfg['initial_head']}:{name}"], cwd=W)
    assert hashlib.sha256(blob).hexdigest() == expected, name
allowed = ['Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_2_ECONOMIE/',
           'Modbus RTU/00_gouvernance/Echanges_Codex/archives/' + cfg['mission_id'] + '/']
allowed_files = {'Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md',
                 'Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md'}
changed = subprocess.check_output(['git', 'diff', '--name-only', cfg['initial_head']], cwd=W).decode().splitlines()
untracked = subprocess.check_output(['git', 'ls-files', '--others', '--exclude-standard'], cwd=W).decode().splitlines()
for name in set(changed + untracked):
    assert name in allowed_files or any(name.startswith(q) for q in allowed), name
for f in P.rglob('*.json'):
    json.loads(f.read_text())
for f in P.rglob('*.py'):
    ast.parse(f.read_text())
for f in P.glob('*.svg'):
    ET.parse(f)
write('preuves/validation-finale.json', {
    'status': 'PASS_ETUDE_ET_PRESERVATION_SEULEMENT',
    'initial_head': cfg['initial_head'], 'old_shield_files_compared_byte_exact': len(protected['files']),
    'source_prices_quantities_MOQ_consistent': True, 'currency_and_VAT_separated': True,
    'BOM_rows': len(rows), 'J5_retained': True, 'clips_not_adopted': True,
    'complete_price_claimed': False, 'physical_validation': False,
    'PCB_CAM_firmware_governance_rules_changed': False,
    'validation_scope': 'calculs budget/contre-exemple, lecture artefacts existants, intégrité documentaire; pas ERC/DRC/build/essai physique répété',
    'all_in_total_EUR_TTC': None,
})
print(json.dumps({'protected_files': len(protected['files']), 'electronics_TTC': list(map(money, electronics)),
                  'Ajile_untracked': money(ajile_untracked), 'Ajile_tracked': money(ajile_tracked),
                  'clip_axial_play_two_ends_mm': 2 * play_per_end, 'PASS': True}, indent=2))
