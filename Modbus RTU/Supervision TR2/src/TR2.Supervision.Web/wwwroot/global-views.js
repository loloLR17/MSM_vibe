(()=>{
  const page=document.body.dataset.globalView;
  const rows=document.getElementById('globalRows');
  const apiState=document.getElementById('apiState');
  const observedAt=document.getElementById('observedAt');
  const errorBanner=document.getElementById('webError');
  const pcClock=document.getElementById('pcClock');
  const esc=value=>String(value??'—').replace(/[&<>"']/g,ch=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[ch]));
  const tick=()=>pcClock.textContent=new Date().toLocaleTimeString('fr-FR',{hour12:false});
  const freshness=o=>{if(!o||!o.hasValue)return 'Jamais reçues';const labels={Fresh:'Fraîches',Aging:'Vieillissantes',Stale:'Périmées',Unavailable:'Indisponibles',NeverReceived:'Jamais reçues'};return labels[o.freshness]||o.freshness;};
  const point=d=>`${d.busId} / adresse ${d.modbusAddress}`;
  const deviceLink=d=>d.deviceId==null?'—':`<a class="endpoint endpoint-link" href="/device.html?deviceId=${encodeURIComponent(d.deviceId)}">${esc(d.deviceId)}</a>`;
  const label=(value,map)=>Object.prototype.hasOwnProperty.call(map,value)?map[value]:`Valeur ${value}`;
  tick();setInterval(tick,1000);

  function renderVibrations(devices){
    const body=devices.filter(d=>d.deviceId!=null).map(d=>{
      const b3=d.telemetry?.vibrationState;
      if(!b3?.hasValue||!b3.value)return `<tr><td>${esc(point(d))}</td><td>${deviceLink(d)}</td><td colspan="4">—</td><td>${esc(freshness(b3))}</td></tr>`;
      const v=b3.value;
      return `<tr><td>${esc(point(d))}</td><td>${deviceLink(d)}</td><td>${esc(v.rmsGlobalMg)} mg</td><td>${esc(v.peakGlobalMg)} mg</td><td>${esc(label(v.severityGlobal,{0:'Non applicable',1:'Normal',2:'Information',3:'Avertissement',4:'Alarme',5:'Critique'}))}</td><td>${esc(label(v.dominantAxis,{0:'Non déterminé',1:'X',2:'Y',3:'Z',4:'Ex aequo / indéterminé'}))}</td><td>${esc(freshness(b3))}${b3.isAvailable?'':' · dernière valeur connue'}</td></tr>`;
    }).join('');
    rows.innerHTML=body||'<tr><td colspan="7" class="empty">Aucun TR2 identifié.</td></tr>';
  }

  async function renderAttention(devices){
    const items=[];
    for(const d of devices){
      const id=d.deviceId;
      if(d.sessionState!=='Compatible')items.push(['Communication PC',point(d),id,'Session non compatible',d.sessionState]);
      const b1=d.telemetry?.systemState;
      if(b1?.hasValue&&b1.value&&(b1.value.faultFlags!==0||b1.value.warningFlags!==0||b1.value.errorCode!==0||b1.value.warningCode!==0))items.push(['État / défaut TR2',point(d),id,'Indication B1 présente',`faultFlags=${b1.value.faultFlags} · warningFlags=${b1.value.warningFlags} · errorCode=${b1.value.errorCode} · warningCode=${b1.value.warningCode}`]);
      const b3=d.telemetry?.vibrationState;
      if(b3?.hasValue&&b3.value&&(b3.value.alarmFlags!==0||b3.value.alarmLatched!==0))items.push(['Vibration B3',point(d),id,'Indication vibration B3 présente',`alarmFlags=${b3.value.alarmFlags} · alarmLatched=${b3.value.alarmLatched}`]);
    }
    const identified=devices.filter(d=>d.deviceId!=null);
    await Promise.all(identified.map(async d=>{
      try{
        const response=await fetch(`/api/v1/devices/${d.deviceId}/commands`,{headers:{Accept:'application/json'},cache:'no-store'});
        if(!response.ok)throw new Error(`HTTP ${response.status}`);
        const payload=await response.json();
        const latest=Array.isArray(payload.transactions)?payload.transactions[0]:null;
        if(latest&&['Prepared','Submitted','Ambiguous'].includes(latest.state))items.push(['Opération B5',point(d),d.deviceId,`Transaction ${latest.state}`,`transaction ${latest.transactionId} · ${latest.requestIdentity}`]);
      }catch(error){
        items.push(['Supervision PC',point(d),d.deviceId,'État transactionnel B5 indisponible',error instanceof Error?error.message:'Lecture impossible']);
      }
    }));
    rows.innerHTML=items.length?items.map(item=>`<tr><td>${esc(item[0])}</td><td>${esc(item[1])}</td><td>${item[2]==null?'—':`<a class="endpoint endpoint-link" href="/device.html?deviceId=${encodeURIComponent(item[2])}">${esc(item[2])}</a>`}</td><td>${esc(item[3])}</td><td>${esc(item[4])}</td></tr>`).join(''):'<tr><td colspan="5" class="empty">Aucune situation nécessitant une attention selon les sources actuellement disponibles.</td></tr>';
  }

  function renderCampaigns(devices){
    const body=devices.filter(d=>d.deviceId!=null).map(d=>{
      const b6=d.telemetry?.campaignInventoryState;
      if(!b6?.hasValue||!b6.value)return `<tr><td>${esc(point(d))}</td><td>${deviceLink(d)}</td><td colspan="7">— · ${esc(freshness(b6))}</td></tr>`;
      const v=b6.value,valid=v.selectedCampaignValid===1;
      const campaign=valid?`#${v.campaignId}`:'Aucune entrée valide exposée';
      const state=valid?label(v.campaignState,{0:'Vide',1:'En préparation',2:'En cours',3:'Terminée',4:'Erreur',5:'Partiellement corrompue'}):'—';
      const integrity=valid?label(v.dataIntegrityStatus,{0:'Inconnue',1:'OK',2:'Corrompue',3:'Partielle'}):'—';
      const duration=valid?`${v.durationSeconds} s`:'—';
      return `<tr><td>${esc(point(d))}</td><td>${deviceLink(d)}</td><td>${esc(v.totalCampaignCount)}</td><td>${esc(v.validCampaignCount)}</td><td>${esc(campaign)}</td><td>${esc(state)}</td><td>${esc(integrity)}</td><td>${esc(duration)}</td><td>${esc(v.storageUsedMb)} MB utilisés / ${esc(v.storageFreeMb)} MB libres<br><small class="muted">${esc(freshness(b6))}${b6.isAvailable?'':' · dernière valeur connue'}</small></td></tr>`;
    }).join('');
    rows.innerHTML=body||'<tr><td colspan="9" class="empty">Aucun TR2 identifié.</td></tr>';
  }

  async function refresh(){
    try{
      const response=await fetch('/api/v1/fleet',{headers:{Accept:'application/json'},cache:'no-store'});
      if(!response.ok)throw new Error(`HTTP ${response.status}`);
      const payload=await response.json();
      const devices=Array.isArray(payload.devices)?payload.devices:[];
      apiState.textContent='✓ Connectée';apiState.className='ok';
      observedAt.textContent=new Date(payload.observedAt).toLocaleString('fr-FR');
      errorBanner.classList.add('hidden');
      if(page==='vibrations')renderVibrations(devices);
      else if(page==='attention')await renderAttention(devices);
      else if(page==='campaigns')renderCampaigns(devices);
      else throw new Error('vue globale inconnue');
    }catch(error){
      apiState.textContent='✕ Mise à jour impossible';apiState.className='bad';
      errorBanner.textContent=`API Web indisponible : ${error.message}.`;
      errorBanner.classList.remove('hidden');
    }finally{setTimeout(refresh,2000);}
  }
  void refresh();
})();
