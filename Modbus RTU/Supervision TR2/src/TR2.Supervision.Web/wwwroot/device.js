const qs=new URLSearchParams(location.search);
const rawDeviceId=qs.get('deviceId');
const deviceId=rawDeviceId!==null&&/^\d+$/.test(rawDeviceId)?Number(rawDeviceId):NaN;
const apiState=document.getElementById('apiState');
const observedAt=document.getElementById('observedAt');
const errorBanner=document.getElementById('webError');
const pcClock=document.getElementById('pcClock');
let lastPayload=null;

const tick=()=>pcClock.textContent=new Date().toLocaleTimeString('fr-FR',{hour12:false});tick();setInterval(tick,1000);
const text=(id,value)=>document.getElementById(id).textContent=value;
const fmt=(value,suffix='')=>value===null||value===undefined?'—':`${value}${suffix}`;
const hex=value=>value===null||value===undefined?'—':`0x${Number(value).toString(16).toUpperCase().padStart(4,'0')}`;
const dateSeconds=value=>value?new Date(Number(value)*1000).toLocaleString('fr-FR'):'—';
const freshness=o=>{if(!o||!o.hasValue)return 'Jamais reçues';const m={Fresh:'Fraîches',Aging:'Vieillissantes',Stale:'Périmées',Unavailable:'Indisponibles',NeverReceived:'Jamais reçues'};return m[o.freshness]||o.freshness;};
const availability=o=>!o?.hasValue?'absence de valeur':o.isAvailable?'disponible':'dernière valeur connue';
const enumLabel=(value,map)=>Object.prototype.hasOwnProperty.call(map,value)?`${map[value]} (${value})`:`Réservé / valeur ${value}`;
const dl=(id,items)=>{document.getElementById(id).innerHTML=items.map(([k,v])=>`<div><dt>${k}</dt><dd>${v}</dd></div>`).join('');};

const systemStatus=v=>enumLabel(v,{0:'UNKNOWN',1:'NOMINAL',2:'DEGRADED',3:'FAULT'});
const acquisition=v=>enumLabel(v,{0:'Arrêtée',1:'En cours',2:'Pause',3:'Erreur'});
const storage=v=>enumLabel(v,{0:'Non disponible',1:'Disponible',2:'Plein',3:'Erreur'});
const b3Status=v=>enumLabel(v,{0:'Non initialisé',1:'Supervision inactive',2:'Acquisition / calcul indisponible',3:'Valeurs valides',4:'Valeurs dégradées',5:'Valeurs invalides'});
const severity=v=>enumLabel(v,{0:'Non applicable',1:'Normal',2:'Information',3:'Avertissement',4:'Alarme',5:'Critique'});
const axis=v=>enumLabel(v,{0:'Non déterminé',1:'X',2:'Y',3:'Z',4:'Ex aequo / indéterminé'});
const health=v=>enumLabel(v,{0:'OK',1:'Warning',2:'Dégradé',3:'Critique'});
const selftest=v=>enumLabel(v,{0:'Jamais exécuté',1:'En cours',2:'OK',3:'Échec'});
const reset=v=>enumLabel(v,{0:'Inconnu',1:'Power-on',2:'Reset logiciel',3:'Watchdog',4:'Brown-out',5:'Reset externe',6:'Mise à jour firmware'});

function render(payload){
 const d=payload.device,t=d.telemetry||{};
 const b1=t.systemState,b2=t.timeState,b3=t.vibrationState,b7=t.diagnosticState;
 text('deviceTitle',`TR2 ${d.deviceId}`);
 text('deviceSubtitle',`${d.busId} / adresse ${d.modbusAddress} · device_id ${d.deviceId}`);
 text('summarySession',d.sessionState);
 text('summaryRms',b3?.hasValue?`${b3.value.rmsGlobalMg} mg`:'—');
 text('summaryPeak',b3?.hasValue?`${b3.value.peakGlobalMg} mg`:'—');
 text('summaryHealth',b7?.hasValue?health(b7.value.systemHealthStatus):'—');
 dl('summaryFreshness',[["B1",freshness(b1)],["B2",freshness(b2)],["B3",freshness(b3)],["B7",freshness(b7)]]);
 dl('summaryState',[["Session",d.sessionState],["État B1",b1?.hasValue?systemStatus(b1.value.systemStatus):'—'],["Acquisition",b1?.hasValue?acquisition(b1.value.acquisitionState):'—'],["Santé B7",b7?.hasValue?health(b7.value.systemHealthStatus):'—']]);

 if(b3?.hasValue){const v=b3.value;
  dl('vibrationGlobal',[["RMS global",fmt(v.rmsGlobalMg,' mg')],["Crête globale",fmt(v.peakGlobalMg,' mg')],["Statut",b3Status(v.statusGlobal)],["Sévérité",severity(v.severityGlobal)]]);
  dl('vibrationAxes',[["RMS X",fmt(v.rmsXMg,' mg')],["RMS Y",fmt(v.rmsYMg,' mg')],["RMS Z",fmt(v.rmsZMg,' mg')],["Crête X",fmt(v.peakXMg,' mg')],["Crête Y",fmt(v.peakYMg,' mg')],["Crête Z",fmt(v.peakZMg,' mg')],["Axe dominant",axis(v.dominantAxis)]]);
  dl('vibrationQuality',[["Fraîcheur PC",`${freshness(b3)} · ${availability(b3)}`],["Validity flags",hex(v.validityFlags)],["Alarm flags",hex(v.alarmFlags)],["Âge valeur TR2",fmt(v.valueAgeMilliseconds,' ms')],["Dernière MAJ TR2",dateSeconds(v.lastUpdateTr2Seconds)],["Fenêtre",fmt(v.windowDurationMilliseconds,' ms')],["Échantillons valides",fmt(v.validSampleCount)]]);
  dl('vibrationCounters',[["Séquence calcul",fmt(v.calculationSequence)],["Dépassements",fmt(v.exceedCount)],["Alarmes",fmt(v.alarmCount)],["Alarme mémorisée",fmt(v.alarmLatched)],["Dépassement global",fmt(v.exceedGlobal)]]);
 } else {['vibrationGlobal','vibrationAxes','vibrationQuality','vibrationCounters'].forEach(id=>dl(id,[["Données",'Jamais reçues']]));}

 if(b1?.hasValue){const v=b1.value;dl('systemState',[["Fraîcheur PC",`${freshness(b1)} · ${availability(b1)}`],["État système",systemStatus(v.systemStatus)],["System flags",hex(v.systemFlags)],["Fault flags",hex(v.faultFlags)],["Warning flags",hex(v.warningFlags)],["Uptime",fmt(v.uptimeSeconds,' s')],["Cause reset",reset(v.lastResetCause)],["Température interne",fmt((v.internalTemperatureDeciCelsius/10).toFixed(1),' °C')],["CPU",fmt(v.cpuLoadPercent,' %')],["Mémoire",fmt(v.memoryUsagePercent,' %')],["Stockage",storage(v.storageStatus)],["Occupation stockage",fmt(v.storageUsagePercent,' %')],["Acquisition",acquisition(v.acquisitionState)],["Campagne active",fmt(v.activeCampaignId)],["Code erreur",fmt(v.errorCode)],["Code warning",fmt(v.warningCode)]]);}else dl('systemState',[["Données",'Jamais reçues']]);
 if(b2?.hasValue){const v=b2.value;dl('timeState',[["Fraîcheur PC",`${freshness(b2)} · ${availability(b2)}`],["Time status",fmt(v.timeStatus)],["Time flags",hex(v.timeFlags)],["Heure TR2",dateSeconds(v.currentTimeSeconds)],["Dernière synchro",dateSeconds(v.lastSyncTimeSeconds)],["Depuis synchro",fmt(v.timeSinceSyncSeconds,' s')],["Temps préparé",dateSeconds(v.preparedTimeSeconds)],["Prepared status",fmt(v.preparedTimeStatus)],["Précision",fmt(v.timeAccuracyMilliseconds,' ms')],["Dérive",fmt(v.driftPpm,' ppm')],["Source synchro",fmt(v.syncSource)]]);}else dl('timeState',[["Données",'Jamais reçues']]);

 if(b7?.hasValue){const v=b7.value;dl('diagnosticState',[["Version structure",fmt(v.diagnosticStructureVersion)],["Santé",health(v.systemHealthStatus)],["Fault flags",hex(v.systemFaultFlags)],["Dernier défaut",fmt(v.lastFaultCode)],["Timestamp défaut",dateSeconds(v.lastFaultTimestampSeconds)],["Autotest",selftest(v.selftestStatus)],["Code autotest",fmt(v.selftestResultCode)],["Détail autotest",fmt(v.selftestDetail)],["Uptime",fmt(v.uptimeSeconds,' s')],["Cause reset",reset(v.resetCause)],["Température interne",fmt((v.internalTemperatureDeciCelsius/10).toFixed(1),' °C')],["Tension alimentation",fmt(v.supplyVoltageMillivolts,' mV')]]);dl('diagnosticContext',[["Fraîcheur PC",freshness(b7)],["Disponibilité",availability(b7)],["Reçu à",b7.receivedAt?new Date(b7.receivedAt).toLocaleString('fr-FR'):'—']]);}else {dl('diagnosticState',[["Données",'Jamais reçues']]);dl('diagnosticContext',[["Fraîcheur PC",freshness(b7)]]);}
}

function showView(name){document.querySelectorAll('.device-view').forEach(x=>x.classList.add('hidden'));document.getElementById(`${name}View`).classList.remove('hidden');document.querySelectorAll('[data-view]').forEach(x=>x.classList.toggle('active',x.dataset.view===name));}
document.querySelectorAll('[data-view]').forEach(x=>x.addEventListener('click',()=>showView(x.dataset.view)));

async function refresh(){
 if(!Number.isSafeInteger(deviceId)||deviceId<0||deviceId>0xFFFFFFFF){apiState.textContent='✕ device_id invalide';apiState.className='bad';errorBanner.textContent='Le paramètre deviceId est absent ou invalide.';errorBanner.classList.remove('hidden');return;}
 try{const response=await fetch(`/api/v1/devices/${deviceId}`,{headers:{Accept:'application/json'},cache:'no-store'});if(response.status===404)throw new Error('device_id inconnu');if(!response.ok)throw new Error(`HTTP ${response.status}`);const payload=await response.json();lastPayload=payload;apiState.textContent='✓ Connectée';apiState.className='ok';observedAt.textContent=new Date(payload.observedAt).toLocaleString('fr-FR');errorBanner.classList.add('hidden');render(payload);}catch(error){apiState.textContent='✕ Mise à jour impossible';apiState.className='bad';errorBanner.textContent=`API Web indisponible : ${error.message}. Les dernières valeurs affichées sont conservées.`;errorBanner.classList.remove('hidden');if(lastPayload)render(lastPayload);}finally{setTimeout(refresh,2000);}}
refresh();
