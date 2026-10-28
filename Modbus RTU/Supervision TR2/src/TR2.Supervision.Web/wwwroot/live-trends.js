"use strict";
(function () {
  const id = o => o.deviceId?.value;
  const value = o => o.snapshot?.value;
  function attention(row) {
    const o = row.observation, s = o.snapshot;
    return !!row.archiveError || ["CommunicationLost", "Rejected"].includes(o.continuity) ||
      !!(s?.valuesValid && s.fresh && !s.held && !s.acquisitionStopped && s.value.severityGlobal >= 3);
  }
  function filterFleet(rows, {bus = "", search = "", onlyAttention = false, sort = "name", descending = false, dynamic = false} = {}) {
    const selected = rows.filter(r => (!bus || r.observation.busId === bus) && (!onlyAttention || attention(r)) &&
      (`TR2 ${id(r.observation) ?? "non identifié"} ${r.observation.busId} ${r.observation.modbusAddress}`).toLowerCase().includes(search.toLowerCase()));
    function key(r) {
      const o = r.observation, v = value(o)?.severityGlobal != null && o.snapshot.valuesValid ? value(o) : value(r.lastKnown ?? {});
      const ac=o.dynamicAcceleration?.valuesValid ? o.dynamicAcceleration : r.lastKnown?.dynamicAcceleration?.valuesValid ? r.lastKnown.dynamicAcceleration : null;
      if(dynamic && (sort==="rms"||sort==="peak")) return sort==="rms" ? ac?.rmsGlobalMg : ac?.peakGlobalMg;
      return sort === "name" ? `TR2 ${id(o) ?? "?"} ${o.busId}/${o.modbusAddress}` : sort === "age" ? r.receptionAgeSeconds :
        sort === "rms" ? v?.rmsGlobalMg : sort === "peak" ? v?.peakGlobalMg : v?.severityGlobal;
    }
    return selected.sort((a, b) => {
      const x = key(a), y = key(b);
      if (x == null || y == null) return x == null ? (y == null ? 0 : 1) : -1;
      const c = typeof x === "string" ? x.localeCompare(y, "fr", {numeric: true}) : x - y;
      return (descending ? -c : c) || String(a.observation.observationId).localeCompare(String(b.observation.observationId));
    });
  }
  function quality(o, owner) {
    if (owner != null && String(id(o)) !== String(owner)) return "Identité différente ou inconnue — rupture";
    const s = o.snapshot;
    if (!s) return "Sans mesure / perte — rupture";
    if (s.held) return "HELD — dernière connue non courante";
    if (!s.valuesValid || !s.fresh) return "Invalide / périmée — rupture";
    return "B3 V/F lors de la réception — historique non courant";
  }
  function plottable(o, owner) {
    const s = o.snapshot;
    return (owner == null || String(id(o)) === String(owner)) && id(o) != null && s?.valuesValid && s.fresh && !s.held;
  }
  function connected(a, b, owner) {
    const seq = value(b)?.calculationSequence - value(a)?.calculationSequence;
    return plottable(a, owner) && plottable(b, owner) && a.segmentId === b.segmentId && id(a) === id(b) &&
      a.busId === b.busId && a.modbusAddress === b.modbusAddress && Date.parse(b.receivedAt) >= Date.parse(a.receivedAt) &&
      ((b.continuity === "Sequential" && seq === 1) || (b.continuity === "RepeatedSequence" && seq === 0));
  }
  function ac1Plottable(o,owner) {
    const d=o.dynamicAcceleration;
    return plottable(o,owner)&&d?.valuesValid&&d.fresh&&!d.held&&d.fullScaleProved;
  }
  function ac1Connected(a,b,owner) {
    const x=a.dynamicAcceleration,y=b.dynamicAcceleration;
    return ac1Plottable(a,owner)&&ac1Plottable(b,owner)&&a.segmentId===b.segmentId&&id(a)===id(b)&&a.busId===b.busId&&a.modbusAddress===b.modbusAddress&&
      Date.parse(b.receivedAt)>=Date.parse(a.receivedAt)&&!["Gap","RegressedSequence","CommunicationLost","Rejected"].includes(b.continuity)&&
      x.runId===y.runId&&x.configurationId===y.configurationId&&x.configurationGeneration===y.configurationGeneration&&x.fullScaleCode===y.fullScaleCode&&x.cutoffMillihertz===y.cutoffMillihertz&&
      (y.sequence-x.sequence===0||y.sequence-x.sequence===1);
  }
  function series(entries, field, owner) {
    const groups = []; let current = [], previous;
    const ac=field.startsWith("ac1."),valid=ac?ac1Plottable:plottable,join=ac?ac1Connected:connected;
    const get=o=>ac ? o.dynamicAcceleration?.[field.slice(4)] : value(o)?.[field];
    for (const {observation: o} of entries) {
      if (!valid(o, owner) || !Number.isFinite(get(o))) { current = []; previous = o; continue; }
      if (!previous || !join(previous, o, owner) || !current.length) { current = []; groups.push(current); }
      current.push({time: Date.parse(o.receivedAt), value: get(o), repeated: ac ? !!previous && previous.dynamicAcceleration?.sequence===o.dynamicAcceleration.sequence : o.continuity === "RepeatedSequence", observation: o}); previous = o;
    }
    return groups;
  }
  const fields = ["rmsGlobalMg", "peakGlobalMg", "rmsXMg", "rmsYMg", "rmsZMg", "peakXMg", "peakYMg", "peakZMg"];
  function cell(v) {
    let s = v == null ? "" : String(v);
    if (/^[=+@\-\t\r]/.test(s) && typeof v !== "number") s = "'" + s;
    return '"' + s.replaceAll('"', '""') + '"';
  }
  function csv(entries, context = "") {
    const acFields=["extension","method","unit","valuesValid","fresh","held","fullScaleCode","axesEnableMask","verifiedCtrl1Xl","fullScaleProved","samplingFrequencyHz","cutoffMillihertz","warmupSamples","runId","configurationId","configurationGeneration","sequence","valueAgeMilliseconds","dcXMicroG","dcYMicroG","dcZMicroG",...fields];
    const names = ["scope", "row_id", "observation_id", "received_pc", "device_id_observed", "bus", "unit", "segment", "continuity", "historical_non_current", "values_valid_at_receipt", "fresh_at_receipt", "held", "stopped_at_receipt", "sequence", "severity_b3", "alarm_flags_b3", "window_utc_unavailable_if_empty", "run_id", "epoch", "configuration_id", "source", ...acFields.map(f=>"ac1_"+f), ...fields];
    return [names, ...entries.map(e => {
      const o = e.observation, s = o.snapshot, v = value(o);
      return [context, e.rowId, o.observationId, o.receivedAt, id(o), o.busId, o.modbusAddress, o.segmentId, o.continuity, true,
        s?.valuesValid, s?.fresh, s?.held, s?.acquisitionStopped, v?.calculationSequence, v?.severityGlobal, v?.alarmFlags,
        s?.windowUtc, o.runId, o.epoch, o.configurationId, o.source, ...acFields.map(f=>o.dynamicAcceleration?.[f]), ...fields.map(f => v?.[f])];
    })].map(r => r.map(cell).join(",")).join("\r\n") + "\r\n";
  }
  function periodRange(receivedAt, hours) {
    const fraction = receivedAt.match(/\.(\d+)(?:Z|[+-]\d\d:\d\d)$/)?.[1] ?? "000";
    const from = new Date(Date.parse(receivedAt) - hours*3600000).toISOString().replace(/\.\d{3}Z$/, "." + fraction + "Z");
    return {from, to: receivedAt}; // Preserve .NET's sub-ms ticks: 7 days must not become 7 days + 1 ms.
  }
  // Presentation scale only. Values, CSV and B3 semantics are never transformed.
  function domain(values, variation = false) {
    const finite = values.filter(Number.isFinite);
    if (!finite.length) return {min:0,max:1,ticks:[0,0.25,0.5,0.75,1]};
    const low = Math.min(...finite), high = Math.max(...finite);
    const span = variation ? Math.max(1,high-low) : Math.max(1,high);
    const stepBase = span / 4, power = Math.pow(10,Math.floor(Math.log10(stepBase)));
    const step = [1,2,2.5,5,10].map(x=>x*power).find(x=>x>=stepBase);
    const min = variation ? Math.max(0,Math.floor((low-step*0.25)/step)*step) : 0;
    const max = Math.max(min+step,Math.ceil((high+step*0.15)/step)*step);
    const ticks=[];for(let y=min;y<=max+step/10;y+=step)ticks.push(Number(y.toPrecision(8)));
    return {min,max,ticks};
  }
  const api = {ac1Plottable, ac1Connected, domain, periodRange, attention, filterFleet, quality, plottable, connected, series, csv};
  if (typeof module !== "undefined") module.exports = api;
  if (typeof window !== "undefined") window.LiveTrends = api;
})();
