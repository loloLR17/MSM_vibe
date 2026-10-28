"use strict";
(function () {
  const severityNames = ["Non exploitable", "Normale", "Dégradée", "Avertissement", "Alarme"];
  function present(row) {
    const o = row.observation, current = o.snapshot;
    const measurement = current?.valuesValid ? o : row.lastKnown;
    const s = measurement?.snapshot, value = s?.value;
    const id = o.deviceId?.value;
    const endpoint = o.busId + " / adresse " + o.modbusAddress;
    const identityLabel = id == null ? "Non identifié · " + endpoint : "TR2 " + id + " · " + endpoint;
    const label = identityLabel + " · " + (o.identity?.profile ?? "profil inconnu") + (o.identity?.profile === "LiveDiagnostic" ? " · NON-PRODUCTION" : "");
    const historical = row.historical;
    const state = historical ? "Historique — communication actuelle non observée" :
      row.transportConnected === false ? "Communication perdue / profil refusé" : "Transport connecté";
    const quality = !current ? "Aucune mesure dans la dernière observation" : current.held ? "HELD — non courante" :
      row.isCurrent ? "Mesure valide et fraîche" : !current.valuesValid && current.acquisitionStopped ? "En attente de mesure · STOP" : historical ? "Mesure historique, non courante" : "Mesure périmée ou invalide";
    const owner = measurement?.deviceId?.value;
    const retained = measurement && (measurement.observationId !== o.observationId || !row.isCurrent);
    const prefix = value ? (retained ? "Dernière connue / périmée" : "Courante") : "Aucune mesure";
    const measurementAgeSeconds = measurement ? Math.max(0, (Date.now() - Date.parse(measurement.receivedAt))/1000) : null;
    const acquisition = historical ? "Historique" : !current ? "Inconnue" : current.acquisitionStopped ? (current.held ? "STOP · HELD" : "STOP") : row.isCurrent ? "RUNNING" : "Non arrêtée · qualité à vérifier";
    return {o, measurement, value, label, endpoint, state, quality, prefix, acquisition, measurementAgeSeconds,
      measurementAge: measurementAgeSeconds == null ? "Aucune mesure valide" : Math.floor(measurementAgeSeconds) + " s · réception de cette mesure",
      retained: !!retained,
      severity: value ? (severityNames[value.severityGlobal] ?? "Inconnue") + " (" + value.severityGlobal + ")" : "Indisponible",
      owner: owner == null ? "inconnu" : "TR2 " + owner,
      age: Math.floor(row.receptionAgeSeconds) + " s depuis réception PC",
      rms: value ? value.rmsGlobalMg + " mg" : "—", peak: value ? value.peakGlobalMg + " mg" : "—"};
  }
  function matchesDevice(row, requestedId) {
    const observed = row.observation.deviceId?.value;
    // The page may follow its previous owner on identity loss, but present() still labels it unknown.
    return String(observed ?? row.lastKnown?.deviceId?.value) === requestedId;
  }
  function selectDevice(devices, id) {
    return devices.find(r => String(r.observation.deviceId?.value) === id) ??
      devices.find(r => r.observation.deviceId == null && matchesDevice(r, id));
  }
  if (typeof module !== "undefined") module.exports = {present, matchesDevice, selectDevice};
  if (typeof document === "undefined") return;
  const fleet = document.getElementById("liveFleetRows"), detail = document.getElementById("liveDeviceContent");
  if (!fleet && !detail) return;
  let tab = "summary";
  const query = new URLSearchParams(location.search);
  const tabs = {summary: "Synthèse", vibration: "Vibrations", state: "État & configuration", commands: "Commandes", campaigns: "Campagnes", diagnostic: "Diagnostic"};
  if (Object.hasOwn(tabs, query.get("view"))) tab = query.get("view");
  document.querySelectorAll("[data-live-tab]").forEach(button => button.addEventListener("click", () => {
    tab = button.dataset.liveTab;
    const url = new URL(location.href); url.searchParams.set("view", tab); history.replaceState(null, "", url);
    if (selectedRow) device(selectedRow);
  }));
  function text(parent, label, value) {
    const p = document.createElement("p"); p.textContent = label + " : " + value; parent.append(p);
  }
  function target(row) {
    const o = row.observation;
    return o.deviceId ? "deviceId=" + o.deviceId.value : "bus=" + encodeURIComponent(o.busId) + "&unit=" + o.modbusAddress;
  }
  // Nodes survive polling: keyboard focus, expanded details and chart controls remain stable.
  function node(tag, cls, content, parent) {
    const n = document.createElement(tag); if (cls) n.className = cls;
    if (content != null) n.textContent = content; if (parent) parent.append(n); return n;
  }
  function put(n, content) { if (n.textContent !== String(content)) n.textContent = content; }
  function badge(parent, content, cls = "muted") { return node("span", "badge " + cls, content, parent); }
  let fleetRows = [], selectedRow, detailTab;
  const fleetNodes = new Map();
  function table(devices) {
    fleetRows = devices;
    const summary = document.getElementById("liveFleetSummary");
    if (summary) put(summary, `${devices.length} endpoint(s) observé(s) · ${devices.filter(r => r.isCurrent).length} mesure(s) courante(s) · ${devices.filter(r => window.LiveTrends.attention(r)).length} à examiner`);
    const controls = document.getElementById("liveFleetControls");
    if (controls) {
      const bus = document.getElementById("fleetBus");
      for (const name of new Set(devices.map(r => r.observation.busId)))
        if (![...bus.options].some(o => o.value === name)) { const option = document.createElement("option"); option.value = option.textContent = name; bus.append(option); }
      devices = window.LiveTrends.filterFleet(devices, {bus:bus.value, search:document.getElementById("fleetSearch").value,
        onlyAttention:document.getElementById("fleetAttention").checked, sort:document.getElementById("fleetSort").value, descending:document.getElementById("fleetDescending").checked});
    }
    const keep = new Set(); let position=0;
    for (const row of devices) {
      const x = present(row), key = x.o.busId + "/" + x.o.modbusAddress;
      keep.add(key); let n = fleetNodes.get(key);
      if (!n) {
        const tr = node("tr"), cells = Array.from({length:6}, () => node("td", "", null, tr));
        n = {tr, a:node("a", "", "", cells[0]), endpoint:node("small", "", "", cells[0]), place:node("small", "", "Équipement / emplacement non renseignés", cells[0]),
          rms:node("span", "numeric", "", cells[1]), rmsNote:node("small", "", "", cells[1]), peak:node("span", "numeric", "", cells[2]),peakNote:node("small", "", "", cells[2]),
          severity:badge(cells[3], ""), severityNote:node("small", "", "", cells[3]), transport:badge(cells[4], ""),acquisition:badge(cells[4], ""),quality:node("small", "", "", cells[4]),date:node("span", "", "", cells[5]),age:node("small", "", "", cells[5])};
        fleetNodes.set(key,n);
      }
      put(n.a,x.o.deviceId ? "TR2 · " + x.o.deviceId.value : "Capteur non identifié"); n.a.href = "/live-device.html?" + target(row);
      put(n.endpoint,x.endpoint); put(n.rms,x.rms); put(n.peak,x.peak);
      put(n.rmsNote,x.value ? (x.retained ? "Dernière connue · " + x.measurementAge : "Courante · B3 V/F") : "Aucune mesure valide");
      put(n.peakNote,x.measurement?.receivedAt ?? "—");
      put(n.severity,x.severity); n.severity.className = "badge " + (x.retained ? "muted" : x.value?.severityGlobal >= 3 ? "caution" : "muted");
      put(n.severityNote,x.retained ? "Sévérité de la dernière mesure" : "Sévérité transmise par B3");
      put(n.transport,row.historical ? "Historique" : row.transportConnected ? "Connecté" : "Communication perdue");
      n.transport.className = "badge " + (row.historical ? "muted" : row.transportConnected ? "good" : "danger");
      put(n.acquisition,x.acquisition); put(n.quality,x.quality); put(n.date,x.o.receivedAt); put(n.age,x.age);
      if(fleet.children[position]!==n.tr)fleet.insertBefore(n.tr,fleet.children[position] ?? null);position++;
    }
    for (const [key,n] of fleetNodes) if (!keep.has(key)) {n.tr.remove(); fleetNodes.delete(key);}
    let empty = document.getElementById("liveFleetEmpty");
    if (!devices.length && !empty) {empty=node("tr", "", null, fleet);empty.id="liveFleetEmpty";node("td","empty-state","Aucun capteur dans cette sélection.",empty).colSpan=6;}
    if (devices.length) empty?.remove();
  }
  document.querySelectorAll("#liveFleetControls input, #liveFleetControls select").forEach(c => c.addEventListener("input", () => table(fleetRows)));
  let fields = new Map();
  function field(key, tag, cls, parent) { const n=node(tag,cls,"",parent);fields.set(key,n);return n; }
  function fact(key, label, parent) { const n=node("div","fact",null,parent);node("span","",label,n);field(key,"span","",n); }
  function buildDetail() {
    detail.replaceChildren(); const technical=document.getElementById("liveTechnicalDetails") ?? detail; if(technical!==detail)technical.replaceChildren(); fields = new Map(); detailTab=tab;
    const strip=node("div","status-strip",null,detail);
    for (const k of ["transport","acquisition","quality"]) field(k,"span","badge",strip);
    if (["summary","vibration"].includes(tab)) {
      const kpis=node("div","live-kpis",null,detail);
      for (const [key,label,cls] of [["rms","RMS global · absolu","rms"],["peak","Crête globale · absolue","peak"],["severity","Sévérité B3","compact"],["measureLabel","Qualité de la mesure","compact"]]) {
        const a=node("article","live-kpi "+cls,null,kpis);node("span","kpi-label",label,a);field(key,"strong","kpi-value",a);field(key+"Note","span","kpi-note",a);
      }
      const axes=node("div","axis-values",null,detail);for(const a of ["X","Y","Z"]) {const v=node("span","",null,axes);node("span","",a+" · RMS / crête ",v);field("axis"+a,"strong","",v);}
      const note=node("aside","metrology-note",null,detail);node("strong","","Composante statique conservée",note);node("span","","RMS et crête incluent la pesanteur. La calibration de démarrage règle le temps FIFO, sans compensation de gravité. 1 g = 1 000 mg.",note);
      const d=node("details","technical",null,technical);node("summary","","Détails B3 et provenance de la mesure",d);const f=node("div","facts",null,d);
      for(const [k,l] of [["measureTime","Réception PC de la mesure affichée"],["sequence","Séquence / âge STM32"],["flags","V / F / HELD de la dernière observation"],["alarms","Flags alarmes / dépassements G X Y Z"],["utc","UTC de fenêtre"],["archive","Archivage"]]) fact(k,l,f);
    } else {
      const panel=node("section","live-panel",null,detail);node("div","eyebrow","LECTURE SEULE / LIVE DIAGNOSTIQUE",panel);node("h2","",tabs[tab],panel);
      const items = tab === "state" ? [["B1 · État système","Indisponible dans LiveDiagnostic ; aucun état déduit."],["B2 · Horloge / UTC","Indisponible dans LiveDiagnostic ; heure PC distincte."],["Configuration active B4 / run / epoch","Identités non exposées dans cette vue partielle. Aucune édition ni déduction depuis un segment PC."],["B7 · Diagnostic capteur","Indisponible dans LiveDiagnostic."]] : tab === "commands" ? [["START / STOP","Utiliser les lanceurs Windows qualifiés START.cmd et STOP.cmd. Attendre le terminal et l’effet observé ; environ 40 secondes dans les essais précédents."],["Autorité unique","Le service libère COM4 avec acquittement tokenisé ; le contrôleur prend ensuite le port en exclusivité. Aucun second maître forcé."],["Actions Web","Non activées. Intégration sécurisée à qualifier séparément ; aucune commande B5 ni acquittement depuis cette page."]] : tab === "campaigns" ? [["Supervision indépendante","Le LIVE ne dépend pas de la présence d’une microSD."],["Inventaire B6","Inventaire de campagnes indisponible dans cette vue LiveDiagnostic. Aucun téléchargement, bulk ou stockage fictif."]] : [["Transport","RS-485 via le service Windows. Cette page n’ouvre aucun port COM."],["B1 / B2 / B7","Indisponibles dans LiveDiagnostic ; aucun score de santé déduit."],["Horodatages","Réception PC connue ; UTC fenêtre / run / epoch / configuration indisponibles si absents du fil."]];
      const f=node("div","facts",null,panel);for(const [l,v] of items) {const n=node("div","fact",null,f);node("span","",l,n);node("span","",v,n);}
      if(tab === "diagnostic") for(const [k,l] of [["identity","Identité observée"],["continuity","Continuité / segment PC"],["readError","Erreur de lecture"],["archive","Archivage"]]) fact(k,l,f);
    }
    const d=node("details","technical",null,technical);node("summary","","Source et couverture du profil",d);for(const k of ["source","context","reception"])field(k,"p","",d);
  }
  function device(row) {
    selectedRow=row;window.showLiveHistory?.(row,tab,query.get("deviceId"));
    document.querySelectorAll("[data-live-tab]").forEach(b => {b.classList.toggle("active",b.dataset.liveTab===tab);b.setAttribute("aria-current",b.dataset.liveTab===tab ? "page" : "false");});
    if (detailTab!==tab || !detail.childNodes.length) buildDetail();
    const x=present(row),o=x.o,v=x.value,s=o.snapshot;
    put(document.getElementById("liveDeviceTitle"),o.deviceId ? "TR2 · "+o.deviceId.value : "Capteur non identifié");
    const subtitle=document.getElementById("liveDeviceSubtitle");if(subtitle)put(subtitle,x.endpoint+" · Équipement / emplacement non renseignés");
    const values={transport:x.state,acquisition:x.acquisition,quality:x.quality,rms:x.rms,peak:x.peak,
      rmsNote:x.value ? x.prefix : "Aucune mesure valide",peakNote:x.value ? x.measurementAge : "En attente d’une mesure valide",
      severity:x.severity,severityNote:x.retained ? "Dernière mesure connue · non courante" : "Transmise par le capteur",
      measureLabel:row.isCurrent ? "Courante" : x.value ? "Dernière connue" : "En attente",
      measureLabelNote:x.value ? x.measurementAge : "STOP · aucune valeur valide",
      measureTime:x.measurement?.receivedAt ?? "Indisponible",sequence:v ? v.calculationSequence+" / "+v.valueAgeMilliseconds+" ms" : "Indisponible",
      flags:s ? `${s.valuesValid} / ${s.fresh} / ${s.held}` : "Indisponible",alarms:v ? `${v.alarmFlags} / ${v.exceedGlobal} ${v.exceedX} ${v.exceedY} ${v.exceedZ}` : "Indisponible",
      utc:x.measurement?.snapshot?.windowUtc ?? "Indisponible dans LiveDiagnostic ; réception PC distincte",archive:row.archiveState+(row.archiveError ? " — "+row.archiveError : ""),
      identity:o.deviceId?.value ?? "Inconnue",continuity:o.continuity+" / "+o.segmentId,readError:o.detail ?? "Aucune dans cette observation",
      source:o.source,context:row.historical ? "Historique / relecture : aucune acquisition actuelle démontrée." : "Réponse du service physique ; cette page n’ouvre aucun COM.",
      reception:"Dernière réception PC : "+o.receivedAt+" · "+x.age};
    for(const a of ["X","Y","Z"]) values["axis"+a]=v ? `${v["rms"+a+"Mg"]} / ${v["peak"+a+"Mg"]} mg` : "—";
    for(const [k,n] of fields)put(n,values[k] ?? "Indisponible");
    fields.get("transport").className="badge "+(row.historical ? "muted" : row.transportConnected ? "good" : "danger");
    fields.get("acquisition").className="badge "+(row.isCurrent ? "good" : "muted");
    fields.get("quality").className="badge "+(row.isCurrent ? "good" : "muted");
  }
  // One refresh timer; navigation does not start another loop or retain a business cache.
  let pending = false;
  async function refresh() {
    if (pending) return; pending = true;
    const state = document.getElementById("liveOverviewState");
    try {
      const response = await fetch("/api/v2/live/overview", {cache: "no-store"});
      if (!response.ok) throw new Error("API LIVE indisponible");
      const data = await response.json();
      state.textContent = `${data.devices.length} endpoint(s) observé(s) · couverture bornée à 1 000 observations · lecture seule` + (data.archiveError ? " · Archive indisponible : " + data.archiveError : "");
      state.title=data.coverage;
      if (fleet) table(data.devices);
      if (detail) {
        const row = query.has("deviceId") ? selectDevice(data.devices, query.get("deviceId")) : data.devices.find(r => r.observation.busId === query.get("bus") && String(r.observation.modbusAddress) === query.get("unit"));
        if (row) device(row); else { window.showLiveHistory?.(null, tab); detail.replaceChildren(); text(detail, "Observation", "Capteur absent de la couverture disponible"); }
      }
    } catch (e) {
      if (fleet) {fleet.replaceChildren();fleetNodes.clear();} if (detail) {detail.replaceChildren();document.getElementById("liveTechnicalDetails")?.replaceChildren();detailTab=null;selectedRow=null;} window.showLiveHistory?.(null, tab);
      state.textContent = e.message + " — aucune valeur présentée comme courante";
    } finally { pending = false; }
  }
  refresh(); setInterval(refresh, 1000);
})();
