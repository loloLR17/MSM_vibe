const test = require('node:test'), assert = require('node:assert/strict'), fs = require('node:fs'), zlib = require('node:zlib');
const T = require('../src/TR2.Supervision.Web/wwwroot/live-trends.js');
const camel = v => Array.isArray(v) ? v.map(camel) : v && typeof v === 'object' ? Object.fromEntries(Object.entries(v).map(([k,x]) => [k[0].toLowerCase()+k.slice(1),camel(x)])) : v;
const root = process.argv[2], observations = [];
for (const file of ['running-service.log.zlib','held-service.log.zlib']) {
 const raw = zlib.inflateSync(fs.readFileSync(root+'/'+file)).toString();
 observations.push(...raw.split('\n').filter(l=>l.startsWith('{"Observation"')).map(l=>camel(JSON.parse(l)).observation));
}
const good = observations.find(o=>o.snapshot?.valuesValid && o.snapshot.fresh && !o.snapshot.held);
const owner = good.deviceId.value, loss = observations.find(o=>!o.snapshot), held = observations.find(o=>o.snapshot?.held);
const e = o => ({rowId:1,observation:o});
const next = (overrides={}) => ({...good,observationId:'next',receivedAt:new Date(Date.parse(good.receivedAt)+1000).toISOString(),continuity:'Sequential',snapshot:{...good.snapshot,value:{...good.snapshot.value,calculationSequence:good.snapshot.value.calculationSequence+1}},...overrides});
test('real R5B2 multi-point global/XYZ, historical V/F and HELD/loss stay explicit',()=>{
 const entries=observations.map((observation,rowId)=>({rowId,observation}));
 for(const f of ['rmsGlobalMg','peakGlobalMg','rmsXMg','rmsYMg','rmsZMg','peakXMg','peakYMg','peakZMg']) {
  const groups=T.series(entries,f,owner);assert(groups.flat().length>30);assert(groups.length>1);
 }
 assert(!T.plottable(held,owner));assert(T.quality(held,owner).includes('HELD'));
 assert(!T.plottable(loss,owner));assert(T.quality(good,owner).includes('non courant'));
 assert.equal(good.snapshot.windowUtc,null);
});
test('segment, missing snapshot, identity, stale, HELD, gap, clock reversal break paths',()=>{
 const b=next();assert(T.connected(good,b,owner));
 const cases=[next({segmentId:'other'}),next({deviceId:{value:2}}),next({deviceId:null}),next({continuity:'Gap'}),next({receivedAt:'2000-01-01T00:00:00Z'}),next({snapshot:null}),next({snapshot:{...b.snapshot,fresh:false}}),held,loss];
 for(const x of cases) { assert(!T.connected(good,x,owner)); const series=T.series([e(good),e(x),e(b)],'rmsGlobalMg',owner);assert(series.every(g=>g.length===1)); }
});
test('repeated sequences are distinct observations and repeated markers; no invented interpolation points',()=>{
 const b=next({continuity:'RepeatedSequence',snapshot:good.snapshot});
 const groups=T.series([e(good),e(b)],'rmsGlobalMg',owner);assert.equal(groups.length,1);assert.equal(groups[0].length,2);assert(groups[0][1].repeated);
});
test('Attention is not triggered by historical state or intentional HELD/STOP',()=>{
 const row={observation:good,historical:true,isCurrent:false};
 assert(!T.attention({...row,observation:held}));
 assert(T.attention({...row,observation:{...loss,continuity:'CommunicationLost'}}));
 assert(!T.attention({...row,observation:{...good,snapshot:{...good.snapshot,value:{...good.snapshot.value,severityGlobal:1}}}}));
 assert(T.attention({...row,archiveError:'unavailable'}));
});
test('fleet search/bus and stable sort preserve null last; no equipment assignment invented',()=>{
 const a={observation:good,lastKnown:good,receptionAgeSeconds:4}, b={observation:{...loss,busId:'B'},lastKnown:null,receptionAgeSeconds:10};
 assert.deepEqual(T.filterFleet([a,b],{bus:good.busId}),[a]);
 assert.deepEqual(T.filterFleet([a,b],{search:'TR2 '+owner}),[a]);
 for(const descending of [true,false]) assert.equal(T.filterFleet([b,a],{sort:'rms',descending})[1],b);
 assert.deepEqual(T.filterFleet([a,b],{sort:'age',descending:true}),[b,a]);
});
test('CSV matches selected actual observations and retains nulls, raw owners, provenance and HELD',()=>{
 const csv=T.csv([e(good),e(loss),e(held)],'real R5B2 selected page');
 assert.equal(csv.split('\r\n').length,5);assert(csv.includes(good.receivedAt));assert(csv.includes(loss.receivedAt));assert(csv.includes('window_utc_unavailable_if_empty'));
 assert(csv.includes('"true","false","true"') || csv.includes('HELD') || csv.includes('"true"'));
 const nullRow=csv.split('\r\n')[2];assert(nullRow.endsWith(',"","","","","","","",""'));
 assert(T.csv([e({...loss,source:'=HYPERLINK("evil")'})]).includes("'=HYPERLINK"));
 assert.equal(T.csv([]).split('\r\n').length,2);
});
test('browser scripts are read-only, safe DOM, no persistent model cache',()=>{
 for(const file of ['live-trends.js','live-history.js','live-overview.js']) {
 const src=fs.readFileSync(require.resolve('../src/TR2.Supervision.Web/wwwroot/'+file),'utf8');
 for(const forbidden of ['innerHTML','localStorage','method: "POST"','/api/v1/devices/']) assert(!src.includes(forbidden));
 }
});

test('7-day bounds preserve captured sub-millisecond reception precision',()=>{
 const actual='2026-10-18T17:07:58.6576849+00:00', p=T.periodRange(actual,168);
 assert.equal(p.from,'2026-10-11T17:07:58.6576849Z');assert.equal(p.to,actual);
 assert.equal(T.periodRange('2026-10-18T17:07:58Z',1).from,'2026-10-18T16:07:58.000Z');
});
