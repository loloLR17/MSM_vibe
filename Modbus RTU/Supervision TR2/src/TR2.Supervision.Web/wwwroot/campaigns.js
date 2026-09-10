(()=>{
  const params=new URLSearchParams(location.search);
  const raw=params.get('deviceId');
  const campaignDeviceId=raw!==null&&/^\d+$/.test(raw)?Number(raw):NaN;
  const input=document.getElementById('campaignIndex');
  const button=document.getElementById('campaignSelectionButton');
  const feedback=document.getElementById('campaignSelectionFeedback');

  function setFeedback(message,kind='neutral'){
    feedback.textContent=message;
    feedback.className=`command-feedback ${kind}`;
  }

  button?.addEventListener('click',async()=>{
    if(!Number.isSafeInteger(campaignDeviceId)||campaignDeviceId<0||campaignDeviceId>0xFFFFFFFF){
      setFeedback('device_id invalide : aucune sélection envoyée.','bad');
      return;
    }

    const campaignIndex=Number(input.value);
    if(!Number.isInteger(campaignIndex)||campaignIndex<0||campaignIndex>65535){
      setFeedback('Saisir un index entier compris entre 0 et 65535.','bad');
      return;
    }

    if(!confirm(`Confirmer la sélection de l'index B6 ${campaignIndex} ?`))return;

    button.disabled=true;
    setFeedback(`Mise en file demandée pour l'index ${campaignIndex}…`);
    try{
      const response=await fetch(`/api/v1/devices/${campaignDeviceId}/campaign-selection`,{
        method:'POST',
        headers:{'Content-Type':'application/json',Accept:'application/json'},
        body:JSON.stringify({campaignIndex})
      });
      let result=null;
      try{result=await response.json();}catch{}

      if(response.status===202){
        setFeedback(`Sélection B6 mise en file — index ${result?.campaignIndex??campaignIndex}, work ${result?.workId??'—'}. Attendre l'observation B6 ultérieure pour conclure.`,'ok');
        return;
      }

      const code=result?.code??`HTTP ${response.status}`;
      const message=result?.message??'La demande de sélection a été rejetée par le serveur.';
      setFeedback(`${code} — ${message}`,'bad');
    }catch{
      setFeedback("Résultat réseau inconnu après émission. Aucun retry automatique n'est effectué ; observer B6 avant toute nouvelle sélection.",'warn');
    }finally{
      button.disabled=false;
    }
  });
})();
