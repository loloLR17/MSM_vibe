"use strict";
const rows = document.getElementById("liveRows");
const error = document.getElementById("liveError");
function text(parent, label, value) { const p = document.createElement("p"); p.textContent = label + " : " + value; parent.append(p); }
async function refreshLive() {
  try {
    const response = await fetch("/api/v2/live", {cache: "no-store"});
    if (!response.ok) throw new Error(response.status === 404 ? "Source LIVE non configurée" : "API indisponible");
    const views = await response.json();
    rows.replaceChildren(); error.textContent = views.length ? "" : "Aucune observation LIVE";
    for (const view of views) {
      const o = view.observation, s = o.snapshot, v = s?.value;
      const card = document.createElement("section"); card.className="live-panel live-log-card";
      const title=document.createElement("h2");title.textContent="TR2 · "+(o.deviceId?.value ?? "non identifié");card.append(title);
      text(card, "Appareil / profil", (o.deviceId?.value ?? "inconnu") + " / " + (o.identity?.profile ?? "inconnu"));

      text(card, "État", !view.transportConnected ? "Communication perdue ou profil refusé" : view.receptionStale ? "Réception périmée" : o.continuity === "Rejected" ? "B3 non conforme — valeurs en quarantaine" : s?.held ? "HELD — dernière valeur retenue, non courante" : s?.acquisitionStopped ? "Arrêt / attente" : view.isCurrent ? "Valeur valide et fraîche reçue" : "Valeur invalide ou non fraîche");
      text(card, "Réception PC UTC", o.receivedAt);
      text(card, "UTC fenêtre déclarée par B3", s?.windowUtc ?? "Indisponible / non synchronisée ; B2 non observé");
      text(card, "Qualité V/F/HELD", s ? `${s.valuesValid}/${s.fresh}/${s.held}` : "Indisponible");
      if (v && s.valuesValid) {
        text(card, "Séquence / âge STM32", `${v.calculationSequence} / ${v.valueAgeMilliseconds} ms`);
        text(card, "RMS global / X / Y / Z (mg), observation conservée", `${v.rmsGlobalMg} / ${v.rmsXMg} / ${v.rmsYMg} / ${v.rmsZMg}`);
        text(card, "Crête globale / X / Y / Z (mg)", `${v.peakGlobalMg} / ${v.peakXMg} / ${v.peakYMg} / ${v.peakZMg}`);
      }
      const details=document.createElement("details"); details.className="technical";
      const summary=document.createElement("summary");summary.textContent="Source et diagnostic détaillé";details.append(summary);
      text(details,"Source",o.source);text(details, "Continuité / segment PC", o.continuity + " / " + o.segmentId);card.append(details);
      text(card, "Archivage", view.archiveState + (view.archiveError ? " — " + view.archiveError : ""));
      if (o.detail) text(card, "Détail", o.detail);
      rows.append(card);
    }
  } catch (e) { rows.replaceChildren(); error.textContent = e.message + " — aucune valeur courante affichée"; }
  setTimeout(refreshLive, 1000);
}
refreshLive();
