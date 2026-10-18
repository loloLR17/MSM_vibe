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
      row.isCurrent ? "Mesure valide et fraîche" : historical ? "Mesure historique, non courante" : "Mesure périmée ou invalide";
    const owner = measurement?.deviceId?.value;
    const retained = measurement && (measurement.observationId !== o.observationId || !row.isCurrent);
    const prefix = value ? (retained ? "Dernière connue / périmée" : "Courante") : "Aucune mesure";
    return {o, measurement, value, label, endpoint, state, quality, prefix,
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
    tab = button.dataset.liveTab; refresh();
  }));
  function text(parent, label, value) {
    const p = document.createElement("p"); p.textContent = label + " : " + value; parent.append(p);
  }
  function target(row) {
    const o = row.observation;
    return o.deviceId ? "deviceId=" + o.deviceId.value : "bus=" + encodeURIComponent(o.busId) + "&unit=" + o.modbusAddress;
  }
  let fleetRows = [];
  function table(devices) {
    fleetRows = devices;
    const controls = document.getElementById("liveFleetControls");
    if (controls) {
      const bus = document.getElementById("fleetBus");
      for (const name of new Set(devices.map(r => r.observation.busId)))
        if (![...bus.options].some(o => o.value === name)) { const option = document.createElement("option"); option.value = option.textContent = name; bus.append(option); }
      devices = window.LiveTrends.filterFleet(devices, {bus:bus.value, search:document.getElementById("fleetSearch").value,
        onlyAttention:document.getElementById("fleetAttention").checked, sort:document.getElementById("fleetSort").value, descending:document.getElementById("fleetDescending").checked});
    }
    fleet.replaceChildren();
    for (const row of devices) {
      const x = present(row), tr = document.createElement("tr"), first = document.createElement("td"), a = document.createElement("a");
      a.textContent = x.label; a.href = "/live-device.html?" + target(row); first.append(a); tr.append(first);
      for (const value of [x.prefix + (x.measurement ? " · " + x.owner + " · " + x.measurement.receivedAt : "") + " · " + x.rms, x.peak, x.severity, x.state + " · " + x.quality, x.o.receivedAt + " · " + x.age]) {
        const td = document.createElement("td"); td.textContent = value; tr.append(td);
      }
      fleet.append(tr);
    }
  }
  document.querySelectorAll("#liveFleetControls input, #liveFleetControls select").forEach(c => c.addEventListener("input", () => table(fleetRows)));
  function device(row) {
    window.showLiveHistory?.(row, tab, query.get("deviceId"));
    detail.replaceChildren();
    document.querySelectorAll("[data-live-tab]").forEach(b => b.classList.toggle("active", b.dataset.liveTab === tab));
    const heading = document.createElement("h2"); heading.textContent = tabs[tab]; detail.append(heading);
    const x = present(row), o = x.o, v = x.value, s = o.snapshot;
    document.getElementById("liveDeviceTitle").textContent = x.label;
    text(detail, "Profil", (o.identity?.profile ?? "Non identifié") + " · lecture seule");
    text(detail, "Provenance", o.source);
    text(detail, "Contexte", row.historical ? "Historique / relecture — aucune acquisition actuelle ni COM ouvert par cette vue" : "Réception du service physique ; cette page n'ouvre aucun COM");
    if (tab === "summary" || tab === "vibration") {
      text(detail, "Communication", x.state); text(detail, "Qualité", x.quality);
      text(detail, x.prefix + " RMS / crête globales", x.rms + " / " + x.peak);
      text(detail, "Sévérité B3 transmise", x.severity);
      text(detail, "Qualité V/F/HELD reçue", s ? `${s.valuesValid}/${s.fresh}/${s.held}` : "Indisponible");
      if (v) {
        text(detail, "Propriétaire et date de la mesure affichée", x.owner + " / " + x.measurement.receivedAt);
        text(detail, "RMS X / Y / Z (mg)", `${v.rmsXMg} / ${v.rmsYMg} / ${v.rmsZMg}`);
        text(detail, "Crêtes X / Y / Z (mg)", `${v.peakXMg} / ${v.peakYMg} / ${v.peakZMg}`);
        text(detail, "Séquence / âge STM32", `${v.calculationSequence} / ${v.valueAgeMilliseconds} ms`);
        text(detail, "Alarme flags B3 / dépassements global X Y Z", `${v.alarmFlags} / ${v.exceedGlobal} ${v.exceedX} ${v.exceedY} ${v.exceedZ}`);
      }
      text(detail, "Acquisition selon dernier B3", !s ? "Inconnue" : s.acquisitionStopped ? "Arrêtée lors de l'observation" : "Non arrêtée lors de l'observation ; état actuel non extrapolé");
      text(detail, "UTC de fenêtre", x.measurement?.snapshot?.windowUtc ?? "Indisponible ; heure PC distincte");
      text(detail, "Tendances", "Observations persistées ci-dessous ; réception PC distincte de l’UTC de fenêtre.");
    } else if (tab === "state") {
      text(detail, "B1 / B2 / B7", "Indisponibles dans le profil diagnostique ; aucun état déduit");
      text(detail, "Configuration de fenêtre / run / epoch", "Inconnus sur le fil LIVE ; aucune édition");
    } else if (tab === "commands") {
      text(detail, "Commandes", "Indisponibles dans cette fiche LIVE en lecture seule. Aucune action B5 ni acquittement.");
    } else if (tab === "campaigns") {
      text(detail, "Campagnes", "Inventaire non exposé par cette vue ; LIVE indépendant de la microSD. Aucun accès bulk.");
    } else {
      text(detail, "Identité actuellement observée", o.deviceId?.value ?? "Inconnue");
      text(detail, "Contexte de mesure retenue", x.owner + " ; identité actuelle non déduite");
      text(detail, "Continuité / segment PC", o.continuity + " / " + o.segmentId);
      text(detail, "Erreur de lecture", o.detail ?? "Aucune dans cette observation");
    }
    text(detail, "Dernière réception PC", o.receivedAt + " · " + x.age);
    text(detail, "Archivage", row.archiveState + (row.archiveError ? " — " + row.archiveError : ""));
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
      state.textContent = (data.devices.length ? "Observations LIVE · " : "Aucune observation LIVE · ") + data.coverage + (data.archiveError ? " Archive indisponible : " + data.archiveError : "");
      if (fleet) table(data.devices);
      if (detail) {
        const row = query.has("deviceId") ? selectDevice(data.devices, query.get("deviceId")) : data.devices.find(r => r.observation.busId === query.get("bus") && String(r.observation.modbusAddress) === query.get("unit"));
        if (row) device(row); else { window.showLiveHistory?.(null, tab); detail.replaceChildren(); text(detail, "Observation", "Capteur absent de la couverture disponible"); }
      }
    } catch (e) {
      if (fleet) fleet.replaceChildren(); if (detail) detail.replaceChildren(); window.showLiveHistory?.(null, tab);
      state.textContent = e.message + " — aucune valeur présentée comme courante";
    } finally { pending = false; }
  }
  refresh(); setInterval(refresh, 1000);
})();
