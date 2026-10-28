const test=require('node:test'),assert=require('node:assert/strict');
const T=require('../src/TR2.Supervision.Web/wwwroot/live-trends.js');
const {present}=require('../src/TR2.Supervision.Web/wwwroot/live-overview.js');
test('mg scale covers actual near-gravity values and high acceleration without a 2 mg cap',()=>{
 for(const values of [[960,962],[0,2100],[12000,16000],[2],[0],[-0,1]]) {
  const d=T.domain(values);assert.equal(d.min,0);assert(d.max>=Math.max(...values));assert(d.max>0);assert(d.ticks.length>=2);
 }
 assert.equal(T.domain([NaN,null]).min,0);
});
test('variation scale never changes values or CSV and keeps an explicit nonzero origin',()=>{
 const values=[960,962],before=[...values],d=T.domain(values,true);
 assert(d.min>0);assert(d.min<=960);assert(d.max>=962);assert.deepEqual(values,before);
});
test('last-known measurement age is independent of a new reception containing no valid value',()=>{
 const old={observationId:'old',deviceId:{value:1},receivedAt:'2026-10-19T15:00:00Z',snapshot:{value:{rmsGlobalMg:962,peakGlobalMg:1000,severityGlobal:0},valuesValid:true,fresh:false,held:true}};
 const row={observation:{observationId:'new',deviceId:{value:1},busId:'live-direct',modbusAddress:1,receivedAt:new Date().toISOString(),snapshot:{valuesValid:false,acquisitionStopped:true}},lastKnown:old,isCurrent:false,historical:false,transportConnected:true,receptionAgeSeconds:0};
 const x=present(row);assert(x.retained);assert.equal(x.rms,'962 mg');assert(x.measurementAgeSeconds>3600);assert.equal(x.acquisition,'STOP');assert(x.age.startsWith('0 s'));
 const empty=present({...row,lastKnown:null});assert.equal(empty.rms,'—');assert.equal(empty.measurementAgeSeconds,null);
});
test('intentional STOP/HELD is distinct from live acquisition and transport loss',()=>{
 const o={observationId:'a',deviceId:{value:1},busId:'b',modbusAddress:1,receivedAt:new Date().toISOString(),snapshot:{valuesValid:true,fresh:false,held:true,acquisitionStopped:true,value:{rmsGlobalMg:962,peakGlobalMg:1000,severityGlobal:0}}};
 const row={observation:o,isCurrent:false,transportConnected:true,receptionAgeSeconds:0};
 assert.equal(present(row).acquisition,'STOP · HELD');assert(!T.attention(row));
 assert.equal(present({...row,isCurrent:true,observation:{...o,snapshot:{...o.snapshot,held:false,fresh:true,acquisitionStopped:false}}}).acquisition,'RUNNING');
});
