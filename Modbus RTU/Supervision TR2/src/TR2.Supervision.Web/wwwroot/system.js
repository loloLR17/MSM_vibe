const apiState=document.getElementById('apiState');
const observedAt=document.getElementById('observedAt');
const pcClock=document.getElementById('pcClock');
const errorBanner=document.getElementById('webError');
let lastPayload=null;

const tick=()=>pcClock.textContent=new Date().toLocaleTimeString('fr-FR',{hour12:false});tick();setInterval(tick,1000);
const text=(id,value)=>document.getElementById(id).textContent=value;
const cell=(value)=>{const td=document.createElement('td');td.textContent=value===null||value===undefined?'—':String(value);return td;};
const row=(values)=>{const tr=document.createElement('tr');values.forEach(value=>tr.append(cell(value)));return tr;};
const emptyRow=(colspan,message)=>{const tr=document.createElement('tr');const td=document.createElement('td');td.colSpan=colspan;td.className='empty';td.textContent=message;tr.append(td);return tr;};

function render(payload){
 const s=payload.system;
 text('hostState',s.hostState);
 text('readiness',s.isReady?'Prêt':'Non prêt');
 text('connectedBusCount',s.connectedBusIds.length);
 text('failureCount',s.recentCommunicationFailures.length);

 const buses=document.getElementById('connectedBuses');
 buses.replaceChildren();
 if(s.connectedBusIds.length===0){
  const wrapper=document.createElement('div');const dt=document.createElement('dt');const dd=document.createElement('dd');dt.textContent='État';dd.textContent='Aucun bus connecté';wrapper.append(dt,dd);buses.append(wrapper);
 }else{
  s.connectedBusIds.forEach((busId,index)=>{const wrapper=document.createElement('div');const dt=document.createElement('dt');const dd=document.createElement('dd');dt.textContent=`Bus ${index+1}`;dd.textContent=busId;wrapper.append(dt,dd);buses.append(wrapper);});
 }

 const endpoints=document.getElementById('endpointRows');
 endpoints.replaceChildren(...(s.endpoints.length?s.endpoints.map(e=>row([e.busId,e.modbusAddress,e.sessionState,e.deviceId])):[emptyRow(4,'Aucun endpoint configuré')])) ;

 const failures=document.getElementById('failureRows');
 failures.replaceChildren(...(s.recentCommunicationFailures.length?s.recentCommunicationFailures.map(f=>row([
  f.failureId,
  `${f.busId}/${f.modbusAddress}`,
  f.deviceId,
  f.operation,
  f.category,
  new Date(f.observedAt).toLocaleString('fr-FR'),
  f.exceptionType,
  f.message
 ])):[emptyRow(8,'Aucun échec de communication dans la fenêtre retournée')])) ;
}

async function refresh(){
 try{
  const response=await fetch('/api/v1/system',{headers:{Accept:'application/json'},cache:'no-store'});
  if(!response.ok)throw new Error(`HTTP ${response.status}`);
  const payload=await response.json();
  lastPayload=payload;
  apiState.textContent='✓ Connectée';apiState.className='ok';
  observedAt.textContent=new Date(payload.observedAt).toLocaleString('fr-FR');
  errorBanner.classList.add('hidden');
  render(payload);
 }catch(error){
  apiState.textContent='✕ Mise à jour impossible';apiState.className='bad';
  errorBanner.textContent=`API système indisponible : ${error.message}. Les dernières valeurs affichées sont conservées.`;
  errorBanner.classList.remove('hidden');
  if(lastPayload)render(lastPayload);
 }finally{setTimeout(refresh,2000);}
}
refresh();
