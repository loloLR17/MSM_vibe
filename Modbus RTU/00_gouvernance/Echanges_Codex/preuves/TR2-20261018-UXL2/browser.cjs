const fs = require('node:fs'), cp = require('node:child_process'), assert = require('node:assert/strict');
const out = process.argv[2], port = 5094, debug = 9396;
const edge = cp.spawn('C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe', ['--headless=new','--disable-gpu','--no-first-run','--no-default-browser-check', '--remote-debugging-address=127.0.0.1','--remote-debugging-port='+debug,'--user-data-dir=C:\\Users\\lolo\\AppData\\Local\\MSM\\UXL2-cdp-'+Date.now(),'about:blank'], {stdio:'ignore'});
const sleep = ms => new Promise(r=>setTimeout(r,ms));
let ws, seq=0; const pending = new Map(), results=[];
function send(method,params={}) { return new Promise((resolve,reject)=>{const id=++seq;pending.set(id,{resolve,reject});ws.send(JSON.stringify({id,method,params}));}); }
async function ev(expression) { const z=await send('Runtime.evaluate',{expression,returnByValue:true,awaitPromise:true});if(z.exceptionDetails) throw Error(JSON.stringify(z.exceptionDetails));return z.result.value; }
async function wait(expression) { for(let i=0;i<100;i++){if(await ev(expression))return;await sleep(100);}throw Error('DOM timeout '+expression); }
async function save(name) {fs.writeFileSync(out+'\\'+name+'.html',await ev('document.documentElement.outerHTML'));}
async function check(name,expr) { assert(await ev(expr),name);results.push({check:name,pass:true}); }
async function change(id,value,checked=false) { await ev(`(()=>{const n=document.getElementById(${JSON.stringify(id)});n.${checked?'checked':'value'}=${JSON.stringify(value)};n.dispatchEvent(new Event('change',{bubbles:true}));n.dispatchEvent(new Event('input',{bubbles:true}));})()`); }
async function click(id){await ev(`document.getElementById(${JSON.stringify(id)}).click()`);}
async function navigate(path) {await send('Page.navigate',{url:'http://127.0.0.1:'+port+path});await wait('document.readyState === "complete"');}
(async()=>{
 try {
 let targets;for(let i=0;i<100;i++){try {targets=await(await fetch('http://127.0.0.1:'+debug+'/json')).json();if(targets.length)break;}catch{}await sleep(100);}
 ws=new WebSocket(targets.find(t=>t.type==='page').webSocketDebuggerUrl);
 await new Promise((r,j)=>{ws.onopen=r;ws.onerror=j;});ws.onmessage=e=>{const v=JSON.parse(e.data),p=pending.get(v.id);if(p){pending.delete(v.id);v.error?p.reject(v.error):p.resolve(v.result);}};
 await send('Page.enable');
 await navigate('/live-fleet.html');await wait('document.querySelectorAll("#liveFleetRows tr").length === 1');
 await check('real archive fleet historical, HELD, read-only', 'document.body.textContent.includes("Historique") && document.body.textContent.includes("HELD") && document.body.textContent.includes("NON-PRODUCTION")');
 await save('fleet');await change('fleetAttention',true,true);await check('intentional historical HELD does not trigger Attention','document.querySelectorAll("#liveFleetRows tr").length === 0');await change('fleetAttention',false,true);
 await change('fleetSearch','not-a-real-device');await check('search excludes unmatched sensor','document.querySelectorAll("#liveFleetRows tr").length === 0');await change('fleetSearch','TR2 1');await check('identity search restores actual sensor','document.querySelectorAll("#liveFleetRows tr").length === 1');
 await navigate('/live-device.html?deviceId=1&view=vibration');await wait('document.querySelectorAll("#historyGlobal circle").length > 2');
 const metrics=await ev('({globalPoints:document.querySelectorAll("#historyGlobal circle").length,xyzPoints:document.querySelectorAll("#historyAxes circle").length,paths:document.querySelectorAll("#historyGlobal polyline").length,breaks:document.querySelectorAll("#historyGlobal [data-break]").length,rows:document.querySelectorAll("#historyRows tr").length,coverage:document.getElementById("historyCoverage").textContent})');
 assert.equal(metrics.rows,500);assert(metrics.globalPoints>100);assert(metrics.xyzPoints>200);assert(metrics.breaks>10);results.push({check:'actual R5B2 multipoint graph from HTTP and SQLite',pass:true,...metrics});await save('vibration');
 await change('historyX',false,true);await check('X visibility independent','document.querySelectorAll("#historyAxes circle").length === '+metrics.xyzPoints*2/3);await change('historyY',false,true);await change('historyZ',false,true);await check('all XYZ hidden without fake points','document.querySelectorAll("#historyAxes circle").length === 0');await change('historyY',true,true);await change('historyMetric','peak');await check('peak Y uses actual legend','document.getElementById("historyAxes").textContent.includes("PEAK Y")');
 await click('historyOlder');await wait('document.querySelectorAll("#historyRows tr").length === 28');await check('older page empty measurements remain observations','document.querySelectorAll("#historyGlobal circle").length === 0 && document.getElementById("historyOlder").disabled');await save('older');
 await click('historyRecent');await wait('document.querySelectorAll("#historyRows tr").length === 500');
 for(const period of ['24','168']) {await change('historyPeriod',period);await wait('document.querySelectorAll("#historyGlobal circle").length > 2');await check('period '+period+'h bounded true coverage','document.querySelectorAll("#historyRows tr").length === 500');}
 await change('historyPeriod','custom');await wait('document.getElementById("historyError").textContent.includes("Plage personnalisée")');await check('invalid range clears curves/export','document.querySelectorAll("#historyGlobal circle").length === 0 && document.getElementById("historyCsv").disabled');
 await change('historyFrom','2026-10-15T00:00');await change('historyTo','2026-10-15T01:00');await click('historyApply');await wait('document.getElementById("historyCoverage").textContent.includes("0 observations")');await check('empty period no invented measurements','document.querySelectorAll("#historyGlobal circle").length === 0');await save('empty');
 await change('historyPeriod','1');await wait('document.querySelectorAll("#historyGlobal circle").length > 2');
 // Capture the actual download payload in-browser without writing outside the evidence folder.
 await ev('window.savedBlob=null;window.savedCreate=URL.createObjectURL;URL.createObjectURL=b=>{window.savedBlob=b;return window.savedCreate(b)};window.savedClick=HTMLAnchorElement.prototype.click;HTMLAnchorElement.prototype.click=function(){}');await click('historyCsv');
 const csv=await ev('window.savedBlob.text()');fs.writeFileSync(out+'\\export-actual-page.csv',csv);assert.equal(csv.split('\r\n').length,502);results.push({check:'real browser CSV exactly 500 displayed observations plus header',pass:true,lines:501});
 await ev('URL.createObjectURL=window.savedCreate;HTMLAnchorElement.prototype.click=window.savedClick;window.originalFetch=window.fetch;window.fetch=(...a)=>String(a[0]).includes("/history?")?Promise.resolve(new Response(JSON.stringify({error:"SIMULATED browser transport failure"}),{status:503})):window.originalFetch(...a)');
 await click('historyRecent');await wait('document.getElementById("historyError").textContent.includes("SIMULATED")');await check('simulated HTTP failure removes chart and disables CSV','document.querySelectorAll("#historyGlobal circle").length === 0 && document.getElementById("historyCsv").disabled');await save('api-error-simulated');
 await ev('window.fetch=window.originalFetch');await click('historyRecent');await wait('document.querySelectorAll("#historyGlobal circle").length > 2');
 for(const view of ['state','commands','campaigns','diagnostic']){await ev(`document.querySelector('[data-live-tab="${view}"]').click()`);await wait('document.getElementById("liveHistory").hidden');await check('tab '+view+' hides prior chart','document.getElementById("liveHistory").hidden');}
 await ev('document.querySelector(\'[data-live-tab="summary"]\').click()');await wait('document.querySelectorAll("#historyGlobal circle").length > 2');await check('summary trend returns, XYZ hidden','document.getElementById("historyAxes").hidden');await save('summary');
 await navigate('/live-device.html?deviceId=999');await wait('document.getElementById("liveDeviceContent").textContent.includes("absent")');await check('switch to absent identity clears historical chart','document.getElementById("liveHistory").hidden && document.querySelectorAll("#historyGlobal circle").length === 0');
 fs.writeFileSync(out+'\\browser-summary.json',JSON.stringify({pass:true,realHeadlessBrowser:true,humanVisualReview:false,newPhysicalAcquisition:false,results},null,2)+'\n');
 } finally { if(ws){try{await send('Browser.close')}catch{}ws.close();} else edge.kill(); }
})().catch(e=>{console.error(e);process.exitCode=1;});
