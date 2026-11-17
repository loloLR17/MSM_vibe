"""Independent reference: vendor sources + explicit TR2 bus assignment policy.

No project matrix, design.json, KiCad symbol, PCB or CAM is read by this stage.
Run with Python and sexpdata available (cache path may be supplied in PYTHONPATH).
"""
from pathlib import Path
import argparse
import hashlib
import json
import xml.etree.ElementTree as ET
import sexpdata

HERE = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument('--reports', type=Path, default=Path('/home/lolo/dev/msm/reports'))
args = parser.parse_args()
reports = args.reports
sources = {}

def source(key, path, url, revision, method):
    sources[key] = dict(local_file=str(path), url=url, revision=revision, method=method,
                        sha256=hashlib.sha256(path.read_bytes()).hexdigest(), bytes=path.stat().st_size)
    return path

netfile = source('ST_C05_NET', reports/'TR2-shield-v19-20261105/ST-manufacturing/MB1549C/Netlist/MB1549-C05_PCAD.NET',
                 'https://www.st.com/resource/en/board_manufacturing_specification/mb1549-manufacturing.zip',
                 'MB1549 C05; native PCAD netlist, 18-Nov-2024', 'native source extraction; cp1252')
tree = sexpdata.loads('(' + netfile.read_text(encoding='cp1252') + ')')
stpins, netnodes = {}, {}
def visit(node):
    if not isinstance(node, list):
        return
    if node and str(node[0]) == 'net':
        net = str(node[1])
        nodes = [(str(x[1]), str(x[2])) for x in node[2:]
                 if isinstance(x, list) and x and str(x[0]) == 'node']
        netnodes[net] = nodes
        for ref, pin in nodes:
            if ref in ['CN7', 'CN8', 'CN9', 'CN10', 'CN12']:
                stpins[ref, pin] = net
    else:
        for child in node:
            visit(child)
visit(tree)
source('ST_C05_PROJECT', reports/'TR2-shield-v19-20261105/ST-design/MB1549C/MB1549.PrjPcb',
       'https://www.st.com/resource/en/board_manufacturing_specification/mb1549-bdp.zip',
       'BDP4.0; Board PCB C, assembly05; U575ZIQ variant', 'variant evidence, not inferred from generic footprint')
source('ST_UM2861', reports/'TR2-shield-v14-20261104/UM2861-Rev8-STJapan.pdf',
       'https://www.stmcu.jp/wp/wp-content/uploads/2021/10/UM2861_Rev8.pdf',
       'Rev8, tables10/11 and15..19, pp28/33/34/37',
       'manufacturer nominal functions for unused multiplexed pins; C05 netlist remains exact routing source')
nominal_mux={'NetCN7_9':'PA4','NetCN7_11':'PB4','NetCN7_13':'PB5','NetCN7_17':'PA4','NetCN7_19':'PB4',
             'NetCN9_3':'PA2','NetCN9_7':'PB0','NetCN10_9':'PC2','NetCN10_13':'PA2',
             'NetCN10_15':'PB10','NetCN10_19':'PE15','NetCN10_21':'PB0','NetCN10_23':'PE12',
             'NetCN10_25':'PE14','NetCN10_26':'PE12','NetCN10_28':'PE14','NetCN10_29':'PA0',
             'NetCN10_30':'PE15','NetCN10_32':'PB10','NetCN12_66':'PG8','NetCN12_67':'PG7'}

# Functional pin choices from the existing TR2 drivers; independent from shield mapping.
policy = {'PB13': 'SD_SCK', 'PD3': 'SD_MISO', 'PD4': 'SD_MOSI', 'PF12': 'SD_CS',
          'PA5': 'FRAM_SCK', 'PA6': 'FRAM_MISO', 'PA7': 'FRAM_MOSI', 'PD14': 'FRAM_CS',
          'PC9': 'ACC_CS', 'PC10': 'ACC_SCK', 'PC11': 'ACC_MISO', 'PC12': 'ACC_MOSI',
          'PG7': 'RTU_TX', 'PG8': 'RTU_RX', 'PG4': 'RTU_DE'}
rows = []
def add(ref, pin, vendor, function, desired, direction, level, src, note=''):
    rows.append(dict(ref=ref, pin=str(pin), vendor_pin=vendor, vendor_function=function,
                     desired_net=desired, direction=direction, level=level, source=src, note=note))

for ref, cn, count in [('J1','CN7',20),('J2','CN8',16),('J3','CN9',30),('J4','CN10',34)]:
    for pin in range(1, count+1):
        native = stpins[cn,str(pin)]
        function = {'ARD_D1_TX':'PG7 via SB25 (SB31 open)', 'ARD_D0_RX':'PG8 via SB27 (SB33 open)',
                    'NetCN8_1':'RES/NC per UM2861; native header net includes R30'}.get(native,native)
        if native in nominal_mux:
            function=nominal_mux[native]+' (nominal UM2861; via C05 solder-bridge net '+native+')'
        mcu = {'ARD_D1_TX':'PG7','ARD_D0_RX':'PG8'}.get(native,native)
        desired = policy.get(mcu, 'NC')
        if (cn,pin)==('CN8',7): desired = '+3V3'
        if cn=='CN8' and pin in [11,13]: desired = 'GND'
        if desired in ['+3V3','GND']: direction,level = 'power','3.3V / logic ground'
        elif desired=='NC':
            direction,level='GPIO configurable; unused on shield','3.3V nominal; actual mode not assumed'
            if native in ['GND','AGND']:direction,level='power ground unused','logic/analog ground, deliberately not routed'
            if native=='5V':direction,level='power output unused','5V, deliberately not routed'
            if native=='VIN':direction,level='power input unused','external VIN domain; deliberately not routed'
            if native in ['VDDA','VREFP_ARD','IOREF']:direction,level='analog/reference unused','reference rail; no external connection; actual level not inferred'
            if native=='NRST':direction,level='reset active low unused','MCU reset domain; no external connection'
            if native=='NetCN8_1':direction,level='reserved unused','not used as signal or supply'
        else: direction,level = ('MCU input' if desired.endswith(('MISO','RX')) else 'MCU output'),'3.3V logic'
        add(ref,pin,f'{cn}-{pin}',function,desired,direction,level,'ST_C05_NET',
            'Native net '+native+('; solder bridge configuration must be checked on actual C05' if native.startswith('ARD_') else ''))
for pin, cn in enumerate(range(65,71),1):
    native=stpins['CN12',str(cn)]
    function=nominal_mux.get(native,native)+(' via solder bridge' if native in nominal_mux else '')
    add('J5',pin,f'CN12-{cn}',function,'RTU_DE' if cn==69 else 'NC',
        'MCU output' if cn==69 else 'GPIO configurable; unused','3.3V logic / VDDIO2','ST_C05_NET',
        'Bottom female2x3; males66/67/68/69 user-reported; GPIOG depends on VDDIO2; native net '+native)

for ref, key, name, commit, filename, rename in [
    ('J6','ADAFRUIT_4718','FRAM','f7d4afa22ddd562f52c557e2e8f003611515b4f4','SPI%20FRAM%202%20and%204%20Mbit.brd',
     {'SS':'FRAM_CS','MOSI':'FRAM_MOSI','MISO_3.3V':'FRAM_MISO','SCK':'FRAM_SCK','GND':'GND','VIN':'+3V3'}),
    ('J7','ADAFRUIT_4682','SD','959792478854acec21fb862650427ffaa32b07f1','Adafruit%20SDIO%20microSD.brd',
     {'3.3V':'+3V3','GND':'GND','SCLK':'SD_SCK','DO':'SD_MISO','DI':'SD_MOSI','CS':'SD_CS'})]:
    repo = 'Adafruit-FRAM-Breakout-PCB' if name=='FRAM' else 'Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB'
    path=source(key,reports/f'TR2-shield-v26-20261105/{name}-official.brd',
                f'https://raw.githubusercontent.com/adafruit/{repo}/{commit}/{filename}',
                commit,'native Eagle JP1 contactrefs, no project pinout used')
    eagle=ET.parse(path).getroot()
    funcs={x.attrib['pad']:s.attrib['name'] for s in eagle.findall('.//board/signals/signal')
           for x in s.findall('contactref') if x.attrib['element']=='JP1'}
    assert len(funcs)==9
    for pin in range(1,10):
        f=funcs[str(pin)]; wanted=rename.get(f,'NC')
        direction = ('module output' if f in ['MISO_3.3V','DO','CARDDET','3.3V'] and not (name=='SD' and f=='3.3V') else 'module input')
        if f in ['VIN','GND'] or name=='SD' and f=='3.3V':direction='power'
        note = 'NC is deliberate non-use, not necessarily an internally unconnected pad'
        if name=='FRAM' and f=='3.3V':note='LDO output: do not backfeed; VIN alone receives shield3V3'
        if name=='FRAM' and f in ['WP','HOLD']:note='On-module pull-up must remain populated; source schematic checked separately'
        add(ref,pin,f'JP1-{pin}',f,wanted,direction,'3.3V domain',key,note)

source('ST_MKI_DRAWING',reports/'TR2-shield-dual-bringup-20261117/ST-MKI208-xon.pdf',
       'https://xonstorage.z8.web.core.windows.net/pdf/stmicroelectronics_stevalmki208v1k_apr22_xonlink.pdf',
       'ST DB4012 Rev3 p2 Fig2; distributor mirror, not official host',
       'manual visual transcription; official schematic URL https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf')
mki={1:'VDD',2:'VDDIO',3:'OCS',4:'CSX',5:'SDX',6:'OSDO',13:'GND',14:'INT1',15:'INT2',19:'CS',20:'SCL',21:'SDA',22:'SDO'}
mkinets={1:'+3V3',2:'+3V3',13:'GND',19:'ACC_CS',20:'ACC_SCK',21:'ACC_MOSI',22:'ACC_MISO'}
for pin in range(1,25):
    f=mki.get(pin,'NC');direction='module output' if f in ['SDO','INT1','INT2'] else 'module input'
    if f in ['VDD','VDDIO','GND']:direction='power'
    if f=='NC':direction='none'
    if f in ['OCS','CSX','SDX','OSDO']:direction='auxiliary; direction depends on module/straps; unused'
    add('J8',pin,('JP1-' if pin<=12 else 'JP2-')+str(pin),f,mkinets.get(pin,'NC'),direction,'3.3V domain','ST_MKI_DRAWING',
        'JP1 numbered1..12; JP2 global24..13 from same end; connector J1 goes to MKI208V1 via ribbon')

source('CLICK_V102',reports/'TR2-shield-dual-bringup-20261117/rs485-isolator-2-click-schematic-v102.pdf',
       'https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf',
       'v102; 2026 copyright; not the v101 mechanical archive', 'manual wire trace to ADM2867E pins8/9/10/11; product pinout corroboration')
click={1:'NC/AN',2:'/RE/RST',3:'DE/CS',4:'NC/SCK',5:'NC/MISO',6:'NC/MOSI',7:'3V3',8:'GND1',9:'GND1',10:'5V',11:'NC/SDA',12:'NC/SCL',13:'RX: UART input to TXD11',14:'TX: UART output from RXD8',15:'INVD/INT',16:'INVR/PWM'}
clicknets={3:'RTU_DE',7:'+3V3',8:'GND',9:'GND',13:'RTU_TX',14:'RTU_RX'}
for pin in range(1,17):
    direction='module input' if pin in [2,3,13,15,16] else ('module output' if pin==14 else 'unused')
    if pin in [7,8,9,10]:direction='power'
    note='v102 schematic and official product pinout; header label is from module perspective'
    if pin==2:note='/RE follows DE via user-declared J2 bridge; actual J3 state still to confirm'
    if pin in [15,16]:note='On-module R15/R16 6.8k pull-down: unused inversion defaults low'
    if pin==10:note='No5V on shield; VCCSEL must select3V3'
    add('J9',pin,'mikroBUS-'+str(pin),click[pin],clicknets.get(pin,'NC'),direction,'logic side3.3V / GND1','CLICK_V102',note)

# These are explicit TR2 circuit requirements, not manufacturer connector pinouts.
sources['TR2_REQUIREMENT'] = dict(
    source_type='audit design intent for generic passives/TP only; not manufacturer evidence',
    local_file=str(Path(__file__).resolve()),
    url='https://github.com/loloLR17/MSM_vibe/blob/main/Modbus%20RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/build_reference.py',
    revision='E01 functional intent 17-Nov-2026; content pinned by SHA256 and publication commit',
    sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
    bytes=Path(__file__).stat().st_size,
    method='explicit resistor/capacitor/test-point design requirements; actual fitted MPN/values remain a hardware gate')
passives={'R1':('+3V3','ACC_CS'),'R2':('+3V3','FRAM_CS'),'R3':('+3V3','SD_CS'),
          'R4':('RTU_DE','GND'),'C1':('+3V3','GND'),'C2':('+3V3','GND')}
for ref,nets in passives.items():
    for pin,net in enumerate(nets,1):add(ref,pin,ref+'-'+str(pin),'10k pull' if ref[0]=='R' else 'decoupling',net,'passive','3.3V domain','TR2_REQUIREMENT','C2 positive on1, negative on2; actual part marking/spacing separate physical gate')
for ref,net in [('TP1','+3V3'),('TP2','GND'),('TP3','ACC_SCK'),('TP4','SD_CS')]:
    add(ref,1,ref,'test access',net,'passive','3.3V domain','TR2_REQUIREMENT')
assert len(rows)==180 and len({(x['ref'],x['pin']) for x in rows})==180
(HERE/'reference-constructeurs.json').write_text(json.dumps({'scope':'Independent source stage; before KiCad/CAM comparison',
        'bus_policy':policy,'sources':sources,'rows':rows},indent=2,ensure_ascii=False)+'\n')
(HERE/'ST-C05-netnodes-relevant.json').write_text(json.dumps({k:v for k,v in netnodes.items() if any(a in ['CN7','CN8','CN9','CN10','CN12','SB25','SB27','SB31','SB33','SB72','SB73'] for a,b in v)},indent=2)+'\n')
print('Frozen independent reference:',len(rows),'pads; sources',len(sources),'; no KiCad input read')
