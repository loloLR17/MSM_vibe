"use strict";
(function () {
  if (typeof document === "undefined") return;
  const root = document.getElementById("liveHistory"); if (!root) return;
  const T = window.LiveTrends, $ = id => document.getElementById(id);
  const isDynamic=()=>$("historyChannel")?.value === "ac1";
  let selected, owner, tab, selectionKey, page, generation = 0, controller, lastLoad = 0;
  function text(tag, content, parent = root) { const n = document.createElement(tag); n.textContent = content; parent.append(n); return n; }
  function svgNode(tag, attrs, parent) {
    const n = document.createElementNS("http://www.w3.org/2000/svg", tag);
    for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, String(v));
    parent.append(n); return n;
  }
  function graph(container, definitions) {
    container.replaceChildren(); if (!page) return;
    const groups = definitions.map(d => ({...d, groups: T.series(page.entries, d.field, owner)}));
    const pts = groups.flatMap(d => d.groups.flat()), times = page.entries.map(e => Date.parse(e.observation.receivedAt));
    if (!pts.length) { text("p", "Aucun point valide/frais du canal sélectionné dans ce lot pour cette identité. Les HELD et ruptures restent listés.", container); return; }
    const xmin = Math.min(...times), xmax = Math.max(...times);
    const variation = $("historyScale")?.value === "variation", domain = T.domain(pts.map(p=>p.value),variation);
    const sx = x => 66 + (x-xmin)/(xmax-xmin || 1)*760, sy = y => 208-(y-domain.min)/(domain.max-domain.min)*175;
    const svg = svgNode("svg", {viewBox:"0 0 880 265",role:"img","aria-label":`${isDynamic()?"Accélération dynamique AC1 expérimentale":"Accélération absolue B3"} en mg, réception PC UTC ; domaine ${domain.min} à ${domain.max} mg${variation ? ", détail de variation, origine non nulle possible" : ", origine zéro"}`,"data-real-points":pts.length,"data-y-min":domain.min,"data-y-max":domain.max},container);
    for(const y of domain.ticks) {
      const pos=sy(y);svgNode("path",{d:`M66 ${pos}H826`,stroke:"#e1e9ed","stroke-width":1},svg);
      svgNode("text",{x:54,y:pos+4,"text-anchor":"end","font-size":11,fill:"#526777"},svg).textContent=y;
    }
    svgNode("text",{x:66,y:17,"font-size":11,fill:"#526777"},svg).textContent=variation ? "mg · DÉTAIL VARIATION / ORIGINE NON NULLE POSSIBLE" : (isDynamic()?"mg · DYNAMIQUE AC1 / HP 2 Hz / ORIGINE ZÉRO":"mg · ABSOLU B3 / DC CONSERVÉ / ORIGINE ZÉRO");
    for (const [x,time] of [[66,xmin],[446,(xmin+xmax)/2],[826,xmax]]) {
      const n=svgNode("text",{x,y:235,"text-anchor":x===826 ? "end" : x===446 ? "middle" : "start","font-size":11,fill:"#526777"},svg);n.textContent=new Date(time).toISOString().slice(11,23);
    }
    svgNode("text",{x:66,y:257,"font-size":10,fill:"#526777"},svg).textContent="Réception PC UTC · "+new Date(xmin).toISOString().slice(0,10)+" · pas l’UTC de fenêtre";
    for (const d of groups) {
      for (const group of d.groups) {
        // Separate SVG paths per uninterrupted run; no resampling or bridging gaps.
        if (group.length > 1) svgNode("polyline", {points: group.map(p => `${sx(p.time)},${sy(p.value)}`).join(" "), stroke: d.color, fill: "none", "stroke-width": 1.8}, svg);
        for (const p of group) {
          const circle = svgNode("circle", {cx: sx(p.time), cy: sy(p.value), r: 2, fill: p.repeated ? "white" : d.color, stroke: d.color, "data-observation-id": p.observation.observationId}, svg);
          const title = svgNode("title", {}, circle); title.textContent = `${d.name} ${p.value} mg · ${p.observation.receivedAt} · ${p.observation.continuity}`;
        }
      }
      text("span", `${d.name} · mg   `, container).style.color = d.color;
    }
    for (const e of page.entries.filter(e => !(isDynamic()?T.ac1Plottable:T.plottable)(e.observation, owner))) {
      const o = e.observation, mark = svgNode("path", {d: `M${sx(Date.parse(o.receivedAt))} 204v8`, stroke: "#b54708", "data-break": o.continuity}, svg);
      svgNode("title", {}, mark).textContent = o.receivedAt + " · " + T.quality(o, owner);
    }
  }
  function render() {
    $("historyError").textContent = "";
    $("historyCsv").disabled = !page?.entries.length;
    $("historyOlder").disabled = !page?.nextBefore;
    $("historyCoverage").textContent = page ? page.coverage + ` ${page.entries.length} observations affichées, ${page.scannedRows} lignes examinées. Historique non courant. UTC fenêtre/run/epoch/config inconnus si absents.` : "";
    const prefix=isDynamic()?"ac1.":"";
    $("historyChannelTitle").textContent=isDynamic()?"Vibrations dynamiques · AC1 expérimental":"Accélération absolue · B3 officiel";
    let globals = [{field: "rmsGlobalMg", name: "RMS global", color: "#006dae"}, {field: "peakGlobalMg", name: "Crête globale", color: "#b34400"}];
    globals=globals.map(d=>({...d,field:prefix+d.field,name:d.name+(isDynamic()?" dynamique AC1":" absolu B3")})).filter(d => $(d.field.endsWith("rmsGlobalMg") ? "historyRms" : "historyPeak").checked);
    const actual=T.series(page?.entries ?? [],prefix+"rmsGlobalMg",owner).flat().map(p=>p.value);
    $("historyRange").textContent=actual.length ? `RMS observé : ${Math.min(...actual)}–${Math.max(...actual)} mg · variation ${Math.max(...actual)-Math.min(...actual)} mg` : "";
    graph($("historyGlobal"), globals);
    const mode = $("historyMetric").value;
    graph($("historyAxes"), ["X", "Y", "Z"].filter(a => $("history" + a).checked).map((a) => ({field: prefix + mode + a + "Mg", name: mode.toUpperCase() + " " + a, color: {X:"#006dae",Y:"#a12b7c",Z:"#287327"}[a]})));
    $("historyAxisControls").hidden = tab !== "vibration"; $("historyAxes").hidden = tab !== "vibration";
    const list = $("historyRows"); list.replaceChildren();
    for (const e of page?.entries ?? []) {
      const o = e.observation, v = o.snapshot?.value, tr = document.createElement("tr");
      for (const s of [e.rowId, o.receivedAt, o.deviceId?.value ?? "Inconnue", T.quality(o, owner), o.continuity + " / " + o.segmentId,
        v ? `${v.rmsGlobalMg} / ${v.peakGlobalMg} mg · séq ${v.calculationSequence} · sévérité ${v.severityGlobal}` : "Aucune mesure", o.source]) text("td", s, tr);
      list.append(tr);
    }
  }
  function period() {
    if ($("historyPeriod").value === "custom") {
      const from = new Date($("historyFrom").value), to = new Date($("historyTo").value);
      if (!Number.isFinite(+from) || !Number.isFinite(+to) || +to <= +from || to - from > 7*86400000) throw new Error("Plage personnalisée : début < fin et durée ≤7 jours requis.");
      return {from: from.toISOString(), to: to.toISOString()};
    }
    return T.periodRange(selected.receivedAt, Number($("historyPeriod").value));
  }
  async function load(before) {
    if (!selected || root.hidden) return;
    const g = ++generation; controller?.abort(); controller = new AbortController();
    $("historyError").textContent = "Lecture du lot archivé…";
    try {
      const p = period(), q = new URLSearchParams({bus:selected.busId, unit:selected.modbusAddress, ...p, limit:500});
      if (before) q.set("before", before);
      const response = await fetch("/api/v2/live/history?" + q, {cache:"no-store", signal:controller.signal});
      if (!response.ok) { const e = await response.json(); throw new Error(e.error ?? "Historique indisponible"); }
      const result = await response.json(); if (g !== generation) return;
      page = {...result, scope: `${selected.busId}/${selected.modbusAddress} · identité ${owner ?? "inconnue (endpoint)"} · ${p.from} → ${p.to} · lot ${before ?? "récent"}`};
      lastLoad = Date.now(); render();
    } catch (e) { if (g === generation && e.name !== "AbortError") { page = null; render(); $("historyError").textContent = e.message + " — aucune courbe conservée comme actuelle"; } }
  }
  $("historyPeriod").addEventListener("change", () => { $("historyCustom").hidden = $("historyPeriod").value !== "custom"; load(); });
  $("historyApply").addEventListener("click", () => load());
  $("historyOlder").addEventListener("click", () => load(page?.nextBefore));
  $("historyRecent").addEventListener("click", () => load());
  for (const name of ["historyMetric", "historyX", "historyY", "historyZ", "historyRms", "historyPeak", "historyScale", "historyChannel"]) $(name).addEventListener("change", render);
  $("historyCsv").addEventListener("click", () => {
    if (!page) return; const url = URL.createObjectURL(new Blob(["\ufeff", T.csv(page.entries, page.scope)], {type:"text/csv;charset=utf-8"}));
    const a = document.createElement("a"); a.href = url; a.download = "tr2-observations-lot.csv"; a.click(); setTimeout(() => URL.revokeObjectURL(url), 1000);
  });
  window.showLiveHistory = function (row, nextTab, requestedOwner) {
    const visible = row && ["summary", "vibration"].includes(nextTab);
    const tabChanged = tab !== nextTab; root.hidden = !visible; tab = nextTab;
    if (!visible) { generation++; controller?.abort(); page = null; selectionKey = null; render(); return; }
    const o = row.observation, key = JSON.stringify([o.busId, o.modbusAddress, requestedOwner ?? o.deviceId?.value]);
    const changed = key !== selectionKey, newer = selected?.observationId !== o.observationId;
    selected = o; owner = requestedOwner ?? o.deviceId?.value; selectionKey = key;
    $("historyAnchor").textContent = "Fin des périodes prédéfinies : dernière réception PC observée " + o.receivedAt + ". Plage personnalisée saisie en heure locale du PC ; axes/export en UTC de réception PC. Pas d’affectation équipement/local disponible.";
    if (changed) { page=null; render(); load(); } else if (newer && Date.now()-lastLoad >= 5000) load(); else if (page && tabChanged) render();
  };
})();
