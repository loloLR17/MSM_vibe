(()=>{
  const params=new URLSearchParams(location.search);
  const raw=params.get('deviceId');
  const commandDeviceId=raw!==null&&/^\d+$/.test(raw)?Number(raw):NaN;
  const feedback=document.getElementById('commandFeedback');
  const transactionState=document.getElementById('commandTransactionState');
  const historyList=document.getElementById('commandHistoryList');
  const commandButtons=[...document.querySelectorAll('[data-b5-command],#ackFaultButton,#ackAllButton,#softwareResetButton')];
  const labels={ApplyConfig:'appliquer la configuration préparée',SyncTime:"synchroniser l'heure préparée",StartAcquisition:"démarrer l'acquisition",StopAcquisition:"arrêter l'acquisition",Selftest:"lancer l'autotest standard",RefreshIndicators:'rafraîchir les indicateurs',EnterMaintenance:'entrer en maintenance',ExitMaintenance:'sortir de maintenance'};
  const stateLabels={Prepared:'Préparée',Submitted:'Soumise',Ambiguous:'Ambiguë',TerminalEvidenceObserved:'Preuve terminale observée'};
  let busy=false;
  let transactionBlocked=true;

  function requestIdentity(){
    const token=globalThis.crypto?.randomUUID?.() ?? `${Date.now()}-${Math.random().toString(16).slice(2)}`;
    return `web-${token}`;
  }

  function applyButtonState(){commandButtons.forEach(button=>button.disabled=busy||transactionBlocked);}
  function setBusy(value){busy=value;applyButtonState();}
  function setFeedback(message,kind='neutral'){feedback.textContent=message;feedback.className=`command-feedback ${kind}`;}
  function setTransactionState(message,kind='neutral'){transactionState.textContent=message;transactionState.className=`command-feedback ${kind}`;}

  function renderHistory(transactions){
    historyList.replaceChildren();
    if(transactions.length===0){
      const dt=document.createElement('dt');dt.textContent='Historique';
      const dd=document.createElement('dd');dd.textContent='Aucune transaction B5 enregistrée.';
      historyList.append(dt,dd);
      return;
    }
    for(const item of transactions){
      const dt=document.createElement('dt');
      dt.textContent=`Transaction ${item.transactionId}`;
      const dd=document.createElement('dd');
      const observed=item.observedAt?new Date(item.observedAt).toLocaleString('fr-FR'):'—';
      dd.textContent=`${stateLabels[item.state]??item.state} · ${item.requestIdentity} · ${observed}`;
      historyList.append(dt,dd);
    }
  }

  async function refreshTransactionState(){
    if(!Number.isSafeInteger(commandDeviceId)||commandDeviceId<0||commandDeviceId>0xFFFFFFFF){
      transactionBlocked=true;applyButtonState();setTransactionState('device_id invalide : état transactionnel indisponible.','bad');return;
    }
    try{
      const response=await fetch(`/api/v1/devices/${commandDeviceId}/commands`,{headers:{Accept:'application/json'},cache:'no-store'});
      if(!response.ok)throw new Error(`HTTP ${response.status}`);
      const result=await response.json();
      const transactions=Array.isArray(result?.transactions)?result.transactions:[];
      renderHistory(transactions);
      const latest=transactions[0]??null;
      transactionBlocked=latest!==null&&['Prepared','Submitted','Ambiguous'].includes(latest.state);
      if(latest===null){
        setTransactionState('Aucune transaction active connue. Les commandes B5 sont disponibles.','ok');
      }else if(latest.state==='Ambiguous'){
        setTransactionState(`Transaction ${latest.transactionId} ambiguë : nouvelles commandes B5 bloquées. Aucun Retry / Ignore / Force.`, 'warn');
      }else if(latest.state==='Prepared'||latest.state==='Submitted'){
        setTransactionState(`Transaction ${latest.transactionId} ${stateLabels[latest.state].toLowerCase()} : attendre une preuve terminale avant une nouvelle commande.`, 'warn');
      }else{
        setTransactionState(`Transaction ${latest.transactionId} : preuve terminale observée. Cela ne signifie pas automatiquement succès métier.`, 'ok');
      }
      applyButtonState();
    }catch{
      transactionBlocked=true;
      applyButtonState();
      setTransactionState("État transactionnel indisponible : commandes B5 bloquées par prudence. Ce défaut Web n'est pas un défaut TR2.",'bad');
    }
  }

  async function submit(body,description){
    if(!Number.isSafeInteger(commandDeviceId)||commandDeviceId<0||commandDeviceId>0xFFFFFFFF){setFeedback('device_id invalide : aucune commande envoyée.','bad');return;}
    if(transactionBlocked){setFeedback('Commande non envoyée : une transaction B5 non terminale est connue ou son état est indisponible.','warn');return;}
    const payload={requestIdentity:requestIdentity(),...body};
    setBusy(true);
    setFeedback(`Envoi de la demande : ${description}…`);
    try{
      const response=await fetch(`/api/v1/devices/${commandDeviceId}/commands`,{
        method:'POST',
        headers:{'Content-Type':'application/json',Accept:'application/json'},
        body:JSON.stringify(payload)
      });
      let result=null;
      try{result=await response.json();}catch{}
      if(response.status===202){
        setFeedback(`Demande mise en file — transaction ${result?.transactionId??'—'}, work ${result?.workId??'—'}. Cela ne prouve pas le succès côté TR2.`,'ok');
        await refreshTransactionState();
        return;
      }
      const code=result?.code??`HTTP ${response.status}`;
      const message=result?.message??'La demande a été rejetée par le serveur.';
      setFeedback(`${code} — ${message}`,'bad');
      await refreshTransactionState();
    }catch{
      transactionBlocked=true;
      setFeedback("Résultat réseau inconnu après émission. Aucun retry automatique n'est effectué ; ne pas conclure à un échec TR2.",'warn');
      setTransactionState("Résultat transactionnel à requalifier : commandes B5 bloquées jusqu'à relecture du journal.",'warn');
    }finally{
      setBusy(false);
    }
  }

  document.querySelectorAll('[data-b5-command]').forEach(button=>button.addEventListener('click',()=>{
    const command=button.dataset.b5Command;
    const description=labels[command]??command;
    if(!confirm(`Confirmer : ${description} ?`))return;
    void submit({command},description);
  }));

  document.getElementById('ackFaultButton')?.addEventListener('click',()=>{
    const input=document.getElementById('faultCode');
    const value=Number(input.value);
    if(!Number.isInteger(value)||value<0||value>65535){setFeedback('Saisir un code défaut entier compris entre 0 et 65535.','bad');return;}
    if(!confirm(`Confirmer l'acquittement du défaut ${value} ?`))return;
    void submit({command:'AcknowledgeFault',faultCode:value,acknowledgeAll:false},`acquitter le défaut ${value}`);
  });

  document.getElementById('ackAllButton')?.addEventListener('click',()=>{
    if(!confirm('Confirmer l’acquittement GLOBAL de tous les défauts acquittables ?'))return;
    void submit({command:'AcknowledgeFault',acknowledgeAll:true},'acquitter globalement les défauts acquittables');
  });

  document.getElementById('softwareResetButton')?.addEventListener('click',()=>{
    if(!confirm('CONFIRMATION REQUISE : demander un reset logiciel contrôlé du TR2 ?'))return;
    void submit({command:'SoftwareReset',confirmProtectedCommand:true},'reset logiciel contrôlé');
  });

  applyButtonState();
  void refreshTransactionState();
  setInterval(()=>void refreshTransactionState(),2000);
})();
