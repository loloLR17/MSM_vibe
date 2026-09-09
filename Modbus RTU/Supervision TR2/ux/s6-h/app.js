const views=["fleet","vibrations","attention","campaigns","system","device"];
const navButtons=[...document.querySelectorAll('.nav-item')];
const showView=name=>{views.forEach(v=>document.getElementById(`view-${v}`).classList.toggle('active',v===name));navButtons.forEach(b=>b.classList.toggle('active',b.dataset.view===name));window.scrollTo({top:0,behavior:'instant'});};
navButtons.forEach(b=>b.addEventListener('click',()=>showView(b.dataset.view)));

const clock=document.getElementById('pcClock');
const tick=()=>clock.textContent=new Date().toLocaleTimeString('fr-FR',{hour12:false});tick();setInterval(tick,1000);

const filterButtons=[...document.querySelectorAll('[data-fleet-filter]')];
filterButtons.forEach(b=>b.addEventListener('click',()=>{filterButtons.forEach(x=>x.classList.remove('active'));b.classList.add('active');document.querySelectorAll('.device-row').forEach(row=>row.hidden=b.dataset.fleetFilter==='attention'&&row.dataset.attention!=='yes');}));

const devices={
 '0042':{title:'Ventilation garage',point:'Palier moteur AV',identity:'TR2-004 · device_id 0042',comm:'✓ Joignable',commClass:'ok',fresh:'Données PC à jour',state:'TR2 NOMINAL',ambiguous:false},
 '0043':{title:'Chiller n°1',point:'Palier compresseur',identity:'TR2-005 · device_id 0043',comm:'✕ Non joignable',commClass:'bad',fresh:'Données PC périmées',state:'État TR2 précédent',ambiguous:false},
 '0048':{title:'Pompe eau douce n°2',point:'Palier moteur',identity:'TR2-006 · device_id 0048',comm:'✓ Joignable',commClass:'ok',fresh:'Données PC à jour',state:'TR2 NOMINAL',ambiguous:true}
};

function setDeviceTab(name){document.querySelectorAll('[data-device-tab]').forEach(b=>b.classList.toggle('active',b.dataset.deviceTab===name));document.querySelectorAll('.device-panel').forEach(p=>p.classList.toggle('active',p.id===`device-${name}`));}
function openDevice(id,tab='summary'){
 const d=devices[id];if(!d)return;
 document.getElementById('deviceTitle').textContent=d.title;document.getElementById('devicePoint').textContent=d.point;document.getElementById('deviceIdentity').textContent=d.identity;
 const comm=document.getElementById('deviceComm');comm.textContent=d.comm;comm.className=`status ${d.commClass}`;document.getElementById('deviceFresh').textContent=d.fresh;document.getElementById('deviceTr2').textContent=d.state;
 document.getElementById('commandNormal').classList.toggle('hidden',d.ambiguous);document.getElementById('commandAmbiguous').classList.toggle('hidden',!d.ambiguous);
 setDeviceTab(tab);showView('device');
}

document.querySelectorAll('.device-row').forEach(row=>row.addEventListener('click',()=>openDevice(row.dataset.device)));
document.querySelectorAll('[data-open-device]').forEach(b=>b.addEventListener('click',()=>openDevice(b.dataset.openDevice,b.dataset.tab||'summary')));
document.getElementById('backToFleet').addEventListener('click',()=>showView('fleet'));
document.querySelectorAll('[data-device-tab]').forEach(b=>b.addEventListener('click',()=>setDeviceTab(b.dataset.deviceTab)));
document.querySelectorAll('[data-device-tab-target]').forEach(b=>b.addEventListener('click',()=>setDeviceTab(b.dataset.deviceTabTarget)));

document.querySelectorAll('[data-system-tab]').forEach(b=>b.addEventListener('click',()=>{document.querySelectorAll('[data-system-tab]').forEach(x=>x.classList.remove('active'));b.classList.add('active');document.querySelectorAll('.system-panel').forEach(p=>p.classList.toggle('active',p.id===`system-${b.dataset.systemTab}`));}));

const confirmDialog=document.getElementById('confirmDialog');
document.querySelectorAll('[data-confirm="reset"]').forEach(b=>b.addEventListener('click',()=>confirmDialog.showModal()));
confirmDialog.addEventListener('close',()=>{if(confirmDialog.returnValue==='confirm'){alert('Maquette uniquement : aucune commande B5 n’a été envoyée.');}});
