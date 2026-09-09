const rows=document.getElementById('fleetRows');
const apiState=document.getElementById('apiState');
const observedAt=document.getElementById('observedAt');
const errorBanner=document.getElementById('webError');
const filters=[...document.querySelectorAll('[data-filter]')];
let lastDevices=[];
let activeFilter='all';

const pcClock=document.getElementById('pcClock');
const tick=()=>pcClock.textContent=new Date().toLocaleTimeString('fr-FR',{hour12:false});
tick();setInterval(tick,1000);

const esc=value=>String(value).replace(/[&<>"']/g,ch=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[ch]));
const isFresh=o=>o?.hasValue===true&&o?.isAvailable===true&&o?.freshness==='Fresh';
const isAttentionFreshness=o=>!o||!o.hasValue||!o.isAvailable||o.freshness!=='Fresh';
const hasB1Attention=o=>o?.hasValue&&o.value&&(o.value.faultFlags!==0||o.value.warningFlags!==0||o.value.errorCode!==0||o.value.warningCode!==0);
const hasB3Attention=o=>o?.hasValue&&o.value&&(o.value.alarmFlags!==0||o.value.alarmLatched!==0);
const needsAttention=d=>d.sessionState!=='Compatible'||!d.telemetry||isAttentionFreshness(d.telemetry.systemState)||isAttentionFreshness(d.telemetry.vibrationState)||hasB1Attention(d.telemetry.systemState)||hasB3Attention(d.telemetry.vibrationState);

function freshnessLabel(o){
 if(!o||!o.hasValue)return 'Jamais reçues';
 const labels={Fresh:'Fraîches',Aging:'Vieillissantes',Stale:'Périmées',Unavailable:'Indisponibles',NeverReceived:'Jamais reçues'};
 return labels[o.freshness]||o.freshness;
}
function freshnessClass(o){if(!o||!o.hasValue)return 'neutral';if(o.freshness==='Fresh')return 'ok';if(o.freshness==='Aging')return 'warn';return 'bad';}
function communication(d){
 const map={Compatible:['✓ Compatible','ok'],Unidentified:['? Non identifié','warn'],Incompatible:['✕ Incompatible','bad'],Disconnected:['✕ Déconnecté','bad']};
 return map[d.sessionState]||[d.sessionState,'neutral'];
}
function latestReceived(d){
 if(!d.telemetry)return null;
 const values=[d.telemetry.systemState,d.telemetry.timeState,d.telemetry.vibrationState].map(x=>x?.receivedAt).filter(Boolean).map(Date.parse).filter(Number.isFinite);
 return values.length?new Date(Math.max(...values)):null;
}
function relative(date){
 if(!date)return '—';
 const seconds=Math.max(0,Math.round((Date.now()-date.getTime())/1000));
 if(seconds<60)return `il y a ${seconds} s`;
 const minutes=Math.round(seconds/60);if(minutes<60)return `il y a ${minutes} min`;
 return date.toLocaleString('fr-FR');
}
function vibration(d){
 const o=d.telemetry?.vibrationState;
 if(!o?.hasValue||!o.value)return ['—',''];
 return [`${o.value.rmsGlobalMg} mg RMS`,o.isAvailable?'':'dernière valeur connue'];
}
function tr2State(d){
 const o=d.telemetry?.systemState;
 if(!o?.hasValue||!o.value)return ['—',''];
 return [`SystemStatus ${o.value.systemStatus}`,o.isAvailable?'':'dernière valeur connue'];
}
function attentionText(d){
 const items=[];
 if(d.sessionState!=='Compatible')items.push(d.sessionState);
 const b1=d.telemetry?.systemState,b3=d.telemetry?.vibrationState;
 if(isAttentionFreshness(b1)||isAttentionFreshness(b3))items.push('Données PC');
 if(hasB1Attention(b1))items.push('État TR2');
 if(hasB3Attention(b3))items.push('Alarme B3');
 return items.length?items.join(' · '):'—';
}
function render(devices){
 const shown=activeFilter==='attention'?devices.filter(needsAttention):devices;
 if(!shown.length){rows.innerHTML='<tr><td colspan="8" class="empty">Aucun point dans ce filtre.</td></tr>';return;}
 rows.innerHTML=shown.map(d=>{
   const comm=communication(d),vib=vibration(d),state=tr2State(d),fresh=d.telemetry?.vibrationState??d.telemetry?.systemState;
   return `<tr data-attention="${needsAttention(d)}"><td><span class="endpoint">${esc(d.busId)} / adresse ${d.modbusAddress}</span><small>endpoint configuré</small></td><td>${d.deviceId==null?'—':esc(d.deviceId)}</td><td><span class="status ${comm[1]}">${esc(comm[0])}</span></td><td><span class="status ${freshnessClass(fresh)}">${esc(freshnessLabel(fresh))}</span></td><td>${esc(vib[0])}${vib[1]?`<small class="muted">${esc(vib[1])}</small>`:''}</td><td>${esc(state[0])}${state[1]?`<small class="muted">${esc(state[1])}</small>`:''}</td><td><span class="${needsAttention(d)?'attention warn':'neutral'}">${esc(attentionText(d))}</span></td><td>${esc(relative(latestReceived(d)))}</td></tr>`;
 }).join('');
}
function updateKpis(devices){
 document.getElementById('kpiTotal').textContent=devices.length;
 document.getElementById('kpiCompatible').textContent=devices.filter(d=>d.sessionState==='Compatible').length;
 document.getElementById('kpiAttention').textContent=devices.filter(needsAttention).length;
 document.getElementById('kpiFresh').textContent=devices.filter(d=>isFresh(d.telemetry?.systemState)&&isFresh(d.telemetry?.vibrationState)).length;
}
async function refresh(){
 try{
   const response=await fetch('/api/v1/fleet',{headers:{Accept:'application/json'},cache:'no-store'});
   if(!response.ok)throw new Error(`HTTP ${response.status}`);
   const payload=await response.json();
   lastDevices=Array.isArray(payload.devices)?payload.devices:[];
   apiState.textContent='✓ Connectée';apiState.className='ok';
   observedAt.textContent=new Date(payload.observedAt).toLocaleString('fr-FR');
   errorBanner.classList.add('hidden');
   updateKpis(lastDevices);render(lastDevices);
 }catch(error){
   apiState.textContent='✕ Mise à jour impossible';apiState.className='bad';
   errorBanner.textContent=`API Web indisponible : ${error.message}. Les dernières valeurs affichées sont conservées.`;
   errorBanner.classList.remove('hidden');
 }
 finally{setTimeout(refresh,2000);}
}
filters.forEach(button=>button.addEventListener('click',()=>{filters.forEach(x=>x.classList.remove('active'));button.classList.add('active');activeFilter=button.dataset.filter;render(lastDevices);}));
refresh();
