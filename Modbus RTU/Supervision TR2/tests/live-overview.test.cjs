const assert = require('node:assert/strict');
const fs = require('node:fs');
const zlib = require('node:zlib');
const {present, matchesDevice, selectDevice} = require('../src/TR2.Supervision.Web/wwwroot/live-overview.js');
const root = process.argv[2];
const raw = zlib.inflateSync(fs.readFileSync(root + '/running-service.log.zlib')).toString('utf8');
// Actual R5-B2 captured fields; historical use only, never a new physical observation.
const views = raw.split('\n').filter(x => x.startsWith('{"Observation"')).map(JSON.parse);
const camel = value => Array.isArray(value) ? value.map(camel) : value && typeof value === 'object' ? Object.fromEntries(Object.entries(value).map(([k,v]) => [k[0].toLowerCase()+k.slice(1),camel(v)])) : value;
const good = camel(views.find(x => x.IsCurrent)).observation;
const row = {observation:good,lastKnown:good,historical:true,transportConnected:null,isCurrent:false,receptionAgeSeconds:100};
let x = present(row); assert(x.state.includes('Historique')); assert(x.prefix.includes('périmée')); assert(x.rms.endsWith(' mg')); assert(x.value.rmsYMg > 0); assert(x.severity.includes(String(good.snapshot.value.severityGlobal)));
const lost = camel(views.find(x => !x.Observation.Snapshot)).observation;
x = present({...row,observation:lost,historical:false,transportConnected:false}); assert(x.state.includes('perdue')); assert(x.label.includes('Non identifié')); assert(x.rms !== '—'); assert(x.prefix.includes('périmée'));
x = present({...row,observation:lost,lastKnown:null}); assert.equal(x.rms,'—'); assert.equal(x.severity,'Indisponible');
const heldraw = zlib.inflateSync(fs.readFileSync(root+'/held-service.log.zlib')).toString('utf8').split('\n').find(x=>x.startsWith('{"Observation"'));
x=present({...row,observation:camel(JSON.parse(heldraw)).observation}); assert(x.quality.includes('HELD')); assert(!x.quality.includes('Mesure valide et fraîche'));
assert.equal(good.snapshot.windowUtc,null);
const source=fs.readFileSync(require.resolve('../src/TR2.Supervision.Web/wwwroot/live-overview.js'),'utf8');assert(!source.includes('innerHTML'));assert(!source.includes('localStorage'));assert(!source.includes('POST'));
console.log('PASS R5B2 historical V/F mg/severity/XYZ, HELD, loss/retained explicit, null/no zero, UTC absent, no writes/cache/innerHTML');

assert(matchesDevice({observation:lost,lastKnown:good}, String(good.deviceId.value)));
assert(!matchesDevice({observation:lost,lastKnown:null}, String(good.deviceId.value)));
assert(present({...row,observation:lost}).label.includes("Non identifié"));
console.log("PASS open detail follows last known owner after identity loss without promoting identity");
const previous = {...row,observation:lost};
assert.equal(selectDevice([previous,row], String(good.deviceId.value)),row);
assert.equal(selectDevice([previous], String(good.deviceId.value)),previous);
console.log("PASS durable current identity takes priority over a retained unknown endpoint");
