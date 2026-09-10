(()=>{
  const params=new URLSearchParams(location.search);
  const raw=params.get('deviceId');
  const commandDeviceId=raw!==null&&/^\d+$/.test(raw)?Number(raw):NaN;
  const feedback=document.getElementById('commandFeedback');
  const commandButtons=[...document.querySelectorAll('[data-b5-command],#ackFaultButton,#ackAllButton,#softwareResetButton')];
  const labels={ApplyConfig:'appliquer la configuration préparée',SyncTime:"synchroniser l'heure préparée",StartAcquisition:"démarrer l'acquisition",StopAcquisition:"arrêter l'acquisition",Selftest:"lancer l'autotest standard",RefreshIndicators:'rafraîchir les indicateurs',EnterMaintenance:'entrer en maintenance',ExitMaintenance:'sortir de maintenance'};

  function requestIdentity(){
    const token=globalThis.crypto?.randomUUID?.() ?? `${Date.now()}-${Math.random().toString(16).slice(2)}`;
    return `web-${token}`;
  }

  function setBusy(value){commandButtons.forEach(button=>button.disabled=value);}
  function setFeedback(message,kind='neutral'){feedback.textContent=message;feedback.className=`command-feedback ${kind}`;}

  async function submit(body,description){
    if(!Number.isSafeInteger(commandDeviceId)||commandDeviceId<0||commandDeviceId>0xFFFFFFFF){setFeedback('device_id invalide : aucune commande envoyée.','bad');return;}
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
        return;
      }
      const code=result?.code??`HTTP ${response.status}`;
      const message=result?.message??'La demande a été rejetée par le serveur.';
      setFeedback(`${code} — ${message}`,'bad');
    }catch{
      setFeedback("Résultat réseau inconnu après émission. Aucun retry automatique n'est effectué ; ne pas conclure à un échec TR2.",'warn');
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
})();
